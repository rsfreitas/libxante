
/*
 * Description: Functions to handle UI from the user level.
 *
 * Author: Rodrigo Freitas
 * Created at: Wed May  3 13:48:23 2017
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

__PUB_API__ int xante_ui_set_backtitle(xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;
    cl_string_t *title = NULL;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    if (xante_runtime_ui_active(xpp) == false) {
        errno_set(XANTE_ERROR_UI_NOT_INITIALIZED);
        return -1;
    }

    title = cl_string_create(cl_tr("%s - Version %s.%d Build %d %s"),
                             cl_string_valueof(x->info.application_name),
                             cl_string_valueof(x->info.version),
                             x->info.revision, x->info.build,
                             (x->info.beta == true) ? "BETA" : "");

    dialog_vars.backtitle = strdup(cl_string_valueof(title));

    return 0;
}

__PUB_API__ int xante_ui_clear_backtitle(xante_t *xpp)
{
    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    if (xante_runtime_ui_active(xpp) == false) {
        errno_set(XANTE_ERROR_UI_NOT_INITIALIZED);
        return -1;
    }

    free(dialog_vars.backtitle);
    dialog_vars.backtitle = NULL;

    return 0;
}

