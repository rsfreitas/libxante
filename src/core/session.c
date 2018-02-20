
/*
 * Description: Functions to manipulate session's widget information.
 *
 * Author: Rodrigo Freitas
 * Created at: Mon Dec 18 22:45:00 2017
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

struct widget_events {
    enum xante_widget   type;
    int                 (*run)(struct session *);
    bool                (*validate_result)(struct session *);
    bool                (*value_changed)(struct session *);
    void                (*update_value)(struct session *);
};

static struct widget_events __widget_events[] = {
    {
        .type = XANTE_WIDGET_UNKNOWN,
        .run = NULL,
        .value_changed = NULL,
        .update_value = NULL,
        .validate_result = NULL
    },

    {
        .type = XANTE_WIDGET_MENU_REFERENCE,
        .run = NULL,
        .value_changed = NULL,
        .update_value = NULL,
        .validate_result = NULL
    },

    {
        .type = XANTE_WIDGET_INPUT_INT,
        .run = input,
        .validate_result = input_validate_result,
        .value_changed = input_value_changed,
        .update_value = input_update_value,
    },

    {
        .type = XANTE_WIDGET_INPUT_FLOAT,
        .run = input,
        .value_changed = input_value_changed,
        .update_value = input_update_value,
        .validate_result = input_validate_result
    },

    {
        .type = XANTE_WIDGET_INPUT_DATE,
        .run = input,
        .value_changed = input_value_changed,
        .update_value = input_update_value,
        .validate_result = input_validate_result
    },

    {
        .type = XANTE_WIDGET_INPUT_STRING,
        .run = input,
        .value_changed = input_value_changed,
        .update_value = input_update_value,
        .validate_result = input_validate_result
    },

    {
        .type = XANTE_WIDGET_INPUT_PASSWD,
        .run = input,
        .value_changed = input_value_changed,
        .update_value = input_update_value,
        .validate_result = input_validate_result
    },

    {
        .type = XANTE_WIDGET_INPUT_TIME,
        .run = input,
        .value_changed = input_value_changed,
        .update_value = input_update_value,
        .validate_result = input_validate_result
    },

    {
        .type = XANTE_WIDGET_CALENDAR,
        .run = calendar,
        .value_changed = calendar_value_changed,
        .update_value = calendar_update_value,
        .validate_result = NULL
    },

    {
        .type = XANTE_WIDGET_TIMEBOX,
        .run = timebox,
        .value_changed = timebox_value_changed,
        .update_value = timebox_update_value,
        .validate_result = NULL
    },

    {
        .type = XANTE_WIDGET_RADIO_CHECKLIST,
        .run = checklist,
        .value_changed = checklist_value_changed,
        .update_value = checklist_update_value,
        .validate_result = checklist_validate_result
    },

    {
        .type = XANTE_WIDGET_CHECKLIST,
        .run = checklist,
        .value_changed = checklist_value_changed,
        .update_value = checklist_update_value,
        .validate_result = checklist_validate_result
    },

    {
        .type = XANTE_WIDGET_YES_NO,
        .run = yesno,
        .value_changed = yesno_value_changed,
        .update_value = yesno_update_value,
        .validate_result = yesno_validate_result
    },

    {
        .type = XANTE_WIDGET_DYNAMIC_MENU_REFERENCE,
        .run = NULL,
        .value_changed = NULL,
        .update_value = NULL,
        .validate_result = NULL
    },

    {
        .type = XANTE_WIDGET_DELETE_DYNAMIC_MENU_ITEM,
        .run = delete_dm,
        .value_changed = delete_dm_value_changed,
        .update_value = NULL,
        .validate_result = NULL
    },

    {
        .type = XANTE_WIDGET_ADD_DYNAMIC_MENU_ITEM,
        .run = add_dm,
        .value_changed = add_dm_value_changed,
        .update_value = NULL,
        .validate_result = NULL
    },

    {
        .type = XANTE_WIDGET_CUSTOM,
        .run = NULL,
        .value_changed = NULL,
        .update_value = NULL,
        .validate_result = NULL
    },

    {
        .type = XANTE_WIDGET_PROGRESS,
        .run = progress,
        .value_changed = NULL,
        .update_value = NULL,
        .validate_result = NULL
    },

    {
        .type = XANTE_WIDGET_SPINNER_SYNC,
        .run = sync_dialog,
        .value_changed = NULL,
        .update_value = NULL,
        .validate_result = NULL
    },

    {
        .type = XANTE_WIDGET_DOTS_SYNC,
        .run = sync_dialog,
        .value_changed = NULL,
        .update_value = NULL,
        .validate_result = NULL
    },

    {
        .type = XANTE_WIDGET_RANGE,
        .run = input,
        .value_changed = input_value_changed,
        .update_value = input_update_value,
        .validate_result = input_validate_result
    },

    {
        .type = XANTE_WIDGET_FILE_SELECT,
        .run = fselect,
        .value_changed = fselect_value_changed,
        .update_value = fselect_update_value,
        .validate_result = fselect_validate_result
    },

    {
        .type = XANTE_WIDGET_DIR_SELECT,
        .run = fselect,
        .value_changed = fselect_value_changed,
        .update_value = fselect_update_value,
        .validate_result = fselect_validate_result
    },

    {
        .type = XANTE_WIDGET_FILE_VIEW,
        .run = file_view,
        .value_changed = NULL,
        .update_value = NULL,
        .validate_result = NULL
    },

    {
        .type = XANTE_WIDGET_TAILBOX,
        .run = tailbox,
        .value_changed = NULL,
        .update_value = NULL,
        .validate_result = NULL
    },

    {
        .type = XANTE_WIDGET_SCROLLTEXT,
        .run = scrolltext,
        .value_changed = NULL,
        .update_value = NULL,
        .validate_result = NULL
    },

    {
        .type = XANTE_WIDGET_UPDATE_OBJECT,
        .run = update_object,
        .value_changed = NULL,
        .update_value = NULL,
        .validate_result = NULL
    },

    {
        .type = XANTE_WIDGET_INPUTSCROLL,
        .run = input,
        .value_changed = input_value_changed,
        .update_value = input_update_value,
        .validate_result = input_validate_result
    },

    {
        .type = XANTE_WIDGET_MIXEDFORM,
        .run = mixedform,
        .value_changed = mixedform_value_changed,
        .update_value = NULL,
        .validate_result = NULL
    },

    {
        .type = XANTE_WIDGET_BUILDLIST,
        .run = buildlist,
        .value_changed = buildlist_value_changed,
        .update_value = NULL,
        .validate_result = NULL
    },

    {
        .type = XANTE_WIDGET_SPREADSHEET,
        .run = spreadsheet,
        .value_changed = spreadsheet_value_changed,
        .update_value = NULL,
        .validate_result = NULL
    },

    {
        .type = XANTE_GADGET_CLOCK,
        .run = gadget_clock,
        .value_changed = NULL,
        .update_value = NULL,
        .validate_result = gadget_clock_validate_result
    }
};

#define MAX_UI_DIALOG       \
    (sizeof(__widget_events) / sizeof(__widget_events[0]))

/*
 *
 * Internal functions
 *
 */

