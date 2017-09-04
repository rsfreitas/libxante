
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Mon May  8 21:03:34 2017
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

#include "libxante.h"
#include "addon_dialogs.h"

#define VIEW_BINDINGS   \
    DLG_KEYS_DATA(DLGK_GRID_UP, KEY_UP), \
    DLG_KEYS_DATA(DLGK_GRID_DOWN, KEY_DOWN)

#define NAVIGATE_BINDINGS \
    DLG_KEYS_DATA(DLGK_FIELD_NEXT, KEY_RIGHT), \
    DLG_KEYS_DATA(DLGK_FIELD_NEXT, TAB), \
    DLG_KEYS_DATA(DLGK_FIELD_PREV, KEY_LEFT), \
    DLG_KEYS_DATA(DLGK_FIELD_PREV, KEY_BTAB)

/*
 * Number of discarded columns both inside the text window and the input
 * editing window.
 */
#define INTERNAL_H_MARGIN                   6

/*
 * Number of discarded lines to know the minimum text window size.
 */
#define INTERNAL_V_MARGIN                   11

#define TOKEN_CHAR                          ','
#define TOKEN_STRING                        ","
#define MAX_COLUMN_SIZE                     32
#define DEFAULT_CELL_SIZE                   5
#define MAX_CELL_SIZE                       25
#define CELL_HEIGHT                         3

#define spreadsheet_draw_box(data, s)       (data)->box_draw(data, s)

static struct __line_st *line_st_init(const char *text)
{
    struct __line_st *t;
    cl_string_t *line = NULL, *tmp = NULL;
    int i, l = 0, length = 0;

    t = malloc(sizeof(struct __line_st));

    if (NULL == t)
        return NULL;

    line = cl_string_create("%s", text);
    t->s = cl_string_split(line, TOKEN_STRING);
    cl_string_unref(line);

    for (i = 0; i < cl_stringlist_size(t->s); i++) {
        tmp = cl_stringlist_get(t->s, i);
        length = cl_string_length(tmp);

        if (length > l)
            l = length;

        cl_string_unref(tmp);
    }

    if (length == 0)
        t->max_len = DEFAULT_CELL_SIZE;
    else if (length > MAX_CELL_SIZE)
        t->max_len = MAX_CELL_SIZE;
    else
        t->max_len = length;

    return t;
}

static void line_st_destroy(struct __line_st *line)
{
    cl_stringlist_destroy(line->s);
    free(line);
}

static void init_box(spreadsheet_box *box, WINDOW *parent, int x, int y, int width,
    int height, COL_DRAW col_draw, DLG_KEYS_BINDING *binding, int col_labels)
{
    box->parent = parent;
    box->x = x;
    box->y = y;
    box->width = width;
    box->height = height;
    box->box_draw = col_draw;
    box->window = derwin(box->parent, box->height, box->width, box->y, box->x);

    /*
     * XXX: If the window is smaller than expected, the above function will
     *      return NULL.
     */

    if (col_labels)
        y += col_labels;

    box->cur_x = x + 3;
    box->cur_y = y + 3;
    box->chr_offset = strlen(box->data);

    keypad(box->window, TRUE);
    dlg_register_window(box->window, "dlg_spreadsheet", binding);
}

static void box_draw(spreadsheet_box *box, char *data)
{
    /*
     * XXX: We must use menubox_border2_attr because if the cells size. The
     *      cursor is not being displayed sometimes.
     */

    dlg_draw_box2(box->parent, box->y - 1, box->x - 1, box->height + 2,
                  box->width + 2, menubox_border2_attr, menubox_attr,
                  menubox_border_attr);

    wattrset(box->window, item_attr);
    mvwprintw(box->window, 0, 0, "%s", data);
}

static int calc_dlg_width(const char *title, const char *subtitle,
    struct __line_st *row, struct __line_st *col)
{
    size_t l_t, l_s;
    int width=0;

    l_t = strlen(title);
    l_s = strlen(subtitle);
    width = max(l_t, l_s);

    if ((cl_stringlist_size(col->s) == 1) &&
        (width > (col->max_len + 1 + row->max_len + 1)))
    {
        return width;
    }

    /*
     * XXX: The cell size is always the maximum number of supported characters
     *      + 2 (columns).
     */

    return INTERNAL_H_MARGIN + (row->max_len + 1) +
            (cl_stringlist_size(col->s) * (col->max_len + 3));
}

static int calc_dlg_height(struct __line_st *row, int labels)
{
    int h=0;

    h = (cl_stringlist_size(row->s) * CELL_HEIGHT) +
        7 /* Additional window rows */ +
        labels;

    return h;
}

/*
 * TODO: This function needs work. It needs to show in a flexible way the
 *       labels. Allowing breaking the titles in several lines, certifying
 *       that it will be centralized.
 */
