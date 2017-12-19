
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Mon Nov 27 15:10:13 2017
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

#define DIALOG_WIDTH                    60
#define DIALOG_HEIGHT                   12
#define DIALOG_UPDATE_INTERVAL          500

struct update_routine_arg {
    struct xante_app    *xpp;
    struct xante_item   *item;
    void                *data;
};

/*
 *
 * Internal functions
 *
 */

static char *update_routine(void *arg)
{
    struct update_routine_arg *user_arg = (struct update_routine_arg *)arg;
    char *string = NULL;
    cl_object_t *value = NULL;

    event_call(EV_UPDATE_ROUTINE, user_arg->xpp, user_arg->item,
               user_arg->data);

    /*
     * The user must update the content to be displayed inside the
     * update-routine event.
     */
    value = item_value(user_arg->item);

    if (NULL == value)
        return NULL;

    string = CL_OBJECT_AS_STRING(value);

    return string;
}

/*
 *
 * Internal API
 *
 */

int update_object(ui_properties_t *properties)
{
    struct xante_app *xpp = properties->xpp;
    struct xante_item *item = properties->item;
    struct update_routine_arg user_arg = {
        .xpp = xpp,
        .item = item
    };

    /* Prepare dialog */
    properties->width = (item->geometry.width == 0) ? DIALOG_WIDTH
                                                    : item->geometry.width;

    properties->height = (item->geometry.height == 0) ? DIALOG_HEIGHT
                                                      : item->geometry.height;

    /* Gets the user custom data */
    user_arg.data = event_item_custom_data(xpp, item);

    return dlgx_update_object(properties->width, properties->height,
                              cl_string_valueof(item->name),
                              cl_string_valueof(item->options),
                              DIALOG_UPDATE_INTERVAL,
                              update_routine, &user_arg);
}

