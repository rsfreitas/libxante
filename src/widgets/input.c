
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

/*
 * Since the callbacks used by the dlgx_inputscroll function receives a
 * void *, this structure is built to hold some informations needed to
 * call events from the module while inside them.
 */
struct inputscroll_data {
    struct xante_app    *xpp;
    struct xante_item   *item;
};

/*
 *
 * Internal functions
 *
 */

static int get_input_length(const struct xante_item *item)
{
    int l = 0;

    switch (item->widget_type) {
        case XANTE_WIDGET_INPUT_INT:
            l = idigits(INT_MAX - 1);
            break;

        case XANTE_WIDGET_INPUT_FLOAT:
            l = idigits((int)FLT_MAX - 1);
            break;

        case XANTE_WIDGET_INPUT_DATE:
            l = DATE_MAX_INPUT_LENGTH;
            break;

        case XANTE_WIDGET_INPUT_TIME:
            l = TIME_MAX_INPUT_LENGTH;
            break;

        case XANTE_WIDGET_INPUT_STRING:
        case XANTE_WIDGET_INPUT_PASSWD:
        case XANTE_WIDGET_INPUTSCROLL:
            l = item->string_length;
            break;

        default:
            break;
    }

    return l;
}

static bool validate_date(struct xante_app *xpp, cl_string_t *value)
{
    cl_stringlist_t *date = NULL;
    cl_string_t *tmp = NULL;
    int day, month, year;

    if (cl_string_cchr(value, '/') != 2) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
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
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                             cl_tr("The day must be between 1 and 31!"));

        return false;
    }

    if ((month <= 0) || (month > 12)) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                             cl_tr("The month must be between 1 and 12!"));

        return false;
    }

    if (year <= 0) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
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
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
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
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                             cl_tr("The hour must be between 0 and 23!"));

        return false;
    }

    if ((minute < 0) || (minute > 59)) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                             cl_tr("The minutes must be between 0 and 59!"));

        return false;
    }

    if ((second < 0) || (second > 59)) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                             cl_tr("The seconds must be between 0 and 59!"));

        return false;
    }

    return true;
}

static int inputscroll_len(const char *value, void *data)
{
    struct inputscroll_data *input = (struct inputscroll_data *)data;

    if (NULL == data)
        return 0;

    return event_call(EV_VALUE_STRLEN, input->xpp, input->item, value);
}

static int inputscroll_check(const char *value, void *data)
{
    struct inputscroll_data *input = (struct inputscroll_data *)data;

    if (NULL == data)
        return 1;

    return event_call(EV_VALUE_CHECK, input->xpp, input->item, value);
}

static int dlgx_passwd(struct xante_item *item, char *input,
    unsigned int input_length, const session_t *session)
{
    DIALOG_FORMITEM fitem;
    int ret_dialog = DLG_EXIT_OK, form_height = 1, selected = -1;
    char password[MAX_INPUT_VALUE] = {0};

    dialog_vars.insecure = 1;
    strcpy(password, input);

    memset(&fitem, 0, sizeof(DIALOG_FORMITEM));
    fitem.type = 1;
    fitem.text_free = 0;
    fitem.text = password;
    fitem.text_len = strlen(password);
    fitem.text_y = 0;
    fitem.text_x = 0;

    if (session->editable_value == true)
        fitem.text_flen = session->width - 6;
    else
        fitem.text_flen = 0;

    fitem.text_ilen = MAX_INPUT_VALUE;
    ret_dialog = dlg_form(cl_string_valueof(item->name),
                          cl_string_valueof(session->text),
                          session->height, session->width,
                          form_height, 1, &fitem, &selected);

    if (ret_dialog == DLG_EXIT_OK)
        snprintf(input, input_length - 1, "%s", fitem.text);

    dialog_vars.insecure = 0;

    return ret_dialog;
}

static void build_session(session_t *session, char *input,
    int max_input)
{
    struct xante_item *item = session->item;
    cl_string_t *value = NULL;

    /* Adjusts current content */
    value = cl_object_to_cstring(item_value(item));
    strncpy(input, cl_string_valueof(value),
            max(max_input, cl_string_length(value)));

    cl_string_unref(value);

    /* UI session */
    session->text = cl_string_dup(item->options);
    cl_string_rplchr(session->text, XANTE_STR_LINE_BREAK, '\n');
    session->width = (item->geometry.width == 0) ? dlgx_get_input_window_width(item)
                                                 : item->geometry.width;

    session->height = (item->geometry.height == 0)
                        ? dlgx_count_lines_by_delimiters(cl_string_valueof(session->text)) +
                            ((item->widget_type == XANTE_WIDGET_RANGE) ? 2 : FORM_HEIGHT_WITHOUT_TEXT)
                        : item->geometry.height;

    if (item->widget_type == XANTE_WIDGET_INPUTSCROLL) {
        /* Gets the scrollable text to be displayed from a module function */
        session->scroll_content = event_item_custom_data(session->xpp,
                                                            session->item);
    }
}

/*
 *
 * Internal API
 *
 */

