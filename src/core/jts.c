
/*
 * Description: Functions to handle the creation of an object from inside a
 *              module or the library itself.
 *
 * Author: Rodrigo Freitas
 * Created at: Tue Nov 21 19:24:44 2017
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

static struct xante_jts *new_xante_jts(void)
{
    struct xante_jts *m = NULL;

    m = calloc(1, sizeof(struct xante_jts));

    if (NULL == m) {
        errno_set(XANTE_ERROR_NO_MEMORY);
        return NULL;
    }

    m->menus = NULL;
    m->object = NULL;

    return m;
}

static int si_jtf_parse(struct xante_jts *jts, cl_json_t *jdata)
{
    cl_json_t *object = NULL;
    struct xante_menu *menu = NULL;
    int i, t;

    /* Our first attempt is to load a complete menu */
    object = cl_json_get_object_item(jdata, "menu");

    if (object != NULL) {
        jts->menus = cl_list_create(xante_menu_destroy, NULL, NULL, NULL);
        t = cl_json_get_array_size(object);

        for (i = 0; i < t; i++) {
            menu = jtf_parse_menu(cl_json_get_array_item(object, i));

            if (NULL == menu)
                return -1;

            cl_list_unshift(jts->menus, menu, -1);
        }

        return 0;
    }

    /* The second is to load a single item */
    object = cl_json_get_object_item(jdata, "item");

    if (object != NULL) {
        jts->object = jtf_parse_item(object, true);

        if (NULL == jts->object)
            return -1;

        return 0;
    }

    errno_set(XANTE_ERROR_MJTF_NO_OBJECT);
    return -1;
}

/*
 *
 * Internal API
 *
 */

struct xante_jts *jts_load(const char *jts)
{
    cl_json_t *jdata = NULL;
    struct xante_jts *m = NULL;

    jdata = cl_json_parse_string(jts);

    if (NULL == jdata) {
        errno_set(XANTE_ERROR_WRONG_JTF_FORMAT);
        return NULL;
    }

    m = new_xante_jts();

    if (NULL == m)
        return NULL;

    /* Parse the MJTF */
    if (si_jtf_parse(m, jdata) < 0) {
        jts_unload(m);
        m = NULL;
    }

    if (jdata != NULL)
        cl_json_delete(jdata);

    return m;
}

void jts_unload(struct xante_jts *jts)
{
    if (NULL == jts)
        return;

    if (jts->menus != NULL)
        cl_list_destroy(jts->menus);

    if (jts->object != NULL)
        xante_item_unref(jts->object);

    free(jts);
}

