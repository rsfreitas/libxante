
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

/* The dialog size onto the screen */
#define DIALOG_HEIGHT                       12
#define DIALOG_WIDTH                        60

/*
 *
 * Internal API
 *
 */

bool fselect_validate_result(ui_properties_t *properties)
{
    if (NULL == properties->result)
        return false;

    return true;
}

bool fselect_value_changed(ui_properties_t *properties)
{
    struct xante_item *item = properties->item;
    bool changed = false;
    cl_string_t *current_path = NULL;

    if (NULL == properties->result)
        return false;

    current_path = cl_object_to_cstring(item_value(item));

    if (cl_string_cmp(properties->result, current_path) != 0) {
        changed = true;

        /* Set up details to save inside the internal changes list */
        properties->change_item_name = cl_string_ref(item->name);
        properties->change_old_value = cl_string_ref(current_path);
        properties->change_new_value = cl_string_ref(properties->result);
    }

    if (current_path != NULL)
        cl_string_unref(current_path);

    return changed;
}

void fselect_update_value(ui_properties_t *properties)
{
    struct xante_item *item = properties->item;

    if (item->value != NULL)
        cl_object_unref(item->value);

    item->value = cl_object_from_cstring(properties->result);
}

int fselect(ui_properties_t *properties)
{
    struct xante_item *item = properties->item;
    int ret_dialog = DLG_EXIT_OK;
    char *chosen_path = NULL;

    properties->width = (item->geometry.width == 0) ? DIALOG_WIDTH
                                                   : item->geometry.width;

    properties->height = (item->geometry.height == 0) ? DIALOG_HEIGHT
                                                     : item->geometry.height;

    if (item->dialog_type == XANTE_UI_DIALOG_FILE_SELECT) {
        ret_dialog = dialog_fselect(cl_string_valueof(item->name),
                                    cl_string_valueof(item->options),
                                    properties->height, properties->width);
    } else {
        ret_dialog = dialog_dselect(cl_string_valueof(item->name),
                                    cl_string_valueof(item->options),
                                    properties->height, properties->width);
    }

    if (ret_dialog == DLG_EXIT_OK) {
        chosen_path = dlgx_get_input_result();
        properties->result = cl_string_create("%s", chosen_path);
        free(chosen_path);
    }

    return ret_dialog;
}

