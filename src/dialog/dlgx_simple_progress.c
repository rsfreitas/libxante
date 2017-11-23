
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

#define MY_LEN                  (MAX_LEN) / 2
#define MIN_HIGH                (4)
#define MIN_WIDE                (10 + 2 * (2 + MARGIN))

/*
 *
 * Internal API
 *
 */

/**
 * @name dlg_simple_progress
 * @brief Creates a progress object.
 *
 * @param [in] title: Object window title.
 * @param [in] cprompt: Object message to be displayed.
 * @param [in] height: Window height.
 * @param [in] width: Window width.
 * @param [in] percent: A number to represent the consumed percentage of
 *                      something.
 *
 * @return Returns DLG_EXIT_OK (always).
 */
int dlg_simple_progress(const char *title, const char *cprompt, int height,
    int width, int percent)
{
    int i, x, y;
    char prompt_buf[MY_LEN];
    char *prompt = dlg_strclone(cprompt);
    WINDOW *dialog;

    curs_set(0);
    dlg_tab_correct_str(prompt);
    dlg_auto_size(title, prompt, &height, &width, MIN_HIGH, MIN_WIDE);
    dlg_print_size(height, width);
    dlg_ctl_size(height, width);

    /* center dialog box on screen */
    x = dlg_box_x_ordinate(width);
    y = dlg_box_y_ordinate(height);

    dialog = dlg_new_window(height, width, y, x);
    (void)werase(dialog);
    dlg_draw_box(dialog, 0, 0, height, width, dialog_attr, border_attr);
    dlg_draw_title(dialog, title);
    wattrset(dialog, dialog_attr);
    dlg_print_autowrap(dialog, prompt, height, width);
    dlg_draw_box(dialog, height - 4, 2 + MARGIN, 2 + MARGIN,
                 width - 2 * (2 + MARGIN), dialog_attr, border_attr);

    (void)wmove(dialog, height - 3, 4);
    wattrset(dialog, title_attr);

    for (i = 0; i < (width - 2 * (3 + MARGIN)); i++)
        (void)waddch(dialog, ' ');

    (void)wmove(dialog, height - 3, (width / 2) - 2);
    (void)wprintw(dialog, "%3d%%", percent);

    x = (percent * (width - 2 * (3 + MARGIN))) / 100;

    if ((title_attr & A_REVERSE) != 0)
        wattroff(dialog, A_REVERSE);
    else
        wattrset(dialog, A_REVERSE);

    (void)wmove(dialog, height - 3, 4);

    for (i = 0; i < x; i++) {
        chtype ch = winch(dialog);

        if (title_attr & A_REVERSE)
            ch &= ~A_REVERSE;

        (void)waddch(dialog, ch);
    }

    (void)wrefresh(dialog);
    curs_set(1);
    dlg_del_window(dialog);

    if (prompt != prompt_buf)
        free(prompt);

    return DLG_EXIT_OK;
}

