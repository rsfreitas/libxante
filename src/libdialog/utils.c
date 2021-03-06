
/*
 * Description: Utility functions related to libdialog internals to be used
 *              inside the whole library.
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

/*
 *
 * Internal functions
 *
 */

static int count_lines_by_maximum_column(const char *text, int width)
{
    size_t l;

    l = strlen(text);

    return (l / (width - DIALOG_COLUMNS)) +
            ((l % (width - DIALOG_COLUMNS)) ? 1 : 0);
}

static int get_longest_line_length(const cl_string_t *text)
{
    cl_string_t *s = cl_string_dup(text), *tmp;
    cl_stringlist_t *list = NULL;
    int length = 0, i, total;

    cl_string_rplchr(s, XANTE_STR_LINE_BREAK, '\n');
    list = cl_string_split(s, "^");
    total = cl_stringlist_size(list);

    for (i = 0; i < total; i++) {
        tmp = cl_stringlist_get(list, i);

        if (cl_string_length(tmp) > length)
            length = cl_string_length(tmp);

        cl_string_unref(tmp);
    }

    if (list != NULL)
        cl_stringlist_destroy(list);

    cl_string_unref(s);

    return length;
}

/*
 * Calculates the number of lines that @text will have when displayed in the
 * screen with size limited by @t.
 *
 */
static int count_lines(const char *text, struct dlgx_text *t)
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
static void split_text(struct dlgx_text *t, const char *text)
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

static int prepare_dlgx_backtitle(xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;
    cl_string_t *title = NULL, *left = NULL, *format = NULL, *right = NULL;
    unsigned int screen_width = dlg_box_x_ordinate(0) * 2;

    if (xante_runtime_ui_active(xpp) == false)
        return -1;

    if (change_has_occourred(xpp)) {
        right = cl_string_create("[%s*] %s",
                                 (x->auth.name != NULL)
                                    ? cl_string_valueof(x->auth.login_and_source)
                                    : "",
                                 x->info.company);
    } else
        right = cl_string_create("[%s] %s",
                                 (x->auth.name != NULL)
                                    ? cl_string_valueof(x->auth.login_and_source)
                                    : "",
                                 x->info.company);

    left = cl_string_create(cl_tr("%s - Version %s.%d Build %d %s"),
                            x->info.application_name,
                            x->info.version,
                            x->info.revision, x->info.build,
                            (x->info.beta == true) ? "BETA" : "");

    if (screen_width  > (unsigned int)(cl_string_length(left) +
                                       cl_string_length(right)))
    {
        format = cl_string_create("%%s%%%ds",
                                  (screen_width - cl_string_length(left)));
    } else
        format = cl_string_create("%%s - %%s");

    title = cl_string_create(cl_string_valueof(format),
                             cl_string_valueof(left),
                             cl_string_valueof(right));

    if (dialog_vars.backtitle != NULL)
        free(dialog_vars.backtitle);

    dialog_vars.backtitle = strdup(cl_string_valueof(title));

    if (title != NULL)
        cl_string_unref(title);

    if (left != NULL)
        cl_string_unref(left);

    if (right != NULL)
        cl_string_unref(right);

    if (format != NULL)
        cl_string_unref(format);

    return 0;
}

static int clear_backtitle(xante_t *xpp)
{
    if (xante_runtime_ui_active(xpp) == false)
        return -1;

    free(dialog_vars.backtitle);
    dialog_vars.backtitle = NULL;

    return 0;
}

/*
 *
 * Internal API
 *
 */

/**
 * @name dlgx_text_init
 * @brief Initializes a dlgx_text structure with information on how to show
 *        @text into the @window.
 *
 * @param [in] window: The window where text will be showed.
 * @param [out] t: The dlgx_text structure.
 * @param [in] height: The window height.
 * @param [in] width: The window width.
 * @param [in] text: The text to be showed.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int dlgx_text_init(WINDOW *window, struct dlgx_text *t, int height, int width,
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
 * @name dlgx_text_destroy
 * @brief Releases all memory allocated by a dlgx_text structure.
 *
 * @param [in,out] t: The dlgx_text structure.
 */
