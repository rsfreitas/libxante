
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
    session_t *session)
{
    DIALOG_LISTITEM *listitem = NULL;
    int index, current_value;
    cl_string_t *option = NULL;

    session->litems = calloc(session->number_of_items,
                                sizeof(DIALOG_LISTITEM));

    if (NULL == session->litems) {
        errno_set(XANTE_ERROR_NO_MEMORY);
        return -1;
    }

    current_value = CL_OBJECT_AS_INT(item_value(item));

    for (index = 0; index < session->number_of_items; index++) {
        listitem = &session->litems[index];
        option = cl_stringlist_get(item->list_items, index);

        listitem->name = strdup("");
        listitem->help = strdup("");
        listitem->text = strdup(cl_string_valueof(option));

        if (((item->widget_checklist_type == FLAG_RADIO) &&
             ((int)index == current_value)) ||
            ((item->widget_checklist_type == FLAG_CHECK) &&
             ((int)pow(2, index) & current_value)))
        {
            listitem->state = 1;
        } else
            listitem->state = 0;

        cl_string_unref(option);
    }

    return 0;
}

static void build_session(const struct xante_item *item,
    session_t *session)
{
    session->number_of_items = cl_stringlist_size(item->list_items);
    session->width = (item->geometry.width == 0) ? DEFAULT_WIDTH
                                                    : item->geometry.width;

    /* The +1 on height is because we have a prompt inside the dialog */
    session->displayed_items = dlgx_get_dlg_items(session->number_of_items);
    session->height = (item->geometry.height == 0)
                ? (session->displayed_items + DIALOG_HEIGHT_WITHOUT_TEXT + 1)
                : item->geometry.height;

    /* Creates the UI content */
    prepare_content(item, session);
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

static void add_internal_change(session_t *session, int current_value,
    int selected_value)
{
    struct xante_item *item = session->item;
    DIALOG_LISTITEM *dlg_items = session->litems;
    int total_items = session->number_of_items;
    cl_stringlist_t *current_entries = NULL, *selected_entries = NULL;

    current_entries = get_checklist_entries(dlg_items, total_items,
                                            item->widget_checklist_type,
                                            current_value);

    selected_entries = get_checklist_entries(dlg_items, total_items,
                                             item->widget_checklist_type,
                                             selected_value);

    /* Set up details to save inside the internal changes list */
    session->change_item_name = cl_string_ref(item->name);
    session->change_old_value = cl_stringlist_flat(current_entries, ',');
    session->change_new_value = cl_stringlist_flat(selected_entries, ',');

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

bool checklist_validate_result(session_t *session)
{
    if (NULL == session->result)
        return false;

    return true;
}

bool checklist_value_changed(session_t *session)
{
    struct xante_item *item = session->item;
    int current_value, selected_items;
    bool changed = false;

    selected_items = cl_string_to_int(session->result);
    current_value = CL_OBJECT_AS_INT(item_value(item));

    if (current_value != selected_items) {
        add_internal_change(session, current_value, selected_items);
        changed = true;
    }

    return changed;
}

void checklist_update_value(session_t *session)
{
    struct xante_item *item = session->item;
    int selected_items;

    selected_items = cl_string_to_int(session->result);

    /* Updates item value */
    if (NULL == item->value)
        item->value = cl_object_create(CL_INT, selected_items);
    else
        cl_object_set(item->value, selected_items);
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
int checklist(session_t *session)
{
    struct xante_app *xpp = session->xpp;
    struct xante_item *item = session->item;
    int ret_dialog = DLG_EXIT_OK, selected_index = -1, selected_items;

    /* Prepares dialog content */
    build_session(item, session);

#ifdef ALTERNATIVE_DIALOG
    ret_dialog = dlg_checklist(cl_string_valueof(item->name),
                               cl_tr("Choose an option"),
                               session->height, session->width,
                               session->displayed_items,
                               session->number_of_items,
                               session->litems, " X",
                               item->widget_checklist_type,
                               &selected_index,
                               update_item_brief, item,
                               session->editable_value);
#else
    ret_dialog = dlg_checklist(cl_string_valueof(item->name),
                               cl_tr("Choose an option"),
                               session->height, session->width,
                               session->displayed_items,
                               session->number_of_items,
                               session->litems,
                               " X", item->widget_checklist_type,
                               &selected_index);
#endif

    if ((ret_dialog == DLG_EXIT_OK) && session->editable_value) {
        selected_items = get_checklist_selected_value(session->litems,
                                                      session->number_of_items,
                                                      item->widget_checklist_type);

        if (selected_items < 0 ) {
            xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                                 "%s", cl_tr("No option was selected."));
        } else
            session->result = cl_string_create("%d", selected_items);
    }

    return ret_dialog;
}

