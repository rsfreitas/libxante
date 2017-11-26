
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

#ifndef _LIBXANTE_XT_EVENT_H
#define _LIBXANTE_XT_EVENT_H          1

#ifndef LIBXANTE_COMPILE
# ifndef _LIBXANTE_H
#  error "Never use <xt_event.h> directly; include <libxante.h> instead."
# endif
#else

/* Internal library declarations */
int event_init(struct xante_app *xpp, bool use_event);
void event_uninit(struct xante_app *xpp);
int event_call(const char *event_name, struct xante_app *xpp, ...);
void *event_update_routine_data(struct xante_app *xpp, struct xante_item *item);
int event_update_routine(struct xante_app *xpp, struct xante_item *item,
                         void *data);

#endif

/**
 * @name xante_event_argument
 * @brief Gets the real value of an event argument.
 *
 * It is common to see this function used inside modules.
 *
 * @param [in] arg: The event argument.
 * @param [in] data_type: The event argument required field.
 *
 * @return On success returns the required event argument real value or NULL
 *         otherwise.
 */
void *xante_event_argument(xante_event_arg_t *arg,
                           enum xante_event_argument_type data_type);

#endif

