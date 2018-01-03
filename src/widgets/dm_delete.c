
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

#define DEFAULT_WIDTH                   DEFAULT_DIALOG_WIDTH

/* XXX: The +1 is because of the window subtitle. */
#define DEFAULT_HEIGHT                  \
    (DIALOG_HEIGHT_WITHOUT_TEXT + MAX_DLG_ITEMS + 1)

/*
 *
 * Internal functions
 *
 */

static int prepare_content(const struct xante_menu *dm_menu,
    session_t *session)
{
    DIALOG_LISTITEM *litems = NULL, *listitem = NULL;
    int index;
    cl_list_node_t *node = NULL;
    struct xante_item *item = NULL;

    session->litems = calloc(session->number_of_items,
                                sizeof(DIALOG_LISTITEM));

    if (NULL == session->litems) {
        errno_set(XANTE_ERROR_NO_MEMORY);
        return -1;
    }

    for (index = 0; index < session->number_of_items; index++) {
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

    return 0;
}

static void build_session(const struct xante_menu *dm_menu,
    session_t *session)
{
    struct xante_item *item = session->item;
    int prompt = 0;

    if (item->options != NULL)
        prompt = 1;

    session->number_of_items = cl_stringlist_size(dm_menu->items);
    session->width = (dm_menu->geometry.width == 0) ? DEFAULT_WIDTH
                                                       : dm_menu->geometry.width;

    session->displayed_items = dlgx_get_dlg_items(session->number_of_items);
    session->height = (dm_menu->geometry.height == 0)
                ? (session->displayed_items + DIALOG_HEIGHT_WITHOUT_TEXT + prompt)
                : dm_menu->geometry.height;


    /* Creates the UI content */
    prepare_content(dm_menu, session);
}

/*
 *
 * Internal API
 *
 */

bool delete_dm_value_changed(session_t *session)
{
    if (NULL == session->result)
        return false;

    /* Set up details to save inside the internal changes list */
    session->change_item_name = cl_string_create("Removed dynamic menu entry");
    session->change_old_value = cl_string_create("EMPTY");
    session->change_new_value = cl_string_create("EMPTY");

    return true;
}

/**
 * @name ui_delete_dm
 * @brief Creates a dialog to delete entries from a dynamic menu.
 *
 * The dialog will be a radio-checklist with the current dynamic menu options.
 *
 * @return Returns a ui_return_t value indicating if an item was deleted or not.
 */
int delete_dm(session_t *session)
{
    struct xante_app *xpp = session->xpp;
    struct xante_item *item = session->item;
    struct xante_menu *dm_menu = NULL;
    int ret_dialog = DLG_EXIT_OK, selected_index = -1;

    dm_menu = xante_menu_search_by_object_id(xpp->ui.menus,
                                             cl_string_valueof(item->referenced_menu));

    if (NULL == dm_menu) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                             cl_tr("No dynamic menu was found!"));

        return DLG_EXIT_OK;
    }

    if (cl_list_size(dm_menu->items) == 0) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                             cl_tr("The dynamic menu does not have any item "
                                   "to remove!"));

        return DLG_EXIT_OK;
    }

    /* Prepare dialog content */
    build_session(dm_menu, session);

#ifdef ALTERNATIVE_DIALOG
    ret_dialog = dlg_checklist(cl_string_valueof(item->name),
                               cl_string_valueof(item->options),
                               session->height, session->width,
                               session->displayed_items,
                               session->number_of_items,
                               session->litems, " X",
                               item->widget_checklist_type,
                               &selected_index, NULL, NULL,
                               session->editable_value);
#else
    ret_dialog = dlg_checklist(cl_string_valueof(item->name),
                               cl_string_valueof(item->options),
                               session->height, session->width,
                               session->displayed_items,
                               session->number_of_items,
                               session->litems, " X",
                               item->widget_checklist_type,
                               &selected_index);
#endif

    if (ret_dialog == DLG_EXIT_OK) {
        dm_delete(dm_menu, selected_index);

        /* We hold a simple string just to know that we have a change */
        session->result = cl_string_create("changed");
    }

    return ret_dialog;
}

