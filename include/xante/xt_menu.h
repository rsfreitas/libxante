
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Wed May 31 10:35:06 2017
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

#ifndef _LIBXANTE_XT_MENU_H
#define _LIBXANTE_XT_MENU_H          1

#ifndef LIBXANTE_COMPILE
# ifndef _LIBXANTE_H
#  error "Never use <xt_menu.h> directly; include <libxante.h> instead."
# endif
#else

/* Internal library declarations */
void xante_menu_ref(struct xante_menu *menu);
void xante_menu_unref(struct xante_menu *menu);
void xante_menu_destroy(void *a);
struct xante_menu *xante_menu_create(enum xante_menu_creator creator);

#endif

/**
 * @name xante_menu_name
 * @brief Gives the current name of a menu object.
 *
 * @param [in] menu: The menu object.
 *
 * @return On success returns the menu name or NULL otherwise.
 */
const char *xante_menu_name(const xante_menu_t *menu);

/**
 * @name xante_menu_object_id
 * @brief Gives the current object_id property of a menu object.
 *
 * @param [in] menu: The menu object.
 *
 * @return On success returns the object identification or NULL otherwise.
 */
const char *xante_menu_object_id(const xante_menu_t *menu);

/**
 * @name xante_menu_type
 * @brief Gives the current menu type of a menu object.
 *
 * @param [in] menu: The menu object.
 *
 * @return On success return the menu type or -1 otherwise.
 */
enum xante_ui_menu xante_menu_type(const xante_menu_t *menu);

#endif

