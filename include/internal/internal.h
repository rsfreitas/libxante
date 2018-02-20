
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Tue May  2 20:49:45 2017
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

#ifndef _LIBXANTE_XT_INTERNAL_H
#define _LIBXANTE_XT_INTERNAL_H

#ifndef DIALOG_H_included
# include <dialog.h>
#endif

#ifndef DLG_KEYS_H_included
# include <dlg_keys.h>
#endif

#ifndef _SQLITE3_H_
# include <sqlite3.h>
#endif

/*
 * An internal representation of a public function. It does not affect the code
 * or the function visibility. Its objective is only to let clear what is and
 * what is not been exported from library by looking at the code.
 *
 * Every exported function must have this at the beginning of its declaration.
 * Example:
 *
 * __PUB_API__ const char *function(int arg)
 * {
 *      // Body
 * }
 */
#define __PUB_API__

/** Timeout to close a dialog */
#define DEFAULT_INACTIVITY_TIMEOUT              120 /* seconds */

/** Supported events */
#define EV_INIT                                 "xapl_init"
#define EV_UNINIT                               "xapl_uninit"
#define EV_CONFIG_LOAD                          "xapl_config_load"
#define EV_CONFIG_UNLOAD                        "xapl_config_unload"
#define EV_CHANGES_SAVED                        "xapl_changes_saved"

/** Environment variables */
#define ENV_XANTE_DB_PATH                       "XANTE_DB_PATH"
#define ENV_XANTE_CFG_PATH                      "XANTE_CFG_PATH"

/** Different ways of creating menus */
enum xante_menu_creator {
    XANTE_MENU_CREATED_FROM_JTF,
    XANTE_MENU_CREATED_INTERNALLY
};

/**
 * Application informations loaded from the JTF file (general object). These
 * all must be primitive types since libcollections is initialized after load
 * them.
 */
struct xante_info {
    char    *cfg_pathname;
    char    *log_pathname;
    char    *log_level;
    char    *application_name;
    char    *module_name;
    char    *version;
    char    *company;
    char    *description;
    int     revision;
    int     build;
    bool    beta;

    bool    esc_key;
    bool    suspend_key;
    bool    stop_key;
};

/** Application runtime informations */
struct xante_runtime {
    /* Read only */
    enum xante_return_value     exit_value;
    bool                        ui_active;
    bool                        execute_module;
    bool                        user_authentication;
    char                        *caller_name;
    bool                        esc_key;
    bool                        suspend_key;        /* Ctrl + Z */
    bool                        stop_key;           /* Ctrl + C */

    /* Read/Write */
    bool                        discard_changes;
    bool                        discard_changes_on_timeout;
    bool                        force_config_file_saving;
    bool                        show_config_saving_question;
    bool                        accent_characters;
    bool                        close_ui;
    int                         inactivity_timeout;          /** seconds */
};

/** XanteItem's flag to be validated when parsed from a JTF file */
struct flag_parser {
    bool    options;
    bool    referenced_menu;
    bool    config;
    bool    ranges;
};

struct geometry {
    int width;
    int height;
};

struct window_labels {
    cl_string_t *ok;
    cl_string_t *cancel;
    cl_string_t *extra;
    cl_string_t *help;
    cl_string_t *title;
};

struct window_buttons {
    bool    ok;
    bool    cancel;
    bool    extra;
    bool    help;
};

/*
 * This structure must hold every needed state while parsing a JTF file
 * to avoid creating internal (unnecessary) variables and increasing too
 * much the number of function arguments.
 */
struct parser_helper {
    void        *min;
    void        *max;
    void        *brief_options_help;
    void        *options;
    cl_string_t *default_value;
};

/* This structure holds an item's behaviour while running. */
struct widget_behaviour {
    bool        skip_config;
};

/** UI Menu Item informations */
struct xante_item {
    /* JTF objects */
    enum XanteAccessMode    mode;
    cl_string_t             *type;
    cl_string_t             *name;
    cl_string_t             *object_id;
    cl_string_t             *config_block;
    cl_string_t             *config_item;
    cl_string_t             *brief_help;
    cl_string_t             *descriptive_help;
    cl_string_t             *referenced_menu;
    cl_object_t             *default_value;
    cl_json_t               *events;

