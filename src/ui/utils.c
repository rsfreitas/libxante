
/*
 * Description: Common functions to the UI dialogs.
 *
 * Author: Rodrigo Freitas
 * Created at: Fri May  5 21:23:32 2017
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
#include "ui_dialogs.h"

/**
 * @name dialog_set_backtitle
 * @brief Sets the application backtitle.
 */
void dialog_set_backtitle(void)
{
    dlg_clear();
    dlg_put_backtitle();
}

/**
 * @name dialog_get_item_value_as_text
 * @brief Gives an item current value as a text string.
 *
 * The function checks if the item has a value loaded from the JTF file (default
 * value) or from the configuration file (value).
 *
 * @param [in] item: The item.
 *
 * @return Case it's an item that may hold a value it's returned as a string
 *         (which must be released after use) or NULL otherwise.
 */
char *dialog_get_item_value_as_text(const struct xante_item *item)
{
    char *text = NULL;
    cl_string_t *value = NULL;
    int index = -1;

    switch (item->type) {
        case XANTE_UI_DIALOG_INPUT_INT:
        case XANTE_UI_DIALOG_INPUT_FLOAT:
        case XANTE_UI_DIALOG_INPUT_DATE:
        case XANTE_UI_DIALOG_INPUT_TIME:
        case XANTE_UI_DIALOG_CALENDAR:
        case XANTE_UI_DIALOG_TIMEBOX:
        case XANTE_UI_DIALOG_INPUT_STRING:
            value = cl_object_to_cstring(item_value(item));
            text = strdup(cl_string_valueof(value));
            cl_string_unref(value);
            break;

        case XANTE_UI_DIALOG_RADIO_CHECKLIST:
            index = CL_OBJECT_AS_INT(item_value(item));
            value = cl_string_list_get(item->checklist_options, index);

            if (value != NULL) {
                text = strdup(cl_string_valueof(value));
                cl_string_unref(value);
            }

            break;

        case XANTE_UI_DIALOG_YES_NO:
            index = CL_OBJECT_AS_INT(item_value(item));

            if (index == 1)
                text = strdup(cl_tr("Yes"));
            else
                text = strdup(cl_tr("No"));

            break;

        default:
            break;
    }

    return text;
}

