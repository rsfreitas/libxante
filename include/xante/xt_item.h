
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sat May 27 20:36:27 2017
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

#ifndef _LIBXANTE_XT_ITEM_H
#define _LIBXANTE_XT_ITEM_H          1

#ifndef LIBXANTE_COMPILE
# ifndef _LIBXANTE_H
#  error "Never use <xt_item.h> directly; include <libxante.h> instead."
# endif
#else

/* Internal library declarations */
void xante_item_destroy(void *a);
void xante_item_ref(struct xante_item *item);
void xante_item_unref(struct xante_item *item);
struct xante_item *xante_item_create(void);

#endif

/**
 * @name xante_item_name
 * @brief Gives the current name of an item object.
 *
 * @param [in] item: The item object.
 *
 * @return On success returns the item name or NULL otherwise.
 */
const char *xante_item_name(const xante_item_t *item);

/**
 * @name xante_item_object_id
 * @brief Gives the current object identification of an item object.
 *
 * @param [in] item: The item object.
 *
 * @return On success returns the object identification or NULL otherwise.
 */
const char *xante_item_object_id(const xante_item_t *item);

/**
 * @name xante_item_access_mode
 * @brief Gives the current access mode of an item object.
 *
 * @param [in] item: The item object.
 *
 * @return On success returns the access mode or -1 otherwise.
 */
enum xante_access_mode xante_item_access_mode(const xante_item_t *item);

/**
 * @name xante_item_default_value
 * @brief Gives the default value of an item object.
 *
 * @param [in] item: The item object.
 *
 * @return On success returns a reference to the default value of the item or
 *         NULL otherwise.
 */
cl_object_t *xante_item_default_value(const xante_item_t *item);

/**
 * @name xante_item_value
 * @brief Gives the current value of an item object.
 *
 * @param [in] item: The item object.
 *
 * @return On success returns a reference to the current item's value or NULL
 *         otherwise. If the item has no value it will also return NULL.
 */
cl_object_t *xante_item_value(const xante_item_t *item);

/**
 * @name xante_item_dialog_type
 * @brief Gives the dialog type of an item object.
 *
 * @param [in] item: The item object.
 *
 * @return On success returns the item dialog type or -1 otherwise.
 */
enum xante_ui_dialog xante_item_dialog_type(const xante_item_t *item);

/**
 * @name xante_item_checklist_type
 * @brief Gives the checklist type of an item object.
 *
 * @param [in] item: The item object.
 *
 * @return On success returns the item checklist type or -1 otherwise.
 */
int xante_item_checklist_type(const xante_item_t *item);

/**
 * @name xante_item_update_value
 * @brief Updates the current value of an item object.
 *
 * @param [in] item: The item object.
 * @param [in] fmt: The new value format, as string.
 * @param [in] ...: The new value values.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int xante_item_update_value(xante_item_t *item, const char *fmt, ...)
                            __attribute__((format(printf, 2, 3)));

#endif

