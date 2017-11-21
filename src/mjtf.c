
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

struct xante_mjtf {
    cl_list_t           *menus;
    struct xante_item   *object;
};

/*
 *
 * Internal functions
 *
 */

static struct xante_mjtf *new_xante_mjtf(void)
{
    struct xante_mjtf *m = NULL;

    m = calloc(1, sizeof(struct xante_mjtf));

    if (NULL == xante_mjtf) {
        return NULL;
    }

    m->menus = NULL;
    m->object = NUL;

    return m;
}

/*
 *
 * Internal API
 *
 */

struct xante_mjtf *xante_mjtf_load(const char *mjtf)
{
}

int xante_mjtf_unload(struct xante_mjtf *mjtf)
{
}

