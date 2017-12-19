
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Tue May  2 21:58:43 2017
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

#ifndef _LIBXANTE_XT_UI_H
#define _LIBXANTE_XT_UI_H          1

#ifndef LIBXANTE_COMPILE
# ifndef _LIBXANTE_H
#  error "Never use <xt_ui.h> directly; include <libxante.h> instead."
# endif
#else

/* Internal library declarations */
void ui_xante_item_ref(struct xante_item *item);
void ui_xante_item_unref(struct xante_item *item);
struct xante_item *ui_new_xante_item(void);
void ui_data_init(struct xante_app *xpp);
void ui_data_uninit(struct xante_app *xpp);
struct xante_menu *ui_search_menu_by_object_id(const cl_list_t *menus,
                                               const char *object_id_to_search);

void ui_print_menu_tree(struct xante_app *xpp);
#endif

/**
 * @name xante_ui_run
 * @brief Puts a libxante application to run.
 *
 * It is important to remember that once we call this function we only leave it
 * when closing the application from the UI.
 *
 * @param [in,out] xpp: The library main object.
 *
 * @return Return an exit value indicating what happened inside (see enum
 *         xante_return_value declaration).
 */
enum xante_return_value xante_ui_run(xante_t *xpp);

/**
 * @name xante_ui_run_mjtf
 * @brief Puts a MJTF dialog to run.
 *
 * This function may be called inside a module event function and also, this
 * event may run with an already running application or not. This way this
 * function is in charge to know this situation and does all required
 * initialization to run the dialog.
 *
 * @param [in,out] xpp: The library main object.
 * @param [in] raw_mjtf: The MJTF dialog in a string format.
 *
 * @return Return an exit value indicating what happened inside (see enum
 *         xante_return_value declaration).
 */
enum xante_return_value xante_ui_run_mjtf(xante_t *xpp, const char *raw_mjtf);

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

