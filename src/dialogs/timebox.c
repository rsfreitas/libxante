
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

/*
 *
 * Internal API
 *
 */

bool timebox_value_changed(ui_properties_t *properties)
{
    struct xante_item *item = properties->item;
    cl_object_t *value = NULL;
    cl_string_t *str_value = NULL;
    bool changed = false;

    value = item_value(item);
    str_value = cl_object_to_cstring(value);

    if (cl_string_cmp(properties->result, str_value) != 0) {
        changed = true;

        /* Set up details to save inside the internal changes list */
        properties->change_item_name = cl_string_ref(item->name);
        properties->change_old_value = cl_string_ref(str_value);
        properties->change_new_value = cl_string_ref(properties->result);
    }

    if (str_value != NULL)
        cl_string_unref(str_value);

    return changed;
}

void timebox_update_value(ui_properties_t *properties)
{
    struct xante_item *item = properties->item;

    /* Updates item value */
    if (NULL == item->value)
        item->value = cl_object_from_cstring(properties->result);
    else
        cl_object_set(item->value, cl_string_valueof(properties->result));
}

/**
 * @name ui_timebox
 * @brief Creates a dialog to choose a time in a timebox.
 *
 * @return Returns a ui_return_t value indicating if the item's value has been
 *         changed (true) or not (false) with the dialog selected button.
 */
int timebox(ui_properties_t *properties)
{
    struct xante_item *item = properties->item;
    int ret_dialog = DLG_EXIT_OK, hour = 0, minutes = 0, seconds = 0;
    char *result = NULL;

    properties->width = (item->geometry.width == 0) ? DEFAULT_WIDTH
                                                   : item->geometry.width;

    properties->height = (item->geometry.height == 0) ? DEFAULT_HEIGHT
                                                     : item->geometry.height;

    /* Adjusts the dialog content using the item content */
    split_item_value(item, &hour, &minutes, &seconds);

    /* Adjusts the window message */
    properties->text = cl_string_dup(item->options);
    cl_string_rplchr(properties->text, XANTE_STR_LINE_BREAK, '\n');

#ifdef ALTERNATIVE_DIALOG
    ret_dialog = dialog_timebox(cl_string_valueof(item->name),
                                cl_string_valueof(properties->text),
                                properties->height, properties->width,
                                hour, minutes, seconds,
                                properties->editable_value);
#else
    ret_dialog = dialog_timebox(cl_string_valueof(item->name),
                                cl_string_valueof(properties->text),
                                properties->height, properties->width,
                                hour, minutes, seconds);
#endif

    if ((ret_dialog == DLG_EXIT_OK) && properties->editable_value) {
        result = dlgx_get_input_result();
        properties->result = cl_string_create("%s", result);
        free(result);
    }

    return ret_dialog;
}

