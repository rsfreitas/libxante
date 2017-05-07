
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

#ifndef _LIBXANTE_XT_UTILS_H
#define _LIBXANTE_XT_UTILS_H          1

#ifndef LIBXANTE_COMPILE
# ifndef _LIBXANTE_H
#  error "Never use <xt_utils.h> directly; include <libxante.h> instead."
# endif
#else

/* Internal library declarations */
bool is_valid_config_file_status(enum xante_config_file_status status);
bool is_valid_ui_dialog(enum xante_ui_dialog type);
bool is_item_available(struct xante_item *item);
enum xante_ui_dialog translate_string_dialog_type(const char *type);

#endif

#endif

