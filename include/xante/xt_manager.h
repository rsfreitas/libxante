
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Tue Dec 19 08:53:16 2017
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

#ifndef _LIBXANTE_XT_MANAGER_H
#define _LIBXANTE_XT_MANAGER_H          1

#ifndef LIBXANTE_COMPILE
# ifndef _LIBXANTE_H
#  error "Never use <xt_manager.h> directly; include <libxante.h> instead."
# endif
#else

/* Internal library declarations */
int manager_run(struct xante_app *xpp, cl_list_t *menus,
                const struct xante_menu *menu, const char *cancel_label);

int manager_run_dialog(struct xante_app *xpp, cl_list_t *menus,
                       struct xante_item *selected_item);

#endif

#endif

