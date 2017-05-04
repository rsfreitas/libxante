
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

/*
 *
 * Internal functions
 *
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

    free(item);
}

static void destroy_xante_menu(const struct cl_ref_s *ref)
{
    struct xante_menu *menu = cl_container_of(ref, struct xante_menu, ref);

    if (NULL == menu)
        return;

    if (menu->name != NULL)
        free(menu->name);

    if (menu->object_id != NULL)
        free(menu->object_id);

    if (menu->items != NULL)
        cl_list_destroy(menu->items);

    free(menu);
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
 * @param [in] name: The menu name.
 * @param [in] object_id: The menu object_id.
 * @param [in] creator: An information to say who is creating the menu, it may
 *                      be created when loaded from a JTF file or created when
 *                      a dynamic menu is requested.
 *
 * @return On success returns a xante_menu structure or NULL otherwise.
 */
struct xante_menu *ui_new_xante_menu(const char *name, const char *object_id,
    enum xante_menu_creator creator)
{
    struct xante_menu *menu = NULL;

    menu = calloc(1, sizeof(struct xante_menu));

    if (NULL == menu) {
        errno_set(XANTE_ERROR_NO_MEMORY);
        return NULL;
    }

    menu->name = strdup(name);
    menu->object_id = strdup(object_id);
    menu->creator = creator;
    menu->move_to_menus_to_be_released = false;
    menu->items = cl_list_create(__destroy_xante_item, NULL, NULL, NULL);

    /* Initialize reference count */
    menu->ref.count = 1;
    menu->ref.free = destroy_xante_menu;

    return menu;
}

/*
 *
 * API
 *
 */

__PUB_API__ void xante_ui_run(void)
{
}

__PUB_API__ void xante_ui_suspend(void)
{
}

__PUB_API__ void xante_ui_continue(void)
{
}

