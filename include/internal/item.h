
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

#ifndef _LIBXANTE_XT_INTERNAL_ITEM_H
#define _LIBXANTE_XT_INTERNAL_ITEM_H

/* Internal library declarations */
void xante_item_destroy(void *a);
void xante_item_ref(struct xante_item *item);
void xante_item_unref(struct xante_item *item);
struct xante_item *xante_item_create(void);

#endif

