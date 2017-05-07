
/*
 * Description: Functions to handle User Interface.
 *
 * Author: Rodrigo Freitas
 * Created at: Tue May  2 21:58:11 2017
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

#include <stdlib.h>
#include <string.h>

#include "libxante.h"
#include "ui/ui_dialogs.h"

/*
 *
 * Internal functions
 *
 */

/*
 * The purpose of this function is to be used inside the cl_list_ API
 */
static void __destroy_xante_item(void *a)
{
    struct xante_item *item = (struct xante_item *)a;

    if (NULL == item)
        return;

    ui_xante_item_unref(item);
}

static void destroy_xante_item(const struct cl_ref_s *ref)
{
    struct xante_item *item = cl_container_of(ref, struct xante_item, ref);

    if (NULL == item)
        return;

    if (item->name != NULL)
        cl_string_unref(item->name);

    if (item->object_id != NULL)
        cl_string_unref(item->object_id);

    if (item->config_block != NULL)
        cl_string_unref(item->config_block);

    if (item->config_item != NULL)
        cl_string_unref(item->config_item);

    if (item->help != NULL)
        cl_string_unref(item->help);

    if (item->options != NULL)
        cl_string_unref(item->options);

    if (item->value != NULL)
        cl_object_unref(item->value);

    if (item->default_value != NULL)
        cl_object_unref(item->default_value);

    free(item);
}

static void destroy_xante_menu(const struct cl_ref_s *ref)
{
    struct xante_menu *menu = cl_container_of(ref, struct xante_menu, ref);

    if (NULL == menu)
        return;

    if (menu->name != NULL)
        cl_string_unref(menu->name);

    if (menu->object_id != NULL)
        cl_string_unref(menu->object_id);

    if (menu->items != NULL)
        cl_list_destroy(menu->items);

    free(menu);
}

/*
 * The purpose of this function is to be used inside the cl_list_ API
 */
static void __destroy_xante_menu(void *a)
{
    struct xante_menu *menu = (struct xante_menu *)a;

    if (NULL == menu)
        return;

    ui_xante_menu_unref(menu);
}

static int search_menu_by_object_id(cl_list_node_t *node, void *a)
{
    struct xante_menu *menu = cl_list_node_content(node);
    char *object_id = (char *)a;

    if (strcmp(cl_string_valueof(menu->object_id), object_id) == 0)
        return 1;

    return 0;
}

/*
 *
 * Internal API
 *
 */

/**
 * @name ui_xante_item_ref
 * @brief Increases the reference count for a xante_item object.
 *
 * @param [in,out] item: The xante_item item.
 */
void ui_xante_item_ref(struct xante_item *item)
{
    if (NULL == item)
        return;

    cl_ref_inc(&item->ref);
}

/**
 * @name ui_xante_item_unref
 * @brief Decreases the reference count for a xante_item object.
 *
 * When its reference count drops to 0, the item is finalized (its memory
 * is freed).
 *
 * @param [in,out] item: The xante_item item.
 */
void ui_xante_item_unref(struct xante_item *item)
{
    if (NULL == item)
        return;

    cl_ref_dec(&item->ref);
}

/**
 * @name ui_new_xante_item
 * @brief Creates a new xante_item to hold informations about a menu item.
 *
 * To release an object of this type, one may call the 'ui_xante_item_unref'.
 *
 * @return On success returns a xante_item structure or NULL otherwise.
 */
struct xante_item *ui_new_xante_item(void)
{
    struct xante_item *item = NULL;

    item = calloc(1, sizeof(struct xante_item));

    if (NULL == item) {
        errno_set(XANTE_ERROR_NO_MEMORY);
        return NULL;
    }

    /* Initialize reference count */
    item->ref.count = 1;
    item->ref.free = destroy_xante_item;

    return item;
}

/**
 * @name ui_xante_menu_ref
 * @brief Increases the reference count for a xante_menu object.
 *
 * @param [in,out] menu: The xante_menu item.
 */
void ui_xante_menu_ref(struct xante_menu *menu)
{
    if (NULL == menu)
        return;

    cl_ref_inc(&menu->ref);
}

/**
 * @name ui_xante_menu_unref
 * @brief Decreases the reference count for a xante_menu object.
 *
 * When its reference count drops to 0, the item is finalized (its memory
 * is freed).
 *
 * @param [in,out] menu: The xante_menu item.
 */
void ui_xante_menu_unref(struct xante_menu *menu)
{
    if (NULL == menu)
        return;

    cl_ref_dec(&menu->ref);
}

