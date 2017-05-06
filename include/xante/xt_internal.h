
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
#define _LIBXANTE_XT_INTERNAL_H             1

#ifndef DIALOG_H_included
# include <dialog.h>
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
#define UI_DIALOG_TIMEOUT                   120 /* seconds */

/** Different ways of creating menus */
enum xante_menu_creator {
    XANTE_MENU_CREATED_FROM_JTF,
    XANTE_MENU_CREATED_INTERNALLY
};

/** Application informations loaded from the JSON file (general object) */
struct xante_info {
    cl_string_t     *cfg_pathname;
    cl_string_t     *application_name;
    cl_string_t     *plugin_name;
    cl_string_t     *version;
    int             revision;
    int             build;
    bool            beta;
};

/** Application runtime informations */
struct xante_runtime {
    bool                            discard_changes;
    bool                            discard_changes_on_timeout;
    bool                            execute_plugin;
    bool                            create_default_config_file;
    bool                            force_config_file_saving;
    bool                            show_config_saving_question;
    bool                            accent_characters;
    bool                            close_ui;
    bool                            ui_active;
    int                             ui_dialog_timeout;          /** seconds */
    int                             exit_value;
    enum xante_config_file_status   config_file_status;
};

/** UI Menu Item informations */
struct xante_item {
    /* JTF objects */
    enum xante_access_mode  mode;
    enum xante_ui_dialog    type;
    cl_string_t             *name;
    cl_string_t             *object_id;
    cl_string_t             *config_block;
    cl_string_t             *config_item;
    cl_string_t             *help;

    /* Internal */
    cl_object_t             *value;
    struct cl_ref_s         ref;
};

/** UI Menu informations */
struct xante_menu {
    /* JTF objects */
    cl_string_t                 *name;
    cl_string_t                 *object_id;
    enum xante_access_mode      mode;

    /* Internal */
    enum xante_menu_creator     creator;
    bool                        move_to_be_released;
    cl_list_t                   *items;
    struct cl_ref_s             ref;
};

/** UI informations */
struct xante_ui {
    cl_list_t               *menus;
    cl_string_t             *main_menu_object_id;
};

/** Library main structure */
struct xante_app {
    struct xante_info       info;
    struct xante_runtime    runtime;
    struct xante_ui         ui;
    struct cl_ref_s         ref;
};

#endif

