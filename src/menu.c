
/*
 * Description: API to manipulate xante_menu structures from outside the
 *              library.
 *
 * Author: Rodrigo Freitas
 * Created at: Wed May 31 10:34:15 2017
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

static void __xante_menu_destroy(const struct cl_ref_s *ref)
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

    if (menu->type != NULL)
        cl_string_unref(menu->type);

    if (menu->dynamic_names != NULL)
        cl_stringlist_destroy(menu->dynamic_names);

    if (menu->dynamic_block_prefix != NULL)
        cl_string_unref(menu->dynamic_block_prefix);

    if (menu->dynamic_origin_block != NULL)
        cl_string_unref(menu->dynamic_origin_block);

    if (menu->dynamic_origin_item != NULL)
        cl_string_unref(menu->dynamic_origin_item);

    if (menu->events != NULL)
        cl_json_delete(menu->events);

    free(menu);
}

/*
 *
 * Internal API
 *
 */

/**
 * @name xante_menu_destroy
 * @brief Releases a xante_menu structure from memory.
 *
 * The purpose of this function is to be used inside the cl_list_ API.
 *
 * @param [in,out] a: The object to be released.
 */
void xante_menu_destroy(void *a)
{
    struct xante_menu *menu = (struct xante_menu *)a;

    if (NULL == menu)
        return;

    xante_menu_unref(menu);
}

/**
 * @name xante_menu_ref
 * @brief Increases the reference count for a xante_menu object.
 *
 * @param [in,out] menu: The xante_menu item.
 */
void xante_menu_ref(struct xante_menu *menu)
{
    if (NULL == menu)
        return;

    cl_ref_inc(&menu->ref);
}

/**
 * @name xante_menu_unref
 * @brief Decreases the reference count for a xante_menu object.
 *
 * When its reference count drops to 0, the item is finalized (its memory
 * is freed).
 *
 * @param [in,out] menu: The xante_menu item.
 */
void xante_menu_unref(struct xante_menu *menu)
{
    if (NULL == menu)
        return;

    cl_ref_dec(&menu->ref);
}

/**
 * @name xante_menu_create
 * @brief Creates a new xante_menu to hold items and informations about an UI
 *        menu.
 *
 * This function creates a new xante_menu structure, holding some informations,
 * such as its name, its object_id and who is creating the menu.
 *
 * To release an object of this type, one may call the 'xante_menu_unref'.
 *
 * @param [in] creator: An information to say who is creating the menu, it may
 *                      be created when loaded from a JTF file or created when
 *                      a dynamic menu is requested.
 *
 * @return On success returns a xante_menu structure or NULL otherwise.
 */
struct xante_menu *xante_menu_create(enum xante_menu_creator creator)
{
    struct xante_menu *menu = NULL;

    menu = calloc(1, sizeof(struct xante_menu));

    if (NULL == menu) {
        errno_set(XANTE_ERROR_NO_MEMORY);
        return NULL;
    }

    menu->creator = creator;
    menu->move_to_be_released = false;
    menu->copies = -1;
    menu->items = cl_list_create(xante_item_destroy, NULL, NULL, NULL);

    /* Initialize reference count */
    menu->ref.count = 1;
    menu->ref.free = __xante_menu_destroy;

    return menu;
}

/**
 * @name xante_menu_head
 * @brief Gives a reference to the head of a menu list.
 *
 * @param [in] menus: The list of menus.
 *
 * @return On success returns a reference the head of the list or NULL
 *         otherwise.
 */
struct xante_menu *xante_menu_head(const cl_list_t *menus)
{
    cl_list_node_t *node = NULL;
    struct xante_menu *head = NULL;

    if (NULL == menus) {
        return NULL;
    }

    node = cl_list_peek(menus);

    if (NULL == node) {
        errno_set(XANTE_ERROR_MENU_HEAD_NOT_FOUND);
        return NULL;
    }

    head = cl_list_node_content(node);
    xante_menu_ref(head);
    cl_list_node_unref(node);

    return head;
}

/*
 *
 * API
 *
 */

__PUB_API__ const char *xante_menu_name(const xante_menu_t *menu)
{
    struct xante_menu *m = (struct xante_menu *)menu;

    errno_clear();

    if (NULL == menu) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return NULL;
    }

    return cl_string_valueof(m->name);
}

__PUB_API__ const char *xante_menu_object_id(const xante_menu_t *menu)
{
    struct xante_menu *m = (struct xante_menu *)menu;

    errno_clear();

    if (NULL == menu) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return NULL;
    }

    return cl_string_valueof(m->object_id);
}

__PUB_API__ enum xante_ui_menu xante_menu_type(const xante_menu_t *menu)
{
    struct xante_menu *m = (struct xante_menu *)menu;

    errno_clear();

    if (NULL == menu) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    return m->menu_type;
}

