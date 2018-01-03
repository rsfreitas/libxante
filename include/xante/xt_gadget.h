
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Dec 23 10:34:31 2017
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

#ifndef _LIBXANTE_XT_GADGET_H
#define _LIBXANTE_XT_GADGET_H          1

#ifndef LIBXANTE_COMPILE
# ifndef _LIBXANTE_H
#  error "Never use <xt_gadget.h> directly; include <libxante.h> instead."
# endif
#endif

/**
 * @name xante_dlg_messagebox
 * @brief Creates a message box dialog.
 *
 * @param [in] xpp: The library main object.
 * @param [in] type: The message box type (info, error, warning).
 * @param [in] title: The dialog title.
 * @param [in] message: The dialog message format.
 * @param [in] ...: The dialog message content.
 *
 * @return On success returns the button selected or -1 otherwise.
 */
int xante_dlg_messagebox(struct xante_app *xpp, enum xante_msgbox_type type,
                         const char *title, const char *message, ...)
                         __attribute__((format(printf, 4, 5)));

#endif

