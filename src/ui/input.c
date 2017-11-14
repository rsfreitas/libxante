
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sat May  6 11:13:02 2017
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

#include <limits.h>
#include <float.h>

#include "libxante.h"
#include "ui_dialogs.h"

#define DEFAULT_STATUSBAR_TEXT              \
    "[ESC] Cancel [Enter] Confirm a selected option [Tab/Left/Right] Select an option"

/*
 *
 * Internal functions
 *
 */

static int get_input_length(const struct xante_item *item)
{
    int l = 0;

    switch (item->dialog_type) {
        case XANTE_UI_DIALOG_INPUT_INT:
            l = idigits(INT_MAX - 1);
            break;

        case XANTE_UI_DIALOG_INPUT_FLOAT:
            l = idigits((int)FLT_MAX - 1);
            break;

        case XANTE_UI_DIALOG_INPUT_DATE:
            l = DATE_MAX_INPUT_LENGTH;
            break;

        case XANTE_UI_DIALOG_INPUT_TIME:
            l = TIME_MAX_INPUT_LENGTH;
            break;

        case XANTE_UI_DIALOG_INPUT_STRING:
        case XANTE_UI_DIALOG_INPUT_PASSWD:
            l = item->string_length;
            break;

        default:
            break;
    }

    return l;
}

static bool item_value_has_changed(struct xante_app *xpp,
    struct xante_item *item, const char *new_value)
{
    bool changed = false;
    cl_object_t *value = NULL;
    cl_string_t *result = NULL, *str_value = NULL;

    value = item_value(item);
    str_value = cl_object_to_cstring(value);

    if (equals(cl_string_valueof(str_value), new_value) == false) {
        changed = true;
        change_add(xpp, cl_string_valueof(item->name),
                   cl_string_valueof(str_value), new_value);

        /* Updates item value */
        if (item->value != NULL)
            cl_object_unref(item->value);

        result = cl_string_create("%s", new_value);

        if (item->dialog_type == XANTE_UI_DIALOG_INPUT_INT)
            item->value = cl_object_create(CL_INT, cl_string_to_int(result));
        else if (item->dialog_type == XANTE_UI_DIALOG_INPUT_FLOAT)
            item->value = cl_object_create(CL_FLOAT, cl_string_to_float(result));
        else
            item->value = cl_object_from_cstring(result);

        cl_string_unref(result);
    }

    if (str_value != NULL)
        free(str_value);

    return changed;
}

static bool validate_date(struct xante_app *xpp, cl_string_t *value)
{
    cl_stringlist_t *date = NULL;
    cl_string_t *tmp = NULL;
    int day, month, year;

    if (cl_string_cchr(value, '/') != 2) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, 0, cl_tr("Error"),
                             cl_tr("Invalid date format!"));

        return false;
    }

    date = cl_string_split(value, "/");

    /* DAY */
    tmp = cl_stringlist_get(date, 0);
    day = cl_string_to_int(tmp);
    cl_string_unref(tmp);

    /* MONTH */
    tmp = cl_stringlist_get(date, 1);
    month = cl_string_to_int(tmp);
    cl_string_unref(tmp);

    /* YEAR */
    tmp = cl_stringlist_get(date, 2);
    year = cl_string_to_int(tmp);
    cl_string_unref(tmp);

    cl_stringlist_destroy(date);

    if ((day <= 0) || (day > 31)) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, 0, cl_tr("Error"),
                             cl_tr("The day must be between 1 and 31!"));

        return false;
    }

    if ((month <= 0) || (month > 12)) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, 0, cl_tr("Error"),
                             cl_tr("The month must be between 1 and 12!"));

        return false;
    }

    if (year <= 0) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, 0, cl_tr("Error"),
                             cl_tr("The year must be greater than 0!"));

        return false;
    }

    return true;
}

static bool validate_time(struct xante_app *xpp, cl_string_t *value)
{
    cl_stringlist_t *time = NULL;
    cl_string_t *tmp = NULL;
    int hour, minute, second;

    if (cl_string_cchr(value, ':') != 2) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, 0, cl_tr("Error"),
                             cl_tr("Invalid timeformat!"));

        return false;
    }

    time = cl_string_split(value, ":");

    /* HOUR */
    tmp = cl_stringlist_get(time, 0);
    hour = cl_string_to_int(tmp);
    cl_string_unref(tmp);

    /* MINUTE */
    tmp = cl_stringlist_get(time, 1);
    minute = cl_string_to_int(tmp);
    cl_string_unref(tmp);

    /* SECOND */
    tmp = cl_stringlist_get(time, 2);
    second = cl_string_to_int(tmp);
    cl_string_unref(tmp);

    cl_stringlist_destroy(time);

    if ((hour < 0) || (hour > 23)) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, 0, cl_tr("Error"),
                             cl_tr("The hour must be between 0 and 23!"));

        return false;
    }

    if ((minute < 0) || (minute > 59)) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, 0, cl_tr("Error"),
                             cl_tr("The minutes must be between 0 and 59!"));

        return false;
    }

    if ((second < 0) || (second > 59)) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, 0, cl_tr("Error"),
                             cl_tr("The seconds must be between 0 and 59!"));

        return false;
    }

    return true;
}

