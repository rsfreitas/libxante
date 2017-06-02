
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
#include "ui_dialogs.h"

#define DEFAULT_STATUSBAR_TEXT          \
    "[ESC] Cancel [Enter] Confirm a selection [Up/Down] Move [TAB/Left/Right] Choose option [Spacebar] Select option"

#define DEFAULT_NOT_EDIT_STATUSBAR_TEXT \
    "[ESC] Cancel [Enter] Confirm a selection [Up/Down] Move [TAB/Left/Right] Choose option"

/*
 *
 * Internal functions
 *
 */

static DIALOG_LISTITEM *prepare_dialog_content(struct xante_item *item,
    int total_items)
{
    DIALOG_LISTITEM *litems = NULL, *listitem = NULL;
    int index, total, current_value;
    cl_string_t *option = NULL;

    litems = calloc(total_items, sizeof(DIALOG_LISTITEM));

    if (NULL == litems) {
        errno_set(XANTE_ERROR_NO_MEMORY);
        return NULL;
    }

    current_value = CL_OBJECT_AS_INT(item_value(item));
    total = cl_string_list_size(item->checklist_options);

    for (index = 0; index < total; index++) {
        listitem = &litems[index];
        option = cl_string_list_get(item->checklist_options, index);

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

    return litems;
}

static void release_dialog_content(DIALOG_LISTITEM *listitem, int total_items)
{
    int i;

    for (i = 0; i < total_items; i++) {
        free(listitem[i].text);
        free(listitem[i].name);
        free(listitem[i].help);
    }

    free(listitem);
}

static void calc_checklist_limits(const struct xante_item *item, int *options,
    int *height, int *list_height)
{
    *list_height = cl_string_list_size(item->checklist_options);
    *options = dialog_get_dlg_items(*list_height);

    /* XXX: This 1 is from the dialog text message. */
    *height = *options + DIALOG_HEIGHT_WITHOUT_TEXT + 1;
}

static cl_string_list_t *get_checklist_entries(DIALOG_LISTITEM *dlg_items,
    int total_items, int checklist_type, int selected_option)
{
    cl_string_list_t *selected_entries = NULL;
    cl_string_t *option = NULL;
    int i;

    selected_entries = cl_string_list_create();

    if (NULL == selected_entries)
        return NULL;

    if (checklist_type == FLAG_RADIO) {
        option = cl_string_create("%s", dlg_items[selected_option].text);
        cl_string_list_add(selected_entries, option);
        cl_string_unref(option);
    } else {
        for (i = 0; i < total_items; i++) {
            if ((int)pow(2, i) & selected_option) {
                option = cl_string_create("%s", dlg_items[i].text);
                cl_string_list_add(selected_entries, option);
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

static void add_internal_change(struct xante_app *xpp, struct xante_item *item,
    DIALOG_LISTITEM *dlg_items, int total_items, int current_value,
    int selected_value)
{
    cl_string_list_t *current_entries = NULL, *selected_entries = NULL;
    cl_string_t *current = NULL, *selected = NULL;

    current_entries = get_checklist_entries(dlg_items, total_items,
                                            item->dialog_checklist_type,
                                            current_value);

    selected_entries = get_checklist_entries(dlg_items, total_items,
                                             item->dialog_checklist_type,
                                             selected_value);

    current = cl_string_list_flat(current_entries, ',');
    selected = cl_string_list_flat(selected_entries, ',');
    change_add(xpp, cl_string_valueof(item->name), cl_string_valueof(current),
               cl_string_valueof(selected));

    if (current != NULL)
        cl_string_unref(current);

    if (selected != NULL)
        cl_string_unref(selected);

    if (current_entries != NULL)
        cl_string_list_destroy(current_entries);

    if (selected_entries != NULL)
        cl_string_list_destroy(selected_entries);
}

static bool item_value_has_changed(struct xante_app *xpp, struct xante_item *item,
    DIALOG_LISTITEM *dlg_items, int total_items, int selected_items)
{
    int current_value;
    bool changed = false;

    current_value = CL_OBJECT_AS_INT(item_value(item));

    if (current_value != selected_items) {
        add_internal_change(xpp, item, dlg_items, total_items, current_value,
                            selected_items);

        changed = true;

        /* Updates item value */
        if (NULL == item->value)
            item->value = cl_object_create(CL_INT, selected_items);
        else
            cl_object_set(item->value, selected_items);
    }

    return changed;
}

static void update_item_brief(int current_index, void *a)
{
    struct xante_item *item = (struct xante_item *)a;
    cl_string_t *brief = NULL;

    if (NULL == item->checklist_brief_options)
        return;

    brief = cl_string_list_get(item->checklist_brief_options, current_index);

    if (NULL == brief)
        return;

    dialog_put_item_brief(cl_string_valueof(brief));
    cl_string_unref(brief);
}

/*
 *
 * Internal API
 *
 */

/**
 * @name ui_dialog_checklist
 * @brief Creates a dialog to choose an option inside a list of options.
 *
 * Since we (and libdialog) use an int variable to store the selected options,
 * we only have 32 available options.
 *
 * @param [in] xpp: The main library object.
 * @param [in] item: The item to be used inside the dialog.
 * @param [in] edit_value: A flag to indicate if the value will be editable
 *                         or not.
 *
 * @return Returns a boolean value indicating if the item's value has been
 *         changed (true) or not (false).
 */
bool ui_dialog_checklist(struct xante_app *xpp, struct xante_item *item,
    bool edit_value)
{
    bool loop = true, value_changed = false;
    int ret_dialog = DLG_EXIT_OK, list_options_height = 0, height = 0,
        number_of_options = 0, selected_index = -1, selected_items;
    DIALOG_LISTITEM *dlg_items = NULL;

    /* Prepare dialog */
    dialog_set_backtitle(xpp);
    dialog_update_cancel_button_label();
    dialog_put_statusbar((edit_value == true) ? DEFAULT_STATUSBAR_TEXT
                                              : DEFAULT_NOT_EDIT_STATUSBAR_TEXT);

    /* Prepares dialog content */
    calc_checklist_limits(item, &number_of_options, &height,
                          &list_options_height);

    dlg_items = prepare_dialog_content(item, list_options_height);

    /* Enables the help button */
    if (item->descriptive_help != NULL)
        dialog_vars.help_button = 1;

    do {
#ifdef ALTERNATIVE_DIALOG
        ret_dialog = dlg_checklist(cl_string_valueof(item->name),
                                   cl_tr("Choose an option"), height,
                                   MINIMUM_WIDTH, list_options_height,
                                   number_of_options, dlg_items, " X",
                                   item->dialog_checklist_type,
                                   &selected_index,
                                   update_item_brief, item,
                                   edit_value);
#else
        ret_dialog = dlg_checklist(cl_string_valueof(item->name),
                                   cl_tr("Choose an option"), height,
                                   MINIMUM_WIDTH, list_options_height,
                                   number_of_options, dlg_items, " X",
                                   item->dialog_checklist_type,
                                   &selected_index,
                                   update_item_brief, item);
#endif

        switch (ret_dialog) {
            case DLG_EXIT_OK:
                selected_items = get_checklist_selected_value(dlg_items,
                                                              number_of_options,
                                                              item->dialog_checklist_type);

                if (selected_items < 0 ) {
                    xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, 0,
                                         cl_tr("Error"),
                                         cl_tr("No option was selected."));

                    break;
                }

                if (event_call(EV_ITEM_VALUE_CONFIRM, xpp, item,
                               selected_items) < 0)
                {
                    break;
                }

                if (item_value_has_changed(xpp, item, dlg_items,
                                           number_of_options, selected_items))
                {
                    value_changed = true;
                }

                loop = false;
                break;

            case DLG_EXIT_ESC:
            case DLG_EXIT_CANCEL:
                loop = false;
                break;

            case DLG_EXIT_HELP:
                dialog_vars.help_button = 0;
                xante_dlg_messagebox(xpp, XANTE_MSGBOX_INFO, 0, cl_tr("Help"),
                                     cl_string_valueof(item->descriptive_help));

                dialog_vars.help_button = 1;
                break;
        }
    } while (loop);

    if (item->descriptive_help != NULL)
        dialog_vars.help_button = 0;

    release_dialog_content(dlg_items, list_options_height);

    return value_changed;
}