static void __dlg_draw_col_labels(WINDOW *dialog, struct dlg_spreadsheet_st *sheet)
{
    char fmt[8];
    int i;
    chtype text_attr;
    cl_string_t *text, *col_title = NULL;;

    col_title = cl_string_create_empty(0);

    for (i = 0; i < cl_stringlist_size(sheet->col_st->s); i++) {
        text = cl_stringlist_get(sheet->col_st->s, i);

        memset(fmt, 0, sizeof(fmt));
        sprintf(fmt, "%%-%ds ", sheet->col_st->max_len + 2);

        cl_string_cat(col_title, fmt, cl_string_valueof(text));
        cl_string_unref(text);
    }

    text_attr = dlg_color_pair(1, 7);
    wattrset(dialog, text_attr);
    cl_string_update_length(col_title);
    cl_string_alltrim(col_title);
    (void)mvwprintw(dialog, 2, sheet->row_st->max_len + 2 + 3, "%s",
                    cl_string_valueof(col_title));

    cl_string_unref(col_title);
}

/*
 * Draws the cells (and row labels).
 */
static int __dlg_draw_spreadsheet_cells(WINDOW *dlg_grid, struct dlg_spreadsheet_st *sheet)
{
    static DLG_KEYS_BINDING cell_b[] = {
        INPUTSTR_BINDINGS,
        ENTERKEY_BINDINGS,
        NAVIGATE_BINDINGS,
        VIEW_BINDINGS,
        END_KEYS_BINDING
    };

    cl_string_t *text = NULL;
    int pos_y=1, pos_x=(sheet->row_st->max_len + 2), w, h=1,
        i, j=0, last_cell=0;

    w = sheet->col_st->max_len;

    for (i = 0; i < sheet->cells; i++) {
        if (i && !(i % cl_stringlist_size(sheet->col_st->s))) {
            pos_x = (sheet->row_st->max_len + 2);
            pos_y += (h + 2);
        }

        init_box(&sheet->cell[i], dlg_grid, pos_x, pos_y, w, h, box_draw,
                 cell_b, sheet->col_labels);

        spreadsheet_draw_box(&sheet->cell[i], sheet->cell[i].data);
        pos_x += (w + 3);

        /*
         * Counts number of cells to move the cursor between the objects
         * inside the main dialog.
         */
        last_cell++;

        if (!(i % cl_stringlist_size(sheet->col_st->s)) && sheet->row_labels) {
            chtype text_attr;
            char fmt[8];

            memset(fmt, 0, sizeof(fmt));
            sprintf(fmt, "%%-0%ds", sheet->row_st->max_len);

            text_attr = dlg_color_pair(4, 7);
            wattrset(dlg_grid, text_attr);
            text = cl_stringlist_get(sheet->row_st->s, j);
            (void)mvwprintw(dlg_grid, pos_y, 0, fmt, cl_string_valueof(text));
            cl_string_unref(text);
            j++;
        }
    }

    last_cell *= (-1);

    return last_cell;
}

/*
 *
 * Internal API
 *
 */

struct dlg_spreadsheet_st *spreadsheet_st_init(const char *row_text,
    const char *col_text)
{
    struct dlg_spreadsheet_st *t;
    cl_string_t *text = NULL;

    t = malloc(sizeof(struct dlg_spreadsheet_st));

    if (NULL == t)
        return NULL;

    t->row_labels = 0;
    t->col_labels = 0;
    t->col_st = line_st_init(col_text);

    if (NULL == t->col_st)
        goto col_error_block;
    else {
        text = cl_stringlist_get(t->col_st->s, 0);

        if (cl_string_length(text) > 0)
            t->col_labels = 1;

        cl_string_unref(text);
    }

    t->row_st = line_st_init(row_text);

    if (NULL == t->row_st)
        goto row_error_block;
    else {
        text = cl_stringlist_get(t->row_st->s, 0);

        if (cl_string_length(text) <= 0)
            t->row_st->max_len = 1;
        else
            t->row_labels = 1;

        cl_string_unref(text);
    }

    t->row = cl_stringlist_size(t->row_st->s);
    t->col = cl_stringlist_size(t->col_st->s);
    t->cells = (t->row * t->col);
    t->cell = malloc(sizeof(spreadsheet_box) * t->cells);

    if (NULL == t->cell)
        goto cell_error_block;

    t->row_text = strdup(row_text);
    t->col_text = strdup(col_text);

    return t;

cell_error_block:
    line_st_destroy(t->row_st);

row_error_block:
    line_st_destroy(t->col_st);

col_error_block:
    free(t);

    return NULL;
}

void spreadsheet_st_destroy(struct dlg_spreadsheet_st *sheet)
{
    line_st_destroy(sheet->row_st);
    line_st_destroy(sheet->col_st);

    free(sheet->col_text);
    free(sheet->row_text);
    free(sheet->cell);
    free(sheet);
}

int spreadsheet_st_add_data(struct dlg_spreadsheet_st *sheet, int pos,
    const char *fmt, ...)
{
    va_list ap;

    if ((pos < 0) || (pos > sheet->cells))
        return -1;

    memset(sheet->cell[pos].data, 0, MAX_CELL_DATA);

    va_start(ap, fmt);
    vsnprintf(sheet->cell[pos].data, MAX_CELL_DATA, fmt, ap);
    va_end(ap);

    return 0;
}

