
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

#ifndef _LIBXANTE_XT_INIT_H
#define _LIBXANTE_XT_INIT_H          1

#ifndef LIBXANTE_COMPILE
# ifndef _LIBXANTE_H
#  error "Never use <xt_init.h> directly; include <libxante.h> instead."
# endif
#endif

/**
 * @name xante_init
 * @brief Initialize a libxante application.
 *
 * @param [in] jtf_pathname: The JTF file.
 * @param [in] flags: Library initialization flags.
 * @param [in] session: The session type.
 * @param [in] username: The username to access the application.
 * @param [in] password: The username's password.
 *
 * @return On success returns a xante_t object or NULL otherwise.
 */
xante_t *xante_init(const char *jtf_pathname, enum xante_init_flags flags,
                    enum xante_session session, const char *username,
                    const char *password);

/**
 * @name xante_uninit
 * @brief Ends a libxante application.
 *
 * @param [in,out] xpp: The library main object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int xante_uninit(xante_t *xpp);

#endif