void dlgx_text_destroy(struct dlgx_text *t)
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
 * @param [in] t: The dlgx_text structure.
 * @param [in] point: Where the text is been printed.
 *
 * @return Returns 1 if the window must be scrolled down/up or 0 if not.
 */
int dlgx_text_print(WINDOW *window, struct dlgx_text *t, int point)
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
int dlgx_cleanup_result(int code, WINDOW *dialog)
{
    if (dialog != 0)
        dlg_del_window(dialog);

    dlg_mouse_free_regions();

    return code;
}

/**
 * @name dlgx_text_clear
 * @brief Clears the text of a window.
 *
 * @param [in] window: The window to be cleared.
 * @param [in] t: The dlgx_text structure.
 */
void dlgx_text_clear(WINDOW *window, struct dlgx_text *t)
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
int dlgx_get_subtitle_lines(const char *s)
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
void dlgx_put_statusbar(const char *text)
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
 * @name dlgx_put_item_brief
 * @brief Writes an item brief help text onto the screen.
 *
 * @param [in] brief: The text.
 */
void dlgx_put_item_brief(const char *brief)
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

/**
 * @name dlgx_uninit
 * @brief Closes libdialog's environment.
 */
void dlgx_uninit(struct xante_app *xpp)
{
    int dialog_return_value = DLG_EXIT_OK;

    clear_backtitle(xpp);
    dlg_killall_bg(&dialog_return_value);

    if (dialog_vars.ok_label != NULL)
        free(dialog_vars.ok_label);

    if (dialog_state.screen_initialized) {
        (void)refresh();
        end_dialog();
    }
}

/**
 * @name dlgx_init
 * @brief Initialize libdialog's environment.
 *
 * @param [in] temporarily: Boolean flag to clear or not internal libdialog
 *                          values.
 */
void dlgx_init(bool temporarily)
{
    memset(&dialog_state, 0, sizeof(dialog_state));

    if (temporarily == false)
        memset(&dialog_vars, 0, sizeof(dialog_vars));

    init_dialog(stdin, stdout);
}

/**
 * @name dlgx_set_backtitle
 * @brief Sets the application backtitle.
 *
 * @param [in] xpp: The main library object.
 */
void dlgx_set_backtitle(struct xante_app *xpp)
{
    dlg_clear();
    prepare_dlgx_backtitle(xpp);
    dlg_put_backtitle();
}

/**
 * @name dlgx_get_item_value_as_text
 * @brief Gives an item current value as a text string.
 *
 * The function checks if the item has a value loaded from the JTF file (default
 * value) or from the configuration file (value).
 *
 * @param [in] item: The item.
 *
 * @return Case it's an item that may hold a value it's returned as a string
 *         (which must be released after use) or NULL otherwise.
 */
char *dlgx_get_item_value_as_text(const struct xante_item *item)
{
    char *text = NULL;
    cl_string_t *value = NULL;
    int index = -1;

    switch (item->widget_type) {
        case XANTE_WIDGET_INPUT_INT:
        case XANTE_WIDGET_INPUT_FLOAT:
        case XANTE_WIDGET_INPUT_DATE:
        case XANTE_WIDGET_INPUT_TIME:
        case XANTE_WIDGET_CALENDAR:
        case XANTE_WIDGET_TIMEBOX:
        case XANTE_WIDGET_INPUT_STRING:
        case XANTE_WIDGET_RANGE:
        case XANTE_WIDGET_INPUTSCROLL:
            value = cl_object_to_cstring(item_value(item));

            if ((value != NULL) && (cl_string_length(value) > 0))
                text = strdup(cl_string_valueof(value));

            break;

        case XANTE_WIDGET_RADIO_CHECKLIST:
            index = CL_OBJECT_AS_INT(item_value(item));
            value = cl_stringlist_get(item->list_items, index);

            if (value != NULL)
                text = strdup(cl_string_valueof(value));

            break;

        case XANTE_WIDGET_YES_NO:
            index = CL_OBJECT_AS_INT(item_value(item));

            if (index == 1)
                text = strdup(cl_tr("Yes"));
            else
                text = strdup(cl_tr("No"));

            break;

        default:
            break;
    }

    if (value != NULL)
        cl_string_unref(value);

    return text;
}

