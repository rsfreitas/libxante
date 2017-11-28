
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Mon Nov 27 15:10:13 2017
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

#define DEFAULT_STATUSBAR_TEXT          \
    "[ESC] Cancel [Enter] Confirm a selected option"

#define DIALOG_WIDTH                    60
#define DIALOG_HEIGHT                   12
#define DIALOG_UPDATE_INTERVAL          500

struct update_routine_arg {
    struct xante_app    *xpp;
    struct xante_item   *item;
    void                *data;
};

/*
 *
 * Internal functions
 *
 */

static char *update_routine(void *arg)
{
    struct update_routine_arg *user_arg = (struct update_routine_arg *)arg;
    char *string = NULL;
    cl_object_t *value = NULL;

    event_call(EV_UPDATE_ROUTINE, user_arg->xpp, user_arg->item,
               user_arg->data);

    /*
     * The user must update the content to be displayed inside the
     * update-routine event.
     */
    value = item_value(user_arg->item);

    if (NULL == value)
        return NULL;

    string = CL_OBJECT_AS_STRING(value);

    return string;
}

/*
 *
 * Internal API
 *
 */

ui_return_t ui_dialog_update_object(struct xante_app *xpp,
    struct xante_item *item)
{
    ui_return_t ret;
    int ret_dialog = DLG_EXIT_OK;
    bool loop = true;
    struct update_routine_arg user_arg = {
        .xpp = xpp,
        .item = item
    };

    /* Prepare dialog */
    dlgx_set_backtitle(xpp);
    dlgx_update_cancel_button_label();
    dlgx_put_statusbar(DEFAULT_STATUSBAR_TEXT);

    /* Gets the user custom data */
    user_arg.data = event_item_custom_data(xpp, item);

    do {
        ret_dialog = dlgx_update_object(DIALOG_WIDTH, DIALOG_HEIGHT,
                                        cl_string_valueof(item->name),
                                        cl_string_valueof(item->options),
                                        DIALOG_UPDATE_INTERVAL,
                                        update_routine, &user_arg);

        switch (ret_dialog) {
            case DLG_EXIT_OK:
            case DLG_EXIT_ESC:
            case DLG_EXIT_CANCEL:
                loop = false;
                break;

#ifdef ALTERNATIVE_DIALOG
            case DLG_EXIT_TIMEOUT:
                loop = false;
                break;
#endif

            case DLG_EXIT_HELP:
                dialog_vars.help_button = 0;
                xante_dlg_messagebox(xpp, XANTE_MSGBOX_INFO, cl_tr("Help"), "%s",
                                     cl_string_valueof(item->descriptive_help));

                dialog_vars.help_button = 1;
                break;
        }
    } while (loop);

    ret.selected_button = ret_dialog;
    ret.updated_value = false;

    return ret;
}

