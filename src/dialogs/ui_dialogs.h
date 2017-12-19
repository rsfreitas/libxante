
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
typedef struct ui_properties {
    bool                editable_value;
    bool                call_item_value_confirm;
    struct xante_app    *xpp;
    struct xante_item   *item;

    /* Window size */
    int                 width;
    int                 height;

    int                 number_of_items;    /** Total number of holded items */
    int                 displayed_items;    /** Number of items available on
                                                screen each time */

    int                 longest_field_name; /** Mixedform's longest field name */
    char                *scroll_content;    /** Scrollable UI text content */
    cl_string_t         *result;            /** A string to hold the result
                                                from a dialog. */

    cl_string_t         *text;              /** A manipulated text to be
                                                displayed on the dialog */

    cl_string_t         *change_item_name;
    cl_string_t         *change_old_value;
    cl_string_t         *change_new_value;

    /* spreadsheet properties */
    cl_string_t                 *title;
    cl_string_t                 *row_title;
    cl_string_t                 *column_title;
    struct dlgx_spreadsheet_st  *sheet;

    /* Items */
    DIALOG_LISTITEM     *litems;    /** buildlist, checklist, dm_delete, menu */
    DIALOG_FORMITEM     *fitems;    /** mixedform */

    /* Required events */
    int                 (*run)(struct ui_properties *);
    bool                (*validate_result)(struct ui_properties *);
} ui_properties_t;

/* yesno */
int yesno(ui_properties_t *properties);
bool yesno_validate_result(ui_properties_t *properties);

/* calendar */
int calendar(ui_properties_t *properties);
bool calendar_validate_result(ui_properties_t *properties);

/* timebox */
int timebox(ui_properties_t *properties);
bool timebox_validate_result(ui_properties_t *properties);

/* checklist */
int checklist(ui_properties_t *properties);
bool checklist_validate_result(ui_properties_t *properties);

/* input */
int input(ui_properties_t *properties);
bool input_validate_result(ui_properties_t *properties);

/* dm_delete */
int delete_dm(ui_properties_t *properties);
bool delete_dm_validate_result(ui_properties_t *properties);

/* dm_add */
int add_dm(ui_properties_t *properties);
bool add_dm_validate_result(ui_properties_t *properties);

/* progress */
int progress(ui_properties_t *properties);

/* sync */
int sync_dialog(ui_properties_t *properties);

/* file-select */
int fselect(ui_properties_t *properties);
bool fselect_validate_result(ui_properties_t *properties);

/* file-view */
int file_view(ui_properties_t *properties);

/* tailbox */
int tailbox(ui_properties_t *properties);

/* scrolltext */
int scrolltext(ui_properties_t *properties);

/* update-object */
int update_object(ui_properties_t *properties);

/* mixedform */
int mixedform(ui_properties_t *properties);
bool mixedform_validate_result(ui_properties_t *properties);
void ui_save_mixedform_item(struct xante_app *xpp, struct xante_item *item);
void ui_mixedform_load_and_set_value(struct xante_item *item, cl_cfg_file_t *cfg);

/* buildlist */
int buildlist(ui_properties_t *properties);
bool buildlist_validate_result(ui_properties_t *properties);

/* spreadsheet */
void ui_spreadsheet_load_and_set_value(struct xante_item *item,
                                       const cl_cfg_file_t *cfg);

void ui_save_spreadsheet_item(struct xante_app *xpp, struct xante_item *item);
int spreadsheet(ui_properties_t *properties);
bool spreadsheet_validate_result(ui_properties_t *properties);

#endif

