
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Fri May  5 21:08:21 2017
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

#define DEFAULT_WIDTH                   DEFAULT_DIALOG_WIDTH

/* XXX: The +1 is because of the window subtitle. */
#define DEFAULT_HEIGHT                  \
    (DIALOG_HEIGHT_WITHOUT_TEXT + MAX_DLG_ITEMS + 1)

/*
 *
 * Internal functions
 *
 */

static int prepare_content(const struct xante_item *item,
    ui_properties_t *properties)
{
    DIALOG_LISTITEM *listitem = NULL;
    int index, current_value;
    cl_string_t *option = NULL;

    properties->litems = calloc(properties->number_of_items,
                                sizeof(DIALOG_LISTITEM));

    if (NULL == properties->litems) {
        errno_set(XANTE_ERROR_NO_MEMORY);
        return -1;
    }

    current_value = CL_OBJECT_AS_INT(item_value(item));

    for (index = 0; index < properties->number_of_items; index++) {
        listitem = &properties->litems[index];
        option = cl_stringlist_get(item->list_items, index);

        listitem->name = strdup("");
        listitem->help = strdup("");
        listitem->text = strdup(cl_string_valueof(option));

        if (((item->dialog_checklist_type == FLAG_RADIO) &&
             ((int)index == current_value)) ||
            ((item->dialog_checklist_type == FLAG_CHECK) &&
             ((int)pow(2, index) & current_value)))
        {
            listitem->state = 1;
        } else
            listitem->state = 0;

        cl_string_unref(option);
    }

    return 0;
}

static void build_properties(const struct xante_item *item,
    ui_properties_t *properties)
{
    properties->number_of_items = cl_stringlist_size(item->list_items);
    properties->width = (item->geometry.width == 0) ? DEFAULT_WIDTH
                                                    : item->geometry.width;

    properties->height = (item->geometry.height == 0) ? DEFAULT_HEIGHT
                                                      : item->geometry.height;

    properties->displayed_items = properties->height - DIALOG_HEIGHT_WITHOUT_TEXT;

    /* Creates the UI content */
    prepare_content(item, properties);
}

static cl_stringlist_t *get_checklist_entries(DIALOG_LISTITEM *dlg_items,
    int total_items, int checklist_type, int selected_option)
{
    cl_stringlist_t *selected_entries = NULL;
    cl_string_t *option = NULL;
    int i;

    selected_entries = cl_stringlist_create();

    if (NULL == selected_entries)
        return NULL;

    if (checklist_type == FLAG_RADIO) {
        option = cl_string_create("%s", dlg_items[selected_option].text);
        cl_stringlist_add(selected_entries, option);
        cl_string_unref(option);
    } else {
        for (i = 0; i < total_items; i++) {
            if ((int)pow(2, i) & selected_option) {
                option = cl_string_create("%s", dlg_items[i].text);
                cl_stringlist_add(selected_entries, option);
                cl_string_unref(option);
            }
        }
    }

    return selected_entries;
}

static int get_checklist_selected_value(DIALOG_LISTITEM *dlg_items,
    int total_items, int checklist_type)
{
    int i, selected_value = 0;

    for (i = 0; i < total_items; i++)
        if (dlg_items[i].state) {
            if (checklist_type == FLAG_RADIO)
                return i;
            else
                selected_value += (int)pow(2, i);
        }

    return (selected_value != 0) ? selected_value : -1;
}

static void add_internal_change(ui_properties_t *properties, int current_value,
    int selected_value)
{
    struct xante_item *item = properties->item;
    DIALOG_LISTITEM *dlg_items = properties->litems;
    int total_items = properties->number_of_items;
    cl_stringlist_t *current_entries = NULL, *selected_entries = NULL;

    current_entries = get_checklist_entries(dlg_items, total_items,
                                            item->dialog_checklist_type,
                                            current_value);

    selected_entries = get_checklist_entries(dlg_items, total_items,
                                             item->dialog_checklist_type,
                                             selected_value);

    /* Set up details to save inside the internal changes list */
    properties->change_item_name = cl_string_ref(item->name);
    properties->change_old_value = cl_stringlist_flat(current_entries, ',');
    properties->change_new_value = cl_stringlist_flat(selected_entries, ',');

    if (current_entries != NULL)
        cl_stringlist_destroy(current_entries);

    if (selected_entries != NULL)
        cl_stringlist_destroy(selected_entries);
}

#ifdef ALTERNATIVE_DIALOG
static void update_item_brief(int current_index, void *a)
{
    struct xante_item *item = (struct xante_item *)a;
    cl_string_t *brief = NULL;

    if (NULL == item->checklist_brief_options)
        return;

    brief = cl_stringlist_get(item->checklist_brief_options, current_index);

    if (NULL == brief)
        return;

    dlgx_put_item_brief(cl_string_valueof(brief));
    cl_string_unref(brief);
}
#endif

/*
 *
 * Internal API
 *
 */

bool checklist_validate_result(ui_properties_t *properties)
{
    struct xante_item *item = properties->item;
    int current_value, selected_items;
    bool changed = false;

    selected_items = cl_string_to_int(properties->result);
    current_value = CL_OBJECT_AS_INT(item_value(item));

    if (current_value != selected_items) {
        add_internal_change(properties, current_value, selected_items);
        changed = true;

        /* Updates item value */
        if (NULL == item->value)
            item->value = cl_object_create(CL_INT, selected_items);
        else
            cl_object_set(item->value, selected_items);
    }

    return changed;
}

/**
 * @name ui_checklist
 * @brief Creates a dialog to choose an option inside a list of options.
 *
 * Since we (and libdialog) use an int variable to store the selected options,
 * we only have 32 available options.
 *
 * @return Returns a ui_return_t value indicating if the item's value has been
 *         changed (true) or not (false) with the dialog selected button.
 */
int checklist(ui_properties_t *properties)
{
    struct xante_app *xpp = properties->xpp;
    struct xante_item *item = properties->item;
    int ret_dialog = DLG_EXIT_OK, selected_index = -1, selected_items;

    /* Prepares dialog content */
    build_properties(item, properties);

#ifdef ALTERNATIVE_DIALOG
    ret_dialog = dlg_checklist(cl_string_valueof(item->name),
                               cl_tr("Choose an option"),
                               properties->height, properties->width,
                               properties->displayed_items,
                               properties->number_of_items,
                               properties->litems, " X",
                               item->dialog_checklist_type,
                               &selected_index,
                               update_item_brief, item,
                               properties->editable_value);
#else
    ret_dialog = dlg_checklist(cl_string_valueof(item->name),
                               cl_tr("Choose an option"),
                               properties->height, properties->width,
                               properties->displayed_items,
                               properties->number_of_items,
                               properties->litems,
                               " X", item->dialog_checklist_type,
                               &selected_index);
#endif

    if ((ret_dialog == DLG_EXIT_OK) && properties->editable_value) {
        selected_items = get_checklist_selected_value(properties->litems,
                                                      properties->number_of_items,
                                                      item->dialog_checklist_type);

        if (selected_items < 0 ) {
            xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                                 "%s", cl_tr("No option was selected."));
        } else
            properties->result = cl_string_create("%d", selected_items);
    }

    return ret_dialog;
}

