
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
    XANTE_UI_DIALOG_CUSTOM,
    XANTE_UI_DIALOG_DYNAMIC_MENU,
    XANTE_UI_DIALOG_RM_DYNAMIC_MENU
//    XANTE_UI_DIALOG_RANGE,
//    XANTE_UI_DIALOG_FORM
};

/** A possible state from the application config file */
enum xante_config_file_status {
    XANTE_CFG_ST_UNKNOWN,
    XANTE_CFG_ST_CREATED,
    XANTE_CFG_ST_LOADED,
    XANTE_CFG_ST_SAVED,
    XANTE_CFG_ST_UNSAVED
};

/** A access mode from a menu or a menu item */
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

/** String keys of a supported dialog */
#define XANTE_UI_STR_DIALOG_MENU                "menu"
#define XANTE_UI_STR_DIALOG_INPUT_INT           "input_int"
#define XANTE_UI_STR_DIALOG_INPUT_FLOAT         "input_float"
#define XANTE_UI_STR_DIALOG_INPUT_DATE          "input_date"
#define XANTE_UI_STR_DIALOG_INPUT_TIME          "input_time"
#define XANTE_UI_STR_DIALOG_INPUT_STRING        "input_string"
#define XANTE_UI_STR_DIALOG_INPUT_PASSWD        "input_passwd"
#define XANTE_UI_STR_DIALOG_CALENDAR            "calendar"
#define XANTE_UI_STR_DIALOG_TIMEBOX             "timebox"
#define XANTE_UI_STR_DIALOG_RADIO_CHECKLIST     "radio_checklist"
#define XANTE_UI_STR_DIALOG_CHECKLIST           "checklist"
#define XANTE_UI_STR_DIALOG_YESNO               "yesno"
#define XANTE_UI_STR_DIALOG_CUSTOM              "custom"
#define XANTE_UI_STR_DIALOG_DYNAMIC_MENU        "dynamic_menu"
#define XANTE_UI_STR_DIALOG_DELETE_DYNAMIC_MENU "delete_dynamic_menu"

/** String keys of supported menus */
#define XANTE_UI_STR_DEFAULT_MENU               "default"
#define XANTE_UI_STR_DYNAMIC_MENU               "dynamic"

/** Event arguments */
#define XANTE_EVT_DATA_XANTE_T                  "xante_t"
#define XANTE_EVT_DATA_XANTE_MENU_T             "xante_menu_t"
#define XANTE_EVT_DATA_XANTE_ITEM_T             "xante_item_t"
#define XANTE_EVT_DATA_XANTE_ITEM_VALUE         "xante_item_value"
#define XANTE_EVT_DATA_XANTE_CONFIG             "xante_config"

/** Main library object */
typedef void                                    xante_t;

/** Item object */
typedef void                                    xante_item_t;

/** Event function argument */
typedef void                                    xante_event_arg_t;

#ifdef LIBXANTE_COMPILE
# define MAJOR_VERSION                          0
# define MINOR_VERSION                          1
# define RELEASE                                1

# include "xante/xt_internal.h"
#endif

#include "xante/xt_auth.h"
#include "xante/xt_changes.h"
#include "xante/xt_config.h"
#include "xante/xt_dialogs.h"
#include "xante/xt_dm.h"
#include "xante/xt_error.h"
#include "xante/xt_event.h"
#include "xante/xt_init.h"
#include "xante/xt_jtf.h"
#include "xante/xt_log.h"
#include "xante/xt_runtime.h"
#include "xante/xt_theme.h"
#include "xante/xt_translation.h"
#include "xante/xt_ui.h"
#include "xante/xt_utils.h"

#endif

