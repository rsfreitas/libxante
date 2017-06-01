
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

#ifndef _LIBXANTE_XT_AUTH_H
#define _LIBXANTE_XT_AUTH_H          1

#ifndef LIBXANTE_COMPILE
# ifndef _LIBXANTE_H
#  error "Never use <xt_auth.h> directly; include <libxante.h> instead."
# endif
#else

/* Internal library declarations */
int auth_init(struct xante_app *xpp, bool use_auth, enum xante_session session,
              const char *username, const char *password);

void auth_uninit(struct xante_app *xpp);
int auth_application_init(struct xante_app *xpp);
enum xante_access_mode auth_get_access_level(const struct xante_app *xpp,
                                              const struct xante_item *item);

bool auth_check_item_access(const struct xante_app *xpp,
                            const struct xante_item *item);

#endif

/**
 * @name xante_auth_create_database
 * @brief Creates an empty default database to applications.
 *
 * The database file name is always the same, auth.xdb.
 *
 * @param [in] pathname: The path to save the database file.
 * @param [in] overwrite: A boolean flag to overwrite or not if the database
 *                        already exists.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int xante_auth_create_database(const char *pathname, bool overwrite);

/**
 * @name xante_auth_export_jxdbi
 * @brief Exports an intermediate file to populate databases.
 *
 * A JXDBI file has information about the UI from an application and is used
 * to populate a database with this info.
 *
 * @param [in] xpp: The library main object.
 * @param [in] filename: The output file name.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int xante_auth_export_jxdbi(xante_t *xpp, const char *filename);

#endif

