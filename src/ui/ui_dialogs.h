
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

/* Maximum number of characters that a user may type */
#define MAX_INPUT_VALUE             1024

/* Default ranges */
#define DATE_MAX_INPUT_LENGTH       10
#define TIME_MAX_INPUT_LENGTH       8

/* utils.c */
void dialog_uninit(void);
void dialog_init(bool temporarily);
void dialog_set_backtitle(void);
char *dialog_get_item_value_as_text(const struct xante_item *item);
int dialog_count_lines_by_delimiters(const char *text);
int dialog_count_lines(const char *text, int width);
bool dialog_question(struct xante_app *xpp, const char *title, const char *msg,
                     const char *button1_label, const char *button2_label);

void dialog_update_cancel_button_label(void);
void dialog_free_input(void);
void dialog_alloc_input(unsigned int bytes);
char *dialog_get_input_result(void);

/* menu */
int ui_dialog_menu(struct xante_app *xpp, const struct xante_menu *menu,
                   const char *cancel_label, bool remove_item_from_menu,
                   struct xante_item *selected_item);

/* yesno */
bool ui_dialog_yesno(struct xante_app *xpp, struct xante_item *item);

/* calendar */
bool ui_dialog_calendar(struct xante_app *xpp, struct xante_item *item,
                        bool edit_value);

/* timebox */
bool ui_dialog_timebox(struct xante_app *xpp, struct xante_item *item,
                       bool edit_value);

/* checklist */
bool ui_dialog_checklist(struct xante_app *xpp, struct xante_item *item,
                         bool edit_value);

/* passwd */
int ui_dialog_passwd(struct xante_item *item, bool edit_value, char *input,
                     unsigned int input_length, int height, cl_string_t *text);

/* input */
bool ui_dialog_input(struct xante_app *xpp, struct xante_item *item,
                     bool edit_value);

#endif

