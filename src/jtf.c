
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

#define GENERAL             "general"
#define APPLICATION_NAME    "name"
#define PLUGIN_NAME         "plugin"
#define CONFIG_PATHNAME     "config_pathname"
#define JTF_VERSION         "version"
#define JTF_REVISION        "revision"
#define JTF_BUILD           "build"
#define JTF_BETA            "beta"

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

    value = cl_json_get_object_value(subnode);

    if (NULL == value) {
        errno_set(XANTE_ERROR_JTF_INFO_WITHOUT_VALUE);
        return -1;
    }

    type = cl_json_get_object_type(subnode);

    if (type == CL_JSON_STRING)
        *data = strdup(cl_string_valueof(value));
    else if (type == CL_JSON_NUMBER)
        *data = (void **)cl_string_to_int(value);
    else if (type == CL_JSON_TRUE)
        *data = (void **)true;
    else if (type == CL_JSON_FALSE)
        *data = (void **)false;

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
    fill_info(general, JTF_VERSION, (void **)&xpp->info.version);
    fill_info(general, JTF_REVISION, (void **)&xpp->info.revision);
    fill_info(general, JTF_BUILD, (void **)&xpp->info.build);
    fill_info(general, JTF_BETA, (void **)&xpp->info.beta);

    return 0;
}

static int parse_ui(cl_json_t *jtf, struct xante_app *xpp)
{
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
        free(xpp->info.cfg_pathname);

    if (xpp->info.application_name != NULL)
        free(xpp->info.application_name);

    if (xpp->info.plugin_name != NULL)
        free(xpp->info.plugin_name);

    if (xpp->info.version != NULL)
        free(xpp->info.version);
}

