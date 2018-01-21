
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Tue May  2 22:54:08 2017
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

#ifndef _LIBXANTE_XT_INTERNAL_RUNTIME_H
#define _LIBXANTE_XT_INTERNAL_RUNTIME_H

/* Internal library declarations */
void runtime_start(struct xante_app *xpp, const char *caller_name);
void runtime_stop(struct xante_app *xpp);

/* These setters are for read only runtime flags */
void runtime_set_exit_value(struct xante_app *xpp, int exit_value);
void runtime_set_ui_active(struct xante_app *xpp, bool ui_active);
void runtime_set_execute_module(struct xante_app *xpp, bool execute_module);
void runtime_set_user_authentication(struct xante_app *xpp, bool auth);
void runtime_set_esc_key(struct xante_app *xpp, bool enabled);
void runtime_set_suspend_key(struct xante_app *xpp, bool enabled);
void runtime_set_stop_key(struct xante_app *xpp, bool enabled);

#endif

