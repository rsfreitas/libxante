
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Dec 23 10:32:41 2017
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

static int __xante_dlg_messagebox(int width, int height,
    enum xante_msgbox_type type, const char *title, const char *text)
{
    int pause_opt = 1, ret_dialog = DLG_EXIT_OK;
    chtype dlg_color, dialog_attr_b, border_attr_b, title_attr_b,
           border2_attr_b;

    switch (type) {
        case XANTE_MSGBOX_INFO:
            dlg_color = dlg_color_pair(COLOR_BLACK, COLOR_GREEN);
            break;

        case XANTE_MSGBOX_WARNING:
            dlg_color = dlg_color_pair(COLOR_BLACK, COLOR_YELLOW);
            break;

        case XANTE_MSGBOX_ERROR:
            dlg_color = dlg_color_pair(COLOR_WHITE, COLOR_RED);
            break;
    }

    /* Saves libdialog dialog attr */
    dialog_attr_b = dialog_attr;
    border_attr_b = border_attr;
    title_attr_b = title_attr;
    border2_attr_b = border2_attr;

    /* Sets our new attr */
    dialog_attr = dlg_color;
    border_attr = dlg_color;
    title_attr = dlg_color;
    border2_attr = dlg_color;

    dlg_put_backtitle();
    ret_dialog = dialog_msgbox(title, text, height, width, pause_opt);

    /* Restores libdialog dialog attr */
    dialog_attr = dialog_attr_b;
    border_attr = border_attr_b;
    title_attr = title_attr_b;
    border2_attr = border2_attr_b;

    return ret_dialog;
}

/*
 *
 * API
 *
 */

__PUB_API__ int xante_dlg_messagebox(xante_t *xpp, enum xante_msgbox_type type,
    const char *title, const char *message, ...)
{
    va_list ap;
    char *msg = NULL;
    enum xante_buttons key;
    int height = 0, ret_dialog;
    cl_string_t *real_msg = NULL;
    bool dialog_needs_close = false, restore_help_button = false,
         restore_extra_button = false;

    va_start(ap, message);
    vasprintf(&msg, message, ap);
    va_end(ap);

    if ((NULL == xpp) ||
        ((xpp != NULL) && xante_runtime_ui_active(xpp) == false))
    {
        dlgx_init(true);
        dialog_needs_close = true;
    }

    if (dialog_vars.help_button) {
        restore_help_button = true;
        dialog_vars.help_button = 0;
    }

    if (dialog_vars.extra_button) {
        restore_extra_button = true;
        dialog_vars.extra_button = 0;
    }

    real_msg = cl_string_create("%s", msg);
    cl_string_rplchr(real_msg, XANTE_STR_LINE_BREAK, '\n');
    height = dlgx_count_lines(msg, DEFAULT_DIALOG_WIDTH);

    if (xpp != NULL)
        xante_log_info("MSGBOX: %s", msg);

    ret_dialog = __xante_dlg_messagebox(DEFAULT_DIALOG_WIDTH, height, type,
                                        title, cl_string_valueof(real_msg));

    if (restore_help_button == true)
        dialog_vars.help_button = 1;

    if (restore_extra_button == true)
        dialog_vars.extra_button = 1;

    if (dialog_needs_close == true)
        dlgx_uninit(xpp);

    if (real_msg != NULL)
        cl_string_unref(real_msg);

    if (msg != NULL)
        free(msg);

    switch (ret_dialog) {
        case DLG_EXIT_OK:
            key = XANTE_BTN_OK;
            break;

        case DLG_EXIT_CANCEL:
#ifdef ALTERNATIVE_DIALOG
        case DLG_EXIT_TIMEOUT:
#endif
            key = XANTE_BTN_CANCEL;
            break;

        case DLG_EXIT_HELP:
            key = XANTE_BTN_HELP;
            break;

        case DLG_EXIT_EXTRA:
            key = XANTE_BTN_INFO;
            break;
    }

    return key;
}

__PUB_API__ int xante_dlg_messagebox_ex(xante_t *xpp, enum xante_msgbox_type type,
    const char *title, const char *message)
{
    return xante_dlg_messagebox(xpp, type, title, "%s", message);
}

