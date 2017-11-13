
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
void ui_init(struct xante_app *xpp);
void ui_uninit(struct xante_app *xpp);
struct xante_menu *ui_search_menu_by_object_id(const struct xante_app *xpp,
                                               const char *object_id_to_search);

struct xante_menu *ui_search_unref_menu_by_object_id(const struct xante_app *xpp,
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
 * @return On success returns 0 or -1 otherwise.
 */
int xante_ui_run(xante_t *xpp);

#endif

