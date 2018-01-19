
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

#ifndef _LIBXANTE_XT_INTERNAL_AUTH_H
#define _LIBXANTE_XT_INTERNAL_AUTH_H

/* Internal library declarations */
int auth_init(struct xante_app *xpp, bool use_auth, enum xante_session session,
              const char *username, const char *password);

void auth_uninit(struct xante_app *xpp);
int auth_application_init(struct xante_app *xpp);
enum XanteAccessMode auth_get_access_level(const struct xante_app *xpp,
                                           const struct xante_item *item);

bool auth_check_item_access(const struct xante_app *xpp,
                            const struct xante_item *item);

#endif

