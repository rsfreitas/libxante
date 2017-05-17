
/*
 * Description: Handles the menu dialog.
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
#include "ui_dialogs.h"

/*
 *
 * Internal functions
 *
 */

/* A structure to be used while mapping lists */
struct list_data {
    int total;
    int name_size;
    int item_size;
};

static int check_item_availability(cl_list_node_t *node, void *a)
{
    struct xante_item *item = cl_list_node_content(node);
    struct list_data *ld = (struct list_data *)a;

    if (is_item_available(item))
        ld->total++;

    return 0;
}

static int calc_menu_items(const struct xante_menu *menu)
{
    struct list_data ld = {
        .total = 0,
    };

    cl_list_map(menu->items, check_item_availability, &ld);

    return ld.total;
}

static bool item_may_have_value(const struct xante_item *item)
{
    switch (item->dialog_type) {
        /*
         * These are all dialogs that may have values and their values are
         * viewed beside its name inside a menu. So the user may know its
         * current value.
         */
        case XANTE_UI_DIALOG_INPUT_INT:
        case XANTE_UI_DIALOG_INPUT_FLOAT:
        case XANTE_UI_DIALOG_INPUT_STRING:
        case XANTE_UI_DIALOG_INPUT_DATE:
        case XANTE_UI_DIALOG_INPUT_TIME:
        case XANTE_UI_DIALOG_CALENDAR:
        case XANTE_UI_DIALOG_TIMEBOX:
        case XANTE_UI_DIALOG_RADIO_CHECKLIST:
        case XANTE_UI_DIALOG_YES_NO:
            return true;

        default:
            break;
    }

    return false;
}

static int check_item_width(cl_list_node_t *node, void *a)
{
    struct xante_item *item = cl_list_node_content(node);
    struct list_data *ld = (struct list_data *)a;
    int length = 0;
    char *s = NULL;

    length = cl_string_length(item->name);

    if (length > ld->name_size)
        ld->name_size = length;

    /* We only check items that may have values */
    if (item_may_have_value(item) == true) {
        s = dialog_get_item_value_as_text(item);

        if (NULL == s)
            return 0;

        length = strlen(s);
        free(s);

        if (length > ld->item_size)
            ld->item_size = length;
    }

    return 0;
}

static int calc_menu_width(const struct xante_menu *menu)
{
    int w = 0;
    struct list_data ld = {
        .name_size = 0,
        .item_size = 0,
    };

    cl_list_map(menu->items, check_item_width, &ld);
    w = (ld.name_size + ld.item_size) + WINDOW_BORDER_SIZE;

    if (w < MINIMUM_WIDTH)
        w = MINIMUM_WIDTH;

    return (w == 0) ? MINIMUM_WIDTH : w;
}

static void calc_menu_limits(const struct xante_menu *menu, int *lines,
    int *width, int *items, int *total)
{
    *total = calc_menu_items(menu);
    *items = dialog_get_dlg_items(*total);
    *width = calc_menu_width(menu);
    *lines = *items + DIALOG_HEIGHT_WITHOUT_TEXT;
}

static int add_item_content(unsigned int index, cl_list_node_t *node, void *a)
{
    struct xante_item *item = cl_list_node_content(node);
    DIALOG_LISTITEM *litems = (DIALOG_LISTITEM *)a;
    DIALOG_LISTITEM *listitem = NULL;
    char *value = NULL;

    if (is_item_available(item) == false)
        return 0;

    /* Fills litems[index] with item content */
    listitem = &litems[index];
    value = dialog_get_item_value_as_text(item);

    if (NULL == value)
        listitem->text = strdup("");
    else
        listitem->text = value;

    listitem->name = strdup(cl_string_valueof(item->name));
    listitem->help = strdup("");
    listitem->state = 0;

    /* XXX: This is from our libdialog fork */
//    listitem->custom_highlight = 0;
//    listitem->idx_highlight = 0;

    return 0;
}

