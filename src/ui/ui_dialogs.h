
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

/* Default width of a window */
#define DEFAULT_DIALOG_WIDTH        60

/* Dialog without text */
#define DIALOG_HEIGHT_WITHOUT_TEXT  6

/* Message box window without text */
#define MSGBOX_HEIGHT_WITHOUT_TEXT  4

/* Form dialog without text */
#define FORM_HEIGHT_WITHOUT_TEXT    7

/* Window border size */
#define WINDOW_BORDER_SIZE          10

/* Default fixed columns of a dialog */
#define DIALOG_COLUMNS              4

/* Columns of a dialog with an internal dialog */
#define WINDOW_COLUMNS              (DIALOG_COLUMNS + 2)

/* Maximum number of characters that a user may type */
#define MAX_INPUT_VALUE             1024

/* Default ranges */
#define DATE_MAX_INPUT_LENGTH       10
#define TIME_MAX_INPUT_LENGTH       8

/* A structure to hold every value returned by a previously called dialog */
typedef struct {
    int     selected_button;
    bool    updated_value;

    /*
     * Values used to be written into the log file when a change is made by
     * the user.
     */
    cl_string_t *item_name;
    cl_string_t *old_value;
    cl_string_t *new_value;
} ui_return_t;

/* A structure to hold properties and information of a running dialog */
typedef struct {
    /* Window size */
    int                 width;
    int                 height;

    int                 number_of_items;    /** Total number of holded items */
    int                 displayed_items;    /** Number of items available on
                                                screen each time */

    int                 longest_field_name; /** Mixedform's longest field name */
    char                *scroll_content;    /** Scrollable UI text content */
    cl_string_t         *text;              /** A manipulated text to be
                                                displayed on the dialog */

    /* spreadsheet properties */
    cl_string_t                 *title;
    cl_string_t                 *row_title;
    cl_string_t                 *column_title;
    struct dlgx_spreadsheet_st  *sheet;

    /* Items */
    DIALOG_LISTITEM     *litems;    /** buildlist, checklist, dm_delete, menu */
    DIALOG_FORMITEM     *fitems;    /** mixedform */
} ui_properties_t;

/* A macro to initialize some UI properties correctly */
#define INIT_PROPERTIES(p)                  \
do {                                        \
    memset(&p, 0, sizeof(ui_properties_t)); \
    p.text = NULL;                          \
    p.litems = NULL;                        \
    p.fitems = NULL;                        \
    p.scroll_content = NULL;                \
    p.title = NULL;                         \
    p.row_title = NULL;                     \
    p.column_title = NULL;                  \
    p.sheet = NULL;                         \
} while (0)

/* A macro to release some UI properties correctly */
#define UNINIT_PROPERTIES(p)                        \
do {                                                \
    int i;                                          \
    DIALOG_LISTITEM *item = NULL;                   \
    DIALOG_FORMITEM *fitem = NULL;                  \
                                                    \
    if (p.text != NULL)                             \
        cl_string_unref(p.text);                    \
                                                    \
    if (p.scroll_content != NULL)                   \
        free(p.scroll_content);                     \
                                                    \
    if (p.litems != NULL) {                         \
        for (i = 0; i < p.number_of_items; i++) {   \
            item = &p.litems[i];                    \
            free(item->text);                       \
            free(item->name);                       \
            free(item->help);                       \
        }                                           \
                                                    \
        free(p.litems);                             \
    }                                               \
                                                    \
    if (p.fitems != NULL) {                         \
        for (i = 0; i < p.number_of_items; i++) {   \
            fitem = &p.fitems[i];                   \
            free(fitem->name);                      \
            free(fitem->text);                      \
        }                                           \
                                                    \
        free(p.fitems);                             \
    }                                               \
                                                    \
    if (p.title != NULL)                            \
        cl_string_unref(p.title);                   \
                                                    \
    if (p.row_title != NULL)                        \
        cl_string_unref(p.row_title);               \
                                                    \
    if (p.column_title != NULL)                     \
        cl_string_unref(p.column_title);            \
                                                    \
    if (p.sheet != NULL)                            \
        spreadsheet_st_destroy(p.sheet);            \
} while (0)

/* menu */
int ui_menu(struct xante_app *xpp, cl_list_t *menus,
            const struct xante_menu *menu, const char *cancel_label);

int ui_item(struct xante_app *xpp, cl_list_t *menus,
            struct xante_item *selected_item);

/* yesno */
ui_return_t ui_yesno(struct xante_app *xpp, struct xante_item *item);

/* calendar */
ui_return_t ui_calendar(struct xante_app *xpp, struct xante_item *item,
                        bool edit_value);

/* timebox */
ui_return_t ui_timebox(struct xante_app *xpp, struct xante_item *item,
                       bool edit_value);

/* checklist */
ui_return_t ui_checklist(struct xante_app *xpp, struct xante_item *item,
                         bool edit_value);

/* input */
ui_return_t ui_input(struct xante_app *xpp, struct xante_item *item,
                     bool edit_value);

/* dm_delete */
ui_return_t ui_delete_dm(struct xante_app *xpp, struct xante_item *item,
                         bool edit_value);

/* dm_add */
ui_return_t ui_add_dm(struct xante_app *xpp, struct xante_item *item);

/* progress */
ui_return_t ui_progress(struct xante_app *xpp, struct xante_item *item);

/* sync */
ui_return_t ui_sync(struct xante_app *xpp, struct xante_item *item);

/* file-select */
ui_return_t ui_fselect(struct xante_app *xpp, struct xante_item *item);

/* file-view */
ui_return_t ui_file_view(struct xante_app *xpp, struct xante_item *item);

/* tailbox */
ui_return_t ui_tailbox(struct xante_app *xpp, struct xante_item *item);

/* scrolltext */
ui_return_t ui_scrolltext(struct xante_app *xpp, struct xante_item *item);

/* update-object */
ui_return_t ui_update_object(struct xante_app *xpp, struct xante_item *item);

/* mixedform */
ui_return_t ui_mixedform(struct xante_app *xpp, struct xante_item *item);
void ui_save_mixedform_item(struct xante_app *xpp, struct xante_item *item);
void ui_mixedform_load_and_set_value(struct xante_item *item, cl_cfg_file_t *cfg);

/* buildlist */
ui_return_t ui_buildlist(struct xante_app *xpp, struct xante_item *item);

/* spreadsheet */
void ui_spreadsheet_load_and_set_value(struct xante_item *item, cl_cfg_file_t *cfg);
void ui_save_spreadsheet_item(struct xante_app *xpp, struct xante_item *item);
ui_return_t ui_spreadsheet(struct xante_app *xpp, struct xante_item *item);

#endif

