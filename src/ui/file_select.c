
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sun Nov 26 15:57:46 2017
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
    "[ESC] Cancel [Enter] Confirm selected button [Tab/Arrows] Navigate through options [Spacebar] Select dir/file"

/* The dialog size onto the screen */
#define DIALOG_HEIGHT                       12
#define DIALOG_WIDTH                        60

/*
 *
 * Internal functions
 *
 */

static void set_internal_value(struct xante_item *item, const char *new_value)
{
    cl_string_t *result = NULL;

    if (item->value != NULL)
        cl_object_unref(item->value);

    result = cl_string_create("%s", new_value);
    item->value = cl_object_from_cstring(result);
    cl_string_unref(result);
}

/*
 *
 * Internal API
 *
 */

ui_return_t ui_fselect(struct xante_app *xpp, struct xante_item *item)
{
    ui_return_t ret;
    int ret_dialog = DLG_EXIT_OK;
    bool choice_selected = false, loop = true;
    char *chosen_path = NULL;
    ui_properties_t properties;

    INIT_PROPERTIES(properties);

    /* Prepare dialog */
    dlgx_set_backtitle(xpp);
    dlgx_update_cancel_button_label();
    dlgx_put_statusbar(DEFAULT_STATUSBAR_TEXT);
    properties.width = (item->geometry.width == 0) ? DIALOG_WIDTH
                                                   : item->geometry.width;

    properties.height = (item->geometry.height == 0) ? DIALOG_HEIGHT
                                                     : item->geometry.height;

    /* Enables the help button */
    if (item->descriptive_help != NULL)
        dialog_vars.help_button = 1;

    do {
        dlgx_free_input();

        if (item->dialog_type == XANTE_UI_DIALOG_FILE_SELECT) {
            ret_dialog = dialog_fselect(cl_string_valueof(item->name),
                                        cl_string_valueof(item->options),
                                        properties.height, properties.width);
        } else {
            ret_dialog = dialog_dselect(cl_string_valueof(item->name),
                                        cl_string_valueof(item->options),
                                        properties.height, properties.width);
        }

        switch (ret_dialog) {
            case DLG_EXIT_OK:
                chosen_path = dlgx_get_input_result();

                if (event_call(EV_ITEM_VALUE_CONFIRM, xpp, item,
                               chosen_path) < 0)
                {
                    free(chosen_path);
                    break;
                }

                /*
                 * Here we can set the item's internal value and consequently
                 * set it as having a change to allow the UPDATED event be
                 * called.
                 */
                set_internal_value(item, chosen_path);
                choice_selected = true;
                free(chosen_path);
                loop = false;
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

    ret.selected_button = DLG_EXIT_OK;
    ret.updated_value = choice_selected;

    return ret;
}

