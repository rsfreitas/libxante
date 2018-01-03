
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Dec 23 20:32:41 2017
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

#define DEFAULT_STATUSBAR_TEXT              \
    cl_tr("[Enter] Confirm a selected option [Tab/Left/Right] Select an option")

/*
 *
 * Internal API
 *
 */

/**
 * @name gadget_question
 * @brief Creates a gadget to choose between two options.
 *
 * @param [in] xpp: The library main object.
 * @param [in] title: The dialog title.
 * @param [in] msg: The dialog message to be displayed.
 * @param [in] button1_label: A label for the first button.
 * @param [in] button2_label: A label fot the second button.
 * @param [in] statusbar_text: An alternative statusbar text.
 *
 * @return Returns true if the first button is selected or false otherwise.
 */
bool gadget_question(struct xante_app *xpp, const char *title, const char *msg,
    const char *button1_label, const char *button2_label,
    const char *statusbar_text)
{
    cl_string_t *form_msg = NULL, *default_text = NULL;
    int width = 45, lines = 0;
    bool ret_value = false, dialog_needs_closing = false;

    if (xante_runtime_ui_active(xpp) == false) {
        dlgx_init(false);
        runtime_set_ui_active(xpp, true);
        dialog_needs_closing = true;
    }

    dlgx_set_backtitle(xpp);
    dialog_vars.yes_label = (char *)button1_label;
    dialog_vars.no_label = (char *)button2_label;
    lines = dlgx_count_lines(msg, width);
    form_msg = cl_string_create("%s", msg);
    cl_string_rplchr(form_msg, XANTE_STR_LINE_BREAK, '\n');

    if (statusbar_text != NULL)
        dlgx_put_statusbar(statusbar_text);
    else {
        if (xante_runtime_esc_key(xpp))
            default_text = cl_string_create("%s", cl_tr("[ESC] Cancel"));
        else
            default_text = cl_string_create_empty(0);

        cl_string_cat(default_text, "%s", DEFAULT_STATUSBAR_TEXT);
        dlgx_put_statusbar(cl_string_valueof(default_text));
    }

    if (dialog_yesno(title, cl_string_valueof(form_msg), lines,
                                              width) == DLG_EXIT_OK)
    {
        ret_value = true;
    } else
        ret_value = false;

    if (dialog_needs_closing == true) {
        dlgx_uninit(xpp);
        runtime_set_ui_active(xpp, false);
    }

    cl_string_unref(form_msg);

    if (default_text != NULL)
        cl_string_unref(default_text);

    return ret_value;
}