/**
 * @name ui_new_xante_menu
 * @brief Creates a new xante_menu to hold items and informations about an UI
 *        menu.
 *
 * This function creates a new xante_menu structure, holding some informations,
 * such as its name, its object_id and who is creating the menu.
 *
 * To release an object of this type, one may call the 'ui_xante_menu_unref'.
 *
 * @param [in] creator: An information to say who is creating the menu, it may
 *                      be created when loaded from a JTF file or created when
 *                      a dynamic menu is requested.
 *
 * @return On success returns a xante_menu structure or NULL otherwise.
 */
struct xante_menu *ui_new_xante_menu(enum xante_menu_creator creator)
{
    struct xante_menu *menu = NULL;

    menu = calloc(1, sizeof(struct xante_menu));

    if (NULL == menu) {
        errno_set(XANTE_ERROR_NO_MEMORY);
        return NULL;
    }

    menu->creator = creator;
    menu->move_to_be_released = false;
    menu->items = cl_list_create(__destroy_xante_item, NULL, NULL, NULL);

    /* Initialize reference count */
    menu->ref.count = 1;
    menu->ref.free = destroy_xante_menu;

    return menu;
}

/**
 * @name ui_init
 * @brief Initializes everything related to UI informations.
 *
 * @param [in,out] xpp: The previously created xante_app structure.
 */
void ui_init(struct xante_app *xpp)
{
    if (NULL == xpp)
        return;

    xpp->ui.menus = cl_list_create(__destroy_xante_menu, NULL, NULL, NULL);
}

/**
 * @name ui_uninit
 * @brief Releases all informations related to the UI.
 *
 * @param [in,out] xpp: The previously created xante_app structure.
 */
void ui_uninit(struct xante_app *xpp)
{
    if (NULL == xpp)
        return;

    if (xpp->ui.main_menu_object_id != NULL)
        cl_string_unref(xpp->ui.main_menu_object_id);

    if (xpp->ui.menus != NULL)
        cl_list_destroy(xpp->ui.menus);
}

/**
 * @name ui_adjusts_item_info
 * @brief Do some adjustments inside an item after its informations is
 *        completely loaded from the JTF file.
 *
 * For example, here we must split a checklist options into a cl_string_list_t
 * object.
 *
 * @param [in,out] item: The item to be adjusted.
 */
void ui_adjusts_item_info(struct xante_item *item, cl_string_t *default_value)
{
    item->dialog_type =
        translate_string_dialog_type(cl_string_valueof(item->type));

    if (default_value != NULL)
        item->default_value = cl_object_from_cstring(default_value);

    switch (item->dialog_type) {
        case XANTE_UI_DIALOG_RADIO_CHECKLIST:
        case XANTE_UI_DIALOG_CHECKLIST:
            if (item->options != NULL)
                item->checklist_options = cl_string_split(item->options, "|");

            break;

        default:
            break;
    }
}

/**
 * @name ui_search_menu_by_object_id
 * @brief Searches a xante_menu structure inside the main menu list which have
 *        a specific object_id.
 *
 * @param [in] xpp: The main library object.
 * @param [in] object_it_to_search: The menu object_id which will be used to
 *                                  search.
 *
 * @return On success, i.e, the menu is found, returns a pointer to its
 *         xante_menu structure or NULL otherwise.
 */
struct xante_menu *ui_search_menu_by_object_id(const struct xante_app *xpp,
    const char *object_id_to_search)
{
    cl_list_node_t *node = NULL;
    struct xante_menu *menu = NULL;

    node = cl_list_map(xpp->ui.menus, search_menu_by_object_id,
                       (void *)object_id_to_search);

    if (NULL == node)
        return NULL;

    menu = cl_list_node_content(node);
    cl_list_node_unref(node);

    return menu;
}

/*
 *
 * API
 *
 */

/*
 * XXX: Remember once we call this function we only leave it when closing the
 *      application from the UI.
 */
__PUB_API__ int xante_ui_run(xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;
    struct xante_menu *root = NULL;
    char *btn_cancel_label = NULL;
    cl_list_node_t *root_node = NULL;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    xante_runtime_set_ui_active(xpp, true);
    dialog_init(false);
    xante_ui_set_backtitle(xpp);
    /* TODO: Rename HELP button */
    btn_cancel_label = strdup(cl_tr(MAIN_MENU_CANCEL_LABEL));
    root_node = cl_list_peek(x->ui.menus);

    if (NULL == root_node) {
        goto end_block;
    }

    root = cl_list_node_content(root_node);
    ui_dialog_menu(xpp, root, btn_cancel_label, false, NULL);
    cl_list_node_unref(root_node);

end_block:
    if (btn_cancel_label != NULL)
        free(btn_cancel_label);

    xante_ui_clear_backtitle(xpp);
    dialog_uninit();
    xante_runtime_set_ui_active(xpp, false);

    return 0;
}

__PUB_API__ void xante_ui_suspend(void)
{
}

__PUB_API__ void xante_ui_continue(void)
{
}

