
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
#define _LIBXANTE_XT_INTERNAL_H          1

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

/** Application informations loaded from the XML file (tag <general>) */
struct xante_info {
    char            *cfg_pathname;
    char            *application_name;
    char            *plugin_name;
    char            *version;
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
    int                             ui_dialog_timeout;          /** seconds */
    enum xante_config_file_status   config_file_status;
};

/** Library main structure */
struct xante_app {
    struct xante_info       info;
    struct xante_runtime    runtime;
};

#endif

