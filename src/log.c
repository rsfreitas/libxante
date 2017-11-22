
/*
 * Description: Functions to handle an application log file from within the
 *              library.
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

#include "libxante.h"

/*
 *
 * Internal functions
 *
 */
static enum cl_log_level tr_log_level(const char *level)
{
    if (NULL == level)
        goto end_block;

    if (strcmp(level, "debug") == 0)
        return CL_LOG_DEBUG;
    else if (strcmp(level, "info") == 0)
        return CL_LOG_INFO;
    else if (strcmp(level, "notice") == 0)
        return CL_LOG_NOTICE;
    else if (strcmp(level, "warning") == 0)
        return CL_LOG_WARNG;
    else if (strcmp(level, "error") == 0)
        return CL_LOG_ERROR;
    else if (strcmp(level, "critical") == 0)
        return CL_LOG_CRITI;
    else if (strcmp(level, "alert") == 0)
        return CL_LOG_ALERT;
    else if (strcmp(level, "emergency") == 0)
        return CL_LOG_EMERG;
    else if (strcmp(level, "off") == 0)
        return CL_LOG_OFF;

end_block:
    return CL_LOG_INFO;
}

/*
 *
 * Internal API
 *
 */

/**
 * We save the log object here, that way we don't need the library as
 * an argument to the log function.
 */
static cl_log_t *__log_xpp = NULL;

int log_init(struct xante_app *xpp)
{
    enum cl_log_level level = CL_LOG_DEBUG;
    char *pathname = NULL;

    level = tr_log_level(xpp->info.log_level);
    asprintf(&pathname, "%s/%s.log", xpp->info.log_pathname,
             xpp->info.application_name);

    xpp->log.log = cl_log_open(pathname, CL_LOG_SYNC_ALL_MSGS, level, 0);
    free(pathname);

    if (NULL == xpp->log.log) {
        return -1;
    }

    __log_xpp = xpp->log.log;

    return 0;
}

void log_uninit(struct xante_app *xpp)
{
    if (NULL == xpp)
        return;

    cl_log_close(xpp->log.log);
}

/*
 *
 * API
 *
 */

__PUB_API__ void xante_log(enum cl_log_level level, const char *function,
    int line, const char *fmt, ...)
{
    va_list ap;
    char *str = NULL;

    va_start(ap, fmt);

    if ((level == CL_LOG_ERROR) || (level == CL_LOG_DEBUG)) {
        vasprintf(&str, fmt, ap);
        cl_log_printf(__log_xpp, level, "%s:%d;%s", function, line, str);
        free(str);
    } else
        cl_log_vprintf(__log_xpp, level, fmt, ap);
}

