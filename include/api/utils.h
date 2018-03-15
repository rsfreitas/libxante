
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

#ifndef _LIBXANTE_API_UTILS_H
#define _LIBXANTE_API_UTILS_H

#ifndef LIBXANTE_COMPILE
# ifndef _LIBXANTE_H
#  error "Never use <utils.h> directly; include <libxante.h> instead."
# endif
#endif

/**
 * @name xante_application_version
 * @brief Gets a string with the application version information.
 *
 * @param [in] xpp: The library main object.
 *
 * @return On success returns a buffer with the application version or NULL
 *         otherwise.
 */
char *xante_application_version(xante_t *xpp);

#endif

