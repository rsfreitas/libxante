
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
    cl_tr_noop("Ok"),
    cl_tr_noop("No internal memory available"),
    cl_tr_noop("A NULL argument was passed"),
    cl_tr_noop("An invalid argument was passed"),
    cl_tr_noop("Unable to parse JTF file"),
    cl_tr_noop("JTF file has no 'general' object"),
    cl_tr_noop("JTF object was not found"),
    cl_tr_noop("JTF object has no value"),
    cl_tr_noop("JTF file has no 'items' object"),
    cl_tr_noop("JTF file has no 'menu' object"),
    cl_tr_noop("JTF file has no 'ui' object"),
    cl_tr_noop("UI has not been initialized"),
    cl_tr_noop("JTF file has no 'input_ranges' object"),
    cl_tr_noop("JTF file has no 'config' object"),
    cl_tr_noop("JTF file has no 'internal' object"),
    cl_tr_noop("JTF file has no 'application' object"),
    cl_tr_noop("Plugin initialization failed"),
    cl_tr_noop("Plugin without internal info"),
    cl_tr_noop("Plugin without internal API"),
    cl_tr_noop("Plugin without mandatory function"),
    cl_tr_noop("Plugin EV_INIT error")
};

static const char *__unknown_error = cl_tr_noop("Unknown error");
#define __errno     (*cl_errno_storage())

/**
 * @name errno_clear
 * @brief Clears the library internal error code.
 */
void errno_clear(void)
{
    __errno = XANTE_NO_ERROR;
}

/**
 * @name errno_set
 * @brief Sets the current value of the library internal error code.
 *
 * @param [in] code: The new erro code.
 */
void errno_set(enum xante_error_code code)
{
    __errno = code;
}

__PUB_API__ enum xante_error_code xante_get_last_error(void)
{
    return __errno;
}

__PUB_API__ const char *xante_strerror(enum xante_error_code code)
{
    if (code >= XANTE_MAX_ERROR_CODE)
        return __unknown_error;

    return __description[code];
}