bool input_validate_result(session_t *session)
{
    struct xante_item *item = session->item;
    struct xante_app *xpp = session->xpp;
    bool valid = false;
    cl_object_t *value = NULL;
    cl_string_t *str_value = NULL;

    str_value = cl_string_ref(session->result);
    value = cl_object_from_cstring(session->result);

    if (item->widget_type == XANTE_WIDGET_INPUT_INT) {
        valid = cl_spec_validate(item->value_spec, item_value(item), false,
                                 CL_VALIDATE_RANGE, CL_OBJECT_AS_INT(value));

        if (valid == false) {
            xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                                 cl_tr("The entered value must be between "
                                       "'%d' and '%d'!"),
                                 CL_OBJECT_AS_INT(item->min),
                                 CL_OBJECT_AS_INT(item->max));
        }
    } else if (item->widget_type == XANTE_WIDGET_INPUT_FLOAT) {
        valid = cl_spec_validate(item->value_spec, item_value(item), false,
                                 CL_VALIDATE_RANGE,
                                 cl_string_to_float(str_value));

        if (valid == false) {
            xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                                 cl_tr("The entered value must be between "
                                       "'%.2f' and '%.2f'!"),
                                 CL_OBJECT_AS_FLOAT(item->min),
                                 CL_OBJECT_AS_FLOAT(item->max));
        }
    } else if (item->widget_type == XANTE_WIDGET_INPUT_DATE) {
        valid = validate_date(xpp, str_value);
    } else if (item->widget_type == XANTE_WIDGET_INPUT_TIME) {
        valid = validate_time(xpp, str_value);
    } else {
        /*
         * XXX: A XANTE_WIDGET_INPUT_PASSWD and XANTE_WIDGET_RANGE must
         *      be validated inside the module, in a EV_ITEM_VALUE_CONFIRM
         *      event, which is called before this function.
         *
         *      We don't hold any information inside the JTF to compare them and
         *      just return true to keep running.
         */

        valid = true;
    }

    cl_string_unref(str_value);

    return valid;
}

bool input_value_changed(session_t *session)
{
    struct xante_item *item = session->item;
    bool changed = false;
    cl_object_t *value = NULL;
    cl_string_t *str_value = NULL;

    value = item_value(item);
    str_value = cl_object_to_cstring(value);

    if (cl_string_cmp(str_value, session->result) != 0) {
        changed = true;

        /* Set up details to save inside the internal changes list */
        session->change_item_name = cl_string_ref(item->name);
        session->change_old_value = cl_string_ref(str_value);
        session->change_new_value = cl_string_ref(session->result);
    }

    if (str_value != NULL)
        cl_string_unref(str_value);

    return changed;
}

void input_update_value(session_t *session)
{
    struct xante_item *item = session->item;
    cl_string_t *result = NULL;

    /* Updates item value */
    if (item->value != NULL)
        cl_object_unref(item->value);

    result = cl_string_dup(session->result);

    if ((item->widget_type == XANTE_WIDGET_INPUT_INT) ||
        (item->widget_type == XANTE_WIDGET_RANGE))
    {
        item->value = cl_object_create(CL_INT, cl_string_to_int(result));
    } else if (item->widget_type == XANTE_WIDGET_INPUT_FLOAT)
        item->value = cl_object_create(CL_FLOAT, cl_string_to_float(result));
    else
        item->value = cl_object_from_cstring(result);

    cl_string_unref(result);
}

/**
 * @name input
 * @brief Creates an object to do some data input.
 *
 * @return Returns a ui_return_t value indicating if the item's value has been
 *         changed (true) or not (false) with the object selected button.
 */
int input(session_t *session)
{
    int ret_dialog = DLG_EXIT_CANCEL;
    char input_result[MAX_INPUT_VALUE] = {0}, *range_value = NULL;
    struct xante_app *xpp = session->xpp;
    struct xante_item *item = session->item;
    struct inputscroll_data input_data = {
        .xpp = xpp,
        .item = item,
    };

    /* Prepares object content and session */
    build_session(session, input_result, sizeof(input_result));

    if (item->widget_type == XANTE_WIDGET_INPUT_PASSWD) {
        ret_dialog = dlgx_passwd(item, input_result, sizeof(input_result),
                                 session);
    } else if (item->widget_type == XANTE_WIDGET_RANGE) {
        dlgx_free_input();
        ret_dialog = dialog_rangebox(cl_string_valueof(item->name),
                                     cl_string_valueof(session->text),
                                     session->height,
                                     session->width,
                                     CL_OBJECT_AS_INT(item->min),
                                     CL_OBJECT_AS_INT(item->max),
                                     strtol(input_result, NULL, 10));

        /*
         * Since this is a libdialog's dialog, we need to copy its internal
         * result to our.
         */
        range_value = dlgx_get_input_result();
        strncpy(input_result, range_value, max(strlen(range_value),
                                               MAX_INPUT_VALUE));

        free(range_value);
    } else if (item->widget_type == XANTE_WIDGET_INPUTSCROLL) {
        ret_dialog = dlgx_inputbox(session->width, 20,// height,
                                   cl_string_valueof(item->name),
                                   cl_string_valueof(session->text),
                                   cl_tr("Enter value:"),
                                   session->scroll_content,
                                   get_input_length(item), input_result,
                                   session->editable_value,
                                   inputscroll_len, inputscroll_check,
                                   &input_data);
    } else {
        ret_dialog = dlgx_inputbox(session->width, session->height,
                                   cl_string_valueof(item->name),
                                   cl_string_valueof(session->text), NULL,
                                   NULL, get_input_length(item), input_result,
                                   session->editable_value, NULL, NULL,
                                   NULL);
    }

    if ((ret_dialog == DLG_EXIT_OK) && session->editable_value)
        session->result = cl_string_create("%s", input_result);

    return ret_dialog;
}

