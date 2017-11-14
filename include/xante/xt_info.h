
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Tue Nov 14 10:45:16 2017
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

#ifndef _LIBXANTE_XT_INFO_H
#define _LIBXANTE_XT_INFO_H          1

#ifndef LIBXANTE_COMPILE
# ifndef _LIBXANTE_H
#  error "Never use <xt_info.h> directly; include <libxante.h> instead."
# endif
#endif

/**
 * @name
 * @brief
 *
 * @param [in] xpp: The library main object.
 *
 * @return
 */
const char *xante_config_path(const xante_t *xpp);

/**
 * @name
 * @brief
 *
 * @param [in] xpp: The library main object.
 *
 * @return
 */
const char *xante_log_path(const xante_t *xpp);

/**
 * @name
 * @brief
 *
 * @param [in] xpp: The library main object.
 *
 * @return
 */
const char *xante_log_level(const xante_t *xpp);

/**
 * @name
 * @brief
 *
 * @param [in] xpp: The library main object.
 *
 * @return
 */
const char *xante_application_name(const xante_t *xpp);

/**
 * @name
 * @brief
 *
 * @param [in] xpp: The library main object.
 *
 * @return
 */
const char *xante_plugin_name(const xante_t *xpp);

/**
 * @name
 * @brief
 *
 * @param [in] xpp: The library main object.
 *
 * @return
 */
const char *xante_version(const xante_t *xpp);

/**
 * @name
 * @brief
 *
 * @param [in] xpp: The library main object.
 *
 * @return
 */
const char *xante_company(const xante_t *xpp);

/**
 * @name
 * @brief
 *
 * @param [in] xpp: The library main object.
 *
 * @return
 */
const char *xante_description(const xante_t *xpp);

/**
 * @name
 * @brief
 *
 * @param [in] xpp: The library main object.
 *
 * @return
 */
int xante_revision(const xante_t *xpp);

/**
 * @name
 * @brief
 *
 * @param [in] xpp: The library main object.
 *
 * @return
 */
int xante_build(const xante_t *xpp);

/**
 * @name
 * @brief
 *
 * @param [in] xpp: The library main object.
 *
 * @return
 */
bool xante_beta(const xante_t *xpp);

/**
 * @name
 * @brief
 *
 * @param [in] xpp: The library main object.
 *
 * @return
 */
const char *xante_info(const xante_t *xpp);

#endif

