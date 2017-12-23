
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

#ifndef _LIBXANTE_WIDGETS_H
#define _LIBXANTE_WIDGETS_H      1

#ifndef LIBXANTE_COMPILE
# ifndef _LIBXANTE_H
#  error "Never use <widgets.h> directly; include <libxante.h> instead."
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
} widget_result_t;

/* A structure to hold session and information of a running dialog */
typedef struct session {
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

    /* spreadsheet session */
    cl_string_t                 *title;
    cl_string_t                 *row_title;
    cl_string_t                 *column_title;
    struct dlgx_spreadsheet_st  *sheet;

    /* Items */
    DIALOG_LISTITEM     *litems;    /** buildlist, checklist, dm_delete, menu */
    DIALOG_FORMITEM     *fitems;    /** mixedform */

    /* Dialog functions to run it */
    int                 (*run)(struct session *);
    bool                (*validate_result)(struct session *);
    bool                (*value_changed)(struct session *);
    void                (*update_value)(struct session *);
} session_t;

/* widgets */
char *widget_statusbar_text(enum xante_widget type, bool editable,
                            bool block_esc_key);

/*
 *
 * Supported basic widgets
 *
 */

/* yesno */
int yesno(session_t *session);
bool yesno_validate_result(session_t *session);
bool yesno_value_changed(session_t *session);
void yesno_update_value(session_t *session);

/* calendar */
int calendar(session_t *session);
bool calendar_value_changed(session_t *session);
void calendar_update_value(session_t *session);

/* timebox */
int timebox(session_t *session);
bool timebox_value_changed(session_t *session);
void timebox_update_value(session_t *session);

/* checklist */
int checklist(session_t *session);
bool checklist_validate_result(session_t *session);
bool checklist_value_changed(session_t *session);
void checklist_update_value(session_t *session);

/* input */
int input(session_t *session);
bool input_validate_result(session_t *session);
bool input_value_changed(session_t *session);
void input_update_value(session_t *session);

/* dm_delete */
int delete_dm(session_t *session);
bool delete_dm_value_changed(session_t *session);

/* dm_add */
int add_dm(session_t *session);
bool add_dm_value_changed(session_t *session);

/* progress */
int progress(session_t *session);

/* sync */
int sync_dialog(session_t *session);

/* file-select */
int fselect(session_t *session);
bool fselect_validate_result(session_t *session);
bool fselect_value_changed(session_t *session);
void fselect_update_value(session_t *session);

/* file-view */
int file_view(session_t *session);

/* tailbox */
int tailbox(session_t *session);

/* scrolltext */
int scrolltext(session_t *session);

/* update-object */
int update_object(session_t *session);

/* mixedform */
int mixedform(session_t *session);
bool mixedform_value_changed(session_t *session);
void ui_save_mixedform_item(struct xante_app *xpp, struct xante_item *item);
void ui_mixedform_load_and_set_value(struct xante_item *item, cl_cfg_file_t *cfg);

/* buildlist */
int buildlist(session_t *session);
bool buildlist_value_changed(session_t *session);

/* spreadsheet */
void ui_spreadsheet_load_and_set_value(struct xante_item *item,
                                       const cl_cfg_file_t *cfg);

void ui_save_spreadsheet_item(struct xante_app *xpp, struct xante_item *item);
int spreadsheet(session_t *session);
bool spreadsheet_value_changed(session_t *session);

#endif

