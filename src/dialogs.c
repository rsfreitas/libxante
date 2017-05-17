
/*
 * Description: Functions to handle UI from the user level.
 *
 * Author: Rodrigo Freitas
 * Created at: Wed May  3 13:48:23 2017
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

#include <stdlib.h>
#include <stdarg.h>

#include "libxante.h"
#include "ui/ui_dialogs.h"

/*
 *
 * Internal functions
 *
 */

static int xante_dlg_messagebox(int width, int height,
    enum xante_msgbox_type type, const char *title, const char *text)
{
    int pause_opt = 1, ret_dialog = DLG_EXIT_OK;
    chtype dlg_color, dialog_attr_b, border_attr_b, title_attr_b,
           border2_attr_b;

    /* TODO: Change here to make theme compatible */
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

__PUB_API__ int xante_ui_set_backtitle(xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;
    cl_string_t *title = NULL;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    if (xante_runtime_ui_active(xpp) == false) {
        errno_set(XANTE_ERROR_UI_NOT_INITIALIZED);
        return -1;
    }

    title = cl_string_create(cl_tr("%s - Version %s.%d Build %d %s"),
                             cl_string_valueof(x->info.application_name),
                             cl_string_valueof(x->info.version),
                             x->info.revision, x->info.build,
                             (x->info.beta == true) ? "BETA" : "");

    dialog_vars.backtitle = strdup(cl_string_valueof(title));

    return 0;
}

__PUB_API__ int xante_ui_clear_backtitle(xante_t *xpp)
{
    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    if (xante_runtime_ui_active(xpp) == false) {
        errno_set(XANTE_ERROR_UI_NOT_INITIALIZED);
        return -1;
    }

    free(dialog_vars.backtitle);
    dialog_vars.backtitle = NULL;

    return 0;
}

/*
 * TODO: Show buttons which was set in @buttons.
 */
__PUB_API__ int xante_messagebox(struct xante_app *xpp,
    enum xante_msgbox_type type, enum xante_buttons buttons __attribute__((unused)),
    const char *title,
    const char *message, ...)
{
    va_list ap;
    char *msg = NULL;
    enum xante_buttons key;
    int height = 0, ret_dialog;
    cl_string_t *real_msg = NULL;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    va_start(ap, message);
    vasprintf(&msg, message, ap);
    va_end(ap);

    if (xante_runtime_ui_active(xpp) == false)
        dialog_init(true);

    real_msg = cl_string_create("%s", msg);
    cl_string_rplchr(real_msg, XANTE_STR_LINE_BREAK, '\n');
    height = dialog_count_lines(msg, MINIMUM_WIDTH);

    xante_info("MSGBOX: %s", msg);
    ret_dialog = xante_dlg_messagebox(MINIMUM_WIDTH, height, type, title,
                                      cl_string_valueof(real_msg));

    if (xante_runtime_ui_active(xpp) == false)
        dialog_uninit();

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

