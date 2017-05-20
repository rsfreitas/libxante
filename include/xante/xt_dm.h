
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Thu May 18 00:13:49 2017
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

#ifndef _LIBXANTE_XT_DM_H
#define _LIBXANTE_XT_DM_H          1

#ifndef LIBXANTE_COMPILE
# ifndef _LIBXANTE_H
#  error "Never use <xt_dm.h> directly; include <libxante.h> instead."
# endif
#else

/* Internal library declarations */
void dm_init(struct xante_app *xpp, cl_cfg_file_t *cfg_file);
void dm_update(struct xante_app *xpp, struct xante_item *selected_item);

#endif

#endif

