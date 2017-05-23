
/*
 * Description: API for our custom libdialog objects.
 *
 * Author: Rodrigo Freitas
 * Created at: Mon May  8 21:05:29 2017
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

#ifndef _LIBXANTE_ADDON_DIALOGS_H
#define _LIBXANTE_ADDON_DIALOGS_H           1

#ifndef LIBXANTE_COMPILE
# ifndef _LIBXANTE_H
#  error "Never use <addon_dialogs.h> directly; include <libxante.h> instead."
# endif
#endif

#define MAX_CELL_DATA                       512

enum dlg_text_print_position {
    PRINT_FIRST,
    PRINT_UP,
    PRINT_DOWN
};

struct __spreadsheet_box;
typedef void (*COL_DRAW) (struct __spreadsheet_box *, char *);

typedef struct __spreadsheet_box {
    WINDOW      *parent;
    WINDOW      *window;
    int         x;
    int         y;
    int         width;
    int         height;
    int         cur_x;
    int         cur_y;
    int         chr_offset;
    char        data[MAX_CELL_DATA + 1];
    COL_DRAW    box_draw;
} spreadsheet_box;

struct __line_st {
    cl_string_list_t    *s;
    int                 max_len;
};

struct dlg_spreadsheet_st {
    /* dialog size */
    int                 row;
    int                 col;
    int                 cells; /* Numeber of cells (row * qtd). */

    /* labels */
    int                 col_labels; /* 0 - no, 1 - yes */
    int                 row_labels; /* 0 - no, 1 - yes */
    char                *row_text;
    char                *col_text;
    struct __line_st    *col_st;
    struct __line_st    *row_st;

    /* cells */
    spreadsheet_box     *cell;
};

/** Text to use inside inputbox dialog */
struct dlg_text {
    int     total_lines;
    int     current_line_start_index;
    int     current_line_final_index;

    /* Internal */
    int     x;
    int     y;
    int     maximum_supported_lines;
    int     maximum_columns_of_line;
    bool    showing_first_line;
    bool    showing_last_line;
    int     *line_breaks_inside_text;
    char    **text;
};

/* utils */
int dlg_text_init(WINDOW *window, struct dlg_text *dlg_text, int height,
                  int width, const char *text);

void dlg_text_destroy(struct dlg_text *dlg_text);
int dlg_text_print(WINDOW *window, struct dlg_text *dlg_text, int point);
void dlg_text_clear(WINDOW *window, struct dlg_text *dlg_text);
int dlg_get_subtitle_lines(const char *s);
int dlg_cleanup_result(int code, WINDOW *dialog);
void dialog_put_statusbar(const char *text);
void dialog_put_item_brief(const char *brief);

/* simple gauge */
int dlg_simple_gauge(const char *title, const char *cprompt, int height,
                     int width, int percent);

/* scrolltext */
int dlg_scrolltext(int width, int height, const char *title,
                   const char *subtitle, const char *text);

/* inputbox */
int dlg_inputbox(int width, int height, const char *title,
                 const char *subtitle, const char *input_title,
                 const char *text, unsigned int max_len, char *input_s,
                 bool edit, int (*input_len)(const char *),
                 int (*input_check)(const char *));

/* spreadsheet */
struct dlg_spreadsheet_st *spreadsheet_st_init(const char *row_text,
                                               const char *col_text);

void spreadsheet_st_destroy(struct dlg_spreadsheet_st *table);
int spreadsheet_st_add_data(struct dlg_spreadsheet_st *table, int pos,
                            const char *fmt, ...);

int dlg_spreadsheet(const char *title, const char *subtitle,
                    struct dlg_spreadsheet_st *table);

/* update object */
int dlg_update_object(int width, int height, const char *title,
                      const char *subtitle, int update_interval,
                      char *(*update_routine)(void *, int *), void *arg,
                      int *status);

#endif

