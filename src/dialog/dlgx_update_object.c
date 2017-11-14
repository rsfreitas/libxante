
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Mon May  8 21:03:35 2017
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
 * Number of discarded columns both inside the text window.
 */
#define INTERNAL_H_MARGIN                   6

/*
 * Number of discarded lines to know the minimum text window size.
 */
#define INTERNAL_V_MARGIN                   7

/*
 *
 * Internal API
 *
 */

/**
 * @name dlg_update_object
 * @brief Creates a custom object that allow us to display messages inside a
 *        window.
 *
 * This message is created and returned from a function. And the dialog will
 * keep running until the user selects an option to quit.
 *
 * The function to create the message will be called from time to time according
 * an internal timeout. It must also return a char *string which will be freed
 * by calling 'free'.
 *
 * @param [in] width: Window width.
 * @param [in] height: Window height.
 * @param [in] title: Window title.
 * @param [in] subtitle: Window subtitle.
 * @param [in] update_internal: The milliseconds value to call the update_routine.
 * @param [in] update_routine: The routine.
 * @param [in] arg: An argument to the update_routine.
 * @param [in] status: A status returned by the update_routine.
 *
 * @return Returns libdialog's default return values of a selected button.
 */
int dlg_update_object(int width, int height, const char *title,
    const char *subtitle, int update_interval,
    char *(*update_routine)(void *, int *), void *arg, int *status)
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

    WINDOW *dialog, *view;
    const char **buttons_str = dlg_ok_labels();
    int selected_btn=0, result=DLG_EXIT_UNKNOWN, key=0, fkey=0, dlg_x, dlg_y,
        show_buttons=TRUE, cur_x, cur_y, first=TRUE, sublines=0;
    cl_timeout_t *timeout = NULL, *dlg_timeout = NULL;
    struct dlgx_text t;
    char *text;

    selected_btn = dialog_vars.defaultno;
    dlg_y = dlg_box_y_ordinate(height);
    dlg_x = dlg_box_x_ordinate(width);
    timeout = cl_timeout_create(update_interval, CL_TM_MSECONDS);
    sublines = dlgx_get_subtitle_lines(subtitle);

    if (dialog_vars.timeout_secs)
        dlg_timeout = cl_timeout_create(dialog_vars.timeout_secs, CL_TM_SECONDS);

    /* main window */
    dialog = dlg_new_window(height, width, dlg_y, dlg_x);
    dlg_register_window(dialog, "dlg_update_object", dialog_b);
    dlg_draw_box(dialog, 0, 0, height, width, dialog_attr, border_attr);
    dlg_draw_bottom_box(dialog);
    dlg_draw_title(dialog, title);
    wattrset(dialog, dialog_attr);
    dlg_print_autowrap(dialog, subtitle, height, width);

    /* window to display a text with variable length */
    getyx(dialog, cur_y, cur_x);
    (void)cur_x;
    dlg_draw_box(dialog, cur_y + 1, 2, height - (4 + sublines), width - 4,
                         border2_attr, border2_attr);

    view = dlg_sub_window(dialog, height - (6 + sublines), /* height */
                                  width - INTERNAL_H_MARGIN,  /* width */
                                  dlg_y + cur_y + 2, dlg_x + 3);

    dlg_register_window(view, "dlg_update_object", view_b);

    /* inicializa status interno da rotina */
    if (status)
        *status = FALSE;

    while (result == DLG_EXIT_UNKNOWN) {
        if (cl_timeout_expired(timeout) || (first == TRUE)) {
            /* run update_routine */
            text = (*update_routine)(arg, status);

            /* Finishes the dialog by an internal routine "signal" */
            if (status && (*status)) {
                result = DLG_EXIT_FROM_STATUS;
                break;
            }

            if (text != NULL) {
                if (first == FALSE)
                    dlgx_text_destroy(&t);

                if (dlgx_text_init(view, &t, height - INTERNAL_V_MARGIN,
                                   width - INTERNAL_H_MARGIN, text) < 0)
                {
                    break;
                }

                /* clear text from subwin */
                dlgx_text_clear(view, &t);

                /* print update_routine return value */
                dlgx_text_print(view, &t, PRINT_FIRST);
                wrefresh(dialog);

                free(text);
                cl_timeout_reset(timeout, update_interval, CL_TM_MSECONDS);
                first = FALSE;
            }
        }

        if (show_buttons) {
            show_buttons = FALSE;

            dlg_draw_buttons(dialog, height - 2, 0, buttons_str, selected_btn,
                             FALSE, width);
        }

        if (cl_stdin_select() == true) {
            key = dlg_mouse_wgetch_nowait(dialog, &fkey);

            if (key == DLG_EXIT_TIMEOUT) {
                result = DLG_EXIT_TIMEOUT;
                break;
            }

            /* Clears the internal library timeout */
            if (dialog_vars.timeout_secs) {
                cl_timeout_reset(dlg_timeout, dialog_vars.timeout_secs,
                                 CL_TM_SECONDS);
            }

            if (dlg_result_key(key, fkey, &result))
                break;
        } else {
            /* Checks the internal library timeout */
            if (dialog_vars.timeout_secs)
                if (cl_timeout_expired(dlg_timeout) == true) {
                    result = DLG_EXIT_TIMEOUT;
                    break;
                }
        }

        if (fkey) {
            switch (key) {
                case DLGK_FIELD_PREV:
                    show_buttons = TRUE;
                    selected_btn = dlg_prev_ok_buttonindex(selected_btn, 0);
                    break;

                case DLGK_FIELD_NEXT:
                    show_buttons = TRUE;
                    selected_btn = dlg_next_ok_buttonindex(selected_btn, 0);
                    break;

                case DLGK_ENTER:
                    result = (selected_btn >= 0) ? dlg_ok_buttoncode(selected_btn)
                                                 : DLG_EXIT_OK;

                    break;
            }

            fkey = 0;
        }
    }

    if (dlg_timeout != NULL)
        cl_timeout_destroy(dlg_timeout);

    if (timeout != NULL)
        cl_timeout_destroy(timeout);

    if (first == FALSE)
        dlgx_text_destroy(&t);

    dlg_unregister_window(view);
    dlg_del_window(view);

    return dlgx_cleanup_result(result, dialog);
}