/**
 * @name dlgx_count_lines_by_delimiters
 * @brief Counts the number of lines of a string by looking at some delimiters.
 *
 * The functions assumes that by each delimiter found a new line will be used
 * inside a dialog.
 *
 * @param [in] text: The text which will be evaluated.
 *
 * @return Returns the number of lines which @text will use inside a dialog.
 */
int dlgx_count_lines_by_delimiters(const char *text)
{
    int c = 0;
    cl_string_t *tmp = NULL;

    if (NULL == text)
        return 0; /* No lines */

    tmp = cl_string_create("%s", text);
    c = cl_string_cchr(tmp, '\n') + cl_string_cchr(tmp, XANTE_STR_LINE_BREAK);
    cl_string_unref(tmp);

    if (c == 0)
        return 1; /* No delimiter was found. We have one line. */

    /*
     * Since we have at least one delimiter inside the text, it will (must)
     * not be in the text last position, and with this we will have another
     * line to be counted.
     */
    return c + 1;
}

/**
 * @name dlgx_count_lines
 * @brief Function to count the lines which a string text will have inside a
 *        dialog.
 *
 * @param [in] text: The string text.
 * @param [in] widht: The window width.
 *
 * @return Returns the number of lines for @text.
 */
int dlgx_count_lines(const char *text, int width)
{
    int lines = 0;

    lines = dlgx_count_lines_by_delimiters(text);

    /*
     * If we have just one line, than we try to count them by lookin at the
     * window width limit.
     */
    if (lines == 1)
        lines = count_lines_by_maximum_column(text, width);

    return lines + MSGBOX_HEIGHT_WITHOUT_TEXT;
}

/**
 * @name dlgx_update_cancel_button_label
 * @brief Sets a default value to the cancel button label used inside dialogs.
 *
 * If \a label is NULL a default value is used.
 *
 * @param [in] label: The button label.
 */
void dlgx_update_cancel_button_label(const cl_string_t *label)
{
    if (dialog_vars.cancel_label != NULL)
        free(dialog_vars.cancel_label);

    dialog_vars.cancel_label = strdup((label != NULL) ? cl_string_valueof(label)
                                                      : cl_tr("Back"));
}

/**
 * @name dlgx_update_ok_button_label
 * @brief Sets a default value to the ok button label used inside dialogs.
 *
 * If \a label is NULL a default value is used.
 *
 * @param [in] label: The button label.
 */
void dlgx_update_ok_button_label(const cl_string_t *label)
{
    if (dialog_vars.ok_label != NULL)
        free(dialog_vars.ok_label);

    dialog_vars.ok_label = strdup((label != NULL) ? cl_string_valueof(label)
                                                  : cl_tr("Ok"));
}

/**
 * @name dlgx_update_extra_button_label
 * @brief Sets the label of the Extra button.
 *
 * If \a label is NULL a default value is used.
 *
 * @param [in] label: The button label.
 */
void dlgx_update_extra_button_label(const cl_string_t *label)
{
    if (dialog_vars.extra_label != NULL)
        free(dialog_vars.extra_label);

    dialog_vars.extra_label = strdup((label != NULL) ? cl_string_valueof(label)
                                                     : cl_tr("Extra"));
}

/**
 * @name dlgx_update_help_button_label
 * @brief Sets the label of the Help button.
 *
 * If \a label is NULL a default value is used.
 *
 * @param [in] label: The button label.
 */
void dlgx_update_help_button_label(const cl_string_t *label)
{
    if (dialog_vars.help_label != NULL)
        free(dialog_vars.help_label);

    dialog_vars.help_label = strdup((label != NULL) ? cl_string_valueof(label)
                                                    : cl_tr("Help"));
}

/**
 * @name dlgx_free_input
 * @brief Releases the dialog input buffer previously allocated.
 */
void dlgx_free_input(void)
{
    if (dialog_vars.input_result != NULL) {
        free(dialog_vars.input_result);
        dialog_vars.input_result = NULL;
    }
}

/**
 * @name dlgx_alloc_input
 * @brief Allocates the dialog input buffer.
 *
 * @param [in] bytes: The input size in bytes.
 */
void dlgx_alloc_input(unsigned int bytes)
{
    if (dialog_vars.input_result != NULL)
        dlgx_free_input();

    dialog_vars.input_result = calloc(bytes, sizeof(char));
}

