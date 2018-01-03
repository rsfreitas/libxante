
/*
 * Description: Gadget's dispatch table.
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Dec 23 20:51:46 2017
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

struct dispatch_function {
    int         (*symbol)(struct xante_app *, struct xante_item *, void *);
};

static cl_hashtable_t *__dispatch_table = NULL;

/*
 *
 * Internal functions
 *
 */

static void destroy_dispatch_function(void *a)
{
    struct dispatch_function *df = (struct dispatch_function *)a;

    if (NULL == df)
        return;

    free(df);
}

static struct dispatch_function *new_dispatch_function(int (*symbol)(struct xante_app *,
                                                                     struct xante_item *,
                                                                     void *))
{
    struct dispatch_function *df = NULL;

    df = calloc(1, sizeof(struct dispatch_function));

    if (NULL == df)
        return NULL;

    df->symbol = symbol;

    return df;
}

/*
 *
 * Internal API
 *
 */

int gadget_dispatch_call(const char *function, struct xante_app *xpp,
    struct xante_item *item, void *data)
{
    struct dispatch_function *df = NULL;

    df = cl_hashtable_get(__dispatch_table, function);

    if ((NULL == df) || (NULL == df->symbol)) {
        xante_log_error(cl_tr("[dispatcher]: Function '%s' not found"),
                        function);

        return 0;
    }

    return (df->symbol)(xpp, item, data);
}

void gadget_dispatch_init(void)
{
    if (__dispatch_table != NULL)
        return;

    __dispatch_table = cl_hashtable_init(1024, true, NULL,
                                         destroy_dispatch_function);

    if (NULL == __dispatch_table) {
        return;
    }

    /* Add internal gadgets event functions */
}

void gadget_dispatch_uninit(void)
{
    if (NULL == __dispatch_table)
        return;

    cl_hashtable_uninit(__dispatch_table);
}

void gadget_dispatch_add(const char *function,
    int (*symbol)(struct xante_app *, struct xante_item *, void *))
{
    if (NULL == __dispatch_table)
        return;

    cl_hashtable_put(__dispatch_table, function,
                     new_dispatch_function(symbol));
}

