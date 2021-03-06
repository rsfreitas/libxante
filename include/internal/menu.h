
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

#ifndef _LIBXANTE_INTERNAL_MENU_H
#define _LIBXANTE_INTERNAL_MENU_H

/* Internal library declarations */
void xante_menu_ref(struct xante_menu *menu);
void xante_menu_unref(struct xante_menu *menu);
void xante_menu_destroy(void *a);
struct xante_menu *xante_menu_create(enum xante_menu_creator creator);
struct xante_menu *xante_menu_head(const cl_list_t *menus);
struct xante_menu *xante_menu_search_by_name(const cl_list_t *menus,
                                             const char *menu_name);

struct xante_menu *xante_menu_search_by_object_id(const cl_list_t *menus,
                                                  const char *object_id);

#endif

