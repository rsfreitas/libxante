
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Thu Jun  1 12:53:40 2017
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
 * Internal functions
 *
 */

static bool is_input_valid(const char *input)
{
    if (strlen(input) == 0)
        return false;

    return true;
}

/*
 *
 * Internal API
 *
 */

bool add_dm_value_changed(ui_properties_t *properties)
{
    if (NULL == properties->result)
        return false;

    /* Set up details to save inside the internal changes list */
    properties->change_item_name = cl_string_create("New dynamic menu entry");
    properties->change_old_value = cl_string_create("EMPTY");
    properties->change_new_value = cl_string_create("EMPTY");

    return true;
}

int add_dm(ui_properties_t *properties)
{
    struct xante_app *xpp = properties->xpp;
    struct xante_item *item = properties->item;
    int ret_dialog = DLG_EXIT_OK;
    char input[MAX_INPUT_VALUE] = {0};

    if (properties->editable_value == false) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                             cl_tr("Cannot add item!"));

        return DLG_EXIT_OK;
    }

    /* Adjusts window width and height */
    properties->text = cl_string_dup(item->options);
    cl_string_rplchr(properties->text, XANTE_STR_LINE_BREAK, '\n');
    properties->width = (item->geometry.width == 0)
                            ? dlgx_get_input_window_width(item)
                            : item->geometry.width;

    properties->height = (item->geometry.height == 0)
                         ? dlgx_count_lines_by_delimiters(cl_string_valueof(properties->text)) +
                           FORM_HEIGHT_WITHOUT_TEXT
                         : item->geometry.height;

    ret_dialog = dlgx_inputbox(properties->width, properties->height,
                               cl_string_valueof(item->name),
                               cl_string_valueof(properties->text), NULL,
                               NULL, sizeof(input) - 1, input,
                               true, NULL, NULL, NULL);

    if (ret_dialog == DLG_EXIT_OK) {
        if (is_input_valid(input) == true) {
            /*
             * We insert the new dynamic menu entry and set this as an
             * internal change, so it will ask the user to save into
             * the configuration file.
             */
            if (dm_insert(xpp, item, input)) {
                /* We hold a simple string just to know that we have a change */
                properties->result = cl_string_create("changed");
            }
        } else
            xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                                 "%s", cl_tr("An invalid input was entered!"));
    }

    return ret_dialog;
}

