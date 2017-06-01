
/*
 * Description: Handles all supported environment variables.
 *
 * Author: Rodrigo Freitas
 * Created at: Thu Jun  1 00:38:35 2017
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

#include "libxante.h"

/*
 *
 * External API
 *
 */

/**
 * @name xante_env_set_theme
 * @brief Sets the UI color theme.
 *
 * This function receives a \a pathname with a dialog's RC file to be used as
 * the UI color theme.
 *
 * @param [in] pathname: The dialog's RC file pathname.
 *
 * @return On success returns 0 or -1 otherwise.
 */
__PUB_API__ int xante_env_set_theme(const char *pathname)
{
    errno_clear();

    if (NULL == pathname) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    setenv("DIALOGRC", pathname, 1);

    return 0;
}

/**
 * @name xante_env_theme
 * @brief Gets the current theme filename.
 *
 * @return On success returns the theme filename, which must be freed after
 *         used, or NULL otherwise.
 */
__PUB_API__ char *xante_env_theme(void)
{
    char *env = NULL;

    env = getenv("DIALOGRC");

    if (NULL == env)
        return NULL;

    return strdup(env);
}

/**
 * @name xante_env_set_auth_path
 * @brief Sets the internal path to the database file.
 *
 * @param [in] pathname: The path.
 *
 * @return On success returns 0 or -1 otherwise.
 */
__PUB_API__ int xante_env_set_auth_path(const char *pathname)
{
    errno_clear();

    if (NULL == pathname) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    setenv(ENV_XANTE_DB_PATH, pathname, 1);

    return 0;
}

/**
 * @name xante_env_auth_path
 * @brief Gets the current database authentication path.
 *
 * @return On success returns the path, which must be freed after used, or
 *         NULL otherwise.
 */
__PUB_API__ char *xante_env_auth_path(void)
{
    char *env = NULL;

    env = getenv(ENV_XANTE_DB_PATH);

    if (NULL == env)
        return NULL;

    return strdup(env);
}

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

__PUB_API__ int xante_env_set_cfg_path(const char *pathname)
{
    errno_clear();

    if (NULL == pathname) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    setenv(ENV_XANTE_CFG_PATH, pathname, 1);

    return 0;
}

/**
 * @name xante_env_cfg_path
 * @brief Gets the current configuration path.
 *
 * @return On success returns the path, which must be freed after used, or
 *         NULL otherwise.
 */
__PUB_API__ char *xante_env_cfg_path(void)
{
    char *env = NULL;

    env = getenv(ENV_XANTE_CFG_PATH);

    if (NULL == env)
        return NULL;

    return strdup(env);
}

