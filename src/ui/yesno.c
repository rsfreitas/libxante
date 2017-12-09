
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

#define DEFAULT_STATUSBAR_TEXT              \
    "[ESC] Cancel [Enter] Confirm a selected option [Tab/Left/Right] Select an option"

#define DEFAULT_WIDTH                       DEFAULT_DIALOG_WIDTH

/*
 *
 * Internal API
 *
 */

/**
 * @name ui_yesno
 * @brief Creates a dialog to choose between two options.
 *
 * @param [in] xpp: The main library object.
 * @param [in] item: The item to be used inside the dialog.
 *
 * @return Returns a ui_return_t value indicating if the item's value has been
 *         changed (true) or not (false) with the dialog selected button.
 */
ui_return_t ui_yesno(struct xante_app *xpp, struct xante_item *item)
{
    const char *yes = cl_tr("Yes"), *no = cl_tr("No");
    bool value_changed = false, loop = true;
    cl_object_t *value = NULL;
    int choice = -1, ret_dialog = DLG_EXIT_OK;
    ui_return_t ret;
    ui_properties_t properties;

    INIT_PROPERTIES(properties);

    /* Prepare dialog */
    dlgx_set_backtitle(xpp);
    dlgx_update_cancel_button_label();
    dlgx_put_statusbar(DEFAULT_STATUSBAR_TEXT);

    /* Sets the button labels using the item value */
    value = item_value(item);
    choice = CL_OBJECT_AS_INT(value);
    dialog_vars.yes_label = (choice == 0 ? (char *)yes : (char *)no);
    dialog_vars.no_label = (choice == 0 ? (char *)no : (char *)yes);

    /* Adjusts the window message */
    properties.text = cl_string_dup(item->options);
    properties.width = (item->geometry.width == 0) ? DEFAULT_WIDTH
                                                   : item->geometry.width;

    properties.height = (item->geometry.height == 0)
                             ? dlgx_count_lines(cl_string_valueof(properties.text),
                                                properties.width)
                             : item->geometry.height;

    cl_string_rplchr(properties.text, XANTE_STR_LINE_BREAK, '\n');

    /* Enables the help button */
    if (item->descriptive_help != NULL)
        dialog_vars.help_button = 1;

    do {
        ret_dialog = dialog_yesno(cl_string_valueof(item->name),
                                  cl_string_valueof(properties.text),
                                  properties.height, properties.width);

        switch (ret_dialog) {
            case DLG_EXIT_OK:
                if (event_call(EV_ITEM_VALUE_CONFIRM, xpp, item, !choice) < 0)
                    break;

                /* Selecting the "OK" button always changes the item value. */
                loop = false;
                value_changed = true;

                change_add(xpp, cl_string_valueof(item->name),
                           dialog_vars.no_label, dialog_vars.yes_label);

                /* Updates the item value */
                if (NULL == item->value)
                    item->value = cl_object_create(CL_INT, !choice);
                else
                    cl_object_set(item->value, !choice);

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
    } while (loop);

    if (item->descriptive_help != NULL)
        dialog_vars.help_button = 0;

    UNINIT_PROPERTIES(properties);

    ret.selected_button = ret_dialog;
    ret.updated_value = value_changed;

    return ret;
}

