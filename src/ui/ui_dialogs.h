
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
#define _LIBXANTE_UI_DIALOGS_H      1

#ifndef LIBXANTE_COMPILE
# ifndef _LIBXANTE_H
#  error "Never use <ui_dialogs.h> directly; include <libxante.h> instead."
# endif
#endif

/* Main cancel button label */
#define MAIN_MENU_CANCEL_LABEL      "Exit"

/* Minimum width of a window */
#define MINIMUM_WIDTH               60

/* Dialog without text */
#define DIALOG_HEIGHT_WITHOUT_TEXT  6

/* Message box window without text */
#define MSGBOX_HEIGHT_WITHOUT_TEXT  4

/* Form dialog without text */
#define FORM_HEIGHT_WITHOUT_TEXT    7

/* Maximum number of items of a dialog */
#define MAX_DLG_ITEMS               15

/* Window border size */
#define WINDOW_BORDER_SIZE          10

/* Default columns of a dialog */
#define DIALOG_COLUMNS              4

/* Columns of a dialog with an internal dialog */
#define WINDOW_COLUMNS              (DIALOG_COLUMNS + 2)

/* Maximum number of characters that a user may type */
#define MAX_INPUT_VALUE             1024

/* Default ranges */
#define DATE_MAX_INPUT_LENGTH       10
#define TIME_MAX_INPUT_LENGTH       8

/* A new type of data to be used as a return value from the dialog functions. */
typedef struct {
    int     selected_button;
    bool    updated_value;
} ui_return_t;

/* menu */
int ui_dialog_menu(struct xante_app *xpp, cl_list_t *menus,
                   const struct xante_menu *menu, const char *cancel_label);

int ui_dialog_item(struct xante_app *xpp, cl_list_t *menus,
                   struct xante_item *selected_item);

/* yesno */
ui_return_t ui_dialog_yesno(struct xante_app *xpp, struct xante_item *item);

/* calendar */
ui_return_t ui_dialog_calendar(struct xante_app *xpp, struct xante_item *item,
                               bool edit_value);

/* timebox */
ui_return_t ui_dialog_timebox(struct xante_app *xpp, struct xante_item *item,
                              bool edit_value);

/* checklist */
ui_return_t ui_dialog_checklist(struct xante_app *xpp, struct xante_item *item,
                                bool edit_value);

/* passwd */
int ui_dialog_passwd(struct xante_item *item, bool edit_value,
                     char *input, unsigned int input_length,
                     int height, cl_string_t *text);

/* input */
ui_return_t ui_dialog_input(struct xante_app *xpp, struct xante_item *item,
                            bool edit_value);

/* dm_delete */
ui_return_t ui_dialog_delete_dm(struct xante_app *xpp, struct xante_item *item,
                                bool edit_value);

/* dm_add */
ui_return_t ui_dialog_add_dm(struct xante_app *xpp, struct xante_item *item);

/* progress */
ui_return_t ui_dialog_progress(struct xante_app *xpp, struct xante_item *item);

/* sync */
ui_return_t ui_dialog_sync(struct xante_app *xpp, struct xante_item *item);

#endif

