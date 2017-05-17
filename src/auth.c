
/*
 * Description: Functions to handle user access control inside an application.
 *
 * Author: Rodrigo Freitas
 * Created at: Tue May  2 21:03:18 2017
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
 * Internal function
 *
 */

/*
 *
 * Internal API
 *
 */

/**
 * @name auth_init
 * @brief Starts the internal user access validation.
 *
 * @param [in,out] xpp: The main library object.
 * @param [in] username: The username.
 * @param [in] password: The username password.
 *
 * @return On succcess returns 0 or -1 otherwise.
 */
int auth_init(struct xante_app *xpp, const char *username,
    const char *password)
{
    xpp->auth.username = cl_string_create("%s", username);
    xpp->auth.password = cl_string_create("%s", password);

    /* TODO: Validate user/pass inside a DB */

    return 0;
}

/**
 * @name auth_uninit
 * @brief Finishes the internal user validation.
 *
 * @param [in,out] xpp: The main library object.
 */
void auth_uninit(struct xante_app *xpp)
{
    if (xpp->auth.password != NULL)
        cl_string_unref(xpp->auth.password);

    if (xpp->auth.username != NULL)
        cl_string_unref(xpp->auth.username);
}

