
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

#ifndef _LIBXANTE_XT_CONFIG_H
#define _LIBXANTE_XT_CONFIG_H          1

#ifndef LIBXANTE_COMPILE
# ifndef _LIBXANTE_H
#  error "Never use <xt_config.h> directly; include <libxante.h> instead."
# endif
#endif

/**
 * @name xante_load_config
 * @brief Loads the configuration file from the application.
 *
 * @param [in,out] xpp: The main library object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int xante_load_config(xante_t *xpp);

/**
 * @name xante_write_config
 * @brief Unloads and save the configuration file from the application.
 *
 * The saving is made when needed.
 *
 * @param [in,out] xpp: The main library object.
 * @param [in] ui_return_status: The UI return value.
 *
 * @return On sucess returns 0 or -1 otherwise.
 */
int xante_write_config(xante_t *xpp, int ui_return_status);

#endif

