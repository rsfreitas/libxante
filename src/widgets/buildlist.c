
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Mon Nov 27 15:10:13 2017
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

/*
 * XXX: This DIALOG_WIDTH is important because we use half of its value
 *      to build the cprompt of the object.
 */
#define DIALOG_WIDTH                60
#define DIALOG_HEIGHT               22

/*
 *
 * Internal functions
 *
 */

static int prepare_content(struct xante_item *item,
    session_t *session)
{
    DIALOG_LISTITEM *listitem = NULL;
    cl_string_t *p = NULL;
    int index;

    session->litems = calloc(session->number_of_items,
                                sizeof(DIALOG_LISTITEM));

    if (NULL == session->litems) {
        errno_set(XANTE_ERROR_NO_MEMORY);
        return -1;
    }

    /* Use the default value if we don't have already selected_items */
    if (NULL == item->selected_items) {
        p = CL_OBJECT_AS_CSTRING(item->default_value);
        item->selected_items = cl_string_split(p, ",");
        cl_string_unref(p);
    }

    for (index = 0; index < session->number_of_items; index++) {
        listitem = &session->litems[index];
        p = cl_stringlist_get(item->list_items, index);

        listitem->text = strdup(cl_string_valueof(p));
        listitem->name = strdup("");
        listitem->help = strdup("");

        if (cl_stringlist_contains(item->selected_items, p))
            listitem->state = 1;
        else
            listitem->state = 0;

        cl_string_unref(p);
    }

    return 0;
}

static void build_session(session_t *session)
{
    struct xante_item *item = session->item;

    session->width = (item->geometry.width == 0) ? DIALOG_WIDTH
                                                    : item->geometry.width;

    session->height = (item->geometry.height == 0) ? DIALOG_HEIGHT
                                                      : item->geometry.height;

    session->number_of_items = cl_stringlist_size(item->list_items);
    session->displayed_items = dlgx_get_dlg_items(session->number_of_items);

    /* Creates the UI content */
    prepare_content(item, session);
}

static cl_string_t *get_current_result(const session_t *session)
{
    int i;
    cl_string_t *ret = NULL;
    DIALOG_LISTITEM *item;

    ret = cl_string_create_empty(0);

    for (i = 0; i < session->number_of_items; i++) {
        item = &session->litems[i];

        if (item->state == 1)
            cl_string_cat(ret, "%s,", item->text);
    }

    if (cl_string_isempty(ret) == false)
        cl_string_truncate(ret, -1);

    return ret;
}

/*
 *
 * Internal API
 *
 */

bool buildlist_value_changed(session_t *session)
{
    struct xante_item *item = session->item;
    cl_string_t *old_value = NULL;
    bool changed = false;

    old_value = cl_stringlist_flat(item->selected_items, ',');

    if (cl_string_cmp(old_value, session->result)) {
        changed = true;

         /* Set up details to save inside the internal changes list */
        session->change_item_name = cl_string_ref(item->name);
        session->change_old_value = cl_string_ref(old_value);
        session->change_new_value = cl_string_ref(session->result);

       /* Updates the item value */
        if (old_value != NULL)
            cl_stringlist_destroy(item->selected_items);

        item->selected_items = cl_string_split(session->result, ",");
    }

    if (old_value != NULL)
        cl_string_unref(old_value);

    return changed;
}

int buildlist(session_t *session)
{
    struct xante_item *item = session->item;
    int ret_dialog = DLG_EXIT_OK, selected_item = 0;;
    cl_string_t *cprompt = NULL;

    /* Prepares object content */
    build_session(session);
    cprompt = cl_string_create(cl_tr("Select an option to move it from origin to "
                                     "destination\n%-30s%-30s"),
                               cl_tr("Origin"),
                               cl_tr("Destination"));

    ret_dialog = dlg_buildlist(cl_string_valueof(item->name),
                               cl_string_valueof(cprompt),
                               session->height, session->width,
                               session->displayed_items,
                               session->number_of_items,
                               session->litems, NULL, 0, &selected_item);

    if (ret_dialog == DLG_EXIT_OK)
        session->result = get_current_result(session);

    cl_string_unref(cprompt);

    return ret_dialog;
}

