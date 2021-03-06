
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Tue May  2 21:07:38 2017
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

#ifndef _LIBXANTE_INTERNAL_JTF_H
#define _LIBXANTE_INTERNAL_JTF_H

/* Internal library declarations */
struct xante_item *jtf_parse_item(const cl_json_t *item, bool single_instance);
struct xante_menu *jtf_parse_menu(const cl_json_t *menu);
int jtf_parse_application_info(const char *pathname, struct xante_app *xpp);
int jtf_parse_application(const char *pathname, struct xante_app *xpp);
void jtf_release_info(struct xante_app *xpp);

#endif

