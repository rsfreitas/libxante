
/*
 * Description: Functions to handle the library internal error.
 *
 * Author: Rodrigo Freitas
 * Created at: Tue May  2 20:25:58 2017
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

static const char *__description[] = {
    cl_tr_noop("Ok")
};

static const char *__unknown_error = cl_tr_noop("Unknown error");
#define __errno     (*cl_errno_storage())

void errno_clear(void)
{
    __errno = XANTE_NO_ERROR;
}

void errno_set(enum xante_error_code code)
{
    __errno = code;
}

enum xante_error_code xante_get_last_error(void)
{
    return __errno;
}

const char *xante_strerror(enum xante_error_code code)
{
    if (code >= XANTE_MAX_ERROR_CODE)
        return __unknown_error;

    return __description[code];
}

