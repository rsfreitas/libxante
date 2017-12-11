
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

/* The dialog size onto the screen */
#define DIALOG_HEIGHT                       26
#define DIALOG_WIDTH                        75

/*
 *
 * Internal API
 *
 */

ui_return_t ui_file_view(struct xante_app *xpp, struct xante_item *item)
{
    ui_return_t ret;
    ui_properties_t properties;
    int ret_dialog = DLG_EXIT_OK;
    bool loop = true;
    cl_object_t *value = NULL;

    INIT_PROPERTIES(properties);
    properties.width = (item->geometry.width == 0) ? DIALOG_WIDTH
                                                   : item->geometry.width;

    properties.height = (item->geometry.height == 0) ? DIALOG_HEIGHT
                                                     : item->geometry.height;

    /* Gets the item voalue */
    value = item_value(item);

    if (NULL == value) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                             cl_tr("No file was selected to view."));

        goto end_block;
    }

    properties.text = CL_OBJECT_AS_CSTRING(value);

    if (file_exists(cl_string_valueof(properties.text)) == false) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                             cl_tr("File '%s' not found."),
                             cl_string_valueof(properties.text));

        goto end_block;
    }

    do {
        ret_dialog = dialog_textbox(cl_string_valueof(item->name),
                                    cl_string_valueof(properties.text),
                                    properties.height, properties.width);

        switch (ret_dialog) {
            case DLG_EXIT_ESC:
                /* Don't let the user close the dialog */
                if (xante_runtime_esc_key(xpp))
                    break;

            case DLG_EXIT_CANCEL:
            case DLG_EXIT_OK:
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

end_block:
    UNINIT_PROPERTIES(properties);

    ret.selected_button = DLG_EXIT_OK;
    ret.updated_value = false;

    return ret;
}

