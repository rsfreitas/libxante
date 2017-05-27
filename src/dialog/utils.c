
/*
 * Description: Utility functions inside the libdialog addons.
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

/*
 *
 * Internal functions
 *
 */

/*
 * Calculates the number of lines that @text will have when displayed in the
 * screen with size limited by @t.
 *
 */
static int count_lines(const char *text, struct dlg_text *t)
{
    int l=0;
    size_t i, len;

    len = strlen(text);

    /*
     * We initialize the array with the inicial index of the first line,
     * which must always be 0 ;-).
     */
    t->line_breaks_inside_text = realloc(t->line_breaks_inside_text,
                                         (l + 1) * sizeof(int));

    if (!t->line_breaks_inside_text)
        return -1;

    t->line_breaks_inside_text[0] = 0;

    /*
     * Stores the index of every line break found in t->line_breaks_inside_text
     * to ease its future copies.
     */
    for (i = 0; i < len; i++)
        if ((text[i] == '\n')) {
            t->line_breaks_inside_text = realloc(t->line_breaks_inside_text,
                                                 (l + 2) * sizeof(int));

            if (!t->line_breaks_inside_text)
                return -1;

            t->line_breaks_inside_text[l + 1] = (text[i] == '\n') ? i + 1 : i;
            l++; /* number of lines */
        }

    if (len % t->maximum_columns_of_line)
        l++;

    return l;
}

/*
 * Splits a string in lines to fit a specific screen width.
 */
static void split_text(struct dlg_text *t, const char *text)
{
    int i, size;

    t->text = malloc(t->total_lines * sizeof(char *));

    if (!t->text)
        return;

    for (i = 0; i < t->total_lines; i++) {
        t->text[i] = calloc((t->maximum_columns_of_line + 1) * sizeof(char),
                            sizeof(char));

        if (!t->text[i])
            return;

        /*
         * Sets the copied line size. If it's the last one, always copy the
         * difference between the total size and its current position.
         */
        if (i == (t->total_lines - 1))
            size = strlen(text) - t->line_breaks_inside_text[i];
        else {
            size = t->line_breaks_inside_text[i + 1] -
                    t->line_breaks_inside_text[i];
        }

        if (size > 1) {
            strncpy(t->text[i], &text[t->line_breaks_inside_text[i]], size);

            /*
             * Removes all \n so it does not interfere while printing into
             * the screen.
             */
            if (t->text[i][size - 1] == '\n')
                t->text[i][size - 1] = '\0';
        }

        if (strlen(t->text[i]) < (size_t)t->maximum_columns_of_line) {
            memset(&t->text[i][strlen(t->text[i])], ' ',
                   t->maximum_columns_of_line - strlen(t->text[i]));
        }
    }
}

/*
 *
 * Internal API
 *
 */

/**
 * @name dlg_text_init
 * @brief Initializes a dlg_text structure with informations on how to show
 *        @text into the @window.
 *
 * @param [in] window: The window where text will be showed.
 * @param [out] t: The dlg_text structure.
 * @param [in] height: The window height.
 * @param [in] width: The window width.
 * @param [in] text: The text to be showed.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int dlg_text_init(WINDOW *window, struct dlg_text *t, int height, int width,
    const char *text)
{
    /* Saves current cursor position. */
    getyx(window, t->y, t->x);

    t->maximum_supported_lines = height;
    t->maximum_columns_of_line = width;
    t->showing_first_line = false;
    t->showing_last_line = false;
    t->current_line_start_index = 0;
    t->current_line_final_index = 0;
    t->line_breaks_inside_text = NULL;

    t->total_lines = count_lines(text, t);

    if (t->total_lines < 0)
        return -1;

    split_text(t, text);

    return 0;
}

/**
 * @name dlg_text_destroy
 * @brief Releases all memory allocated by a dlg_text structure.
 *
 * @param [in,out] t: The dlg_text structure.
 */
void dlg_text_destroy(struct dlg_text *t)
{
    int i;

    free(t->line_breaks_inside_text);

    for (i = 0; i < t->total_lines; i++)
        free(t->text[i]);

    free(t->text);
}

/**
 * @name dlg_print_text
 * @brief Puts a text or a part of that into the screen.
 *
 * @param [in] window: The window where the text will be printed.
 * @param [in] t: The dlg_text structure.
 * @param [in] point: Where the text is been printed.
 *
 * @return Returns 1 if the window must be scrolled down/up or 0 if not.
 */