static bool validate_input_value(struct xante_app *xpp, struct xante_item *item,
    const char *new_value)
{
    bool valid = false;
    cl_object_t *value = NULL;
    cl_string_t *str_value = NULL;

    str_value = cl_string_create("%s", new_value);
    value = cl_object_from_cstring(str_value);

    if (item->dialog_type == XANTE_UI_DIALOG_INPUT_INT) {
        valid = cl_spec_validate(item->value_spec, item_value(item), false,
                                 CL_VALIDATE_RANGE, CL_OBJECT_AS_INT(value));

        if (valid == false) {
            xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, 0, cl_tr("Error"),
                                 cl_tr("The entered value must be between "
                                       "'%d' and '%d'!"),
                                 CL_OBJECT_AS_INT(item->min),
                                 CL_OBJECT_AS_INT(item->max));
        }
    } else if (item->dialog_type == XANTE_UI_DIALOG_INPUT_FLOAT) {
        valid = cl_spec_validate(item->value_spec, item_value(item), false,
                                 CL_VALIDATE_RANGE,
                                 cl_string_to_float(str_value));

        if (valid == false) {
            xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, 0, cl_tr("Error"),
                                 cl_tr("The entered value must be between "
                                       "'%.2f' and '%.2f'!"),
                                 CL_OBJECT_AS_FLOAT(item->min),
                                 CL_OBJECT_AS_FLOAT(item->max));
        }
    } else if (item->dialog_type == XANTE_UI_DIALOG_INPUT_DATE) {
        valid = validate_date(xpp, str_value);
    } else if (item->dialog_type == XANTE_UI_DIALOG_INPUT_TIME) {
        valid = validate_time(xpp, str_value);
    } else {
        /*
         * XXX: A XANTE_UI_DIALOG_INPUT_PASSWD must be validated inside a plugin,
         *      in a EV_ITEM_VALUE_CONFIRM event, which is called before this
         *      function.
         *
         *      We don't hold any information inside the JTF to compare them and
         *      return true to keep running.
         */

        valid = true;
    }

    return valid;
}

/*
 *
 * Internal API
 *
 */

/**
 * @name ui_dialog_input
 * @brief Creates a dialog to do some data input.
 *
 * @param [in] xpp: The main library object.
 * @param [in] item: The item to be used inside the dialog.
 * @param [in] edit_value: A flag to indicate if the value will be editable
 *                         or not.
 *
 * @return Returns a boolean value indicating if the item's value has been
 *         changed (true) or not (false).
 */
bool ui_dialog_input(struct xante_app *xpp, struct xante_item *item,
    bool edit_value)
{
    bool loop = true, value_changed = false;
    int ret_dialog = DLG_EXIT_OK, height, width;
    char input[MAX_INPUT_VALUE] = {0};
    cl_string_t *text = NULL, *value = NULL;

    /* Prepare dialog */
    dlgx_set_backtitle(xpp);
    dlgx_update_cancel_button_label();
    dlgx_put_statusbar(DEFAULT_STATUSBAR_TEXT);

    /* Prepares dialog content */
    value = cl_object_to_cstring(item_value(item));
    strcpy(input, cl_string_valueof(value));
    cl_string_unref(value);

    /* Enables the help button */
    if (item->descriptive_help != NULL)
        dialog_vars.help_button = 1;

    /* Adjusts window width and height */
    text = cl_string_dup(item->options);
    cl_string_rplchr(text, XANTE_STR_LINE_BREAK, '\n');
    width = dlgx_get_input_window_width(item);
    height = dlgx_count_lines_by_delimiters(cl_string_valueof(text)) +
        FORM_HEIGHT_WITHOUT_TEXT;

    do {
        if (item->dialog_type == XANTE_UI_DIALOG_INPUT_PASSWD) {
            ret_dialog = ui_dialog_passwd(item, edit_value, input, sizeof(input),
                                          height, text);
        } else {
            ret_dialog = dlgx_inputbox(width, height,
                                       cl_string_valueof(item->name),
                                       cl_string_valueof(text), NULL, NULL,
                                       get_input_length(item), input,
                                       edit_value, NULL, NULL);
        }

        switch (ret_dialog) {
            case DLG_EXIT_OK:
                if (event_call(EV_ITEM_VALUE_CONFIRM, xpp, item, input) < 0)
                    break;

                if (validate_input_value(xpp, item, input) == false)
                    break;

                if (item_value_has_changed(xpp, item, input) == true)
                    value_changed = true;

                loop = false;
                break;

            case DLG_EXIT_ESC:
            case DLG_EXIT_CANCEL:
                loop = false;
                break;

            case DLG_EXIT_HELP:
                dialog_vars.help_button = 0;
                xante_dlg_messagebox(xpp, XANTE_MSGBOX_INFO, cl_tr("Help"),
                                     cl_string_valueof(item->descriptive_help));

                dialog_vars.help_button = 1;
                break;
        }
    } while (loop);

    if (item->descriptive_help != NULL)
        dialog_vars.help_button = 0;

    return value_changed;
}

