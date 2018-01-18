
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
#define _LIBXANTE_XT_INFO_H

#ifndef LIBXANTE_COMPILE
# ifndef _LIBXANTE_H
#  error "Never use <xt_info.h> directly; include <libxante.h> instead."
# endif
#endif

/**
 * @name xante_config_path
 * @brief Gives the current application configuration path.
 *
 * @param [in] xpp: The library main object.
 *
 * @return On success returns the application configuration path or NULL otherwise.
 */
const char *xante_config_path(const xante_t *xpp);

/**
 * @name xante_log_path
 * @brief Gives the current application log path.
 *
 * @param [in] xpp: The library main object.
 *
 * @return On success returns the application log path or NULL otherwise.
 */
const char *xante_log_path(const xante_t *xpp);

/**
 * @name xante_log_level
 * @brief Gives the current application message log level.
 *
 * @param [in] xpp: The library main object.
 *
 * @return On success returns the application message log level or NULL otherwise.
 */
const char *xante_log_level(const xante_t *xpp);

/**
 * @name xante_application_name
 * @brief Gives the application name.
 *
 * @param [in] xpp: The library main object.
 *
 * @return On success returns the application name or NULL otherwise.
 */
const char *xante_application_name(const xante_t *xpp);

/**
 * @name xante_module_name
 * @brief Gives the module name used by the application.
 *
 * @param [in] xpp: The library main object.
 *
 * @return On success returns the module name or NULL otherwise.
 */
const char *xante_module_name(const xante_t *xpp);

/**
 * @name xante_version
 * @brief Gives the application version.
 *
 * @param [in] xpp: The library main object.
 *
 * @return On success returns the application version or NULL otherwise.
 */
const char *xante_version(const xante_t *xpp);

/**
 * @name xante_company
 * @brief Gives the application company name.
 *
 * @param [in] xpp: The library main object.
 *
 * @return On success returns the company name or NULL otherwise.
 */
const char *xante_company(const xante_t *xpp);

/**
 * @name xante_description
 * @brief Gives the application description.
 *
 * @param [in] xpp: The library main object.
 *
 * @return On success returns the application description or NULL otherwise.
 */
const char *xante_description(const xante_t *xpp);

/**
 * @name xante_revision
 * @brief Gives the application version revision.
 *
 * @param [in] xpp: The library main object.
 *
 * @return On success returns the application version revision or -1 otherwise.
 */
int xante_revision(const xante_t *xpp);

/**
 * @name xante_build
 * @brief Gives the application version build number.
 *
 * @param [in] xpp: The library main object.
 *
 * @return On success returns the application version build number or -1
 *         otherwise.
 */
int xante_build(const xante_t *xpp);

/**
 * @name xante_beta
 * @brief Gives the information if the application is in beta stage or not.
 *
 * @param [in] xpp: The library main object.
 *
 * @return Returns true/false if the application is in beta stage.
 */
bool xante_beta(const xante_t *xpp);

/**
 * @name xante_info
 * @brief Gives all informations about an application in a JSON string.
 *
 * @param [in] xpp: The library main object.
 *
 * @return On success returns the JSON string with all application informations
 *         or NULL otherwise.
 */
const char *xante_info(const xante_t *xpp);

#endif

