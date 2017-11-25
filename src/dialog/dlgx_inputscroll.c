
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

#define STEXT                               -1

#define VIEW_BINDINGS   \
    DLG_KEYS_DATA(DLGK_GRID_UP, KEY_UP), \
    DLG_KEYS_DATA(DLGK_GRID_DOWN, KEY_DOWN)

#define NAVIGATE_BINDINGS \
    DLG_KEYS_DATA(DLGK_FIELD_NEXT, KEY_RIGHT), \
    DLG_KEYS_DATA(DLGK_FIELD_NEXT, TAB), \
    DLG_KEYS_DATA(DLGK_FIELD_PREV, KEY_LEFT), \
    DLG_KEYS_DATA(DLGK_FIELD_PREV, KEY_BTAB)

/*
 * Number of discarded columns both inside the text window and outsize the
 * input editing window.
 */
#define INTERNAL_H_MARGIN                   6

/*
 * Number of discarded lines to know the minimum text window size.
 */
#define INTERNAL_V_MARGIN                   11

/*
 *
 * Internal functions
 *
 */

/*
 * Moves the cursor to the right position inside the inputbox.
 */
static void update_cursor_position(WINDOW *dialog, char *input_s, int width,
    int height, int chr_offset)
{
    int col_offset=0;

    col_offset = dlg_edit_offset(input_s, chr_offset, width - 6);
    (void)wmove(dialog, (height - 2) - 3, 3 + col_offset);
}

static void draw_scrollbar(WINDOW *dialog, struct dlgx_text *t, int width,
    int height)
{
    int first_data=0;

    if (t->current_line_start_index)
        first_data = 1;
    else
        first_data = 0;

    dlg_draw_scrollbar(dialog, first_data, t->current_line_start_index,
                       t->current_line_final_index, t->total_lines,
                       0, width - 3, 2, height - 8, border2_attr, border2_attr);
}

static int is_only_char(int c)
{
    if (isalnum(c) ||
        isascii(c) ||
        (c == ' '))
    {
        return 1;
    }

    return 0;
}

static int is_valid_char(int c)
{
    if (isalnum(c) ||
        isascii(c) ||
        (c == ' ') ||
        (c == 536) ||   // left
        (c == 537) ||   // right
        (c == 538) ||   // backspace
        (c == 539))     // delete
    {
        return 1;
    }

    return 0;
}

static int is_moving_key(int c)
{
    if ((c == 536) ||   // left
        (c == 537) ||   // right
        (c == 538) ||   // backspace
        (c == 539) ||   // delete
        (c == 542) ||   // home
        (c == 543))     // end
    {
        return 1;
    }

    return 0;
}

static int calc_string_length(const char *s, int (*input_len)(const char *))
{
    int len=0;

    /* TODO: We need to count special chars either */
    if (NULL == input_len)
        len = strlen(s);
    else
        len = (input_len)(s);

    return len;
}

/*
 *
 * Internal API
 *
 */

/**
 * @name dlg_inputbox
 * @brief Creates a widget of input kind with the possibility of show a variable
 *        text size, as an editor, allowing one the scroll it with the keyboard.
 *
 * IMPORTANTE: To use the scroll feature, the subtitle width cannot be larger
 *             to have a line break. This widget is not ready yet for a dynamic
 *             resizing.
 *
 * @param [in] width: The window width.
 * @param [in] height: The window height.
 * @param [in] title: The window title.
 * @param [in] subtitle: The window subtitle.
 * @param [in] input_title: A text to show inside the input box.
 * @param [in] text: The window text.
 * @param [in] max_len: The maximum input length.
 * @param [in,out] input_s: The input/output buffer.
 * @param [in] edit: A boolean flag to edit the item or not.
 * @param [in] input_len: A custom function to calculate the input length.
 * @param [in] input_check: A custom function to validate the input.
 *
 * @return Returns libdialog's default return values of a selected button.
 */
