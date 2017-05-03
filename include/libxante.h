
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

#ifndef _LIBXANTE_H
#define _LIBXANTE_H         1

#ifndef _COLLECTIONS_H
# include <collections.h>
#endif

enum xante_config_file_status {
    XANTE_CFG_ST_UNKNOWN,
    XANTE_CFG_ST_CREATED,
    XANTE_CFG_ST_LOADED,
    XANTE_CFG_ST_SAVED,
    XANTE_CFG_ST_UNSAVED
};

#ifdef LIBXANTE_COMPILE
# define MAJOR_VERSION      0
# define MINOR_VERSION      1
# define RELEASE            1

# include "xante/xt_internal.h"
#endif

#include "xante/xt_access.h"
#include "xante/xt_changes.h"
#include "xante/xt_config.h"
#include "xante/xt_error.h"
#include "xante/xt_events.h"
#include "xante/xt_init.h"
#include "xante/xt_log.h"
#include "xante/xt_plugin.h"
#include "xante/xt_runtime.h"
#include "xante/xt_translation.h"
#include "xante/xt_ui.h"
#include "xante/xt_utils.h"
#include "xante/xt_xml.h"

#endif

