
/*
 * Description:
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

#ifndef _LIBXANTE_XT_ERROR_H
#define _LIBXANTE_XT_ERROR_H          1

#ifndef LIBXANTE_COMPILE
# ifndef _LIBXANTE_H
#  error "Never use <xt_error.h> directly; include <libxante.h> instead."
# endif
#endif

enum xante_error_code {
    XANTE_NO_ERROR,

    XANTE_ERROR_NO_MEMORY,
    XANTE_ERROR_NULL_ARG,
    XANTE_ERROR_INVALID_ARG,
    XANTE_ERROR_WRONG_JTF_FORMAT,
    XANTE_ERROR_JTF_NO_GENERAL_OBJECT,
    XANTE_ERROR_JTF_NO_INFO,
    XANTE_ERROR_JTF_INFO_WITHOUT_VALUE,
    XANTE_ERROR_JTF_NO_ITEMS_OBJECT,
    XANTE_ERROR_JTF_NO_MENUS_OBJECT,
    XANTE_ERROR_JTF_NO_UI_OBJECT,
    XANTE_ERROR_UI_NOT_INITIALIZED,

    XANTE_MAX_ERROR_CODE
};

#ifdef LIBXANTE_COMPILE

void errno_clear(void);
void errno_set(enum xante_error_code code);

#endif

/**
 * @name xante_get_last_error
 * @brief Gets the current library internal error code.
 *
 * @return Returns the current error code.
 */
enum xante_error_code xante_get_last_error(void);

/**
 * @name xante_strerror
 * @brief Gives a description message about an error code.
 *
 * @param [in] code: The error code to be translated.
 *
 * @return Returns the descriptive string of the error.
 */
const char *xante_strerror(enum xante_error_code code);

#endif
