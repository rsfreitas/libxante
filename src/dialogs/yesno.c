
/*
 * Description: Handles a yesno dialog.
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

/*
 *
 * Internal API
 *
 */

bool yesno_validate_result(ui_properties_t *properties)
{
    struct xante_item *item = properties->item;
    int choice;

    if (NULL == properties->result)
        return false;

    /* Set up details to save inside the internal changes list */
    properties->change_item_name = cl_string_ref(item->name);
    properties->change_old_value = cl_string_create("%s", dialog_vars.no_label);
    properties->change_new_value = cl_string_create("%s", dialog_vars.yes_label);

    /* Updates the item value */
    choice = cl_string_to_int(properties->result);

    if (NULL == item->value)
        item->value = cl_object_create(CL_INT, choice);
    else
        cl_object_set(item->value, choice);

    return true;
}

/**
 * @name ui_yesno
 * @brief Creates a dialog to choose between two options.
 *
 * @return Returns a ui_return_t value indicating if the item's value has been
 *         changed (true) or not (false) with the dialog selected button.
 */
int yesno(ui_properties_t *properties)
{
    struct xante_app *xpp = properties->xpp;
    struct xante_item *item = properties->item;
    const char *yes = cl_tr("Yes"), *no = cl_tr("No");
    cl_object_t *value = NULL;
    int choice = -1, ret_dialog = DLG_EXIT_OK;

    if (properties->editable_value == false) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                             cl_tr("Cannot change the value of this item!"));

        return DLG_EXIT_CANCEL;
    }

    /* Sets the button labels using the item value */
    value = item_value(item);
    choice = CL_OBJECT_AS_INT(value);
    dialog_vars.yes_label = (choice == 0 ? (char *)yes : (char *)no);
    dialog_vars.no_label = (choice == 0 ? (char *)no : (char *)yes);

    /* Adjusts the window message */
    properties->text = cl_string_dup(item->options);
    properties->width = (item->geometry.width == 0) ? DEFAULT_WIDTH
                                                   : item->geometry.width;

    properties->height = (item->geometry.height == 0)
                             ? dlgx_count_lines(cl_string_valueof(properties->text),
                                                properties->width)
                             : item->geometry.height;

    cl_string_rplchr(properties->text, XANTE_STR_LINE_BREAK, '\n');
    ret_dialog = dialog_yesno(cl_string_valueof(item->name),
                              cl_string_valueof(properties->text),
                              properties->height, properties->width);

    if (ret_dialog == DLG_EXIT_OK)
        properties->result = cl_string_create("%d", !choice);

    return ret_dialog;
}

