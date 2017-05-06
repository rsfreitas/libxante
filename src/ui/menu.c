
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
#include "ui_dialogs.h"

/*
 *
 * Internal functions
 *
 */

static int check_item_availability(cl_list_node_t *node, void *a)
{
    struct xante_item *item = cl_list_node_content(node);
    int *total = (int *)a;

    if (is_item_available(item))
        (*total)++;

    return 0;
}

static int calc_menu_items(const struct xante_menu *menu)
{
    int total = 0;

    cl_list_map(menu->items, check_item_availability, &total);

    return total;
}

static int calc_menu_width(const struct xante_menu *menu)
{
    return 0;
}

static void calc_menu_limits(const struct xante_menu *menu, int *lines,
    int *width, int *items, int *total)
{
    *total = calc_menu_items(menu);
    *items = dialog_get_dlg_items(*total);
    *width = calc_menu_width(menu);
    *lines = *items;
}

static DIALOG_LISTITEM *prepare_dialog_content(const struct xante_menu *menu)
{
    return NULL;
}

static void prepare_dialog_look(struct xante_app *xpp,
    const char *cancel_label)
{
    int timeout = -1;

    dialog_set_backtitle();
    dialog_vars.cancel_label = strdup(cancel_label);
    timeout = xante_runtime_ui_dialog_timeout(xpp);

    if (timeout > 0)
        dialog_vars.timeout_secs = timeout;
}

static void release_dialog_look(void)
{
    if (dialog_vars.cancel_label != NULL) {
        free(dialog_vars.cancel_label);
        dialog_vars.cancel_label = NULL;
    }
}

static void release_dialog_content(DIALOG_LISTITEM *listitem, int total_items)
{
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

    release_dialog_look();

    do {
        if (xante_runtime_close_ui(xpp) == true)
            break;

        calc_menu_limits(menu, &dlg_lines, &dlg_width, &items_to_select,
                         &total_items);

        dlg_items = prepare_dialog_content(menu);
        prepare_dialog_look(xpp, cancel_label);
        ret_dialog = dlg_menu(cl_string_valueof(menu->name), "", dlg_lines,
                              dlg_width, items_to_select, total_items,
                              dlg_items, &selected_index, NULL);

        switch (ret_dialog) {
            case DLG_EXIT_OK:
                break;

            case DLG_EXIT_ESC:
            case DLG_EXIT_CANCEL:
                loop = true;
                break;

            case DLG_EXIT_HELP:
                /*
                 * TODO: Think better of this, we don't have this info for
                 *       menus yet.
                 */
                break;
        }

        release_dialog_look();
        release_dialog_content(dlg_items, total_items);
    } while (loop);

    return ret_dialog;
}

