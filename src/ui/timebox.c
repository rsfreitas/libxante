
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Fri May  5 21:08:15 2017
 * Project: libxante
 *
 * Copyright (C) 2017 Rodrigo Freitas
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 */

#include "libxante.h"

#define DEFAULT_WIDTH                       DEFAULT_DIALOG_WIDTH
#define DEFAULT_HEIGHT                      3

/*
 *
 * Internal functions
 *
 */

static void split_item_value(struct xante_item *item, int *hour, int *minutes,
    int *seconds)
{
    cl_object_t *value = NULL;
    cl_string_t *tmp = NULL;
    cl_stringlist_t *time = NULL;
    char *str_value = NULL;

    value = item_value(item);
    str_value = CL_OBJECT_AS_STRING(value);
    tmp = cl_string_create("%s", str_value);
    free(str_value);

    time = cl_string_split(tmp, ":");
    cl_string_unref(tmp);

    /* HOUR */
    tmp = cl_stringlist_get(time, 0);
    *hour = cl_string_to_int(tmp);
    cl_string_unref(tmp);

    /* MINUTES */
    tmp = cl_stringlist_get(time, 1);
    *minutes = cl_string_to_int(tmp);
    cl_string_unref(tmp);

    /* SECONDS */
    tmp = cl_stringlist_get(time, 2);
    *seconds = cl_string_to_int(tmp);
    cl_string_unref(tmp);

    cl_stringlist_destroy(time);
}

static bool item_value_has_changed(struct xante_app *xpp,
    struct xante_item *item, const char *result)
{
    cl_object_t *value = NULL;
    char *str_value = NULL;
    bool changed = false;

    value = item_value(item);
    str_value = CL_OBJECT_AS_STRING(value);

    if (equals(result, str_value) == false) {
        change_add(xpp, cl_string_valueof(item->name), str_value, result);
        changed = true;

        /* Updates item value */
        if (NULL == item->value)
            item->value = cl_object_create(CL_STRING, result);
        else
            cl_object_set(item->value, result);
    }

    free(str_value);

    return changed;
}

/*
 *
 * Internal API
 *
 */

/**
 * @name ui_timebox
 * @brief Creates a dialog to choose a time in a timebox.
 *
 * @param [in] xpp: The main library object.
 * @param [in] item: The item to be used inside the dialog.
 * @param [in] edit_value: A flag to indicate if the value will be editable
 *                         or not.
 *
 * @return Returns a ui_return_t value indicating if the item's value has been
 *         changed (true) or not (false) with the dialog selected button.
 */
ui_return_t ui_timebox(struct xante_app *xpp, struct xante_item *item,
    bool edit_value)
{
    bool value_changed = false, loop = true;
    int ret_dialog = DLG_EXIT_OK, hour = 0, minutes = 0, seconds = 0;
    char *result = NULL;
    ui_return_t ret;
    ui_properties_t properties;

    INIT_PROPERTIES(properties);
    properties.width = (item->geometry.width == 0) ? DEFAULT_WIDTH
                                                   : item->geometry.width;

    properties.height = (item->geometry.height == 0) ? DEFAULT_HEIGHT
                                                     : item->geometry.height;

    /* Adjusts the dialog content using the item content */
    split_item_value(item, &hour, &minutes, &seconds);

    /* Adjusts the window message */
    properties.text = cl_string_dup(item->options);
    cl_string_rplchr(properties.text, XANTE_STR_LINE_BREAK, '\n');

    do {
#ifdef ALTERNATIVE_DIALOG
        ret_dialog = dialog_timebox(cl_string_valueof(item->name),
                                    cl_string_valueof(properties.text),
                                    properties.height, properties.width,
                                    hour, minutes, seconds,
                                    edit_value);
#else
        ret_dialog = dialog_timebox(cl_string_valueof(item->name),
                                    cl_string_valueof(properties.text),
                                    properties.height, properties.width,
                                    hour, minutes, seconds);
#endif

        switch (ret_dialog) {
            case DLG_EXIT_OK:
                loop = false;

                if (edit_value == false)
                    break;
                else {
                    result = dlgx_get_input_result();

                    if (event_call(EV_ITEM_VALUE_CONFIRM, xpp, item,
                                   result) < 0)
                    {
                        loop = true;
                        break;
                    }

                    if (item_value_has_changed(xpp, item, result)) {
                        value_changed = true;
                        break;
                    }
                }

                break;

#ifdef ALTERNATIVE_DIALOG
            case DLG_EXIT_TIMEOUT:
                loop = false;
                break;
#endif

            case DLG_EXIT_ESC:
                /* Don't let the user close the dialog */
                if (xante_runtime_esc_key(xpp))
                    break;

            case DLG_EXIT_CANCEL:
                loop = false;
                break;

            case DLG_EXIT_HELP:
                dialog_vars.help_button = 0;
                xante_dlg_messagebox(xpp, XANTE_MSGBOX_INFO, cl_tr("Help"), "%s",
                                     cl_string_valueof(item->descriptive_help));

                dialog_vars.help_button = 1;
                break;
        }

        if (result != NULL) {
            free(result);
            result = NULL;
        }
    } while (loop);

    UNINIT_PROPERTIES(properties);

    ret.selected_button = ret_dialog;
    ret.updated_value = value_changed;

    return ret;
}

