
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Wed May  3 11:19:24 2017
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
 * Internal functions
 *
 */

/*
 *
 * Internal API
 *
 */

/*
 *
 * API
 *
 */

__PUB_API__ int xante_theme_set(const char *pathname)
{
    errno_clear();

    if (NULL == pathname) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    setenv("DIALOGRC", pathname, 1);

    return 0;
}

