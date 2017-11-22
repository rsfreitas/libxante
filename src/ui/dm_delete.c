
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Thu Jun  1 12:53:38 2017
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

static DIALOG_LISTITEM *prepare_dialog_content(struct xante_menu *dm_menu,
    int total_items)
{
    DIALOG_LISTITEM *litems = NULL, *listitem = NULL;
    int index;
    cl_list_node_t *node = NULL;
    struct xante_item *item = NULL;

    litems = calloc(total_items, sizeof(DIALOG_LISTITEM));

    if (NULL == litems) {
        errno_set(XANTE_ERROR_NO_MEMORY);
        return NULL;
    }

    for (index = 0; index < total_items; index++) {
        node = cl_list_at(dm_menu->items, index);
        item = cl_list_node_content(node);

        listitem = &litems[index];
        listitem->name = strdup("");
        listitem->help = strdup("");
        listitem->text = strdup(cl_string_valueof(item->name));

        cl_list_node_unref(node);
    }

    /* Let the first option selected */
    litems[0].state = 1;

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

static void calc_checklist_limits(const struct xante_menu *dm_menu,
    int *options, int *height, int *list_height)
{
    *list_height = cl_list_size(dm_menu->items);
    *options = dialog_get_dlg_items(*list_height);

    /* XXX: This 1 is from the dialog text message. */
    *height = *options + DIALOG_HEIGHT_WITHOUT_TEXT + 1;
}

/*
 *
 * Internal API
 *
 */

/**
 * @name ui_dialog_delete_dm
 * @brief Creates a dialog to delete entries from a dynamic menu.
 *
 * The dialog will be a radio-checklist with the current dynamic menu options.
 *
 * @param [in,out] xpp: The library main object.
 * @param [in] item: The previously selected item.
 * @param [in] edit_value: A boolean flag indicating if is possible to edit
 *                         the option or not (removing an item).
 *
 * @return Returns a ui_return_t value indicating if an item was deleted or not.
 */
ui_return_t ui_dialog_delete_dm(struct xante_app *xpp, struct xante_item *item,
    bool edit_value)
{
    bool deleted = false, loop = true;
    struct xante_menu *dm_menu = NULL;
    DIALOG_LISTITEM *dlg_items = NULL;
    ui_return_t ret;
    int ret_dialog = DLG_EXIT_OK, list_options_height = 0, height = 0,
        number_of_options = 0, selected_index = -1;

    dm_menu = ui_search_menu_by_object_id(xpp->ui.menus,
                                          cl_string_valueof(item->menu_id));

    if (NULL == dm_menu) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                             cl_tr("No dynamic menu was found!"));

        ret.selected_button = ret_dialog;
        ret.updated_value = deleted;

        return ret;
    }

    if (cl_list_size(dm_menu->items) == 0) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                             cl_tr("The dynamic menu does not have any item "
                                   "to remove!"));

        ret.selected_button = ret_dialog;
        ret.updated_value = deleted;

        return ret;
    }

    /* Prepare dialog */
    dlgx_set_backtitle(xpp);
    dlgx_update_cancel_button_label();
    dlgx_put_statusbar((edit_value == true) ? DEFAULT_STATUSBAR_TEXT
                                            : DEFAULT_NOT_EDIT_STATUSBAR_TEXT);

    /* Prepare dialog content */
    calc_checklist_limits(dm_menu, &number_of_options, &height,
                          &list_options_height);

    dlg_items = prepare_dialog_content(dm_menu, list_options_height);

    /* Enables the help button */
    if (item->descriptive_help != NULL)
        dialog_vars.help_button = 1;

    do {
#ifdef ALTERNATIVE_DIALOG
        ret_dialog = dlg_checklist(cl_string_valueof(item->name),
                                   cl_string_valueof(item->options),
                                   height, MINIMUM_WIDTH, list_options_height,
                                   number_of_options, dlg_items, " X",
                                   item->dialog_checklist_type,
                                   &selected_index, NULL, NULL,
                                   edit_value);
#else
        ret_dialog = dlg_checklist(cl_string_valueof(item->name),
                                   cl_string_valueof(item->options), height,
                                   MINIMUM_WIDTH, list_options_height,
                                   number_of_options, dlg_items, " X",
                                   item->dialog_checklist_type,
                                   &selected_index);
#endif

        switch (ret_dialog) {
            case DLG_EXIT_OK:
                /*
                 * An item was selected. Now we're going to remove it from the
                 * dynamic menu.
                 */
                dm_delete(dm_menu, selected_index);
                loop = false;
                deleted = true;
                break;

#ifdef ALTERNATIVE_DIALOG
            case DLG_EXIT_TIMEOUT:
                loop = false;
                break;
#endif

            case DLG_EXIT_ESC:
            case DLG_EXIT_CANCEL:
                loop = false;
                break;

            case DLG_EXIT_HELP:
                dialog_vars.help_button = 0;
                xante_dlg_messagebox(xpp, XANTE_MSGBOX_INFO, cl_tr("Help"), "%s",
                                     cl_string_valueof(item->descriptive_help));

                dialog_vars.help_button = 1;
                break;
        }
    } while (loop);

    if (item->descriptive_help != NULL)
        dialog_vars.help_button = 0;

    release_dialog_content(dlg_items, list_options_height);

    ret.selected_button = ret_dialog;
    ret.updated_value = deleted;

    return ret;
}

