
/*
 * Description: Functions to handle the dynamic menus from an application.
 *
 *              A dynamic menu is a special menu in which it may be internally
 *              replicated _N_ times. This _N_ times must be of three diffrente
 *              ways:
 *
 *                  1 - From an array of strings (object copies inside the JTF
 *                      file).
 *
 *                  2 - A fixed number of copies (also object copies from the
 *                      JTF file).
 *
 *                  3 - An (integer) item inside another menu.
 *
 * Author: Rodrigo Freitas
 * Created at: Thu May 18 00:13:14 2017
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

struct list_data {
    struct xante_app    *xpp;
    cl_cfg_file_t       *cfg_file;
    struct xante_menu   *menu;
    int                 number_of_copies;
};

/*
 *
 *
 * Internal functions
 *
 */

static int dm_replicate(struct xante_app *xpp, struct xante_menu *menu,
                        int copies);

static int find_item(cl_list_node_t *a, void *b)
{
    struct xante_item *item = cl_list_node_content(a);
    struct list_data *ld = (struct list_data *)b;
    struct xante_menu *menu = ld->menu;

    if ((cl_string_cmp(menu->dynamic_origin_block,
                       item->config_block) == 0) &&
        (cl_string_cmp(menu->dynamic_origin_item,
                       item->config_item) == 0))
    {
        ld->number_of_copies = CL_OBJECT_AS_INT(item->default_value);
    }

    return 0;
}

static int find_menu(cl_list_node_t *a, void *b)
{
    struct xante_menu *menu = cl_list_node_content(a);

    cl_list_map(menu->items, find_item, b);

    return 0;
}

static int find_number_of_copies(struct xante_app *xpp,
    struct xante_menu *menu)
{
    struct list_data ld = {
        .menu = menu,
        .number_of_copies = -1,
    };

    cl_list_map(xpp->ui.menus, find_menu, &ld);

    return ld.number_of_copies;
}

static int dm_find_number_of_copies(struct xante_app *xpp,
    cl_cfg_file_t *cfg_file, struct xante_menu *menu)
{
    cl_cfg_key_t *key = NULL;
    cl_object_t *value = NULL;
    int copies = -1;

    /* Fixed number of copies */
    if (menu->copies != -1)
        return menu->copies;

    /* Also fixed but from an array of names */
    if (menu->dynamic_names != NULL)
        return cl_string_list_size(menu->dynamic_names);

    /* Search into the configuration file */
    key = cl_cfg_get_key(cfg_file,
                         cl_string_valueof(menu->dynamic_origin_block),
                         cl_string_valueof(menu->dynamic_origin_item));

    if (NULL == key) {
        /*
         * No configuration was found. Works with the item's default
         * value. So we need to search this one.
         */
        copies = find_number_of_copies(xpp, menu);
    } else {
        value = cl_cfg_key_value(key);
        copies = CL_OBJECT_AS_INT(value);
        cl_object_unref(value);
    }

    return copies;
}

static cl_string_t *create_menu_name(struct xante_menu *menu, int copy_index)
{
    if (menu->dynamic_names != NULL)
        return cl_string_list_get(menu->dynamic_names, copy_index);

    return cl_string_create("%s (%d)", cl_string_valueof(menu->name),
                            copy_index + 1);
}

static cl_string_t *create_object_id(struct xante_menu *menu,
    int copy_index)
{
    return cl_string_create("%s_%d", cl_string_valueof(menu->object_id),
                            copy_index);
}

static cl_string_t *create_item_config_block(struct xante_menu *menu,
    int menu_index, struct xante_item *item)
{
    if (menu->dynamic_names != NULL)
        return cl_string_list_get(menu->dynamic_names, menu_index);

    if (menu->dynamic_block_prefix != NULL)
        return cl_string_create("%s_%d",
                                cl_string_valueof(menu->dynamic_block_prefix),
                                menu_index);

    return cl_string_create("%s_%d",
                            cl_string_valueof(item->config_block), menu_index);
}

static struct xante_item *dup_item(struct xante_menu *menu, int item_index,
    int menu_index)
{
    struct xante_item *item, *d_item = NULL;
    cl_list_node_t *node;

    node = cl_list_at(menu->items, item_index);

    if (NULL == node)
        return NULL;

    item = cl_list_node_content(node);
    d_item = ui_new_xante_item();

    /* Duplicate all item informations */
    d_item->name = cl_string_dup(item->name);
    d_item->type = cl_string_dup(item->type);
    d_item->help = cl_string_dup(item->help);
    d_item->options = cl_string_dup(item->options);
    d_item->object_id = cl_string_dup(item->object_id);
    cl_string_cat(d_item->object_id, "_%d", menu_index);

    d_item->mode = item->mode;
    d_item->dialog_type = item->dialog_type;
    d_item->dialog_checklist_type = item->dialog_checklist_type;
    d_item->string_length = item->string_length;

    d_item->checklist_options = cl_string_list_dup(item->checklist_options);
    d_item->default_value = cl_object_dup(item->default_value);
    d_item->min = cl_object_dup(item->min);
    d_item->max = cl_object_dup(item->max);

    if (is_menu_item(d_item->type) == false) {
        d_item->config_item = cl_string_dup(item->config_item);
        d_item->config_block = create_item_config_block(menu, menu_index, item);
    }

    if (is_input_item(d_item->type) == true)
        d_item->value_spec = cl_spec_create(CL_READABLE | CL_WRITABLE,
                                            d_item->min, d_item->max,
                                            d_item->string_length);

    cl_list_unref(node);

    return d_item;
}

static struct xante_menu *dup_menu(struct xante_menu *menu, int copy_index)
{
    int total_items = 0, i;
    struct xante_item *d_item = NULL;
    struct xante_menu *d_menu = NULL;

