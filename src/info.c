
/*
 * Description: API to expose relevant informations from the main library
 *              object.
 *
 * Author: Rodrigo Freitas
 * Created at: Tue Nov 14 10:25:38 2017
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
 * API
 *
 */

__PUB_API__ const char *xante_config_path(const xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return NULL;
    }

    return x->info.cfg_pathname;
}

__PUB_API__ const char *xante_log_path(const xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return NULL;
    }

    return x->info.log_pathname;
}

__PUB_API__ const char *xante_log_level(const xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return NULL;
    }

    return x->info.log_level;
}

__PUB_API__ const char *xante_application_name(const xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return NULL;
    }

    return x->info.application_name;
}

__PUB_API__ const char *xante_module_name(const xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return NULL;
    }

    return x->info.module_name;
}

__PUB_API__ const char *xante_version(const xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return NULL;
    }

    return x->info.version;
}

__PUB_API__ const char *xante_company(const xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return NULL;
    }

    return x->info.company;
}

__PUB_API__ const char *xante_description(const xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return NULL;
    }

    return x->info.description;
}

__PUB_API__ int xante_revision(const xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    return x->info.revision;
}

__PUB_API__ int xante_build(const xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    return x->info.build;
}

__PUB_API__ bool xante_beta(const xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    return x->info.beta;
}

/*
 * Here we give all informations in a JSON string format.
 */
__PUB_API__ const char *xante_info(const xante_t *xpp)
{
    char *info = NULL;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return NULL;
    }

    asprintf(&info, "{\"config_path\":\"%s\","
                     "\"log_path\":\"%s\","
                     "\"log_level\":\"%s\","
                     "\"application_name\":\"%s\","
                     "\"module_name\":\"%s\","
                     "\"version\":\"%s\","
                     "\"company\":\"%s\","
                     "\"description\":\"%s\","
                     "\"revision\":\"%d\","
                     "\"build\":\"%d\","
                     "\"beta\":%s}",
                     xante_config_path(xpp),
                     xante_log_path(xpp),
                     xante_log_level(xpp),
                     xante_application_name(xpp),
                     xante_module_name(xpp),
                     xante_version(xpp),
                     xante_company(xpp),
                     xante_description(xpp),
                     xante_revision(xpp),
                     xante_build(xpp),
                     xante_beta(xpp) ? "true" : "false");

    return info;
}