    /* Ranges */
    cl_object_t             *min;
    cl_object_t             *max;
    int                     string_length;
    cl_spec_t               *value_spec;

    /* Internal */
    cl_string_t             *options;
    cl_object_t             *value;
    cl_stringlist_t         *list_items;
    cl_stringlist_t         *checklist_brief_options;
    cl_stringlist_t         *selected_items;
    cl_json_t               *form_options;
    int                     widget_checklist_type;
    enum xante_widget       widget_type;
    struct flag_parser      flags;
    struct cl_ref_s         ref;
    bool                    cancel_update;
    struct geometry         geometry;
    struct window_labels    label;
    struct window_buttons   button;
    struct parser_helper    __helper;
    struct widget_behaviour behaviour;
};

/** UI Menu informations */
struct xante_menu {
    /* JTF objects */
    cl_string_t                 *name;
    cl_string_t                 *object_id;
    cl_string_t                 *type;
    cl_json_t                   *events;

    /* Dynamic menu details */
    cl_stringlist_t             *dynamic_names;
    cl_string_t                 *dynamic_block_prefix;
    int                         copies;
    cl_string_t                 *dynamic_origin_block;
    cl_string_t                 *dynamic_origin_item;

    /* Internal */
    enum xante_menu_creator     creator;
    enum xante_menu_type        menu_type;
    bool                        move_to_be_released;
    cl_list_t                   *items;
    struct cl_ref_s             ref;
    struct geometry             geometry;
};

/** UI informations */
struct xante_ui {
    /* From JTF */
    cl_string_t             *main_menu_object_id;

    /* Internal */
    cl_list_t               *menus;
    cl_list_t               *unreferenced_menus;
};

struct xante_log {
    cl_log_t                *log;
};

struct xante_module {
    cl_plugin_t             *module;
    cl_plugin_info_t        *info;
    cl_stringlist_t         *functions;
};

struct xante_config {
    cl_cfg_file_t           *cfg_file;
    char                    *filename;
};

struct xante_changes {
    cl_list_t               *user_changes;
};

struct xante_auth {
    cl_string_t                 *username;
    cl_string_t                 *password;
    cl_string_t                 *name;
    cl_string_t                 *login_and_source;
    int                         id_application;
    int                         id_group;
    int                         id_user;
    int                         session_pid;
    enum xante_session          session_type;
    enum xante_session_source   session_source;
    cl_string_t                 *source_description;
    sqlite3                     *db;
};

/** Library main structure */
struct xante_app {
    struct xante_info       info;
    struct xante_runtime    runtime;
    struct xante_ui         ui;
    struct xante_log        log;
    struct xante_config     config;
    struct xante_changes    changes;
    struct xante_module     module;
    struct xante_auth       auth;
    struct cl_ref_s         ref;
};

/** The structure to hold a single widget to run */
struct xante_single_instance_jtf {
    cl_list_t           *menus;
    struct xante_item   *object;
};

/* Just gives us the right item value */
#define item_value(item)            \
    ((item->value != NULL) ? item->value : item->default_value)

/*
 * Internal macros
 */

#define max(a,b)                            ((a) > (b) ? (a) : (b))
#define min(a,b)                            ((a) < (b) ? (a) : (b))
#define bit_test(data, bit)                 ((((data) & (bit)) == (bit)) ? (true) : (false))

/* Exclusive internal library headers */
#include "libdialog.h"
#include "widgets.h"
#include "gadgets.h"

#include "auth.h"
#include "changes.h"
#include "dm.h"
#include "event.h"
#include "instance.h"
#include "internal.h"
#include "item.h"
#include "jtf.h"
#include "log.h"
#include "manager.h"
#include "menu.h"
#include "runtime.h"
#include "session.h"
#include "si_jtf.h"
#include "utils.h"

#endif

