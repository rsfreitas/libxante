
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Thu Jan  4 19:16:37 2018
 * Project: libxante
 *
 * Copyright (C) 2018 Rodrigo Freitas
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

#ifndef _LIBXANTE_DEFS_H
#define _LIBXANTE_DEFS_H                        1

#ifdef __cplusplus
extern "C" {
#endif

/* JTF object names */
#define XANTE_JTF_GENERAL                       "general"
#define XANTE_JTF_APPLICATION_NAME              "name"
#define XANTE_JTF_MODULE_NAME                   "module"
#define XANTE_JTF_CONFIG_PATHNAME               "config_pathname"
#define XANTE_JTF_LOG_PATHNAME                  "log_pathname"
#define XANTE_JTF_LOG_LEVEL                     "log_level"
#define XANTE_JTF_VERSION                       "version"
#define XANTE_JTF_REVISION                      "revision"
#define XANTE_JTF_BUILD                         "build"
#define XANTE_JTF_BETA                          "beta"
#define XANTE_JTF_UI                            "ui"
#define XANTE_JTF_MAIN_MENU                     "main_menu"
#define XANTE_JTF_MENUS                         "menus"
#define XANTE_JTF_NAME                          "name"
#define XANTE_JTF_OBJECT_ID                     "object_id"
#define XANTE_JTF_MODE                          "mode"
#define XANTE_JTF_TYPE                          "type"
#define XANTE_JTF_BLOCK                         "block"
#define XANTE_JTF_ITEM                          "item"
#define XANTE_JTF_HELP                          "help"
#define XANTE_JTF_ITEMS                         "items"
#define XANTE_JTF_OPTIONS                       "options"
#define XANTE_JTF_DEFAULT_VALUE                 "default_value"
#define XANTE_JTF_RANGES                        "ranges"
#define XANTE_JTF_STRING_LENGTH                 "string_length"
#define XANTE_JTF_MAX_RANGE                     "max"
#define XANTE_JTF_MIN_RANGE                     "min"
#define XANTE_JTF_CONFIG                        "config"
#define XANTE_JTF_INTERNAL                      "internal"
#define XANTE_JTF_APPLICATION                   "application"
#define XANTE_JTF_COMPANY                       "company"
#define XANTE_JTF_DYNAMIC                       "dynamic"
#define XANTE_JTF_COPIES                        "copies"
#define XANTE_JTF_BLOCK_PREFIX                  "block_prefix"
#define XANTE_JTF_ORIGIN                        "origin"
#define XANTE_JTF_EVENTS                        "events"
#define XANTE_JTF_DESCRIPTION                   "description"
#define XANTE_JTF_BRIEF                         "brief"
#define XANTE_JTF_REFERENCED_MENU               "referenced_menu"
#define XANTE_JTF_BLOCKED_KEYS                  "blocked_keys"
#define XANTE_JTF_ESC_KEY                       "esc"
#define XANTE_JTF_SUSPEND_KEY                   "suspend_key"
#define XANTE_JTF_STOP_KEY                      "stop_key"
#define XANTE_JTF_GEOMETRY                      "geometry"
#define XANTE_JTF_WIDTH                         "width"
#define XANTE_JTF_HEIGHT                        "height"
#define XANTE_JTF_DATA                          "data"
#define XANTE_JTF_BTN_OK_LABEL                  "ok"
#define XANTE_JTF_BTN_CANCEL_LABEL              "cancel"
#define XANTE_JTF_BTN_EXTRA_LABEL               "extra"
#define XANTE_JTF_BTN_HELP_LABEL                "help"
#define XANTE_JTF_BUTTONS                       "buttons"
#define XANTE_JTF_TITLE                         "title"
#define XANTE_JTF_BTN_OK                        "btn_ok"
#define XANTE_JTF_BTN_CANCEL                    "btn_cancel"
#define XANTE_JTF_BTN_EXTRA                     "btn_extra"
#define XANTE_JTF_BTN_HELP                      "btn_help"
#define XANTE_JTF_LABELS                        "labels"
#define XANTE_JTF_FILE_REVISION                 "jtf_revision"
#define XANTE_JTF_LANGUAGE                      "language"

/** String keys of supported menus */
#define XANTE_STR_DEFAULT_MENU                  "default"
#define XANTE_STR_DYNAMIC_MENU                  "dynamic"

/* FIXME: Put the library prefix here */
/** Object events */
#define EV_ITEM_SELECTED                        "item-selected"
#define EV_ITEM_VALUE_CONFIRM                   "item-value-confirm"
#define EV_ITEM_VALUE_UPDATED                   "item-value-updated"
#define EV_ITEM_EXIT                            "item-exit"
#define EV_MENU_EXIT                            "menu-exit"
#define EV_CUSTOM                               "custom-event"
#define EV_UPDATE_ROUTINE                       "update-routine"
#define EV_ITEM_CUSTOM_DATA                     "item-custom-data"
#define EV_SYNC_ROUTINE                         "sync-routine"
#define EV_VALUE_STRLEN                         "value-strlen"
#define EV_VALUE_CHECK                          "value-check"
#define EV_EXTRA_BUTTON_PRESSED                 "extra-button-pressed"

/** String keys of a supported widget */
#define XANTE_STR_WIDGET_MENU                   "menu"
#define XANTE_STR_WIDGET_INPUT_INT              "input-int"
#define XANTE_STR_WIDGET_INPUT_FLOAT            "input-float"
#define XANTE_STR_WIDGET_INPUT_DATE             "input-date"
#define XANTE_STR_WIDGET_INPUT_TIME             "input-time"
#define XANTE_STR_WIDGET_INPUT_STRING           "input-string"
#define XANTE_STR_WIDGET_INPUT_PASSWD           "input-passwd"
#define XANTE_STR_WIDGET_CALENDAR               "calendar"
#define XANTE_STR_WIDGET_TIMEBOX                "timebox"
#define XANTE_STR_WIDGET_RADIO_CHECKLIST        "radio-checklist"
#define XANTE_STR_WIDGET_CHECKLIST              "checklist"
#define XANTE_STR_WIDGET_YESNO                  "yesno"
#define XANTE_STR_WIDGET_DYNAMIC_MENU           "dynamic-menu"
#define XANTE_STR_WIDGET_DELETE_DYNAMIC_MENU    "delete-dynamic-menu"
#define XANTE_STR_WIDGET_ADD_DYNAMIC_MENU       "add-dynamic-menu"
#define XANTE_STR_WIDGET_CUSTOM                 "custom"
#define XANTE_STR_WIDGET_PROGRESS               "progress-bar"
#define XANTE_STR_WIDGET_SPINNER_SYNC           "spinner-sync"
#define XANTE_STR_WIDGET_DOTS_SYNC              "dots-sync"
#define XANTE_STR_WIDGET_RANGE                  "range"
#define XANTE_STR_WIDGET_FILE_SELECT            "file-select"
#define XANTE_STR_WIDGET_DIR_SELECT             "dir-select"
#define XANTE_STR_WIDGET_FILE_VIEW              "file-view"
#define XANTE_STR_WIDGET_TAILBOX                "tailbox"
#define XANTE_STR_WIDGET_SCROLLTEXT             "scrolltext"
#define XANTE_STR_WIDGET_UPDATE_OBJECT          "update-object"
#define XANTE_STR_WIDGET_INPUTSCROLL            "inputscroll"
#define XANTE_STR_WIDGET_MIXEDFORM              "mixedform"
#define XANTE_STR_WIDGET_BUILDLIST              "buildlist"
#define XANTE_STR_WIDGET_SPREADSHEET            "spreadsheet"
#define XANTE_STR_GADGET_CLOCK                  "clock"

/** The access mode from a menu or a menu item */
enum XanteAccessMode {
    XanteAccessHidden   = 0,
    XanteAccessView     = (1 << 0),
    XanteAccessEdit     = (1 << 1)
};

#ifdef __cplusplus
}
#endif

#endif

