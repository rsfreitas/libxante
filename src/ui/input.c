
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sat May  6 11:13:02 2017
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

#include <limits.h>
#include <float.h>

#include "libxante.h"
#include "ui_dialogs.h"
#include "../dialog/addon_dialogs.h"

/*
 *
 * Internal functions
 *
 */

static int get_input_length(const struct xante_item *item)
{
    int l = 0;

    switch (item->dialog_type) {
        case XANTE_UI_DIALOG_INPUT_INT:
            l = idigits(INT_MAX - 1);
            break;

        case XANTE_UI_DIALOG_INPUT_FLOAT:
            l = idigits((int)FLT_MAX - 1);
            break;

        case XANTE_UI_DIALOG_INPUT_DATE:
            l = DATE_MAX_INPUT_LENGTH;
            break;

        case XANTE_UI_DIALOG_INPUT_TIME:
            l = TIME_MAX_INPUT_LENGTH;
            break;

        case XANTE_UI_DIALOG_INPUT_STRING:
        case XANTE_UI_DIALOG_INPUT_PASSWD:
            l = item->string_length;
            break;

        default:
            break;
    }

    return l;
}

static int get_longest_line_length(cl_string_t *text)
{
    cl_string_t *s = cl_string_dup(text), *tmp;
    cl_string_list_t *list = NULL;
    int length = 0, i, total;

    cl_string_rplchr(s, XANTE_STR_LINE_BREAK, '\n');
    list = cl_string_split(s, "^");
    total = cl_string_list_size(list);

    for (i = 0; i < total; i++) {
        tmp = cl_string_list_get(list, i);

        if (cl_string_length(tmp) > length)
            length = cl_string_length(tmp);

        cl_string_unref(tmp);
    }

    if (list != NULL)
        cl_string_list_destroy(list);

    cl_string_unref(s);

    return length;
}

static int get_window_width(const struct xante_item *item)
{
    int w = 0;

    if (dialog_count_lines_by_delimiters(cl_string_valueof(item->options)) > 1)
        return get_longest_line_length(item->options) + WINDOW_COLUMNS;

    w = cl_string_length(item->options);

    if (w < MINIMUM_WIDTH)
        w = MINIMUM_WIDTH;
    else
        w += WINDOW_COLUMNS;

    return w;
}

/*
 *
 * Internal API
 *
 */

/**
 * @name ui_dialog_input
 * @brief Creates a dialog to do some data input.
 *
 * @param [in] xpp: The main library object.
 * @param [in] item: The item to be used inside the dialog.
 * @param [in] edit_value: A flag to indicate if the value will be editable
 *                         or not.
 *
 * @return Returns a boolean value indicating if the item's value has been
 *         changed (true) or not (false).
 */
bool ui_dialog_input(struct xante_app *xpp, struct xante_item *item,
    bool edit_value)
{
    bool loop = true, value_changed = false;
    int ret_dialog = DLG_EXIT_OK, height, width;
    char input[MAX_INPUT_VALUE] = {0};
    cl_string_t *text = NULL;

    /* Prepare dialog */
    dialog_set_backtitle();
    dialog_update_cancel_button_label();

    /* Prepares dialog content */

    /* Enables the help button */
    if (item->help != NULL)
        dialog_vars.help_button = 1;

    /* Adjusts window width and height */
    text = cl_string_dup(item->options);
    cl_string_rplchr(text, XANTE_STR_LINE_BREAK, '\n');
    width = get_window_width(item);
    height = dialog_count_lines_by_delimiters(cl_string_valueof(text)) +
        FORM_HEIGHT_WITHOUT_TEXT;

    do {
        if (item->dialog_type == XANTE_UI_DIALOG_INPUT_PASSWD) {
            ret_dialog = ui_dialog_passwd(item, edit_value, input, sizeof(input),
                                          height, text);
        } else {
            ret_dialog = dlg_inputbox(width, height,
                                      cl_string_valueof(item->name),
                                      cl_string_valueof(text), NULL, NULL,
                                      get_input_length(item), input,
                                      edit_value, NULL, NULL);
        }

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

    return value_changed;
}

