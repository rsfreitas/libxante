
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
#define _LIBXANTE_H             1

#ifndef _COLLECTIONS_H
# include <collections.h>
#endif

/** Supported line break character to be used inside JTF strings */
#define XANTE_STR_LINE_BREAK    '^'

/** Supported UI dialogs */
enum xante_ui_dialog {
    XANTE_UI_DIALOG_UNKNOWN,
    XANTE_UI_DIALOG_MENU,
    XANTE_UI_DIALOG_INPUT_INT,
    XANTE_UI_DIALOG_INPUT_FLOAT,
    XANTE_UI_DIALOG_INPUT_DATE,
    XANTE_UI_DIALOG_INPUT_STRING,
    XANTE_UI_DIALOG_INPUT_PASSWD,
    XANTE_UI_DIALOG_INPUT_TIME,
    XANTE_UI_DIALOG_CALENDAR,
    XANTE_UI_DIALOG_TIMEBOX,
    XANTE_UI_DIALOG_RADIO_CHECKLIST,
    XANTE_UI_DIALOG_CHECKLIST,
    XANTE_UI_DIALOG_YES_NO,
    XANTE_UI_DIALOG_DYNAMIC_MENU,
    XANTE_UI_DIALOG_DELETE_DYNAMIC_MENU_ITEM,
    XANTE_UI_DIALOG_ADD_DYNAMIC_MENU_ITEM,
    XANTE_UI_DIALOG_CUSTOM,
    XANTE_UI_DIALOG_PROGRESS,
    XANTE_UI_DIALOG_SPINNER_SYNC,
    XANTE_UI_DIALOG_DOTS_SYNC,
    XANTE_UI_DIALOG_RANGE,
    XANTE_UI_DIALOG_FILE_SELECT,
    XANTE_UI_DIALOG_DIR_SELECT,
    XANTE_UI_DIALOG_FILE_VIEW,
    XANTE_UI_DIALOG_TAILBOX,
    XANTE_UI_DIALOG_SCROLLTEXT,
    XANTE_UI_DIALOG_UPDATE_OBJECT,
    XANTE_UI_DIALOG_INPUTSCROLL,
    XANTE_UI_DIALOG_MIXEDFORM,
    XANTE_UI_DIALOG_BUILDLIST
};

/** An access mode from a menu or a menu item */
enum xante_access_mode {
    XANTE_ACCESS_HIDDEN     = 0,
    XANTE_ACCESS_VIEW       = (1 << 0),
    XANTE_ACCESS_EDIT       = (1 << 1)
};

/** Buttons from a dialog */
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
enum xante_ui_menu {
    XANTE_UI_MENU_DEFAULT,
    XANTE_UI_MENU_DYNAMIC
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
# define MAJOR_VERSION  0
# define MINOR_VERSION  0
# define RELEASE        8

# include "xante/xt_internal.h"
#endif

#include "xante/xt_auth.h"
#include "xante/xt_changes.h"
#include "xante/xt_config.h"
#include "xante/xt_dialogs.h"
#include "xante/xt_dm.h"
#include "xante/xt_env.h"
#include "xante/xt_error.h"
#include "xante/xt_event.h"
#include "xante/xt_info.h"
#include "xante/xt_init.h"
#include "xante/xt_instance.h"
#include "xante/xt_item.h"
#include "xante/xt_jtf.h"
#include "xante/xt_log.h"
#include "xante/xt_menu.h"
#include "xante/xt_mjtf.h"
#include "xante/xt_runtime.h"
#include "xante/xt_ui.h"
#include "xante/xt_utils.h"

#endif

