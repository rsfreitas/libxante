
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

#ifndef _LIBXANTE_API_MANAGER_H
#define _LIBXANTE_API_MANAGER_H

#ifndef LIBXANTE_COMPILE
# ifndef _LIBXANTE_H
#  error "Never use <manager.h> directly; include <libxante.h> instead."
# endif
#endif

/**
 * @name xante_manager_run
 * @brief Puts a libxante application to run.
 *
 * It is important to remember that once we call this function we only leave it
 * when closing the application from the UI.
 *
 * @param [in,out] xpp: The library main object.
 *
 * @return Return an exit value indicating what happened inside (see enum
 *         xante_return_value declaration).
 */
enum xante_return_value xante_manager_run(xante_t *xpp);

/**
 * @name xante_manager_single_run
 * @brief Puts a JTS object to run.
 *
 * This function may be called inside a module event function and also, this
 * event may run with an already running application or not. This way this
 * function is in charge to know this situation and does all required
 * initialization to run the object.
 *
 * @param [in,out] xpp: The library main object.
 * @param [in] jts: The JTS object in a string format.
 *
 * @return Return an exit value indicating what happened inside (see enum
 *         xante_return_value declaration).
 */
enum xante_return_value xante_manager_single_run(xante_t *xpp, const char *jts);

#endif

