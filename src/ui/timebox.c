
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

static void split_item_value(struct xante_item *item, int *hour, int *minutes,
    int *seconds)
{
    cl_object_t *value = NULL;
    cl_string_t *tmp = NULL;
    cl_string_list_t *time = NULL;
    char *str_value = NULL;

    value = item_value(item);
    str_value = CL_OBJECT_AS_STRING(value);
    tmp = cl_string_create("%s", str_value);
    free(str_value);

    time = cl_string_split(tmp, ":");
    cl_string_unref(tmp);

    /* HOUR */
    tmp = cl_string_list_get(time, 0);
    *hour = cl_string_to_int(tmp);
    cl_string_unref(tmp);

    /* MINUTES */
    tmp = cl_string_list_get(time, 1);
    *minutes = cl_string_to_int(tmp);
    cl_string_unref(tmp);

    /* SECONDS */
    tmp = cl_string_list_get(time, 2);
    *seconds = cl_string_to_int(tmp);
    cl_string_unref(tmp);

    cl_string_list_destroy(time);
}

/*
 *
 * Internal API
 *
 */

/**
 * @name ui_dialog_timebox
 * @brief Creates a dialog to choose a time in a timebox.
 *
 * @param [in] xpp: The main library object.
 * @param [in] item: The item to be used inside the dialog.
 *
 * @return Returns a boolean value indicating if the item's value has been
 *         changed (true) or not (false).
 */
bool ui_dialog_timebox(struct xante_app *xpp, struct xante_item *item,
    bool edit_value)
{
    bool value_changed = false, loop = true;
    cl_string_t *text = NULL;
    int ret_dialog = DLG_EXIT_OK, hour = 0, minutes = 0, seconds = 0;

    /* Prepare dialog */
    dialog_set_backtitle();
    dialog_update_cancel_button_label();
    dialog_alloc_input(64);

    /* Adjusts the dialog content using the item content */
    split_item_value(item, &hour, &minutes, &seconds);

    /* Adjusts the window message */
    text = cl_string_dup(item->options);
    cl_string_rplchr(text, XANTE_STR_LINE_BREAK, '\n');

    /* Enables the help button */
    if (item->help != NULL)
        dialog_vars.help_button = 1;

    do {
        ret_dialog = dialog_timebox(cl_string_valueof(item->name),
                                    cl_string_valueof(text), 3,
                                    MINIMUM_WIDTH, hour, minutes, seconds);

        switch (ret_dialog) {
            case DLG_EXIT_OK:
                loop = false;
                value_changed = true;
                /* TODO: Update value */
                break;

            case DLG_EXIT_ESC:
            case DLG_EXIT_CANCEL:
                loop = false;
                break;

            case DLG_EXIT_HELP:
                break;
        }
    } while (loop);

    if (item->help != NULL)
        dialog_vars.help_button = 0;

    if (text != NULL)
        cl_string_unref(text);

    dialog_free_input();

    return value_changed;
}