/**
 * @name dlg_spreadsheet
 * @brief Creates a spreadsheet dialog.
 *
 * XXX: When we increase the size of a column, the cursor is not being
 *      displayed, but it is properly positioned.
 *
 * @param [in] title: Window title.
 * @param [in] subtitle: Window subtitle.
 * @param [in,out] sheet: The spreadsheet data.
 *
 * @return Returns libdialog's default return values of a selected button.
 */
int dlg_spreadsheet(const char *title, const char *subtitle,
    struct dlg_spreadsheet_st *sheet)
{
    static DLG_KEYS_BINDING dialog_b[] = {
        VIEW_BINDINGS,
        ENTERKEY_BINDINGS,
        NAVIGATE_BINDINGS,
        END_KEYS_BINDING
    };

    static DLG_KEYS_BINDING grid_b[] = {
        VIEW_BINDINGS,
        ENTERKEY_BINDINGS,
        NAVIGATE_BINDINGS,
        END_KEYS_BINDING
    };

    const char **buttons_str = dlg_ok_labels();
    int width, height, selected_btn=0, result=DLG_EXIT_UNKNOWN, key=0, fkey=0, dlg_x,
        dlg_y, cur_x=0, cur_y, show_buttons=TRUE, first=FALSE, last_cell=0;
    WINDOW *dialog, *dlg_grid;
    spreadsheet_box *b;

    if (sheet == NULL)
        return -1;

    cur_x = cur_x; /* XXX: Just to remove the warning */

    width = calc_dlg_width(title, subtitle, sheet->row_st, sheet->col_st);
    height = calc_dlg_height(sheet->row_st, sheet->col_labels);

    dlg_y = dlg_box_y_ordinate(height);
    dlg_x = dlg_box_x_ordinate(width);

    /* main window */
    dialog = dlg_new_window(height, width, dlg_y, dlg_x);
    dlg_register_window(dialog, "dlg_spreadsheet", dialog_b);
    dlg_draw_box(dialog, 0, 0, height, width, dialog_attr, border_attr);
    dlg_draw_bottom_box(dialog);
    dlg_draw_title(dialog, title);
    wattrset(dialog, dialog_attr);
    dlg_print_autowrap(dialog, subtitle, height, width);

    /* grid */
    getyx(dialog, cur_y, cur_x);
    dlg_draw_box(dialog, cur_y + 1 + sheet->col_labels, 2,
                 height - 5 - sheet->col_labels, width - 4,
                 menubox_border_attr, menubox_attr);

    dlg_grid = dlg_sub_window(dialog, height - 7, width - INTERNAL_H_MARGIN,
                              dlg_y + cur_y + 2 + sheet->col_labels, dlg_x + 3);

    dlg_register_window(dlg_grid, "dlg_spreadsheet", grid_b);

    if (sheet->col_labels)
        __dlg_draw_col_labels(dialog, sheet);

    last_cell = __dlg_draw_spreadsheet_cells(dlg_grid, sheet);

    while (result == DLG_EXIT_UNKNOWN) {
        if (show_buttons) {
            show_buttons = FALSE;

            if (selected_btn < 0) {
                b = &sheet->cell[sheet->cells - abs(selected_btn)];
                wmove(dialog, b->cur_y, b->cur_x);
            }

            dlg_draw_buttons(dialog, height - 2, 0, buttons_str, selected_btn,
                             FALSE, width);
        }

        if (!first) {
            b = &sheet->cell[sheet->cells - abs(selected_btn)];
            key = dlg_mouse_wgetch((selected_btn < 0) ? b->window : dialog, &fkey);

            if (key == DLG_EXIT_TIMEOUT) {
                result = DLG_EXIT_TIMEOUT;
                break;
            }

            if (dlg_result_key(key, fkey, &result))
                break;
        }

        if (selected_btn < 0) {
            b = &sheet->cell[sheet->cells - abs(selected_btn)];

            if (dlg_edit_string(b->data, &b->chr_offset, key, fkey, first)) {
                dlg_show_string(b->parent, b->data, b->chr_offset,
                                form_active_text_attr,
                                b->cur_y - 3 - sheet->col_labels, b->cur_x - 3,
                                sheet->col_st->max_len, FALSE, first);

                first = FALSE;
                continue;
            } else if (first) {
                first = FALSE;
                continue;
            }
        }

        if (fkey) {
            switch (key) {
                case DLGK_FIELD_PREV:
                    show_buttons = TRUE;
                    selected_btn = dlg_prev_ok_buttonindex(selected_btn, last_cell);
                    break;

                case DLGK_FIELD_NEXT:
                    show_buttons = TRUE;
                    selected_btn = dlg_next_ok_buttonindex(selected_btn, last_cell);
                    break;

                case DLGK_ENTER:
                    result = (selected_btn >= 0) ? dlg_ok_buttoncode(selected_btn)
                                                 : DLG_EXIT_OK;

                    break;
            }
        }
    }

    return dlg_cleanup_result(result, dialog);
}

