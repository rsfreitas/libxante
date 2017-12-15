
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

static void build_properties(struct xante_item *item,
    ui_properties_t *properties)
{
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

static bool item_value_has_changed(struct xante_app *xpp, struct xante_item *item,
    const cl_string_t *new_value)
{
    cl_string_t *old_value = NULL;
    bool changed = false;

    old_value = cl_stringlist_flat(item->selected_items, ',');

    if (cl_string_cmp(old_value, new_value)) {
        changed = true;
        change_add(xpp, cl_string_valueof(item->name),
                   cl_string_valueof(old_value),
                   cl_string_valueof(new_value));

        /* Updates the item value */
        if (old_value != NULL)
            cl_stringlist_destroy(item->selected_items);

        item->selected_items = cl_string_split(new_value, ",");
    }

    if (old_value != NULL)
        cl_string_unref(old_value);

    return changed;
}

/*
 *
 * Internal API
 *
 */

ui_return_t ui_buildlist(struct xante_app *xpp, struct xante_item *item)
{
    int ret_dialog = DLG_EXIT_OK, selected_item = 0;;
    bool loop = true, value_changed = false;
    ui_return_t ret;
    ui_properties_t properties;
    cl_string_t *cprompt = NULL, *result = NULL;

    /* Prepares dialog content */
    INIT_PROPERTIES(properties);
    build_properties(item, &properties);
    cprompt = cl_string_create(cl_tr("Select an option to move it from origin to "
                                     "destination\n%-30s%-30s"),
                               cl_tr("Origin"),
                               cl_tr("Destination"));

    do {
        if (result != NULL) {
            cl_string_unref(result);
            result = NULL;
        }

        ret_dialog = dlg_buildlist(cl_string_valueof(item->name),
                                   cl_string_valueof(cprompt),
                                   properties.height, properties.width,
                                   properties.displayed_items,
                                   properties.number_of_items,
                                   properties.litems, NULL, 0, &selected_item);

        switch (ret_dialog) {
            case DLG_EXIT_OK:
                result = get_current_result(&properties);

                if (event_call(EV_ITEM_VALUE_CONFIRM, xpp, item,
                               cl_string_valueof(result)) < 0)
                {
                    break;
                }

                value_changed = item_value_has_changed(xpp, item, result);
                loop = false;
                break;

            case DLG_EXIT_EXTRA:
                if (item->button.extra == true)
                    event_call(EV_EXTRA_BUTTON_PRESSED, xpp, item);

                break;

#ifdef ALTERNATIVE_DIALOG
            case DLG_EXIT_TIMEOUT:
                loop = false;
                break;
#endif

            case DLG_EXIT_ESC:
                /* Don't let the user close the dialog */
                if (xante_runtime_esc_key(xpp))
                    break;

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

    if (result != NULL)
        cl_string_unref(result);

    cl_string_unref(cprompt);
    UNINIT_PROPERTIES(properties);

    ret.selected_button = ret_dialog;
    ret.updated_value = value_changed;

    return ret;
}

