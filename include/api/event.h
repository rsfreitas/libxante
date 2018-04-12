
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Tue Mar 27 11:00:08 -03 2018
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

#ifndef _LIBXANTE_API_EVENT_H
#define _LIBXANTE_API_EVENT_H

#ifndef LIBXANTE_COMPILE
# ifndef _LIBXANTE_H
#  error "Never use <event.h> directly; include <libxante.h> instead."
# endif
#endif

/**
 * @name xante_event_argument
 * @brief Macro to get a reference to an argument inside an exported plugin
 *        function.
 *
 * @param [in] argument_name: The argument name
 *
 * @return Returns the reference to the desired argument.
 */
#define xante_event_argument(argument_name)                                 \
    ({                                                                      \
     void *__p;                                                             \
     cl_plugin_argument_pointer(CL_PLUGIN_ARGUMENT, argument_name, &__p);   \
     __p;                                                                   \
     })

#endif

