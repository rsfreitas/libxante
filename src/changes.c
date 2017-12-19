
/*
 * Description: Functions to handle changes made by an user inside an
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

static void destroy_change(void *a)
{
    struct xante_change_entry *c = (struct xante_change_entry *)a;

    if (NULL == c)
        return;

    if (c->item_name != NULL)
        free(c->item_name);

    if (c->old_value != NULL)
        free(c->old_value);

    if (c->new_value != NULL)
        free(c->new_value);

    free(c);
}

static struct xante_change_entry *new_change(void)
{
    struct xante_change_entry *c = NULL;

    c = calloc(1, sizeof(struct xante_change_entry));

    if (NULL == c) {
        errno_set(XANTE_ERROR_NO_MEMORY);
        return NULL;
    }

    return c;
}

/*
 *
 * Internal API
 *
 */

/**
 * @name change_has_occourred
 * @brief Checks if the user has made any configuration change.
 *
 * @param [in] xpp: The main library object.
 *
 * @return Returns true if we have at least one change made or false otherwise.
 */
bool change_has_occourred(struct xante_app *xpp)
{
    if (cl_list_size(xpp->changes.user_changes) > 0)
        return true;

    return false;
}

/**
 * @name change_init
 * @brief Starts the internal changes monitoring.
 *
 * @param [in,out] xpp: The main library object.
 */
void change_init(struct xante_app *xpp)
{
    xpp->changes.user_changes = cl_list_create(destroy_change, NULL, NULL,
                                               NULL);
}

/**
 * @name change_uninit
 * @brief Finishes the internal changes monitoring.
 *
 * @param [in,out] xpp: The main library object.
 */
void change_uninit(struct xante_app *xpp)
{
    cl_list_destroy(xpp->changes.user_changes);
}

/**
 * @name change_add
 * @brief Adds a configuration change made by the user.
 *
 * @param [in,out] xpp: The main library object.
 * @param [in] item_name: The item name which was changed.
 * @param [in] old_value: The old item value.
 * @param [in] new_value: The new item value.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int change_add(struct xante_app *xpp, const char *item_name,
    const char *old_value, const char *new_value)
{
    struct xante_change_entry *c = NULL;

    xante_log_info("%s: %d, %s, %s, %s", __FUNCTION__, (xpp == NULL),
            item_name, old_value, new_value);

    if ((NULL == xpp) || (NULL == item_name) || (NULL == old_value) ||
        (NULL == new_value))
    {
        return -1;
    }

    c = new_change();

    if (NULL == c)
        return -1;

    c->item_name = strdup(item_name);
    c->old_value = strdup(old_value);
    c->new_value = strdup(new_value);

    cl_list_unshift(xpp->changes.user_changes, c, -1);
    xante_log_debug(cl_tr("Change [%s] -> old [%s] new [%s]"),
                    item_name, old_value, new_value);

    return 0;
}

