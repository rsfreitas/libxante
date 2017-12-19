
/*
 * Description: Functions to manipulate the ui_properties_t type.
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

struct dialog_function_events {
    enum xante_ui_dialog    type;
    int                     (*run)(struct ui_properties *);
    bool                    (*validate_result)(struct ui_properties *);
};

static struct dialog_function_events __dialog_function_events[] = {
    {
        .type = XANTE_UI_DIALOG_UNKNOWN,
        .run = NULL,
        .validate_result = NULL
    },

    {
        .type = XANTE_UI_DIALOG_MENU,
        .run = NULL,
        .validate_result = NULL
    },

    {
        .type = XANTE_UI_DIALOG_INPUT_INT,
        .run = input,
        .validate_result = input_validate_result
    },

    {
        .type = XANTE_UI_DIALOG_INPUT_FLOAT,
        .run = input,
        .validate_result = input_validate_result
    },

    {
        .type = XANTE_UI_DIALOG_INPUT_DATE,
        .run = input,
        .validate_result = input_validate_result
    },

    {
        .type = XANTE_UI_DIALOG_INPUT_STRING,
        .run = input,
        .validate_result = input_validate_result
    },

    {
        .type = XANTE_UI_DIALOG_INPUT_PASSWD,
        .run = input,
        .validate_result = input_validate_result
    },

    {
        .type = XANTE_UI_DIALOG_INPUT_TIME,
        .run = input,
        .validate_result = input_validate_result
    },

    {
        .type = XANTE_UI_DIALOG_CALENDAR,
        .run = calendar,
        .validate_result = calendar_validate_result
    },

    {
        .type = XANTE_UI_DIALOG_TIMEBOX,
        .run = timebox,
        .validate_result = timebox_validate_result
    },

    {
        .type = XANTE_UI_DIALOG_RADIO_CHECKLIST,
        .run = checklist,
        .validate_result = checklist_validate_result
    },

    {
        .type = XANTE_UI_DIALOG_CHECKLIST,
        .run = checklist,
        .validate_result = checklist_validate_result
    },

    {
        .type = XANTE_UI_DIALOG_YES_NO,
        .run = yesno,
        .validate_result = yesno_validate_result
    },

    {
        .type = XANTE_UI_DIALOG_DYNAMIC_MENU,
        .run = NULL,
        .validate_result = NULL
    },

    {
        .type = XANTE_UI_DIALOG_DELETE_DYNAMIC_MENU_ITEM,
        .run = delete_dm,
        .validate_result = delete_dm_validate_result
    },

    {
        .type = XANTE_UI_DIALOG_ADD_DYNAMIC_MENU_ITEM,
        .run = add_dm,
        .validate_result = add_dm_validate_result
    },

    {
        .type = XANTE_UI_DIALOG_CUSTOM,
        .run = NULL,
        .validate_result = NULL
    },

    {
        .type = XANTE_UI_DIALOG_PROGRESS,
        .run = progress,
        .validate_result = NULL
    },

    {
        .type = XANTE_UI_DIALOG_SPINNER_SYNC,
        .run = sync_dialog,
        .validate_result = NULL
    },

    {
        .type = XANTE_UI_DIALOG_DOTS_SYNC,
        .run = sync_dialog,
        .validate_result = NULL
    },

    {
        .type = XANTE_UI_DIALOG_RANGE,
        .run = input,
        .validate_result = input_validate_result
    },

    {
        .type = XANTE_UI_DIALOG_FILE_SELECT,
        .run = fselect,
        .validate_result = NULL
    },

    {
        .type = XANTE_UI_DIALOG_DIR_SELECT,
        .run = fselect,
        .validate_result = fselect_validate_result
    },

    {
        .type = XANTE_UI_DIALOG_FILE_VIEW,
        .run = file_view,
        .validate_result = NULL
    },

    {
        .type = XANTE_UI_DIALOG_TAILBOX,
        .run = tailbox,
        .validate_result = NULL
    },

    {
        .type = XANTE_UI_DIALOG_SCROLLTEXT,
        .run = scrolltext,
        .validate_result = NULL
    },

    {
        .type = XANTE_UI_DIALOG_UPDATE_OBJECT,
        .run = update_object,
        .validate_result = NULL
    },

    {
        .type = XANTE_UI_DIALOG_INPUTSCROLL,
        .run = input,
        .validate_result = input_validate_result
    },

    {
        .type = XANTE_UI_DIALOG_MIXEDFORM,
        .run = mixedform,
        .validate_result = mixedform_validate_result
    },

    {
        .type = XANTE_UI_DIALOG_BUILDLIST,
        .run = buildlist,
        .validate_result = buildlist_validate_result
    },

    {
        .type = XANTE_UI_DIALOG_SPREADSHEET,
        .run = spreadsheet,
        .validate_result = spreadsheet_validate_result
    }
};

#define MAX_UI_DIALOG       \
    (sizeof(__dialog_function_events) / sizeof(__dialog_function_events[0]))

/*
 *
 * Internal functions
 *
 */

