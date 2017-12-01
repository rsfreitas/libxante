
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

#define DEFAULT_STATUSBAR_TEXT      \
    "[ESC] Cancel [Enter] Confirm button [Spacebar] Move option between lists [^] Select origin [$] Select destination"

/*
 * XXX: This DIALOG_WIDTH is important because we use half of its value
 *      to build the cprompt of the dialog.
 */
#define DIALOG_WIDTH                60
#define DIALOG_HEIGHT               22

struct dialog_properties {
    int             width;
    int             height;
    int             list_height;
    int             number_of_options;
    cl_stringlist_t *default_value;
};

/*
 *
 * Internal functions
 *
 */

static void build_dialog_properties(const struct xante_item *item,
    struct dialog_properties *properties)
{
    properties->default_value = NULL;
    properties->width = DIALOG_WIDTH;
    properties->height = DIALOG_HEIGHT;
    properties->number_of_options = cl_stringlist_size(item->list_items);
    properties->list_height = dialog_get_dlg_items(properties->number_of_options);
}

static cl_stringlist_t *get_default_value(const struct xante_item *item)
{
    cl_stringlist_t *l = NULL;
    cl_string_t *s = NULL;

    if (NULL == item->default_value)
        return NULL;

    s = CL_OBJECT_AS_CSTRING(item->default_value);
    l = cl_string_split(s, ",");
    cl_string_unref(s);

    return l;
}

static DIALOG_LISTITEM *prepare_dialog_content(const struct xante_item *item,
    struct dialog_properties *properties)
{
    DIALOG_LISTITEM *litems = NULL, *listitem = NULL;
    cl_string_t *option = NULL;
    cl_stringlist_t *slist = NULL;
    int index;

    if (NULL == item->selected_items) {
        properties->default_value = get_default_value(item);
        slist = properties->default_value;
    } else
        slist = item->selected_items;

    litems = calloc(properties->number_of_options, sizeof(DIALOG_LISTITEM));

    if (NULL == litems) {
        errno_set(XANTE_ERROR_NO_MEMORY);
        return NULL;
    }

    for (index = 0; index < properties->number_of_options; index++) {
        listitem = &litems[index];
        option = cl_stringlist_get(item->list_items, index);

        listitem->text = strdup(cl_string_valueof(option));
        listitem->name = strdup("");
        listitem->help = strdup("");

        if (cl_stringlist_contains(slist, option))
            listitem->state = 1;
        else
            listitem->state = 0;

        cl_string_unref(option);
    }

    return litems;
}

static void release_dialog_content(DIALOG_LISTITEM *listitem,
    struct dialog_properties *properties)
{
    int i;

    for (i = 0; i < properties->number_of_options; i++) {
        free(listitem[i].text);
        free(listitem[i].name);
        free(listitem[i].help);
    }

    free(listitem);

    if (properties->default_value != NULL)
        cl_stringlist_destroy(properties->default_value);
}

static cl_string_t *get_current_result(DIALOG_LISTITEM *items,
    const struct dialog_properties *properties)
{
    int i;
    cl_string_t *ret = NULL;
    DIALOG_LISTITEM *item;

    ret = cl_string_create_empty(0);

    for (i = 0; i < properties->number_of_options; i++) {
        item = &items[i];

        if (item->state == 1)
            cl_string_cat(ret, "%s,", item->text);
    }

    if (cl_string_isempty(ret) == false)
        cl_string_truncate(ret, -1);

    return ret;
}

static bool item_value_has_changed(struct xante_app *xpp, struct xante_item *item,
    const cl_string_t *new_value, const struct dialog_properties *properties)
{
    cl_string_t *old_value = NULL;
    bool changed = false;

    old_value = cl_stringlist_flat((item->selected_items != NULL)
                                        ? item->selected_items
                                        : properties->default_value, ',');

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

ui_return_t ui_dialog_buildlist(struct xante_app *xpp, struct xante_item *item)
{
    int ret_dialog = DLG_EXIT_OK, selected_item = 0;;
    bool loop = true, value_changed = false;
    ui_return_t ret;
    struct dialog_properties properties;
    DIALOG_LISTITEM *items;
    cl_string_t *cprompt = NULL, *result = NULL;

    /* Prepare dialog */
    dlgx_set_backtitle(xpp);
    dlgx_update_cancel_button_label();
    dlgx_put_statusbar(DEFAULT_STATUSBAR_TEXT);

    /* Prepares dialog content */
    build_dialog_properties(item, &properties);
    items = prepare_dialog_content(item, &properties);

    /* Enables the help button */
    if (item->descriptive_help != NULL)
        dialog_vars.help_button = 1;

    cprompt = cl_string_create(cl_tr("Select an option from origin to destination"
                                     "\n%-30s%-30s"),
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
                                   properties.list_height,
                                   properties.number_of_options,
                                   items, NULL, 0, &selected_item);

        switch (ret_dialog) {
            case DLG_EXIT_OK:
                result = get_current_result(items, &properties);

                if (event_call(EV_ITEM_VALUE_CONFIRM, xpp, item,
                               cl_string_valueof(result)) < 0)
                {
                    break;
                }

                value_changed = item_value_has_changed(xpp, item, result,
                                                       &properties);

                loop = false;
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

    if (result != NULL)
        cl_string_unref(result);

    cl_string_unref(cprompt);
    release_dialog_content(items, &properties);
    ret.selected_button = ret_dialog;
    ret.updated_value = value_changed;

    return ret;
}

