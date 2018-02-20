
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

#ifndef _LIBXANTE_INTERNAL_UTILS_H
#define _LIBXANTE_INTERNAL_UTILS_H

/* Internal library declarations */
bool is_valid_ui_object(enum xante_object type);
bool is_item_available(struct xante_item *item);
enum xante_menu_type translate_string_menu_type(const char *type);
enum xante_object translate_string_widget_type(const cl_string_t *type);
bool item_has_ranges(enum xante_object dlg_type);
bool is_menu_item(const cl_string_t *type);
int idigits(int n);
bool file_exists(const char *pathname);
bool is_gadget(enum xante_object type);

#endif

