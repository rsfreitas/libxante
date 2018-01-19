
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Tue May  2 21:41:44 2017
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

#ifndef _LIBXANTE_XT_INTERNAL_EVENT_H
#define _LIBXANTE_XT_INTERNAL_EVENT_H

/* Internal library declarations */
int event_init(struct xante_app *xpp, bool use_event);
void event_uninit(struct xante_app *xpp);
int event_call(const char *event_name, struct xante_app *xpp, ...);
void *event_item_custom_data(struct xante_app *xpp, struct xante_item *item);
int event_update_routine(struct xante_app *xpp, struct xante_item *item,
                         void *data);

#endif