static DIALOG_LISTITEM *prepare_dialog_content(const struct xante_menu *menu,
    int total_items)
{
    DIALOG_LISTITEM *litems = NULL;

    litems = calloc(total_items, sizeof(DIALOG_LISTITEM));

    if (NULL == litems) {
        errno_set(XANTE_ERROR_NO_MEMORY);
        return NULL;
    }

    cl_list_map_indexed(menu->items, add_item_content, litems);

    return litems;
}

static void prepare_dialog_look(struct xante_app *xpp,
    const char *cancel_label)
{
    int timeout = -1;

    dialog_set_backtitle(xpp);
    dialog_vars.cancel_label = strdup(cancel_label);
    timeout = xante_runtime_ui_dialog_timeout(xpp);

    if (timeout > 0)
        dialog_vars.timeout_secs = timeout;
}

static void release_dialog_labels(void)
{
    if (dialog_vars.cancel_label != NULL) {
        free(dialog_vars.cancel_label);
        dialog_vars.cancel_label = NULL;
    }
}

static void release_dialog_content(DIALOG_LISTITEM *listitem, int total_items)
{
    int i;

    for (i = 0; i < total_items; i++) {
        free(listitem[i].name);
        free(listitem[i].help);
        free(listitem[i].text);
    }

    free(listitem);
}

static void call_menu_dialog(struct xante_app *xpp,
    struct xante_item *selected_item, bool remove_item_from_menu)
{
    struct xante_menu *menu = NULL;
    char *btn_cancel_label = NULL;

    menu = ui_search_menu_by_object_id(xpp,
                                       cl_string_valueof(selected_item->object_id));

    if (NULL == menu) {
        return;
    }

    btn_cancel_label = strdup(cl_tr("Back"));
    ui_dialog_menu(xpp, menu, btn_cancel_label, remove_item_from_menu,
                   selected_item);

    free(btn_cancel_label);
}

static bool call_input_dialog(struct xante_app *xpp,
    struct xante_item *selected_item, bool edit_value)
{
    return ui_dialog_input(xpp, selected_item, edit_value);
}

static bool call_calendar_dialog(struct xante_app *xpp,
    struct xante_item *selected_item, bool edit_value)
{
    return ui_dialog_calendar(xpp, selected_item, edit_value);
}

static bool call_timebox_dialog(struct xante_app *xpp,
    struct xante_item *selected_item, bool edit_value)
{
    return ui_dialog_timebox(xpp, selected_item, edit_value);
}

static bool call_checklist_dialog(struct xante_app *xpp,
    struct xante_item *selected_item, bool edit_value)
{
    return ui_dialog_checklist(xpp, selected_item, edit_value);
}

static bool call_yesno_dialog(struct xante_app *xpp,
    struct xante_item *selected_item)
{
    return ui_dialog_yesno(xpp, selected_item);
}

