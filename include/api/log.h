
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Tue May  2 21:07:38 2017
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

#ifndef _LIBXANTE_API_LOG_H
#define _LIBXANTE_API_LOG_H

#ifndef LIBXANTE_COMPILE
# ifndef _LIBXANTE_H
#  error "Never use <log.h> directly; include <libxante.h> instead."
# endif
#endif

/**
 * @name xante_log
 * @brief Logs a string into the log file.
 *
 * @param [in] level: The message level inside the file.
 * @param [in] function: The function name which is calling the log function.
 * @param [in] line: The line number which the function is been called.
 * @param [in] fmt: The message format.
 * @param [in] ...: The message content.
 */
void xante_log(enum cl_log_level level, const char *function, int line,
               const char *fmt, ...) __attribute__((format(printf, 4, 5)));

/*
 * This is just a wrapper to be used on places where variadic functions
 * aren't supported, such as Go.
 */
void xante_log_ex(enum cl_log_level level, const char *function, int line,
                  const char *content);

/**
 * Macros to log messages with several levels.
 */
#define xante_log_info(fmt, args...)     \
    xante_log(CL_LOG_INFO, NULL, 0, fmt, ## args)

#define xante_log_debug(fmt, args...)    \
    xante_log(CL_LOG_DEBUG, __FUNCTION__, __LINE__, fmt, ## args)

#define xante_log_error(fmt, args...)    \
    xante_log(CL_LOG_ERROR, __FUNCTION__, __LINE__, fmt, ## args)

#define xante_log_warning(fmt, args...)  \
    xante_log(CL_LOG_WARNG, __FUNCTION__, __LINE__, fmt, ## args)

#endif

