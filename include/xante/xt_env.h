
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Thu Jun  1 00:46:53 2017
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

#ifndef _LIBXANTE_XT_ENV_H
#define _LIBXANTE_XT_ENV_H          1

#ifndef LIBXANTE_COMPILE
# ifndef _LIBXANTE_H
#  error "Never use <xt_env.h> directly; include <libxante.h> instead."
# endif
#else

/* Internal library declarations */
#endif

/**
 * @name xante_env_set_theme
 * @brief Sets the UI color theme.
 *
 * This function receives a \a pathname with a libdialog's RC file to be used as
 * the UI color theme.
 *
 * @param [in] pathname: The libdialog's RC file pathname.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int xante_env_set_theme(const char *pathname);

/**
 * @name xante_env_theme
 * @brief Gets the current theme filename.
 *
 * @return On success returns the theme filename, which must be freed after
 *         used, or NULL otherwise.
 */
char *xante_env_theme(void);

/**
 * @name xante_env_set_auth_path
 * @brief Sets the internal path to the database file.
 *
 * @param [in] pathname: The path.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int xante_env_set_auth_path(const char *pathname);

/**
 * @name xante_env_auth_path
 * @brief Gets the current database authentication path.
 *
 * @return On success returns the path, which must be freed after used, or
 *         NULL otherwise.
 */
char *xante_env_auth_path(void);

/**
 * @name xante_env_set_cfg_path
 * @brief Sets the internal path to the configuration file.
 *
 * Replaces the path used inside the JTF file.
 *
 * @param [in] pathname: The path.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int xante_env_set_cfg_path(const char *pathname);

/**
 * @name xante_env_cfg_path
 * @brief Gets the current configuration path.
 *
 * @return On success returns the path, which must be freed after used, or
 *         NULL otherwise.
 */
char *xante_env_cfg_path(void);

#endif

