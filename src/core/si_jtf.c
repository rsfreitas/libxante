
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

static struct xante_single_instance_jtf *new_xante_single_instance_jtf(void)
{
    struct xante_single_instance_jtf *m = NULL;

    m = calloc(1, sizeof(struct xante_single_instance_jtf));

    if (NULL == m) {
        errno_set(XANTE_ERROR_NO_MEMORY);
        return NULL;
    }

    m->menus = NULL;
    m->object = NULL;

    return m;
}

static int si_jtf_parse(struct xante_single_instance_jtf *si, cl_json_t *jdata)
{
    cl_json_t *object = NULL;
    struct xante_menu *menu = NULL;
    int i, t;

    /* Our first attempt is to load a complete menu */
    object = cl_json_get_object_item(jdata, "menu");

    if (object != NULL) {
        si->menus = cl_list_create(xante_menu_destroy, NULL, NULL, NULL);
        t = cl_json_get_array_size(object);

        for (i = 0; i < t; i++) {
            menu = jtf_parse_menu(cl_json_get_array_item(object, i));

            if (NULL == menu)
                return -1;

            cl_list_unshift(si->menus, menu, -1);
        }

        return 0;
    }

    /* The second is to load a single item */
    object = cl_json_get_object_item(jdata, "item");

    if (object != NULL) {
        si->object = jtf_parse_item(object, true);

        if (NULL == si->object)
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

struct xante_single_instance_jtf *si_jtf_load(const char *si)
{
    cl_json_t *jdata = NULL;
    struct xante_single_instance_jtf *m = NULL;

    jdata = cl_json_parse_string(si);

    if (NULL == jdata) {
        errno_set(XANTE_ERROR_WRONG_JTF_FORMAT);
        return NULL;
    }

    m = new_xante_single_instance_jtf();

    if (NULL == m)
        return NULL;

    /* Parse the MJTF */
    if (si_jtf_parse(m, jdata) < 0) {
        si_jtf_unload(m);
        m = NULL;
    }

    if (jdata != NULL)
        cl_json_delete(jdata);

    return m;
}

void si_jtf_unload(struct xante_single_instance_jtf *si)
{
    if (NULL == si)
        return;

    if (si->menus != NULL)
        cl_list_destroy(si->menus);

    if (si->object != NULL)
        xante_item_unref(si->object);

    free(si);
}