static struct widget_events *get_function_events(enum xante_widget type)
{
    unsigned int i;

    for (i = 0; i < MAX_UI_DIALOG; i++)
        if (__widget_events[i].type == type)
            return &__widget_events[i];

    return NULL;
}

/*
 *
 * Internal API
 *
 */

/**
 * @name session_init
 * @brief Initializes everything related to the session of a selected item.
 *
 * The function expects an already allocated session_t object, which is
 * going to be cleared and correctly initialized, so the respective dialog can
 * successfully run.
 *
 * @param [in,out] xpp: The library main object.
 * @param [in,out] item: The selected item.
 * @param [in,out] session: The already allocated session of the item.
 */
void session_init(struct xante_app *xpp, struct xante_item *item,
    session_t *session)
{
    struct widget_events *ptr = NULL;

    memset(session, 0, sizeof(session_t));

    session->result = NULL;
    session->text = NULL;
    session->litems = NULL;
    session->fitems = NULL;
    session->scroll_content = NULL;
    session->title = NULL;
    session->row_title = NULL;
    session->column_title = NULL;
    session->sheet = NULL;
    session->change_item_name = NULL;
    session->change_old_value = NULL;
    session->change_new_value = NULL;
    session->xpp = xpp;
    session->item = item;

    /*
     * If we're an item we may have some function which can be executed inside
     * the manager code.
     */
    if (item != NULL) {
        if (bit_test(item->mode, XanteAccessEdit) == false)
            session->editable_value = false;
        else
            session->editable_value = true;

        ptr = get_function_events(item->widget_type);

        if (ptr != NULL) {
            session->run = ptr->run;
            session->validate_result = ptr->validate_result;
        }
    }
}

/**
 * @name session_uninit
 * @brief Clears the session of a selected item.
 *
 * Clears all session releasing all previously allocated memory for the items'
 * property
 *
 * @param [in,out] session: The session of the item.
 */
void session_uninit(session_t *session)
{
    int i;
    DIALOG_LISTITEM *item = NULL;
    DIALOG_FORMITEM *fitem = NULL;

    if (session->scroll_content != NULL)
        free(session->scroll_content);

    if (session->litems != NULL) {
        for (i = 0; i < session->number_of_items; i++) {
            item = &session->litems[i];
            free(item->text);
            free(item->name);
            free(item->help);
        }

        free(session->litems);
    }

    if (session->fitems != NULL) {
        for (i = 0; i < session->number_of_items; i++) {
            fitem = &session->fitems[i];
            free(fitem->name);
            free(fitem->text);
        }

        free(session->fitems);
    }

    if (session->text != NULL)
        cl_string_unref(session->text);

    if (session->result != NULL)
        cl_string_unref(session->result);

    if (session->title != NULL)
        cl_string_unref(session->title);

    if (session->row_title != NULL)
        cl_string_unref(session->row_title);

    if (session->column_title != NULL)
        cl_string_unref(session->column_title);

    if (session->change_item_name != NULL)
        cl_string_unref(session->change_item_name);

    if (session->change_old_value != NULL)
        cl_string_unref(session->change_old_value);

    if (session->change_new_value != NULL)
        cl_string_unref(session->change_new_value);

    if (session->sheet != NULL)
        spreadsheet_st_destroy(session->sheet);
}