static struct dialog_function_events *get_function_events(enum xante_ui_dialog type)
{
    unsigned int i;

    for (i = 0; i < MAX_UI_DIALOG; i++)
        if (__dialog_function_events[i].type == type)
            return &__dialog_function_events[i];

    return NULL;
}

/*
 *
 * Internal API
 *
 */

/**
 * @name properties_init
 * @brief Initializes everything related to the properties of a selected item.
 *
 * The function expects an already allocated ui_properties_t object, which is
 * going to be cleared and correctly initialized, so the respective dialog can
 * successfully run.
 *
 * @param [in,out] xpp: The library main object.
 * @param [in,out] item: The selected item.
 * @param [in,out] properties: The already allocated properties of the item.
 */
void properties_init(struct xante_app *xpp, struct xante_item *item,
    ui_properties_t *properties)
{
    struct dialog_function_events *ptr = NULL;

    memset(properties, 0, sizeof(ui_properties_t));

    properties->result = NULL;
    properties->text = NULL;
    properties->litems = NULL;
    properties->fitems = NULL;
    properties->scroll_content = NULL;
    properties->title = NULL;
    properties->row_title = NULL;
    properties->column_title = NULL;
    properties->sheet = NULL;
    properties->change_item_name = NULL;
    properties->change_old_value = NULL;
    properties->change_new_value = NULL;
    properties->xpp = xpp;
    properties->item = item;
    properties->call_item_value_confirm = false;

    /*
     * If we're an item we may have some function which can be executed inside
     * the manager code.
     */
    if (item != NULL) {
        if (bit_test(item->mode, XANTE_ACCESS_EDIT) == false)
            properties->editable_value = false;
        else
            properties->editable_value = true;

        ptr = get_function_events(item->dialog_type);

        if (ptr != NULL) {
            properties->run = ptr->run;
            properties->validate_result = ptr->validate_result;
        }
    }
}

/**
 * @name properties_uninit
 * @brief Clears all properties of a selected item.
 *
 * Clears all properties releasing all previously allocated memory for the items'
 * property
 *
 * @param [in,out] properties: The properties of the item.
 */
void properties_uninit(ui_properties_t *properties)
{
    int i;
    DIALOG_LISTITEM *item = NULL;
    DIALOG_FORMITEM *fitem = NULL;

    if (properties->scroll_content != NULL)
        free(properties->scroll_content);

    if (properties->litems != NULL) {
        for (i = 0; i < properties->number_of_items; i++) {
            item = &properties->litems[i];
            free(item->text);
            free(item->name);
            free(item->help);
        }

        free(properties->litems);
    }

    if (properties->fitems != NULL) {
        for (i = 0; i < properties->number_of_items; i++) {
            fitem = &properties->fitems[i];
            free(fitem->name);
            free(fitem->text);
        }

        free(properties->fitems);
    }

    if (properties->text != NULL)
        cl_string_unref(properties->text);

    if (properties->result != NULL)
        cl_string_unref(properties->result);

    if (properties->title != NULL)
        cl_string_unref(properties->title);

    if (properties->row_title != NULL)
        cl_string_unref(properties->row_title);

    if (properties->column_title != NULL)
        cl_string_unref(properties->column_title);

    if (properties->change_item_name != NULL)
        cl_string_valueof(properties->change_item_name);

    if (properties->change_old_value != NULL)
        cl_string_valueof(properties->change_old_value);

    if (properties->change_new_value != NULL)
        cl_string_valueof(properties->change_new_value);

    if (properties->sheet != NULL)
        spreadsheet_st_destroy(properties->sheet);
}

