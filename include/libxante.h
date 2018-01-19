
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
#define _LIBXANTE_H

#ifdef LIBXANTE_COMPILE
# define MAJOR_VERSION          0
# define MINOR_VERSION          0
# define RELEASE                14

/* Common declarations to all library versions */
# include "common/defs.h"
#endif

#ifndef __cplusplus

#ifndef _COLLECTIONS_H
# include <collections.h>
#endif

/** Supported line break character to be used inside JTF strings */
#define XANTE_STR_LINE_BREAK    '^'

/** Supported UI widgets */
enum xante_widget {
    XANTE_WIDGET_UNKNOWN,
    XANTE_WIDGET_MENU_REFERENCE,
    XANTE_WIDGET_INPUT_INT,
    XANTE_WIDGET_INPUT_FLOAT,
    XANTE_WIDGET_INPUT_DATE,
    XANTE_WIDGET_INPUT_STRING,
    XANTE_WIDGET_INPUT_PASSWD,
    XANTE_WIDGET_INPUT_TIME,
    XANTE_WIDGET_CALENDAR,
    XANTE_WIDGET_TIMEBOX,
    XANTE_WIDGET_RADIO_CHECKLIST,
    XANTE_WIDGET_CHECKLIST,
    XANTE_WIDGET_YES_NO,
    XANTE_WIDGET_DYNAMIC_MENU_REFERENCE,
    XANTE_WIDGET_DELETE_DYNAMIC_MENU_ITEM,
    XANTE_WIDGET_ADD_DYNAMIC_MENU_ITEM,
    XANTE_WIDGET_CUSTOM,
    XANTE_WIDGET_PROGRESS,
    XANTE_WIDGET_SPINNER_SYNC,
    XANTE_WIDGET_DOTS_SYNC,
    XANTE_WIDGET_RANGE,
    XANTE_WIDGET_FILE_SELECT,
    XANTE_WIDGET_DIR_SELECT,
    XANTE_WIDGET_FILE_VIEW,
    XANTE_WIDGET_TAILBOX,
    XANTE_WIDGET_SCROLLTEXT,
    XANTE_WIDGET_UPDATE_OBJECT,
    XANTE_WIDGET_INPUTSCROLL,
    XANTE_WIDGET_MIXEDFORM,
    XANTE_WIDGET_BUILDLIST,
    XANTE_WIDGET_SPREADSHEET
};

/** Buttons from a widget */
enum xante_buttons {
    XANTE_BTN_OK        = (1 << 0),
    XANTE_BTN_CANCEL    = (1 << 1),
    XANTE_BTN_HELP      = (1 << 2),
    XANTE_BTN_INFO      = (1 << 3)
};

/** Messagebox type */
enum xante_msgbox_type {
    XANTE_MSGBOX_INFO,
    XANTE_MSGBOX_WARNING,
    XANTE_MSGBOX_ERROR
};

/** Types of menu */
enum xante_menu_type {
    XANTE_MENU_DEFAULT,
    XANTE_MENU_DYNAMIC
};

/** Session formats */
enum xante_session {
    XANTE_SESSION_CONTINUOUS = 1,
    XANTE_SESSION_SINGLE
};

/** Session sources */
enum xante_session_source {
    XANTE_SESSION_LOCALHOST = 1,
    XANTE_SESSION_SSH
};

/** Libxante initialization flags */
enum xante_init_flags {
    XANTE_USE_MODULE        = (1 << 0), // Enable/Disable module calls.
    XANTE_USE_AUTH          = (1 << 1), // Enable/Disable database authentication.
    XANTE_SINGLE_INSTANCE   = (1 << 2), // Enable/Disable application single
                                        // instance mode.
};

/** Event arguments */
enum xante_event_argument_type {
    XANTE_EVENT_DATA_UNKNOWN = -1,
    XANTE_EVENT_DATA_XANTE_T,
    XANTE_EVENT_DATA_XANTE_MENU_T,
    XANTE_EVENT_DATA_XANTE_ITEM_T,
    XANTE_EVENT_DATA_XANTE_ITEM_VALUE,
    XANTE_EVENT_DATA_XANTE_CONFIG,
    XANTE_EVENT_DATA_XANTE_CHANGES_LIST,
    XANTE_EVENT_DATA_CUSTOM
};

/** Return values of an application */
enum xante_return_value {
    XANTE_RETURN_ERROR = -1,        //** Internal error.
    XANTE_RETURN_OK,                //** Application ended without modifications.
    XANTE_RETURN_CONFIG_SAVED,      //** Application ended with user saving modifications.
    XANTE_RETURN_CONFIG_UNSAVED,    //** Application ended with unsaved modifications.
    XANTE_RETURN_TIMEOUT            //** Application ended by timeout.
};

/** Modes to search for an item */
enum xante_item_search_mode {
    XANTE_ITEM_INVALID_SEARCH,
    XANTE_ITEM_SEARCH_BY_NAME,
    XANTE_ITEM_SEARCH_BY_CONFIG_NAME,
    XANTE_ITEM_SEARCH_BY_OBJECT_ID
};

/** Main library object */
typedef void    xante_t;

/** UI objects */
typedef void    xante_menu_t;
typedef void    xante_item_t;

/** Event function argument */
typedef void    xante_event_arg_t;

/** A internal configuration modification entry */
struct xante_change_entry {
    char    *item_name;
    char    *old_value;
    char    *new_value;
};

#ifdef LIBXANTE_COMPILE
# include "internal/internal.h"
#endif

#include "api/auth.h"
#include "api/config.h"
#include "api/env.h"
#include "api/error.h"
#include "api/event.h"
#include "api/gadget.h"
#include "api/info.h"
#include "api/init.h"
#include "api/item.h"
#include "api/log.h"
#include "api/manager.h"
#include "api/menu.h"
#include "api/runtime.h"
#include "api/utils.h"

#else   // __cplusplus

#include "cpp/item.hpp"
#include "cpp/menu.hpp"
#include "cpp/jtf.hpp"
#include "cpp/config.hpp"

#endif  // __cplusplus

#endif

