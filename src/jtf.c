
/*
 * Description: Functions to handle the JSON file from an application.
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

#include <stdlib.h>
#include <string.h>

#include "libxante.h"

/*
 *
 * Internal functions
 *
 */

/* JTF object names */
#define GENERAL             "general"
#define APPLICATION_NAME    "name"
#define PLUGIN_NAME         "plugin"
#define CONFIG_PATHNAME     "config_pathname"
#define LOG_PATHNAME        "log_pathname"
#define LOG_LEVEL           "log_level"
#define JTF_VERSION         "version"
#define JTF_REVISION        "revision"
#define JTF_BUILD           "build"
#define JTF_BETA            "beta"
#define UI                  "ui"
#define MAIN_MENU           "main_menu"
#define MENUS               "menus"
#define NAME                "name"
#define OBJECT_ID           "object_id"
#define MODE                "mode"
#define TYPE                "type"
#define CONFIG_BLOCK        "config_block"
#define CONFIG_ITEM         "config_item"
#define HELP                "help"
#define ITEMS               "items"
#define OPTIONS             "options"
#define DEFAULT_VALUE       "default_value"

static int fill_info(cl_json_t *node, const char *subnode_name, void **data)
{
    cl_json_t *subnode = NULL;
    cl_string_t *value = NULL;
    enum cl_json_type type;

    subnode = cl_json_get_object_item(node, subnode_name);

    if (NULL == subnode) {
        errno_set(XANTE_ERROR_JTF_NO_INFO);
        return -1;
    }

    type = cl_json_get_object_type(subnode);

    if (type == CL_JSON_TRUE) {
        *data = (void **)true;
        goto end_block;
    } else if (type == CL_JSON_FALSE) {
        *data = (void **)false;
        goto end_block;
    }

    value = cl_json_get_object_value(subnode);

    if (NULL == value) {
        errno_set(XANTE_ERROR_JTF_INFO_WITHOUT_VALUE);
        return -1;
    }

    if (type == CL_JSON_STRING)
        *data = cl_string_ref(value);
    else if (type == CL_JSON_NUMBER)
        *data = (void **)cl_string_to_int(value);

end_block:
    return 0;
}

static int parse_jtf_info(cl_json_t *jtf, struct xante_app *xpp)
{
    cl_json_t *general = NULL;

    general = cl_json_get_object_item(jtf, GENERAL);

    if (NULL == general) {
        errno_set(XANTE_ERROR_JTF_NO_GENERAL_OBJECT);
        return -1;
    }

    fill_info(general, APPLICATION_NAME, (void **)&xpp->info.application_name);
    fill_info(general, PLUGIN_NAME, (void **)&xpp->info.plugin_name);
    fill_info(general, CONFIG_PATHNAME, (void **)&xpp->info.cfg_pathname);
    fill_info(general, LOG_PATHNAME, (void **)&xpp->info.log_pathname);
    fill_info(general, LOG_LEVEL, (void **)&xpp->info.log_level);
    fill_info(general, JTF_VERSION, (void **)&xpp->info.version);
    fill_info(general, JTF_REVISION, (void **)&xpp->info.revision);
    fill_info(general, JTF_BUILD, (void **)&xpp->info.build);
    fill_info(general, JTF_BETA, (void **)&xpp->info.beta);

    return 0;
}

static int parse_menu_item(cl_json_t *item, struct xante_menu *menu)
{
    struct xante_item *i;
    cl_string_t *default_value = NULL;

    i = ui_new_xante_item();

    if (NULL == i)
        return -1;

    fill_info(item, NAME, (void **)&i->name);
    fill_info(item, TYPE, (void **)&i->type);
    fill_info(item, MODE, (void **)&i->mode);
    fill_info(item, HELP, (void **)&i->help);
    fill_info(item, OPTIONS, (void **)&i->options);
    fill_info(item, DEFAULT_VALUE, (void **)&default_value);
    fill_info(item, CONFIG_BLOCK, (void **)&i->config_block);
    fill_info(item, CONFIG_ITEM, (void **)&i->config_item);

    ui_adjusts_item_info(i, default_value);
    cl_list_unshift(menu->items, i, -1);

    if (default_value != NULL)
        cl_string_unref(default_value);

    return 0;
}

