
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
 *      to build the cprompt of the dialog.
 */
#define DIALOG_WIDTH                60
#define DIALOG_HEIGHT               22

/*
 *
 * Internal functions
 *
 */

static int prepare_content(struct xante_item *item,
    ui_properties_t *properties)
{
    DIALOG_LISTITEM *listitem = NULL;
    cl_string_t *p = NULL;
    int index;

    properties->litems = calloc(properties->number_of_items,
                                sizeof(DIALOG_LISTITEM));

    if (NULL == properties->litems) {
        errno_set(XANTE_ERROR_NO_MEMORY);
        return -1;
    }

    /* Use the default value if we don't have already selected_items */
    if (NULL == item->selected_items) {
        p = CL_OBJECT_AS_CSTRING(item->default_value);
        item->selected_items = cl_string_split(p, ",");
        cl_string_unref(p);
    }

    for (index = 0; index < properties->number_of_items; index++) {
        listitem = &properties->litems[index];
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

static void build_properties(ui_properties_t *properties)
{
    struct xante_item *item = properties->item;

    properties->width = (item->geometry.width == 0) ? DIALOG_WIDTH
                                                    : item->geometry.width;

    properties->height = (item->geometry.height == 0) ? DIALOG_HEIGHT
                                                      : item->geometry.height;

    properties->number_of_items = cl_stringlist_size(item->list_items);
    properties->displayed_items = dlgx_get_dlg_items(properties->number_of_items);

    /* Creates the UI content */
    prepare_content(item, properties);
}

static cl_string_t *get_current_result(const ui_properties_t *properties)
{
    int i;
    cl_string_t *ret = NULL;
    DIALOG_LISTITEM *item;

    ret = cl_string_create_empty(0);

    for (i = 0; i < properties->number_of_items; i++) {
        item = &properties->litems[i];

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

bool buildlist_value_changed(ui_properties_t *properties)
{
    struct xante_item *item = properties->item;
    cl_string_t *old_value = NULL;
    bool changed = false;

    old_value = cl_stringlist_flat(item->selected_items, ',');

    if (cl_string_cmp(old_value, properties->result)) {
        changed = true;

         /* Set up details to save inside the internal changes list */
        properties->change_item_name = cl_string_ref(item->name);
        properties->change_old_value = cl_string_ref(old_value);
        properties->change_new_value = cl_string_ref(properties->result);

       /* Updates the item value */
        if (old_value != NULL)
            cl_stringlist_destroy(item->selected_items);

        item->selected_items = cl_string_split(properties->result, ",");
    }

    if (old_value != NULL)
        cl_string_unref(old_value);

    return changed;
}

int buildlist(ui_properties_t *properties)
{
    struct xante_item *item = properties->item;
    int ret_dialog = DLG_EXIT_OK, selected_item = 0;;
    cl_string_t *cprompt = NULL;

    /* Prepares dialog content */
    build_properties(properties);
    cprompt = cl_string_create(cl_tr("Select an option to move it from origin to "
                                     "destination\n%-30s%-30s"),
                               cl_tr("Origin"),
                               cl_tr("Destination"));

    ret_dialog = dlg_buildlist(cl_string_valueof(item->name),
                               cl_string_valueof(cprompt),
                               properties->height, properties->width,
                               properties->displayed_items,
                               properties->number_of_items,
                               properties->litems, NULL, 0, &selected_item);

    if (ret_dialog == DLG_EXIT_OK)
        properties->result = get_current_result(properties);

    cl_string_unref(cprompt);

    return ret_dialog;
}

