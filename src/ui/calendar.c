
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
#include "ui_dialogs.h"

/*
 *
 * Internal functions
 *
 */

static void split_item_value(struct xante_item *item, int *day, int *month,
    int *year)
{
    cl_object_t *value = NULL;
    cl_string_t *tmp = NULL;
    cl_string_list_t *date = NULL;
    char *str_value = NULL;

    value = item_value(item);
    str_value = CL_OBJECT_AS_STRING(value);
    tmp = cl_string_create("%s", str_value);
    free(str_value);

    date = cl_string_split(tmp, "/");
    cl_string_unref(tmp);

    /* DAY */
    tmp = cl_string_list_get(date, 0);
    *day = cl_string_to_int(tmp);
    cl_string_unref(tmp);

    /* MONTH */
    tmp = cl_string_list_get(date, 1);
    *month = cl_string_to_int(tmp);
    cl_string_unref(tmp);

    /* YEAR */
    tmp = cl_string_list_get(date, 2);
    *year = cl_string_to_int(tmp);
    cl_string_unref(tmp);

    cl_string_list_destroy(date);
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
 * @name ui_dialog_calendar
 * @brief Creates a dialog to choose a date in a calendar.
 *
 * @param [in] xpp: The main library object.
 * @param [in] item: The item to be used inside the dialog.
 * @param [in] edit_value: A flag to indicate if the value will be editable
 *                         or not.
 *
 * @return Returns a boolean value indicating if the item's value has been
 *         changed (true) or not (false).
 */
bool ui_dialog_calendar(struct xante_app *xpp, struct xante_item *item,
    bool edit_value)
{
    bool value_changed = false, loop = true;
    cl_string_t *text = NULL;
    int ret_dialog = DLG_EXIT_OK, day = 0, month = 0, year = 0;
    char *result = NULL;

    /* Prepare dialog */
    dialog_set_backtitle(xpp);
    dialog_update_cancel_button_label();
    dialog_alloc_input(64);

    /* Adjusts the dialog content using the item content */
    split_item_value(item, &day, &month, &year);

    /* Adjusts the window message */
    text = cl_string_dup(item->options);
    cl_string_rplchr(text, XANTE_STR_LINE_BREAK, '\n');

    /* Enables the help button */
    if (item->help != NULL)
        dialog_vars.help_button = 1;

    do {
        ret_dialog = dialog_calendar(cl_string_valueof(item->name),
                                     cl_string_valueof(text), 3,
                                     MINIMUM_WIDTH, day, month, year);

        switch (ret_dialog) {
            case DLG_EXIT_OK:
                loop = false;

                if (edit_value == false)
                    break;
                else {
                    result = dialog_get_input_result();

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

            case DLG_EXIT_ESC:
            case DLG_EXIT_CANCEL:
                loop = false;
                break;

            case DLG_EXIT_HELP:
                dialog_vars.help_button = 0;
                xante_messagebox(xpp, XANTE_MSGBOX_INFO, 0, cl_tr("Help"),
                                 cl_string_valueof(item->help));

                dialog_vars.help_button = 1;
                break;
        }

        if (result != NULL) {
            free(result);
            result = NULL;
        }
    } while (loop);

    if (item->help != NULL)
        dialog_vars.help_button = 0;

    if (text != NULL)
        cl_string_unref(text);

    dialog_free_input();

    return value_changed;
}