static void call_selected_item(struct xante_app *xpp,
    const struct xante_menu *menu, cl_list_node_t *selected_item_node,
    bool remove_item_from_menu)
{
    bool update_internal_menus = false, edit_item_value = true;
    struct xante_item *selected_item = cl_list_node_content(selected_item_node);

    cl_list_node_unref(selected_item_node);

    if (NULL == selected_item) {
        return;
    }

    /* Are we leaving? */
    if (xante_runtime_close_ui(xpp) == true)
        return;

    if (remove_item_from_menu == true) {
        /* TODO: Future */
        return;
    }

    /* TODO: Verify input edit */

    switch (selected_item->dialog_type) {
        case XANTE_UI_DIALOG_MENU:
        case XANTE_UI_DIALOG_DYNAMIC_MENU:
            call_menu_dialog(xpp, selected_item, remove_item_from_menu);
            break;

        case XANTE_UI_DIALOG_INPUT_INT:
        case XANTE_UI_DIALOG_INPUT_FLOAT:
        case XANTE_UI_DIALOG_INPUT_DATE:
        case XANTE_UI_DIALOG_INPUT_STRING:
        case XANTE_UI_DIALOG_INPUT_TIME:
        case XANTE_UI_DIALOG_INPUT_PASSWD:
            update_internal_menus = call_input_dialog(xpp, selected_item,
                                                      edit_item_value);

            break;

        case XANTE_UI_DIALOG_CALENDAR:
            update_internal_menus = call_calendar_dialog(xpp, selected_item,
                                                         edit_item_value);

            break;

        case XANTE_UI_DIALOG_TIMEBOX:
            update_internal_menus = call_timebox_dialog(xpp, selected_item,
                                                        edit_item_value);

            break;

        case XANTE_UI_DIALOG_RADIO_CHECKLIST:
        case XANTE_UI_DIALOG_CHECKLIST:
            update_internal_menus = call_checklist_dialog(xpp, selected_item,
                                                          edit_item_value);

            break;

        case XANTE_UI_DIALOG_YES_NO:
            if (edit_item_value == true)
               update_internal_menus = call_yesno_dialog(xpp, selected_item);
            else {
            }

            break;

        case XANTE_UI_DIALOG_RM_DYNAMIC_MENU:
            break;

        default:
            break;
    }

    if (update_internal_menus == true) {
        /* TODO */
    }
}

/*
 *
 * Internal API
 *
 */

/**
 * @name ui_dialog_menu
 * @brief Creates a dialog of menu type.
 *
 * @param [in,out] xpp: The main library object.
 * @param [in] menu: The menu data to be used as content to the dialog.
 * @param [in] cancel_label: A string label of the cancel button.
 * @param [in] remove_item_from_menu: A boolean flag used when removing
 *                                    item from a dynamic menu.
 * @param [in] selected_item: The previously selected item.
 *
 * @return Returns the libdialog type of return value.
 */
int ui_dialog_menu(struct xante_app *xpp, const struct xante_menu *menu,
    const char *cancel_label, bool remove_item_from_menu,
    struct xante_item *selected_item)
{
    DIALOG_LISTITEM *dlg_items = NULL;
    bool loop = true;
    int ret_dialog = DLG_EXIT_OK, items_to_select = 0, total_items = 0,
        dlg_width = 0, selected_index = -1, dlg_lines = 0;

    release_dialog_labels();

    do {
        if (xante_runtime_close_ui(xpp) == true)
            break;

        calc_menu_limits(menu, &dlg_lines, &dlg_width, &items_to_select,
                         &total_items);

        dlg_items = prepare_dialog_content(menu, total_items);
        prepare_dialog_look(xpp, cancel_label);
        ret_dialog = dlg_menu(cl_string_valueof(menu->name), "", dlg_lines,
                              dlg_width, items_to_select, total_items,
                              dlg_items, &selected_index, NULL);

        switch (ret_dialog) {
            case DLG_EXIT_OK:
                call_selected_item(xpp, menu,
                                   cl_list_at(menu->items, selected_index),
                                   remove_item_from_menu);

                break;

            case DLG_EXIT_ESC:
            case DLG_EXIT_CANCEL:
                /* Are we leaving the application? */
                if (strcmp(cancel_label, MAIN_MENU_CANCEL_LABEL) == 0) {
                    if (dialog_question(xpp, cl_tr("Closing"),
                                        cl_tr("Do you really want to quit?"),
                                        cl_tr("Yes"), cl_tr("No")) == true)
                    {
                        loop = false;
                    }
                } else
                    loop = false;

                break;

            case DLG_EXIT_HELP:
                /*
                 * TODO: Think better of this, we don't have this info for
                 *       menus yet.
                 */
                break;
        }

        release_dialog_labels();
        release_dialog_content(dlg_items, total_items);
    } while (loop);

    return ret_dialog;
}

