
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Thu Jun  1 12:53:40 2017
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

static bool is_input_valid(const char *input)
{
    if (strlen(input) == 0)
        return false;

    return true;
}

/*
 *
 * Internal API
 *
 */

ui_return_t ui_add_dm(struct xante_app *xpp, struct xante_item *item)
{
    bool added = false, loop = true;
    int ret_dialog = DLG_EXIT_OK;
    char input[MAX_INPUT_VALUE] = {0};
    ui_properties_t properties;
    ui_return_t ret;

    INIT_PROPERTIES(properties);

    /* Adjusts window width and height */
    properties.text = cl_string_dup(item->options);
    cl_string_rplchr(properties.text, XANTE_STR_LINE_BREAK, '\n');
    properties.width = (item->geometry.width == 0)
                            ? dlgx_get_input_window_width(item)
                            : item->geometry.width;

    properties.height = (item->geometry.height == 0)
                         ? dlgx_count_lines_by_delimiters(cl_string_valueof(properties.text)) +
                           FORM_HEIGHT_WITHOUT_TEXT
                         : item->geometry.height;

    do {
        ret_dialog = dlgx_inputbox(properties.width, properties.height,
                                   cl_string_valueof(item->name),
                                   cl_string_valueof(properties.text), NULL,
                                   NULL, sizeof(input) - 1, input,
                                   true, NULL, NULL, NULL);

        switch (ret_dialog) {
            case DLG_EXIT_OK:
                if (is_input_valid(input) == true) {
                    /*
                     * We insert the new dynamic menu entry and set this as an
                     * internal change, so it will ask the user to save into
                     * the configuration file.
                     */
                    if (dm_insert(xpp, item, input)) {
                        change_add(xpp, "New dynamic menu entry", "EMPTY", input);
                        added = true;
                        loop = false;
                    }
                } else
                    xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                                         "%s", cl_tr("An invalid input was entered!"));

                break;

#ifdef ALTERNATIVE_DIALOG
            case DLG_EXIT_TIMEOUT:
                loop = false;
                break;
#endif

            case DLG_EXIT_ESC:
                /* Don't let the user close the dialog */
                if (xante_runtime_esc_key(xpp))
                    break;

            case DLG_EXIT_CANCEL:
                loop = false;
                break;

            case DLG_EXIT_HELP:
                dialog_vars.help_button = 0;
                xante_dlg_messagebox(xpp, XANTE_MSGBOX_INFO, cl_tr("Help"), "%s",
                                     cl_string_valueof(item->descriptive_help));

                dialog_vars.help_button = 1;
                break;
        }
    } while (loop);

    UNINIT_PROPERTIES(properties);

    ret.selected_button = ret_dialog;
    ret.updated_value = added;

    return ret;
}

