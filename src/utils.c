
/*
 * Description: Utility functions.
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
        case XANTE_UI_DIALOG_CHECKLIST:
        case XANTE_UI_DIALOG_RADIO_CHECKLIST:
        case XANTE_UI_DIALOG_YES_NO:
        case XANTE_UI_DIALOG_DYNAMIC_MENU:
        case XANTE_UI_DIALOG_DELETE_DYNAMIC_MENU_ITEM:
        case XANTE_UI_DIALOG_ADD_DYNAMIC_MENU_ITEM:
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
 * Its mode must be different of XANTE_ACCESS_HIDDEN to be available.
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


/**
 * @name translate_string_menu_type
 * @brief Translates a string representing a menu type to its numeric value.
 *
 * @param [in] type: The menu type as a string.
 *
 * @return Returns the numeric value of the menu type.
 */
enum xante_ui_menu translate_string_menu_type(const char *type)
{
    enum xante_ui_menu menu = XANTE_UI_MENU_DEFAULT;

    if (NULL == type)
        return menu;

    if (strcmp(type, XANTE_UI_STR_DYNAMIC_MENU) == 0)
        menu = XANTE_UI_MENU_DYNAMIC;

    return menu;
}

/**
 * @name translate_string_dialog_type
 * @brief Translates a string representing a dialog type to its numeric value.
 *
 * @param [in] type: The dialog type as a string.
 *
 * @return Returns the numeric value of the dialog type.
 */
enum xante_ui_dialog translate_string_dialog_type(const char *type)
{
    enum xante_ui_dialog dialog = XANTE_UI_DIALOG_UNKNOWN;

    if (strcmp(type, XANTE_UI_STR_DIALOG_MENU) == 0)
        dialog = XANTE_UI_DIALOG_MENU;
    else if (strcmp(type, XANTE_UI_STR_DIALOG_INPUT_INT) == 0)
        dialog = XANTE_UI_DIALOG_INPUT_INT;
    else if (strcmp(type, XANTE_UI_STR_DIALOG_INPUT_FLOAT) == 0)
        dialog = XANTE_UI_DIALOG_INPUT_FLOAT;
    else if (strcmp(type, XANTE_UI_STR_DIALOG_INPUT_DATE) == 0)
        dialog = XANTE_UI_DIALOG_INPUT_DATE;
    else if (strcmp(type, XANTE_UI_STR_DIALOG_INPUT_STRING) == 0)
        dialog = XANTE_UI_DIALOG_INPUT_STRING;
    else if (strcmp(type, XANTE_UI_STR_DIALOG_INPUT_PASSWD) == 0)
        dialog = XANTE_UI_DIALOG_INPUT_PASSWD;
    else if (strcmp(type, XANTE_UI_STR_DIALOG_INPUT_TIME) == 0)
        dialog = XANTE_UI_DIALOG_INPUT_TIME;
    else if (strcmp(type, XANTE_UI_STR_DIALOG_CALENDAR) == 0)
        dialog = XANTE_UI_DIALOG_CALENDAR;
    else if (strcmp(type, XANTE_UI_STR_DIALOG_TIMEBOX) == 0)
        dialog = XANTE_UI_DIALOG_TIMEBOX;
    else if (strcmp(type, XANTE_UI_STR_DIALOG_RADIO_CHECKLIST) == 0)
        dialog = XANTE_UI_DIALOG_RADIO_CHECKLIST;
    else if (strcmp(type, XANTE_UI_STR_DIALOG_CHECKLIST) == 0)
        dialog = XANTE_UI_DIALOG_CHECKLIST;
    else if (strcmp(type, XANTE_UI_STR_DIALOG_YESNO) == 0)
        dialog = XANTE_UI_DIALOG_YES_NO;
    else if (strcmp(type, XANTE_UI_STR_DIALOG_DYNAMIC_MENU) == 0)
        dialog = XANTE_UI_DIALOG_DYNAMIC_MENU;
    else if (strcmp(type, XANTE_UI_STR_DIALOG_DELETE_DYNAMIC_MENU) == 0)
        dialog = XANTE_UI_DIALOG_DELETE_DYNAMIC_MENU_ITEM;
    else if (strcmp(type, XANTE_UI_STR_DIALOG_ADD_DYNAMIC_MENU) == 0)
        dialog = XANTE_UI_DIALOG_ADD_DYNAMIC_MENU_ITEM;

    return dialog;
}

/**
 * @name is_menu_item
 * @brief Checks if an item is of a menu kind.
 *
 * @param [in] type: The item type, in a cl_string_t format.
 *
 * @return Returns true is is a menu kind or false if not.
 */
bool is_menu_item(const cl_string_t *type)
{
    enum xante_ui_dialog dlg_type;

    dlg_type = translate_string_dialog_type(cl_string_valueof(type));

    switch (dlg_type) {
        case XANTE_UI_DIALOG_MENU:
        case XANTE_UI_DIALOG_DYNAMIC_MENU:
        case XANTE_UI_DIALOG_DELETE_DYNAMIC_MENU_ITEM:
            return true;

        default:
            break;
    }

    return false;
}

/**
 * @name is_input_item
 * @brief Checks if an item is of an input kind.
 *
 * @param [in] type: The item type.
 *
 * @return Returns true is is an input kind or false if not.
 */
bool is_input_item(enum xante_ui_dialog dlg_type)
{
    switch (dlg_type) {
        case XANTE_UI_DIALOG_INPUT_INT:
        case XANTE_UI_DIALOG_INPUT_FLOAT:
        case XANTE_UI_DIALOG_INPUT_STRING:
        case XANTE_UI_DIALOG_INPUT_PASSWD:
            return true;

        default:
            break;
    }

    return false;
}

/**
 * @name idigits
 * @brief Gets the number of digits of an integer number.
 *
 * @param [in] n: The integer number.
 *
 * @return Returns the number of digits.
 */
int idigits(int n)
{
    return floor(log10(abs(n))) + 1;
}

/**
 * @name equals
 * @brief Compares two strings.
 *
 * @param [in] a: The first string.
 * @param [in] b: The second string.
 *
 * @return Returns true if \a a and \a b are equals or false otherwise.
 */
bool equals(const char *a, const char *b)
{
    if (!strcmp(a, b))
        return true;

    return false;
}

/*
 *
 * API
 *
 */

/**
 * @name xante_application_version
 * @brief Gets a string with the application version information.
 *
 * @param [in] xpp: The library main object.
 *
 * @return On success returns a buffer with the application version or NULL
 *         otherwise.
 */
__PUB_API__ char *xante_application_version(xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;
    char *str_version = NULL;
    cl_string_t *version = NULL;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return NULL;
    }

    version = cl_string_create(cl_tr("%s - Version %s.%d Build %d %s"),
                               x->info.application_name,
                               x->info.version,
                               x->info.revision, x->info.build,
                               (x->info.beta == true) ? "BETA" : "");

    str_version = strdup(cl_string_valueof(version));
    cl_string_unref(version);

    return str_version;
}

