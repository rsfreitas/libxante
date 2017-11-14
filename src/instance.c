
/*
 * Description: Instance control of an application and its dependencies.
 *
 * Author: Rodrigo Freitas
 * Created at: Mon Nov 13 22:05:02 2017
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
 * Internal API
 *
 */

/**
 * @name instance_init
 * @brief Initializes the internal SO information about the current application
 *        instance.
 *
 * @param [in] xpp: The library main structure.
 * @param [in] single_instance: A boolean flag indicating if we can have several
 *                              instances of this application or not.
 *
 * @return On success returns 0, and the application may continue to run, or -1
 *         otherwise.
 */
int instance_init(const struct xante_app *xpp, bool single_instance)
{
    if (single_instance == false)
        goto end_block;

    if (cl_instance_active(xpp->info.application_name)) {
        errno_set(XANTE_ERROR_INSTANCE_ALREADY_RUNNING);
        return -1;
    }

end_block:
    cl_set_instance_as_active();
    return 0;
}

