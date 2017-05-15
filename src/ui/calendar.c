
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

    /* Prepare dialog */
    dialog_set_backtitle();
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

