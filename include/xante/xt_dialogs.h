
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Wed May  3 14:17:57 2017
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

#ifndef _LIBXANTE_XT_DIALOGS_H
#define _LIBXANTE_XT_DIALOGS_H          1

#ifndef LIBXANTE_COMPILE
# ifndef _LIBXANTE_H
#  error "Never use <xt_dialogs.h> directly; include <libxante.h> instead."
# endif
#else

/* Internal library declarations */

#endif

/**
 * @name xante_dlg_set_backtitle
 * @brief Sets the application backtitle to its default value.
 *
 * @param [in,out] xpp: The library main object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int xante_dlg_set_backtitle(xante_t *xpp);

/**
 * @name xante_dlg_clear_backtitle
 * @brief Clears the application backtitle content.
 *
 * @param [in,out] xpp: The library main object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int xante_dlg_clear_backtitle(xante_t *xpp);

/**
 * @name xante_dlg_messagebox
 * @brief Creates a message box dialog.
 *
 * @param [in] xpp: The library main object.
 * @param [in] type: The message box type (info, error, warning).
 * @param [in] buttons: The dialog buttons.
 * @param [in] title: The dialog title.
 * @param [in] message: The dialog message format.
 * @param [in] ...: The dialog message content.
 *
 * @return On success returns the button selected or -1 otherwise.
 */
int xante_dlg_messagebox(struct xante_app *xpp, enum xante_msgbox_type type,
                         enum xante_buttons buttons, const char *title,
                         const char *message, ...);

#endif

