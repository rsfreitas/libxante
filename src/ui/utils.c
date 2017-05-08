
/*
 * Description: Common functions to the UI dialogs.
 *
 * Author: Rodrigo Freitas
 * Created at: Fri May  5 21:23:32 2017
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
#include "ui_dialogs.h"

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

/*
 *
 * Internal API
 *
 */

/**
 * @name dialog_uninit
 * @brief Closes libdialog's environment.
 */
void dialog_uninit(void)
{
    int dialog_return_value = DLG_EXIT_OK;

    dlg_killall_bg(&dialog_return_value);

    if (dialog_state.screen_initialized) {
        (void)refresh();
        end_dialog();
    }
}

/**
 * @name dialog_init
 * @brief Initialize libdialog's environment.
 *
 * @param [in] temporarily: Boolean flag to clear or not internal libdialog
 *                          values.
 */
void dialog_init(bool temporarily)
{
    memset(&dialog_state, 0, sizeof(dialog_state));

    if (temporarily == false)
        memset(&dialog_vars, 0, sizeof(dialog_vars));

    init_dialog(stdin, stdout);
}

/**
 * @name dialog_set_backtitle
 * @brief Sets the application backtitle.
 */
void dialog_set_backtitle(void)
{
    dlg_clear();
    dlg_put_backtitle();
}

/**
 * @name dialog_get_item_value_as_text
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
char *dialog_get_item_value_as_text(const struct xante_item *item)
{
    char *text = NULL;
    cl_string_t *value = NULL;
    int index = -1;

    switch (item->dialog_type) {
        case XANTE_UI_DIALOG_INPUT_INT:
        case XANTE_UI_DIALOG_INPUT_FLOAT:
        case XANTE_UI_DIALOG_INPUT_DATE:
        case XANTE_UI_DIALOG_INPUT_TIME:
        case XANTE_UI_DIALOG_CALENDAR:
        case XANTE_UI_DIALOG_TIMEBOX:
        case XANTE_UI_DIALOG_INPUT_STRING:
            value = cl_object_to_cstring(item_value(item));
            text = strdup(cl_string_valueof(value));
            cl_string_unref(value);
            break;

        case XANTE_UI_DIALOG_RADIO_CHECKLIST:
            index = CL_OBJECT_AS_INT(item_value(item));
            value = cl_string_list_get(item->checklist_options, index);

            if (value != NULL) {
                text = strdup(cl_string_valueof(value));
                cl_string_unref(value);
            }

            break;

        case XANTE_UI_DIALOG_YES_NO:
            index = CL_OBJECT_AS_INT(item_value(item));

            if (index == 1)
                text = strdup(cl_tr("Yes"));
            else
                text = strdup(cl_tr("No"));

            break;

        default:
            break;
    }

    return text;
}

/**
 * @name dialog_question
 * @brief Creates a dialog to choose between two options.
 *
 * @param [in] xpp: The library main object.
 * @param [in] title: The dialog title.
 * @param [in] msg: The dialog message to be displayed.
 * @param [in] button1_label: A label for the first button.
 * @param [in] button2_label: A label fot the second button.
 *
 * @return Returns true if the first button is selected or false otherwise.
 */
bool dialog_question(struct xante_app *xpp, const char *title, const char *msg,
    const char *button1_label, const char *button2_label)
{
    cl_string_t *form_msg = NULL;
    int width = 40, lines = 0;
    bool ret_value = false;

    if (xante_runtime_ui_active(xpp) == false)
        dialog_init(false);

    dialog_set_backtitle();
    dialog_vars.yes_label = (char *)button1_label;
    dialog_vars.no_label = (char *)button2_label;
    lines = dialog_count_lines(msg, width);
    form_msg = cl_string_create("%s", msg);
    cl_string_rplchr(form_msg, XANTE_STR_LINE_BREAK, '\n');

    if (dialog_yesno(title, cl_string_valueof(form_msg), lines,
                                              width) == DLG_EXIT_OK)
    {
        ret_value = true;
    } else
        ret_value = false;

    if (xante_runtime_ui_active(xpp) == false)
        dialog_uninit();

    cl_string_unref(form_msg);

    return ret_value;
}

/**
 * @name dialog_count_lines_by_delimiters
 * @brief Counts the number of lines of a string by looking at some delimiters.
 *
 * The functions assumes that by each delimiter found a new line will be used
 * inside a dialog.
 *
 * @param [in] text: The text which will be evaluated.
 *
 * @return Returns the number of lines which @text will use inside a dialog.
 */
int dialog_count_lines_by_delimiters(const char *text)
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
 * @name dialog_count_lines
 * @brief Function to count the lines which a string text will have inside a
 *        dialog.
 *
 * @param [in] text: The string text.
 * @param [in] widht: The window width.
 *
 * @return Returns the number of lines for @text.
 */
int dialog_count_lines(const char *text, int width)
{
    int lines = 0;

    lines = dialog_count_lines_by_delimiters(text);

    /*
     * If we have just one line, than we try to count them by lookin at the
     * window width limit.
     */
    if (lines == 1)
        lines = count_lines_by_maximum_column(text, width);

    return lines + MSGBOX_HEIGHT_WITHOUT_TEXT;
}

/**
 * @name dialog_update_cancel_button_label
 * @brief Sets a default value to the cancel button label used inside dialogs.
 */
void dialog_update_cancel_button_label(void)
{
    if (dialog_vars.cancel_label != NULL)
        free(dialog_vars.cancel_label);

    dialog_vars.cancel_label = strdup(cl_tr("Back"));
}

/**
 * @name dialog_free_input
 * @brief Releases the dialog input buffer previously allocated.
 */
void dialog_free_input(void)
{
    if (dialog_vars.input_result != NULL) {
        free(dialog_vars.input_result);
        dialog_vars.input_result = NULL;
    }
}

/**
 * @name dialog_alloc_input
 * @brief Allocates the dialog input buffer.
 *
 * @param [in] bytes: The input size in bytes.
 */
void dialog_alloc_input(unsigned int bytes)
{
    if (dialog_vars.input_result != NULL)
        dialog_free_input();

    dialog_vars.input_result = calloc(bytes, sizeof(char));
}

