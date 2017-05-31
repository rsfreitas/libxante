
/*
 * Description: Functions to handle the library initialization an
 *              uninitialization.
 *
 * Author: Rodrigo Freitas
 * Created at: Tue May  2 21:03:17 2017
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

#include <stdlib.h>

#include "libxante.h"

/*
 *
 * Internal functions
 *
 */

static void destroy_xante_app(const struct cl_ref_s *ref)
{
    struct xante_app *xpp = cl_container_of(ref, struct xante_app, ref);

    if (NULL == xpp)
        return;

    event_uninit(xpp);
    xante_info(cl_tr("Finishing application"));
    change_uninit(xpp);
    ui_uninit(xpp);
    jtf_release_info(xpp);
    log_uninit(xpp);
    auth_uninit(xpp);
    free(xpp);
}

static struct xante_app *new_xante_app(void)
{
    struct xante_app *xpp = NULL;

    xpp = calloc(1, sizeof(struct xante_app));

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NO_MEMORY);
        return NULL;
    }

    ui_init(xpp);

    /* Initialize the reference count */
    xpp->ref.count = 1;
    xpp->ref.free = destroy_xante_app;

    return xpp;
}

/*
 *
 * API
 *
 */

__PUB_API__ xante_t *xante_init(const char *jtf_pathname, bool use_plugin,
    bool use_auth, enum xante_session session, const char *username,
    const char *password)
{
    struct xante_app *xpp = NULL;

    errno_clear();

    if (NULL == jtf_pathname) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return NULL;
    }

    xpp = new_xante_app();

    if (NULL == xpp)
        return NULL;

    /* Set runtime flags */
    runtime_start(xpp);

    /* Start translation environment */

    /* Parse the JTF file */
    if (jtf_parse(jtf_pathname, xpp) < 0)
        goto error_block;

    /* Start user access control */
    if (auth_init(xpp, use_auth, session, username, password) < 0)
        goto error_block;

    /* Start user modifications monitoring */
    change_init(xpp);

    /* Start log file */
    log_init(xpp);

    /* Call the plugin initialization function or disable its using */
    if (event_init(xpp, use_plugin) < 0)
        goto error_block;

    xante_info(cl_tr("Initializing application - %s"),
               cl_string_valueof(xpp->info.application_name));

    return xpp;

error_block:
    return NULL;
}

__PUB_API__ int xante_uninit(xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    cl_ref_dec(&x->ref);

    return 0;
}

