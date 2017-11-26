
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

/*
 *
 * Internal API
 *
 */

/**
 * @name ui_dialog_passwd
 * @brief Creates a dialog to do input inside a password form.
 *
 * @param [in] item: The item to be used inside the dialog.
 * @param [in] edit_value: A flag to indicate if the value will be editable
 *                         or not.
 * @param [in,out] input: The result input.
 * @param [in] input_lenght: The maximum input length.
 *
 * @return Returns the libdialog's return value type of a dialog.
 */
int ui_dialog_passwd(struct xante_item *item, bool edit_value, char *input,
    unsigned int input_length, int height, cl_string_t *text)
{
    DIALOG_FORMITEM fitem;
    int ret_dialog = DLG_EXIT_OK, form_height = 1, selected = -1;

    dialog_vars.insecure = 1;

    fitem.type = 1;
    fitem.text_free = 0;
    fitem.text = input;
    fitem.text_len = strlen(input);
    fitem.text_y = 0;
    fitem.text_x = 0;

    if (edit_value == true)
        fitem.text_flen = 53;
    else
        fitem.text_flen = 0;

    fitem.text_ilen = MAX_INPUT_VALUE;
    ret_dialog = dlg_form(cl_string_valueof(item->name),
                          cl_string_valueof(text), height, MINIMUM_WIDTH,
                          form_height, 1, &fitem, &selected);

    if (ret_dialog == DLG_EXIT_OK)
        snprintf(input, input_length - 1, "%s", fitem.text);

    dialog_vars.insecure = 0;

    return ret_dialog;
}