int dlg_text_print(WINDOW *window, struct dlg_text *t, int point)
{
    int i, limit=0;
    chtype text_attr;

    switch (point) {
        case PRINT_FIRST:
            limit = (t->maximum_supported_lines > t->total_lines)
                            ? t->total_lines
                            : t->maximum_supported_lines;

            break;

        case PRINT_UP:
            if (t->showing_first_line == true)
                return 0;

            t->showing_last_line = false;
            t->current_line_start_index--;
            limit = t->current_line_final_index - 1;
            (void)wmove(window, t->y, t->x);
            break;

        case PRINT_DOWN:
            if (t->showing_last_line == true)
                return 0;

            t->showing_first_line = false;
            t->current_line_start_index++;
            limit = t->current_line_final_index + 1;
            (void)wmove(window, t->y, t->x);
            break;
    }

    /* Changes the current text color */
    text_attr = dlg_color_pair(4, 7);
    wattrset(window, text_attr);

    for (i = t->current_line_start_index; i < limit; i++)
        waddnstr(window, t->text[i], strlen(t->text[i]));

    t->current_line_final_index = i;

    if (t->current_line_final_index == t->total_lines)
        t->showing_last_line = true;

    if (t->current_line_start_index == 0)
        t->showing_first_line = true;

    wnoutrefresh(window);

    return 1; /* may scroll */
}

/**
 * @name dlg_cleanup_result
 * @brief Clears a window content.
 *
 * @param [in] code: The returned code.
 * @param [in] dialog: The window to be cleared.
 *
 * @return Returns @code.
 */
int dlg_cleanup_result(int code, WINDOW *dialog)
{
    if (dialog != 0)
        dlg_del_window(dialog);

    dlg_mouse_free_regions();

    return code;
}

/**
 * @name dlg_text_clear
 * @brief Clears the text of a window.
 *
 * @param [in] window: The window to be cleared.
 * @param [in] t: The dlg_text structure.
 */
void dlg_text_clear(WINDOW *window, struct dlg_text *t)
{
    int i;
    char tmp[256]={0};

    (void)wmove(window, 0, 0);
    memset(tmp, ' ', (t->maximum_columns_of_line >= 256)
                            ? 255
                            : t->maximum_columns_of_line);

    for (i = 0; i < t->total_lines; i++)
        waddnstr(window, tmp, strlen(tmp));

    wnoutrefresh(window);
    (void)wmove(window, 0, 0);
}

/**
 * @name dlg_get_subtitle_lines
 * @brief Gets the number of lines of a string.
 *
 * @param [in] s: The string text.
 *
 * @return Returns the number of lines of @s.
 */
int dlg_get_subtitle_lines(const char *s)
{
    cl_string_t *tmp = NULL;
    int c = 0;

    if (s != NULL) {
        tmp = cl_string_create("%s", s);
        c = cl_string_cchr(tmp, '\n');
        cl_string_unref(tmp);
    }

    return c;
}

/**
 * @name dlg_put_statusbar
 * @brief Draws a statusbar inside a window.
 *
 * @param [in] text: The text to be displayed.
 */
void dialog_put_statusbar(const char *text)
{
    int i;
#ifdef ALTERNATIVE_DIALOG
    chtype attr = statusbar_attr;
#else
    chtype attr = A_NORMAL;
#endif
    int backwidth = dlg_count_columns(text);

    (void)wattrset(stdscr, screen_attr);
    (void)wmove(stdscr, getmaxy(stdscr) - 1, 1);
    dlg_print_text(stdscr, text, COLS - 2, &attr);

    for (i = 0; i < COLS - backwidth; i++)
        (void)waddch(stdscr, ' ');

    (void)wmove(stdscr, getmaxy(stdscr) - 2, 1);

    for (i = 0; i < COLS - 2; i++)
        (void)waddch(stdscr, dlg_boxchar(ACS_HLINE));

    (void)wnoutrefresh(stdscr);
}

/**
 * @name dialog_put_item_brief
 * @brief Writes an item brief help text onto the screen.
 *
 * @param [in] brief: The text.
 */
void dialog_put_item_brief(const char *brief)
{
    int i;
#ifdef ALTERNATIVE_DIALOG
    chtype attr = brief_help_attr;
#else
    chtype attr = A_NORMAL;
#endif
    int backwidth = dlg_count_columns(brief);

    (void)wattrset(stdscr, screen_attr);
    (void)wmove(stdscr, getmaxy(stdscr) - 3, 1);
    dlg_print_text(stdscr, brief, COLS - 2, &attr);

    for (i = 0; i < COLS - backwidth; i++)
        (void)waddch(stdscr, ' ');

    (void)wnoutrefresh(stdscr);
}

