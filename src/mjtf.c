
/*
 * Description:
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

static struct xante_mjtf *new_xante_mjtf(void)
{
    struct xante_mjtf *m = NULL;

    m = calloc(1, sizeof(struct xante_mjtf));

    if (NULL == m) {
        return NULL;
    }

    m->menus = NULL;
    m->object = NULL;

    return m;
}

static int mjtf_parse(struct xante_mjtf *mjtf, cl_json_t *jdata)
{
    cl_json_t *object = NULL;
    struct xante_menu *menu = NULL;
    int i, t;

    /* Our first attempt is to load a complete menu */
    object = cl_json_get_object_item(jdata, "menu");

    if (object != NULL) {
        mjtf->menus = cl_list_create(xante_menu_destroy, NULL, NULL, NULL);
        t = cl_json_get_array_size(object);

        for (i = 0; i < t; i++) {
            menu = jtf_parse_menu(cl_json_get_array_item(object, i));

            if (NULL == menu) {
                return -1;
            }

            cl_list_unshift(mjtf->menus, menu, -1);
        }

        return 0;
    }

    /* The second is to load a single item */
    object = cl_json_get_object_item(jdata, "item");

    if (object != NULL) {
        mjtf->object = jtf_parse_item(object);

        if (NULL == mjtf->object) {
            return -1;
        }

        return 0;
    }

    return -1;
}

/*
 *
 * Internal API
 *
 */

struct xante_mjtf *mjtf_load(const char *mjtf)
{
    cl_json_t *jdata = NULL;
    struct xante_mjtf *m = NULL;

    jdata = cl_json_parse_string(mjtf);

    if (NULL == jdata) {
        return NULL;
    }

    m = new_xante_mjtf();

    if (NULL == m) {
        return NULL;
    }

    /* Parse the MJTF */
    if (mjtf_parse(m, jdata) < 0) {
    }

    if (jdata != NULL)
        cl_json_delete(jdata);

    return m;
}

void mjtf_unload(struct xante_mjtf *mjtf)
{
    if (NULL == mjtf)
        return;

    if (mjtf->menus != NULL)
        cl_list_destroy(mjtf->menus);

    if (mjtf->object != NULL)
        xante_item_unref(mjtf->object);

    free(mjtf);
}

