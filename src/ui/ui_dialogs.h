
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Fri May  5 21:12:51 2017
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

#ifndef _LIBXANTE_UI_DIALOGS_H
#define _LIBXANTE_UI_DIALOGS_H          1

#ifndef LIBXANTE_COMPILE
# ifndef _LIBXANTE_H
#  error "Never use <xt_dialogs.h> directly; include <libxante.h> instead."
# endif
#endif

/* Main cancel button label */
#define MAIN_MENU_CANCEL_LABEL      "Exit"

/* Minimum width of a window */
#define MINIMUM_WIDTH               60

/* Dialog without text */
#define DIALOG_HEIGHT_WITHOUT_TEXT  6

/* Maximum number of items of a dialog */
#define MAX_DLG_ITEMS               15

#define dialog_get_dlg_items(a)     \
    ((a) > MAX_DLG_ITEMS ? MAX_DLG_ITEMS : (a))

/* Window border size */
#define WINDOW_BORDER_SIZE          10

/* Default columns of a dialog */
#define DIALOG_COLUMNS              4

/* Columns of a dialog with an internal dialog */
#define WINDOW_COLUMNS              (DIALOG_COLUMNS + 2)

/* Just gives us the right item value */
#define item_value(item)            \
    ((item->value != NULL) ? item->value : item->default_value)

/* utils.c */
void dialog_set_backtitle(void);
char *dialog_get_item_value_as_text(const struct xante_item *item);

/* menu */
int ui_dialog_menu(struct xante_app *xpp, const struct xante_menu *menu,
                   const char *cancel_label, bool remove_item_from_menu,
                   struct xante_item *selected_item);

#endif