int dlgx_inputbox(int width, int height, const char *title,
    const char *subtitle, const char *input_title, const char *text,
    unsigned int max_len, char *input_s, bool edit,
    int (*input_len)(const char *), int (*input_check)(const char *))
{
    static DLG_KEYS_BINDING dialog_b[] = {
        VIEW_BINDINGS,
        ENTERKEY_BINDINGS,
        NAVIGATE_BINDINGS,
        END_KEYS_BINDING
    };

    static DLG_KEYS_BINDING view_b[] = {
        NAVIGATE_BINDINGS,
        VIEW_BINDINGS,
        END_KEYS_BINDING
    };

    static DLG_KEYS_BINDING editor_b[] = {
        INPUTSTR_BINDINGS,
        ENTERKEY_BINDINGS,
        NAVIGATE_BINDINGS,
        VIEW_BINDINGS,
        END_KEYS_BINDING
    };

    WINDOW *dialog, *t_dlg=NULL, *editor, *view=NULL, *counter;
    const char **buttons_str = dlg_ok_labels();
    int selected_btn=0, result=DLG_EXIT_UNKNOWN, key=0, fkey=0, dlg_x, dlg_y,
        cur_x, cur_y, show_buttons=TRUE, chr_offset=0, first=FALSE, jump_y=0, d=0;
    struct dlgx_text t;
    size_t len=0;
    char s[16]={0}, tmp[2048]={0};
    chtype blocked_attr, form_attr;

    if (edit == true) {
        selected_btn = dialog_vars.defaultno ? dlg_defaultno_button() : STEXT;
        first = (selected_btn == STEXT);
    } else
        selected_btn = dialog_vars.defaultno;

    blocked_attr = dlg_color_pair(COLOR_RED, COLOR_WHITE);
    dlg_y = dlg_box_y_ordinate(height);
    dlg_x = dlg_box_x_ordinate(width);

    /* Main window */
    dialog = dlg_new_window(height, width, dlg_y, dlg_x);
    dlg_register_window(dialog, "dlg_inputbox", dialog_b);
    dlg_draw_box2(dialog, 0, 0, height, width, dialog_attr, border_attr,
                  border2_attr);

    dlg_draw_bottom_box2(dialog, border_attr, border2_attr, dialog_attr);
    dlg_draw_title(dialog, title);
    wattrset(dialog, dialog_attr);
    dlg_print_autowrap(dialog, subtitle, height, width);

    /* Internal window to show the text with variable size */
    if (text != NULL) {
        getyx(dialog, cur_y, cur_x);
        dlg_draw_box(dialog, cur_y + 1, 2, height - 9, width - 4,
                     border2_attr, border2_attr);

        view = dlg_sub_window(dialog,
                              height - INTERNAL_V_MARGIN, /* height */
                              width - INTERNAL_H_MARGIN,  /* width */
                              dlg_y + cur_y + 2, dlg_x + 3);

        if (dlgx_text_init(view, &t, height - INTERNAL_V_MARGIN,
                           width - INTERNAL_H_MARGIN, text) < 0)
        {
            free(input_s);
            return -2;
        }

        dlgx_text_print(view, &t, PRINT_FIRST);
        dlg_register_window(view, "dlg_inputbox", view_b);
        getyx(dialog, cur_y, cur_x);

        /* Aditional message above the input line */
        t_dlg = dlg_sub_window(dialog, 1, width - 5, dlg_y + cur_y + 1, dlg_x + 2);
        dlg_print_autowrap(t_dlg, input_title, cur_y, cur_x);

        jump_y = 1;
    } else
        getyx(dialog, cur_y, cur_x);

    /* Internal window to input */
    dlg_draw_box2(dialog, cur_y + 1 + jump_y, 2, 3, width - 4, dialog_attr,
                  border_attr, border2_attr);

    editor = dlg_sub_window(dialog, 1, width - INTERNAL_H_MARGIN,
                            dlg_y + (cur_y + 2 + jump_y), dlg_x + 3);

    dlg_register_window(editor, "dlg_inputbox", editor_b);

    /* Characters counter */
    counter = dlg_sub_window(dialog, 1, width - 5, dlg_y + cur_y + 3 + jump_y,
                             dlg_x + 3);

    getyx(dialog, cur_y, cur_x);
    len = calc_string_length(input_s, input_len);
    max_len += 1;

    if (len > max_len) {
        input_s[max_len - 1] = '\0';
        len = calc_string_length(input_s, input_len);
    }

    if (edit == true) {
        d = idigits(max_len);
        sprintf(s, "%0*zu/%d", d, len, max_len - 1);
        (void)wmove(counter, 0, width - 6 - strlen(s));
        dlg_print_autowrap(counter, s, cur_y, cur_x);
        wrefresh(counter);
    }

    if (text != NULL)
        draw_scrollbar(dialog, &t, width, height);

    /*
     * If it is not editing the text, we show it before wait for a key, so
     * we can start the dialog with a filled text.
     */
    if (edit == false) {
        dlg_show_string(dialog, input_s, chr_offset, blocked_attr,
                        cur_y - 1, 3, width - 6, FALSE, first);
    }

    while (result == DLG_EXIT_UNKNOWN) {
        if (show_buttons) {
            show_buttons = FALSE;

            update_cursor_position(dialog, input_s, width, height, chr_offset);
            dlg_draw_buttons(dialog, height - 2, 0, buttons_str, selected_btn,
                             FALSE, width);
        }

        if (!first) {
            key = dlg_mouse_wgetch((selected_btn == STEXT) ? editor : dialog,
                                   &fkey);

#ifdef ALTERNATIVE_DIALOG
            if (key == DLG_EXIT_TIMEOUT) {
                result = DLG_EXIT_TIMEOUT;
                break;
            }
#endif

            if (dlg_result_key(key, fkey, &result))
                break;
        }


        if ((edit == true) && (selected_btn == STEXT) &&
            ((len < max_len) || is_moving_key(key)))
        {
            /*
             * Backups the string, since we don't know if its modification
             * was valid.
             */
            sprintf(tmp, "%s", input_s);

            if (dlg_edit_string(input_s, &chr_offset, key, fkey, first)) {
                len = calc_string_length(input_s, input_len);

                if ((len >= max_len) && is_only_char(key)) {
                    /*
                     * Restores the string since it's not worth us to know where
                     * the wrong modification was made.
                     */
                    sprintf(input_s, "%s", tmp);
                    continue;
                }

                if (input_check != NULL) {
#ifdef ALTERNATIVE_DIALOG
                    /*
                     * If we detect something invalid inside the string we can
                     * change its color.
                     */
                    if ((input_check)(input_s) < -1)
                        form_attr = inputbox_error_attr;
                    else
                        form_attr = form_active_text_attr;
#endif
                } else
                    form_attr = form_active_text_attr;

                /* Updates string */
                dlg_show_string(dialog, input_s, chr_offset, form_attr,
                                cur_y - 1, 3, width - 6, FALSE, first);

                /* Checks if we just hit a valid key */
                if (is_valid_char(key)) {
                    sprintf(s, "%0*zu/%d", d, len, max_len - 1);
                    (void)wmove(counter, 0, width - 6 - strlen(s));
                    dlg_print_autowrap(counter, s, cur_y, cur_x);
                    wrefresh(counter);
                }

                /* Repostions the cursor */
                dlg_show_string(dialog, input_s, chr_offset, form_attr,
                                cur_y - 1, 3, width - 6, FALSE, first);

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
                    selected_btn = dlg_prev_ok_buttonindex(selected_btn,
                                                           (edit == true ? STEXT : 0));

                    break;

                case DLGK_FIELD_NEXT:
                    show_buttons = TRUE;
                    selected_btn = dlg_next_ok_buttonindex(selected_btn,
                                                           (edit == true ? STEXT : 0));

                    break;

                case DLGK_ENTER:
                    result = (selected_btn >= 0) ? dlg_ok_buttoncode(selected_btn)
                                                 : DLG_EXIT_OK;

                    break;

                case DLGK_GRID_UP:
                case DLGK_GRID_DOWN:
                    if (text == NULL)
                        break;

                    if (dlgx_text_print(view, &t,
                                        (key == DLGK_GRID_UP) ? PRINT_UP : PRINT_DOWN))
                    {
                        draw_scrollbar(dialog, &t, width, height);
                        update_cursor_position(dialog, input_s, width, height,
                                               chr_offset);

                        wrefresh(dialog);
                    }

                    break;
            }
        }
    }

    dlg_del_window(counter);
    dlg_unregister_window(editor);
    dlg_del_window(editor);

    if (text != NULL) {
        dlg_unregister_window(t_dlg);
        dlg_del_window(t_dlg);

        dlg_unregister_window(view);
        dlg_del_window(view);

        dlgx_text_destroy(&t);
    }

    return dlgx_cleanup_result(result, dialog);
}