    d_menu = ui_new_xante_menu(XANTE_MENU_CREATED_INTERNALLY);
    total_items = cl_list_size(menu->items);

    /* Duplicate all menu information */
    d_menu->name = create_menu_name(menu, copy_index);
    d_menu->object_id = create_object_id(menu, copy_index);
    d_menu->menu_type = menu->menu_type;
    d_menu->mode = menu->mode;

    for (i = 0; i < total_items; i++) {
        d_item = dup_item(menu, i, copy_index);
        cl_list_unshift(d_menu->items, d_item, -1);
    }

    return d_menu;
}

static int replicate(struct xante_app *xpp, struct xante_menu *menu,
    int copies)
{
    int i;
    struct xante_menu *d_menu = NULL;

    /* Replicate the menu _N_ times */
    for (i = 0; i < copies; i++) {
        d_menu = dup_menu(menu, i);

        /*
         * XXX: Insert the new menu at the of the list, so it does not
         *      mess with our mapping.
         */
        cl_list_unshift(xpp->ui.menus, d_menu, -1);
    }

    return 0;
}

static int find_submenu_to_replicate(cl_list_node_t *a, void *b)
{
    struct xante_item *item = cl_list_node_content(a);
    struct list_data *ld = (struct list_data *)b;
    struct xante_menu *menu = NULL;

    /* We don't handle dynamic menus inside dynamic menus */
    if (item->dialog_type == XANTE_UI_DIALOG_DYNAMIC_MENU)
        return -1;

    /* Is this a submenu? */
    if (item->dialog_type == XANTE_UI_DIALOG_MENU) {
        menu = ui_search_menu_by_object_id(ld->xpp,
                                           cl_string_valueof(item->object_id));

        if (dm_replicate(ld->xpp, menu, ld->number_of_copies) < 0)
            return -1;
    }

    return 0;
}

static int dm_replicate(struct xante_app *xpp, struct xante_menu *menu,
    int copies)
{
    cl_list_node_t *node;
    struct list_data ld = {
        .xpp = xpp,
        .number_of_copies = copies,
        .menu = menu,
    };

    node = cl_list_map(menu->items, find_submenu_to_replicate, &ld);

    if (NULL == node) {
        if (replicate(xpp, menu, copies) < 0)
            return -1;

        return 0; /* ok */
    }

    cl_list_node_unref(node);

    return -1;
}

static void rme_create(struct xante_app *xpp, struct xante_menu *menu,
    int copies)
{
    struct xante_menu *rme = NULL;
    struct xante_item *rme_item = NULL;
    int i;

    rme = ui_new_xante_menu(XANTE_MENU_CREATED_INTERNALLY);

    /* Create some required menu's information */
    rme->name = cl_string_dup(menu->name);
    rme->object_id = cl_string_dup(menu->object_id);
    rme->menu_type = XANTE_UI_MENU_DEFAULT;
    rme->mode = menu->mode;

    /* Create items to point at every previously created (sub)menu */
    for (i = 0; i < copies; i++) {
        rme_item = ui_new_xante_item();

        /* Create the required item's informations */
        if (menu->dynamic_names != NULL)
            rme_item->name = cl_string_list_get(menu->dynamic_names, i);
        else {
            rme_item->name = cl_string_create("%s (%d)",
                                              cl_string_valueof(menu->name),
                                              i + 1);
        }

        rme_item->object_id = cl_string_create("%s_%d",
                                               cl_string_valueof(menu->object_id),
                                               i);

        rme_item->type = cl_string_create("menu");
        rme_item->mode = XANTE_ACCESS_VIEW;
        rme_item->dialog_type = XANTE_UI_DIALOG_MENU;

        cl_list_unshift(rme->items, rme_item, -1);
    }

    cl_list_unshift(xpp->ui.menus, rme, -1);

    /* Mark the original menu to be released from our main list */
    menu->move_to_be_released = true;
}

static int dm_push_menu(cl_list_node_t *a, void *b)
{
    struct xante_menu *menu = cl_list_node_content(a);
    struct list_data *ld = (struct list_data *)b;
    struct xante_app *xpp = ld->xpp;
    cl_cfg_file_t *cfg_file = ld->cfg_file;
    int copies = -1;

    /* Don't need to handle internally create menus */
    if (menu->creator == XANTE_MENU_CREATED_INTERNALLY)
        return 0;

    if (menu->menu_type == XANTE_UI_MENU_DEFAULT)
        return 0;

    /* Discover how many copies we'll have from this menu */
    copies = dm_find_number_of_copies(xpp, cfg_file, menu);

    /* Replicate menus (submenus and items) */
    if (dm_replicate(xpp, menu, copies) < 0)
        return -1;

    /* Create the reference menu entry (RME) */
    rme_create(xpp, menu, copies);

    return 0;
}

static int detect_unreferenced_menu(cl_list_node_t *node,
    void *a __attribute__((unused)))
{
    struct xante_menu *menu = cl_list_node_content(node);

    if (menu->move_to_be_released == true)
        return 1;

    return 0;
}

static void remove_unreferenced_menus(struct xante_app *xpp)
{
    cl_list_set_filter(xpp->ui.menus, detect_unreferenced_menu);
    cl_list_delete(xpp->ui.menus, NULL);
}

/*
 *
 * Internal API
 *
 */

void dm_init(struct xante_app *xpp, cl_cfg_file_t *cfg_file)
{
    struct list_data ld = {
        .xpp = xpp,
        .cfg_file = cfg_file,
    };

    cl_list_map(xpp->ui.menus, dm_push_menu, &ld);
    remove_unreferenced_menus(xpp);
    ui_print_menu_tree(xpp);
}

