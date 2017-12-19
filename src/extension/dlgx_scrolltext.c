
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

/* Number of discarded columns inside the text window */
#define INTERNAL_H_MARGIN                   6

/*
 *
 * Internal functions
 *
 */

static void draw_scrollbar(WINDOW *dialog, struct dlgx_text *t, int width,
    int height, int sublines)
{
    int first_data=0;

    if (t->current_line_start_index)
        first_data = 1;
    else
        first_data = 0;

    dlg_draw_scrollbar(dialog, first_data, t->current_line_start_index,
                       t->current_line_final_index, t->total_lines,
                       0, width - 3, 1 + sublines, height - 4, border2_attr,
                       border2_attr);
}

/*
 *
 * Internal API
 *
 */

/**
 * @name dlgx_scrolltext
 * @brief Creates an object to show a static text with scrolling available.
 *
 * @param [in] width: Window width.
 * @param [in] height: Window height.
 * @param [in] title: Window title.
 * @param [in] subtitle: Window subtitle.
 * @param [in] text: The static text.
 *
 * @return Returns libdialog's default return values of a selected button.
 */
int dlgx_scrolltext(int width, int height, const char *title,
    const char *subtitle, const char *text)
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

    WINDOW *dialog, *view=NULL;
    const char **buttons_str = dlg_ok_label();
    int selected_btn=0, result=DLG_EXIT_UNKNOWN, key=0, fkey=0, dlg_x, dlg_y,
        cur_x, cur_y, show_buttons=TRUE, first=FALSE, sublines=0;
    struct dlgx_text t;

    selected_btn = dialog_vars.defaultno;
    dlg_y = dlg_box_y_ordinate(height);
    dlg_x = dlg_box_x_ordinate(width);
    sublines = dlgx_get_subtitle_lines(subtitle);

    if (sublines == 0)
        sublines += 1;

    /* main window */
    dialog = dlg_new_window(height, width, dlg_y, dlg_x);
    dlg_register_window(dialog, "dlg_scrolltext", dialog_b);
    dlg_draw_box2(dialog, 0, 0, height, width, dialog_attr, border_attr,
                  border2_attr);

    dlg_draw_bottom_box2(dialog, border_attr, border2_attr, dialog_attr);
    dlg_draw_title(dialog, title);
    wattrset(dialog, dialog_attr);
    dlg_print_autowrap(dialog, subtitle, height, width);

    /* Internal window to show the text */
    getyx(dialog, cur_y, cur_x);
    (void)cur_x;
    dlg_draw_box(dialog, cur_y + 1, 2, height - (4 + sublines), width - 4,
                 border2_attr, border2_attr);

    view = dlg_sub_window(dialog,
                          height - (6 + sublines),    /* height */
                          width - INTERNAL_H_MARGIN,  /* width */
                          dlg_y + cur_y + 2, dlg_x + 3);

    if (dlgx_text_init(view, &t, height - (6 + sublines),
                       width - INTERNAL_H_MARGIN, text) < 0)
    {
        return -1;
    }

    dlgx_text_print(view, &t, PRINT_FIRST);
    dlg_register_window(view, "dlg_scrolltext", view_b);
    draw_scrollbar(dialog, &t, width, (sublines > 0 ) ? height : height + 1,
                   (sublines > 0) ? sublines : sublines + 1);

    while (result == DLG_EXIT_UNKNOWN) {
        if (show_buttons) {
            show_buttons = FALSE;

            dlg_draw_buttons(dialog, height - 2, 0, buttons_str, selected_btn,
                             FALSE, width);
        }

        if (!first) {
            key = dlg_mouse_wgetch(dialog, &fkey);

#ifdef ALTERANTIVE_DIALOG
            if (key == DLG_EXIT_TIMEOUT) {
                result = DLG_EXIT_TIMEOUT;
                break;
            }
#endif

            if (dlg_result_key(key, fkey, &result))
                break;
        }

        if (fkey) {
            switch (key) {
                case DLGK_FIELD_PREV:
                case DLGK_FIELD_NEXT:
                    show_buttons = TRUE;
                    break;

                case DLGK_ENTER:
                    result = (selected_btn >= 0) ? dlg_ok_buttoncode(selected_btn)
                                                 : DLG_EXIT_OK;

                    break;

                case DLGK_GRID_UP:
                case DLGK_GRID_DOWN:
                    if (dlgx_text_print(view, &t,
                                        (key == DLGK_GRID_UP) ? PRINT_UP : PRINT_DOWN))
                    {
                        draw_scrollbar(dialog, &t, width, height, sublines);
                        wrefresh(dialog);
                    }

                    break;
            }

            fkey = 0;
        }
    }

    dlg_unregister_window(view);
    dlg_del_window(view);
    dlgx_text_destroy(&t);

    return dlgx_cleanup_result(result, dialog);
}