static int parse_ui_menu(cl_json_t *menu, struct xante_app *xpp)
{
    cl_json_t *items;
    int i, t;
    struct xante_menu *m = NULL;

    m = ui_new_xante_menu(XANTE_MENU_CREATED_FROM_JTF);

    if (NULL == m)
        return -1;

    fill_info(menu, NAME, (void **)&m->name);
    fill_info(menu, OBJECT_ID, (void **)&m->object_id);
    fill_info(menu, MODE, (void **)&m->mode);
    items = cl_json_get_object_item(menu, ITEMS);

    if (NULL == items) {
        errno_set(XANTE_ERROR_JTF_NO_ITEMS_OBJECT);
        return -1;
    }

    t = cl_json_get_array_size(items);

    for (i = 0; i < t; i++)
        if (parse_menu_item(cl_json_get_array_item(items, i), m) < 0)
            return -1;

    cl_list_unshift(xpp->ui.menus, m, -1);

    return 0;
}

static int parse_ui(cl_json_t *jtf, struct xante_app *xpp)
{
    cl_json_t *ui, *node;
    int i, t;

    ui = cl_json_get_object_item(jtf, UI);

    if (NULL == ui) {
        errno_set(XANTE_ERROR_JTF_NO_UI_OBJECT);
        return -1;
    }

    /* menus */
    node = cl_json_get_object_item(ui, MENUS);

    if (NULL == node) {
        errno_set(XANTE_ERROR_JTF_NO_MENUS_OBJECT);
        return -1;
    }

    t = cl_json_get_array_size(node);

    for (i = 0; i < t; i++)
        if (parse_ui_menu(cl_json_get_array_item(node, i), xpp) < 0)
            return -1;

    /* main menu */
    fill_info(ui, MAIN_MENU, (void **)&xpp->ui.main_menu_object_id);

    return 0;
}

static int parse_jtf_json(cl_json_t *jtf, struct xante_app *xpp)
{
    /* Parse main JTF informations */
    if (parse_jtf_info(jtf, xpp) < 0)
        return -1;

    /* Parse the interface (menus and its items) */
    if (parse_ui(jtf, xpp) < 0)
        return -1;

    return 0;
}

/*
 *
 * Internal API
 *
 */

/**
 * @name jtf_parse
 * @brief Parses a JTF to the xante_app structure.
 *
 * This function is responsible to parse all application information from
 * within a JTF file and put them inside a xante_app structure.
 *
 * @param [in] pathname: The JTF pathname.
 * @param [in,out] xpp: The previously created xante_app structure.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int jtf_parse(const char *pathname, struct xante_app *xpp)
{
    cl_json_t *jtf = NULL;
    int ret = -1;

    if ((NULL == pathname) || (NULL == xpp)) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    jtf = cl_json_read_file(pathname);

    if (NULL == jtf) {
        errno_set(XANTE_ERROR_WRONG_JTF_FORMAT);
        return -1;
    }

    /* Translate the JSON format to our menus and its items */
    ret = parse_jtf_json(jtf, xpp);

    /*
     * FIXME: libcollections shares the same errno variable with us,
     *        and it's cleaning it here. We must fix there.
     */
    cl_json_delete(jtf);

    return ret;
}

/**
 * @name jtf_release_info
 * @brief Releases all previously loaded information when parsing the JTF file.
 *
 * @param [in,out] xpp: The previously filled xante_app structure.
 */
void jtf_release_info(struct xante_app *xpp)
{
    if (NULL == xpp)
        return;

    if (xpp->info.cfg_pathname != NULL)
        cl_string_unref(xpp->info.cfg_pathname);

    if (xpp->info.log_pathname != NULL)
        cl_string_unref(xpp->info.log_pathname);

    if (xpp->info.log_level != NULL)
        cl_string_unref(xpp->info.log_level);

    if (xpp->info.application_name != NULL)
        cl_string_unref(xpp->info.application_name);

    if (xpp->info.plugin_name != NULL)
        cl_string_unref(xpp->info.plugin_name);

    if (xpp->info.version != NULL)
        cl_string_unref(xpp->info.version);
}

