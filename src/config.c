
/*
 * Description: Functions to handle the main configuration file from an
 *              application.
 *
 * Author: Rodrigo Freitas
 * Created at: Tue May  2 21:03:18 2017
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

static cl_cfg_file_t *load_cfg_file(struct xante_app *xpp)
{
    cl_cfg_file_t *cfg = NULL;
    char *pathname = NULL;

    pathname = xante_env_cfg_path();

    if (NULL == pathname)
        pathname = strdup(xpp->info.cfg_pathname);

    asprintf(&xpp->config.filename, "%s/%s.cfg", pathname,
             xpp->info.application_name);

    free(pathname);

    if (access(xpp->config.filename, 0x00) == -1)
        return NULL;

    cfg = cl_cfg_load(xpp->config.filename);

    return cfg;
}

static int load_item_config(cl_list_node_t *node, void *a)
{
    struct xante_item *item = cl_list_node_content(node);
    cl_cfg_file_t *cfg_file = (cl_cfg_file_t *)a;
    cl_cfg_entry_t *key = NULL;

    key = cl_cfg_entry(cfg_file, cl_string_valueof(item->config_block),
                       cl_string_valueof(item->config_item));

    /* Work with the item's default value */
    if (NULL == key)
        return 0;

    item->value = cl_cfg_entry_value(key);

    return 0;
}

static int load_menu_config(cl_list_node_t *node, void *a)
{
    struct xante_menu *menu = cl_list_node_content(node);

    cl_list_map(menu->items, load_item_config, a);

    return 0;
}

static int load_config(struct xante_app *xpp)
{
    cl_cfg_file_t *cfg_file = NULL;

    cfg_file = load_cfg_file(xpp);

    if (NULL == cfg_file) {
        /*
         * We're going to work with the default values loaded from the JTF
         * file and force the configuration to be saved when exiting the
         * application.
         */
        xante_runtime_set_force_config_file_saving(xpp, true);
        dm_init(xpp, NULL);
        goto ok_block;
    }

    /*
     * If we have a dynamic menu we'll need to replicate it, its submenus and
     * items.
     */
    dm_init(xpp, cfg_file);

    /* Load the configuration values */
    cl_list_map(xpp->ui.menus, load_menu_config, cfg_file);

ok_block:
    event_call(EV_CONFIG_LOAD, xpp, cfg_file);
    xpp->config.cfg_file = cfg_file;

    return 0;
}

static bool need_to_write_config_file(struct xante_app *xpp,
    enum xante_return_value ui_return_status)
{
    /* Discard changes? */
    if (xante_runtime_discard_changes(xpp) == true) {
        xante_log_info(cl_tr("Discarding changes"));
        return false;
    }

#ifdef ALTERNATIVE_DIALOG
    /* Discard changes by timeout */
    if ((xante_runtime_discard_changes_on_timeout(xpp) == true) &&
        (ui_return_status == XANTE_RETURN_TIMEOUT))
    {
        xante_log_info(cl_tr("Discarding changes by timeout"));
        return false;
    }
#endif

    /* Did we already save the file? */
    if (ui_return_status == XANTE_RETURN_CONFIG_SAVED) {
        xante_log_info(cl_tr("The config file was already saved"));
        return false;
    }

    /* Do we have any internal modification? */
    if ((change_has_occourred(xpp) == false) &&
        xante_runtime_force_config_file_saving(xpp) == false)
    {
        xante_log_info(cl_tr("No internal change has been made. "
                             "The config file does not need to be save."));

        return false;
    }

    return true;
}

static int save_item_config(cl_list_node_t *node, void *a)
{
    struct xante_item *item = cl_list_node_content(node);
    struct xante_app *xpp = (struct xante_app *)a;
    cl_string_t *value = NULL;

    /* Checks if we can save the item */
    if (item->flags.config == false)
        return 0;

    value = cl_object_to_cstring(item_value(item));
    cl_cfg_set_value(xpp->config.cfg_file,
                     cl_string_valueof(item->config_block),
                     cl_string_valueof(item->config_item),
                     "%s", cl_string_valueof(value));

    xante_log_debug("saving item: %s", cl_string_valueof(value));
    cl_string_unref(value);

    return 0;
}

static int save_menu_config(cl_list_node_t *node, void *a)
{
    struct xante_menu *menu = cl_list_node_content(node);

    cl_list_map(menu->items, save_item_config, a);

    return 0;
}

static int write_config(struct xante_app *xpp)
{
    enum xante_return_value ui_return_status = xante_runtime_exit_value(xpp);

    if (need_to_write_config_file(xpp, ui_return_status) == false)
        goto end_block;

    /* Do we need to ask the user for saving the changes? */
    if (xante_runtime_show_config_saving_question(xpp) == true) {
        if (dlgx_question(xpp, cl_tr("Closing"),
                          cl_tr("Do you want to save all modifications?"),
                          cl_tr("Yes"), cl_tr("No"), NULL) == false)
        {
            runtime_set_exit_value(xpp, XANTE_RETURN_CONFIG_UNSAVED);
            xante_log_info(cl_tr("User chose not to save internal modifications"));

            goto end_block;
        }
    }

    /* We didn't have the configuration file */
    if (NULL == xpp->config.cfg_file)
        xpp->config.cfg_file = cl_cfg_create();

    /* Write configurations */
    cl_list_map(xpp->ui.menus, save_menu_config, xpp);

    cl_cfg_sync(xpp->config.cfg_file, xpp->config.filename);
    runtime_set_exit_value(xpp, XANTE_RETURN_CONFIG_SAVED);

    if (change_has_occourred(xpp) == true)
        event_call(EV_CHANGES_SAVED, xpp, NULL);

end_block:
    event_call(EV_CONFIG_UNLOAD, xpp, xpp->config.cfg_file);

    if (xpp->config.filename != NULL)
        free(xpp->config.filename);

    cl_cfg_unload(xpp->config.cfg_file);

    return 0;
}

/*
 *
 * External API
 *
 */

__PUB_API__ int xante_config_load(xante_t *xpp)
{
    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    return load_config(xpp);
}

__PUB_API__ int xante_config_write(xante_t *xpp)
{
    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    return write_config(xpp);
}

