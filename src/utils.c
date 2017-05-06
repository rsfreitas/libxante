
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Tue May  2 20:25:58 2017
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
 * @name is_valid_config_file_status
 * @brief Checks if a given status is valid inside the library environment.
 *
 * @param [in] status: The status to be checked.
 *
 * @return Returns true if is a valid internal status or false otherwise.
 */
bool is_valid_config_file_status(enum xante_config_file_status status)
{
    switch (status) {
        case XANTE_CFG_ST_CREATED:
        case XANTE_CFG_ST_LOADED:
        case XANTE_CFG_ST_SAVED:
        case XANTE_CFG_ST_UNSAVED:
            return true;

        default:
            break;
    }

    return false;
}

/**
 * @name is_valid_ui_dialog
 * @brief Checks if a given dialog type is valid inside the library
 *        environment.
 *
 * @param [in] type: The dialog type to be checked.
 *
 * @return Returns true if is a valid dialog type or false otherwise.
 */
bool is_valid_ui_dialog(enum xante_ui_dialog type)
{
    switch (type) {
        case XANTE_UI_DIALOG_MENU:
        case XANTE_UI_DIALOG_INPUT_INT:
        case XANTE_UI_DIALOG_INPUT_FLOAT:
        case XANTE_UI_DIALOG_INPUT_DATE:
        case XANTE_UI_DIALOG_INPUT_STRING:
        case XANTE_UI_DIALOG_INPUT_PASSWD:
        case XANTE_UI_DIALOG_INPUT_TIME:
        case XANTE_UI_DIALOG_CALENDAR:
        case XANTE_UI_DIALOG_TIMEBOX:
        case XANTE_UI_DIALOG_OPTION:
        case XANTE_UI_DIALOG_YES_NO:
        case XANTE_UI_DIALOG_CUSTOM:
        case XANTE_UI_DIALOG_DYNAMIC_MENU:
        case XANTE_UI_DIALOG_RM_DYNAMIC_MENU:
            return true;

        default:
            break;
    }

    return false;
}

/**
 * @name is_available_item
 * @brief Checks if a specific item is available to the user.
 *
 * @param [in] item: The item to be checked.
 *
 * @return Returns true if the item is available or false otherwise.
 */
bool is_item_available(struct xante_item *item)
{
    if (item->mode == XANTE_ACCESS_HIDDEN)
        return false;

    return true;
}

