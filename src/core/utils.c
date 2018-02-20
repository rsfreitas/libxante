
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
 * Internal functions
 *
 */

static cl_string_t *extract_object_type(const cl_string_t *type)
{
    cl_stringlist_t *l = NULL;
    cl_string_t *prefix = NULL, *object = NULL;

    l = cl_string_split(type, ":");

    if ((NULL == l) || (cl_stringlist_size(l) != 2))
        goto end_block;

    prefix = cl_stringlist_get(l, 0);

    if (strcmp(cl_string_valueof(prefix), "gadget") != 0)
        goto end_block;

    object = cl_stringlist_get(l, 1);

end_block:
    cl_stringlist_destroy(l);
    cl_string_unref(prefix);

    if (object != NULL) {
        errno_set(XANTE_ERROR_UNKNOWN_OBJECT_PREFIX);
        errno_store_additional_content(cl_string_valueof(type));
    }

    return object;
}

/*
 *
 * Internal API
 *
 */

/**
 * @name is_valid_ui_object
 * @brief Checks if a given widget type is valid inside the library
 *        environment.
 *
 * @param [in] type: The widget type to be checked.
 *
 * @return Returns true if is a valid widget type or false otherwise.
 */
bool is_valid_ui_object(enum xante_object type)
{
    switch (type) {
        case XANTE_WIDGET_MENU_REFERENCE:
        case XANTE_WIDGET_INPUT_INT:
        case XANTE_WIDGET_INPUT_FLOAT:
        case XANTE_WIDGET_INPUT_DATE:
        case XANTE_WIDGET_INPUT_STRING:
        case XANTE_WIDGET_INPUT_PASSWD:
        case XANTE_WIDGET_INPUT_TIME:
        case XANTE_WIDGET_CALENDAR:
        case XANTE_WIDGET_TIMEBOX:
        case XANTE_WIDGET_CHECKLIST:
        case XANTE_WIDGET_RADIO_CHECKLIST:
        case XANTE_WIDGET_YES_NO:
        case XANTE_WIDGET_DYNAMIC_MENU_REFERENCE:
        case XANTE_WIDGET_DELETE_DYNAMIC_MENU_ITEM:
        case XANTE_WIDGET_ADD_DYNAMIC_MENU_ITEM:
        case XANTE_WIDGET_CUSTOM:
        case XANTE_WIDGET_PROGRESS:
        case XANTE_WIDGET_SPINNER_SYNC:
        case XANTE_WIDGET_DOTS_SYNC:
        case XANTE_WIDGET_RANGE:
        case XANTE_WIDGET_FILE_SELECT:
        case XANTE_WIDGET_DIR_SELECT:
        case XANTE_WIDGET_FILE_VIEW:
        case XANTE_WIDGET_TAILBOX:
        case XANTE_WIDGET_SCROLLTEXT:
        case XANTE_WIDGET_UPDATE_OBJECT:
        case XANTE_WIDGET_INPUTSCROLL:
        case XANTE_WIDGET_MIXEDFORM:
        case XANTE_WIDGET_BUILDLIST:
        case XANTE_WIDGET_SPREADSHEET:
        case XANTE_GADGET_CLOCK:
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
    if (item->mode == XanteAccessHidden)
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
enum xante_menu_type translate_string_menu_type(const char *type)
{
    enum xante_menu_type menu = XANTE_MENU_DEFAULT;

    if (NULL == type)
        return menu;

    if (strcmp(type, XANTE_STR_DYNAMIC_MENU) == 0)
        menu = XANTE_MENU_DYNAMIC;

    return menu;
}

/**
 * @name translate_string_widget_type
 * @brief Translates a string representing a widget type to its numeric value.
 *
 * @param [in] type: The widget type as a string.
 *
 * @return Returns the numeric value of the widget type.
 */
enum xante_object translate_string_widget_type(const cl_string_t *type)
{
    enum xante_object widget = XANTE_WIDGET_UNKNOWN;
    cl_string_t *object = NULL;
    char *ptype = NULL;

    /* Are we a gadget? */
    if (cl_string_find(type, ':') >= 0) {
        object = extract_object_type(type);

        if (NULL == object)
            return widget;

        ptype = (char *)cl_string_valueof(object);

        if (strcmp(ptype, XANTE_STR_GADGET_CLOCK) == 0)
            widget = XANTE_GADGET_CLOCK;

        cl_string_unref(object);
        return widget;
    }

    ptype = (char *)cl_string_valueof(type);

    if (strcmp(ptype, XANTE_STR_WIDGET_MENU) == 0)
        widget = XANTE_WIDGET_MENU_REFERENCE;
    else if (strcmp(ptype, XANTE_STR_WIDGET_INPUT_INT) == 0)
        widget = XANTE_WIDGET_INPUT_INT;
    else if (strcmp(ptype, XANTE_STR_WIDGET_INPUT_FLOAT) == 0)
        widget = XANTE_WIDGET_INPUT_FLOAT;
    else if (strcmp(ptype, XANTE_STR_WIDGET_INPUT_DATE) == 0)
        widget = XANTE_WIDGET_INPUT_DATE;
    else if (strcmp(ptype, XANTE_STR_WIDGET_INPUT_STRING) == 0)
        widget = XANTE_WIDGET_INPUT_STRING;
    else if (strcmp(ptype, XANTE_STR_WIDGET_INPUT_PASSWD) == 0)
        widget = XANTE_WIDGET_INPUT_PASSWD;
    else if (strcmp(ptype, XANTE_STR_WIDGET_INPUT_TIME) == 0)
        widget = XANTE_WIDGET_INPUT_TIME;
    else if (strcmp(ptype, XANTE_STR_WIDGET_CALENDAR) == 0)
        widget = XANTE_WIDGET_CALENDAR;
    else if (strcmp(ptype, XANTE_STR_WIDGET_TIMEBOX) == 0)
        widget = XANTE_WIDGET_TIMEBOX;
    else if (strcmp(ptype, XANTE_STR_WIDGET_RADIO_CHECKLIST) == 0)
        widget = XANTE_WIDGET_RADIO_CHECKLIST;
    else if (strcmp(ptype, XANTE_STR_WIDGET_CHECKLIST) == 0)
        widget = XANTE_WIDGET_CHECKLIST;
    else if (strcmp(ptype, XANTE_STR_WIDGET_YESNO) == 0)
        widget = XANTE_WIDGET_YES_NO;
    else if (strcmp(ptype, XANTE_STR_WIDGET_DYNAMIC_MENU) == 0)
        widget = XANTE_WIDGET_DYNAMIC_MENU_REFERENCE;
    else if (strcmp(ptype, XANTE_STR_WIDGET_DELETE_DYNAMIC_MENU) == 0)
        widget = XANTE_WIDGET_DELETE_DYNAMIC_MENU_ITEM;
    else if (strcmp(ptype, XANTE_STR_WIDGET_ADD_DYNAMIC_MENU) == 0)
        widget = XANTE_WIDGET_ADD_DYNAMIC_MENU_ITEM;
    else if (strcmp(ptype, XANTE_STR_WIDGET_CUSTOM) == 0)
        widget = XANTE_WIDGET_CUSTOM;
    else if (strcmp(ptype, XANTE_STR_WIDGET_PROGRESS) == 0)
        widget = XANTE_WIDGET_PROGRESS;
    else if (strcmp(ptype, XANTE_STR_WIDGET_SPINNER_SYNC) == 0)
        widget = XANTE_WIDGET_SPINNER_SYNC;
    else if (strcmp(ptype, XANTE_STR_WIDGET_DOTS_SYNC) == 0)
        widget = XANTE_WIDGET_DOTS_SYNC;
    else if (strcmp(ptype, XANTE_STR_WIDGET_RANGE) == 0)
        widget = XANTE_WIDGET_RANGE;
    else if (strcmp(ptype, XANTE_STR_WIDGET_FILE_SELECT) == 0)
        widget = XANTE_WIDGET_FILE_SELECT;
    else if (strcmp(ptype, XANTE_STR_WIDGET_DIR_SELECT) == 0)
        widget = XANTE_WIDGET_DIR_SELECT;
    else if (strcmp(ptype, XANTE_STR_WIDGET_FILE_VIEW) == 0)
        widget = XANTE_WIDGET_FILE_VIEW;
    else if (strcmp(ptype, XANTE_STR_WIDGET_TAILBOX) == 0)
        widget = XANTE_WIDGET_TAILBOX;
    else if (strcmp(ptype, XANTE_STR_WIDGET_SCROLLTEXT) == 0)
        widget = XANTE_WIDGET_SCROLLTEXT;
    else if (strcmp(ptype, XANTE_STR_WIDGET_UPDATE_OBJECT) == 0)
        widget = XANTE_WIDGET_UPDATE_OBJECT;
    else if (strcmp(ptype, XANTE_STR_WIDGET_INPUTSCROLL) == 0)
        widget = XANTE_WIDGET_INPUTSCROLL;
    else if (strcmp(ptype, XANTE_STR_WIDGET_MIXEDFORM) == 0)
        widget = XANTE_WIDGET_MIXEDFORM;
    else if (strcmp(ptype, XANTE_STR_WIDGET_BUILDLIST) == 0)
        widget = XANTE_WIDGET_BUILDLIST;
    else if (strcmp(ptype, XANTE_STR_WIDGET_SPREADSHEET) == 0)
        widget = XANTE_WIDGET_SPREADSHEET;

    return widget;
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
    enum xante_object dlg_type;

    dlg_type = translate_string_widget_type(type);

    switch (dlg_type) {
        case XANTE_WIDGET_MENU_REFERENCE:
        case XANTE_WIDGET_DYNAMIC_MENU_REFERENCE:
        case XANTE_WIDGET_DELETE_DYNAMIC_MENU_ITEM:
            return true;

        default:
            break;
    }

    return false;
}

/**
 * @name item_has_ranges
 * @brief Checks if an item is of an input kind.
 *
 * @param [in] type: The item type.
 *
 * @return Returns true is is an input kind or false if not.
 */
bool item_has_ranges(enum xante_object dlg_type)
{
    switch (dlg_type) {
        case XANTE_WIDGET_INPUT_INT:
        case XANTE_WIDGET_INPUT_FLOAT:
        case XANTE_WIDGET_INPUT_STRING:
        case XANTE_WIDGET_INPUT_PASSWD:
        case XANTE_WIDGET_PROGRESS:
        case XANTE_WIDGET_SPINNER_SYNC:
        case XANTE_WIDGET_DOTS_SYNC:
        case XANTE_WIDGET_RANGE:
        case XANTE_WIDGET_INPUTSCROLL:
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
 * @name file_exists
 * @brief Checks if a specific file exists.
 *
 * @param [in] pathname: The file path.
 *
 * @return Returns true or false if file exists or not.
 */
bool file_exists(const char *pathname)
{
    if (access(pathname, 0x00) == -1)
        return false;

    return true;
}

/**
 * @name is_gadget
 * @brief Checks if an object is of a gadget type.
 *
 * @param [in] type: The object type.
 *
 * @return Returns true or false if the object is a gadget or not.
 */
bool is_gadget(enum xante_object type)
{
    switch (type) {
        case XANTE_GADGET_CLOCK:
            return true;

        default:
            break;
    }

    return false;
}

/*
 *
 * API
 *
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

