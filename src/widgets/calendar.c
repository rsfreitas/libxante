
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Fri May  5 21:08:15 2017
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

#define DEFAULT_WIDTH                       DEFAULT_DIALOG_WIDTH
#define DEFAULT_HEIGHT                      3

/*
 *
 * Internal functions
 *
 */

static void split_item_value(struct xante_item *item, int *day, int *month,
    int *year)
{
    cl_object_t *value = NULL;
    cl_string_t *tmp = NULL;
    cl_stringlist_t *date = NULL;
    char *str_value = NULL;

    value = item_value(item);
    str_value = CL_OBJECT_AS_STRING(value);
    tmp = cl_string_create("%s", str_value);
    free(str_value);

    date = cl_string_split(tmp, "/");
    cl_string_unref(tmp);

    /* DAY */
    tmp = cl_stringlist_get(date, 0);
    *day = cl_string_to_int(tmp);
    cl_string_unref(tmp);

    /* MONTH */
    tmp = cl_stringlist_get(date, 1);
    *month = cl_string_to_int(tmp);
    cl_string_unref(tmp);

    /* YEAR */
    tmp = cl_stringlist_get(date, 2);
    *year = cl_string_to_int(tmp);
    cl_string_unref(tmp);

    cl_stringlist_destroy(date);
}

/*
 *
 * Internal API
 *
 */

bool calendar_value_changed(session_t *session)
{
    struct xante_item *item = session->item;
    cl_object_t *value = NULL;
    cl_string_t *str_value = NULL;
    bool changed = false;

    value = item_value(item);
    str_value = cl_object_to_cstring(value);

    if (cl_string_cmp(session->result, str_value) != 0) {
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

void calendar_update_value(session_t *session)
{
    struct xante_item *item = session->item;

    /* Updates item value */
    if (NULL == item->value)
        item->value = cl_object_from_cstring(session->result);
    else
        cl_object_set(item->value, cl_string_valueof(session->result));
}

/**
 * @name calendar
 * @brief Creates an object to choose a date in a calendar.
 *
 * @return Returns a ui_return_t value indicating if the item's value has been
 *         changed (true) or not (false) with the object selected button.
 */
int calendar(session_t *session)
{
    struct xante_item *item = session->item;
    int ret_dialog = DLG_EXIT_OK, day = 0, month = 0, year = 0;
    char *result = NULL;

    session->width = (item->geometry.width == 0) ? DEFAULT_WIDTH
                                                 : item->geometry.width;

    session->height = (item->geometry.height == 0) ? DEFAULT_HEIGHT
                                                   : item->geometry.height;

    /* Adjusts the object content using the item content */
    split_item_value(item, &day, &month, &year);

    /* Adjusts the window message */
    session->text = cl_string_dup(item->options);
    cl_string_rplchr(session->text, XANTE_STR_LINE_BREAK, '\n');

#ifdef ALTERNATIVE_DIALOG
    ret_dialog = dialog_calendar(cl_string_valueof(item->name),
                                 cl_string_valueof(session->text),
                                 session->height, session->width,
                                 day, month, year,
                                 session->editable_value);
#else
    ret_dialog = dialog_calendar(cl_string_valueof(item->name),
                                 cl_string_valueof(session->text),
                                 session->height, session->width,
                                 day, month, year);
#endif

    if ((ret_dialog == DLG_EXIT_OK) && session->editable_value) {
        result = dlgx_get_input_result();
        session->result = cl_string_create("%s", result);
        free(result);
    }

    return ret_dialog;
}