/**
 * @name dlgx_get_input_result
 * @brief Gets libdialog's input result content.
 *
 * @return On success returns a copy of libdialog's input result or NULL
 *         otherwise.
 */
char *dlgx_get_input_result(void)
{
    char *result = NULL;
    size_t size;
    unsigned int last_position;

    result = calloc(1, strlen(dialog_vars.input_result));

    if (NULL == result)
        return NULL;

    /* Removes an invalid char at the input end if there is one there */
    size = strlen(dialog_vars.input_result);
    last_position = size - 1;

    strncpy(result, dialog_vars.input_result,
            (isalnum(dialog_vars.input_result[last_position]) ? size
                                                              : last_position));

    return result;
}

/**
 * @name dlgx_get_input_window_width
 * @brief Gets the width of an input dialog.
 *
 * Function to be used to calculate an input window width.
 *
 * @param [in] item: The input item object.
 *
 * @return Returns the window width.
 */
int dlgx_get_input_window_width(const struct xante_item *item)
{
    int w = 0, item_width;

    if (dlgx_count_lines_by_delimiters(cl_string_valueof(item->options)) > 1)
        return get_longest_line_length(item->options) + WINDOW_COLUMNS;

    w = cl_string_length(item->options);
    item_width = (item->geometry.width == 0) ? DEFAULT_DIALOG_WIDTH
                                             : item->geometry.width;

    if (w < item_width)
        w = item_width;
    else
        w += WINDOW_COLUMNS;

    return w;
}

/**
 * @name dlgx_session_init
 * @brief Initializes the libdialog's environment to run a widget or in its
 *        own naming convention, a dialog.
 */
void dlgx_session_init(struct xante_app *xpp, const struct xante_item *item,
    bool edit_value)
{
    char *text = NULL;

    dlgx_set_backtitle(xpp);
    text = widget_statusbar_text(item->widget_type, edit_value,
                                 xante_runtime_esc_key(xpp));

    if (text != NULL) {
        dlgx_put_statusbar(text);
        free(text);
    }

    if ((item->widget_type == XANTE_WIDGET_CALENDAR) ||
        (item->widget_type == XANTE_WIDGET_TIMEBOX) ||
        (item->widget_type == XANTE_WIDGET_FILE_SELECT) ||
        (item->widget_type == XANTE_WIDGET_DIR_SELECT))
    {
        dlgx_alloc_input(1024);
    }

    if (item->widget_type == XANTE_WIDGET_MIXEDFORM) {
        /* Since we may have a password item, enable '*' on screen */
        dialog_vars.insecure = 1;
    }

    if (item->widget_type != XANTE_WIDGET_TAILBOX) {
        /* Enables the help button */
        if (item->descriptive_help != NULL) {
            dialog_vars.help_button = 1;
            dlgx_update_help_button_label(item->label.help);
        }

        /*
         * Enables the extra button and does not allow this button on
         * yesno dialogs.
         */
        if (item->button.extra &&
            (item->widget_type != XANTE_WIDGET_YES_NO))
        {
            dlgx_update_extra_button_label(item->label.extra);
            dialog_vars.extra_button = 1;
        }
    }

    dlgx_update_ok_button_label(item->label.ok);
    dlgx_update_cancel_button_label(item->label.cancel);
}

/**
 * @name dlgx_session_uninit
 * @brief Clears all relevant libdialog's environment information used to run
 *        a widget.
 */
void dlgx_session_uninit(const struct xante_item *item)
{
    /* Removes the Help button off the screen */
    if (item->descriptive_help != NULL)
        dialog_vars.help_button = 0;

    /* Removes the Extra button off the screen */
    if (item->button.extra)
        dialog_vars.extra_button = 0;

    if ((item->widget_type == XANTE_WIDGET_CALENDAR) ||
        (item->widget_type == XANTE_WIDGET_TIMEBOX) ||
        (item->widget_type == XANTE_WIDGET_FILE_SELECT) ||
        (item->widget_type == XANTE_WIDGET_DIR_SELECT))
    {
        dlgx_free_input();
    }

    dialog_vars.insecure = 0;
}

