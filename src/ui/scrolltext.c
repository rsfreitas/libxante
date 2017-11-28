
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Mon Nov 27 15:10:12 2017
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
    "[ESC] Cancel [Enter] Confirm selected button [Up/Down/Pg Up/Pg Down] Navigate through the text"

/* The dialog size onto the screen */
#define DIALOG_HEIGHT                       12
#define DIALOG_WIDTH                        60

/*
 *
 * Internal API
 *
 */

ui_return_t ui_dialog_scrolltext(struct xante_app *xpp, struct xante_item *item)
{
    ui_return_t ret;
    int ret_dialog = DLG_EXIT_OK;
    bool loop = true;
    cl_object_t *value = NULL;
    char *text = NULL;

    /* Prepare dialog */
    dlgx_set_backtitle(xpp);
    dlgx_update_cancel_button_label();
    dlgx_put_statusbar(DEFAULT_STATUSBAR_TEXT);

    /* Gets the item voalue */
    value = item_value(item);
    text = CL_OBJECT_AS_STRING(value);

    /* Enables the help button */
    if (item->descriptive_help != NULL)
        dialog_vars.help_button = 1;

    xante_log_info("%s: '%s' '%s'", __FUNCTION__, text,
                    cl_string_valueof(item->options));
    do {
        ret_dialog = dlgx_scrolltext(DIALOG_WIDTH, DIALOG_HEIGHT,
                                     cl_string_valueof(item->name),
                                     cl_string_valueof(item->options),
                                     text);

        switch (ret_dialog) {
            case DLG_EXIT_OK:
                loop = false;
                break;

#ifdef ALTERNATIVE_DIALOG
            case DLG_EXIT_TIMEOUT:
                loop = false;
                break;
#endif

            case DLG_EXIT_ESC:
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

    if (text != NULL)
        free(text);

    ret.selected_button = DLG_EXIT_OK;
    ret.updated_value = false;

    return ret;
}

