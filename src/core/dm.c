
/*
 * Description: Functions to handle the dynamic menus from an application.
 *
 *              A dynamic menu is a special menu in which it may be internally
 *              replicated _N_ times. This _N_ times must be of three different
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
    int                 first_copy_index;
    char                *input_name;
};

/*
 *
 *
 * Internal functions
 *
 */

static int dm_replicate(struct xante_app *xpp, struct xante_menu *menu,
                        int copies, int first_copy_index,
                        const char *input_name);

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
    cl_cfg_entry_t *key = NULL;
    cl_object_t *value = NULL;
    int copies = -1;

    /* Fixed number of copies */
    if (menu->copies != -1)
        return menu->copies;

    /* Also fixed but from an array of names */
    if (menu->dynamic_names != NULL)
        return cl_stringlist_size(menu->dynamic_names);

    /* Search into the configuration file */
    key = cl_cfg_entry(cfg_file,
                       cl_string_valueof(menu->dynamic_origin_block),
                       cl_string_valueof(menu->dynamic_origin_item));

    if (NULL == key) {
        /*
         * No configuration was found. Works with the item's default
         * value. So we need to search this one.
         */
        copies = find_number_of_copies(xpp, menu);
    } else {
        value = cl_cfg_entry_value(key);
        copies = CL_OBJECT_AS_INT(value);
        cl_object_unref(value);
    }

    return copies;
}

/*
 * Here we have three ways of creating a new menu name:
 *
 * 1 - From an input string (add-dynamic-menu object).
 * 2 - An string from an array of strings.
 * 3 - A copy of the parent menu name with an index (name_index).
 */
static cl_string_t *create_menu_name(struct xante_menu *menu, int copy_index,
    const char *input_name)
{
    if (input_name != NULL)
        return cl_string_create("%s", input_name);

    if (menu->dynamic_names != NULL)
        return cl_stringlist_get(menu->dynamic_names, copy_index);

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
        return cl_stringlist_get(menu->dynamic_names, menu_index);

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
    d_item = xante_item_create();

    /* Duplicate all item information */
    d_item->name = cl_string_dup(item->name);
    d_item->type = cl_string_dup(item->type);
    d_item->descriptive_help = cl_string_dup(item->descriptive_help);
    d_item->brief_help = cl_string_dup(item->brief_help);
    d_item->options = cl_string_dup(item->options);
    d_item->object_id = cl_string_dup(item->object_id);
    cl_string_cat(d_item->object_id, "_%d", menu_index);
    d_item->referenced_menu = cl_string_dup(item->referenced_menu);
    cl_string_cat(d_item->referenced_menu, "_%d", menu_index);

    d_item->mode = item->mode;
    d_item->widget_type = item->widget_type;
    d_item->widget_checklist_type = item->widget_checklist_type;
    d_item->string_length = item->string_length;

    d_item->list_items = cl_stringlist_dup(item->list_items);
    d_item->default_value = cl_object_dup(item->default_value);
    d_item->min = cl_object_dup(item->min);
    d_item->max = cl_object_dup(item->max);

    if (is_menu_item(d_item->type) == false) {
        d_item->config_item = cl_string_dup(item->config_item);
        d_item->config_block = create_item_config_block(menu, menu_index, item);
    }

    if (item_has_ranges(d_item->widget_type) == true)
        d_item->value_spec = cl_spec_create(CL_READABLE | CL_WRITABLE,
                                            d_item->min, d_item->max,
                                            d_item->string_length);

    cl_list_unref(node);

    return d_item;
}

static struct xante_menu *dup_menu(struct xante_menu *menu, int copy_index,
    const char *input_name)
{
    int total_items = 0, i;
    struct xante_item *d_item = NULL;
    struct xante_menu *d_menu = NULL;

    d_menu = xante_menu_create(XANTE_MENU_CREATED_INTERNALLY);
    total_items = cl_list_size(menu->items);

    /* Duplicate all menu information */
    d_menu->name = create_menu_name(menu, copy_index, input_name);
    d_menu->object_id = create_object_id(menu, copy_index);
    d_menu->menu_type = menu->menu_type;

    for (i = 0; i < total_items; i++) {
        d_item = dup_item(menu, i, copy_index);
        cl_list_unshift(d_menu->items, d_item, -1);
    }

    return d_menu;
}

static int replicate(struct xante_app *xpp, struct xante_menu *menu,
    int copies, int first_copy_index, const char *input_name)
{
    int i, j;
    struct xante_menu *d_menu = NULL;

    /* Replicate the menu _N_ times */
    for (i = 0, j = first_copy_index; i < copies; i++, j++) {
        d_menu = dup_menu(menu, j, input_name);

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
    if (item->widget_type == XANTE_WIDGET_DYNAMIC_MENU_REFERENCE)
        return -1;

    /* Is this a submenu? */
    if (item->widget_type == XANTE_WIDGET_MENU_REFERENCE) {
        menu = xante_menu_search_by_object_id(ld->xpp->ui.menus,
                                              cl_string_valueof(item->referenced_menu));

        if (dm_replicate(ld->xpp, menu, ld->number_of_copies,
                         ld->first_copy_index, ld->input_name) < 0)
        {
            return -1;
        }
    }

    return 0;
}

static int dm_replicate(struct xante_app *xpp, struct xante_menu *menu,
    int copies, int first_copy_index, const char *input_name)
{
    cl_list_node_t *node;
    struct list_data ld = {
        .xpp = xpp,
        .first_copy_index = first_copy_index,
        .number_of_copies = copies,
        .menu = menu,
    };

    if (input_name != NULL)
        ld.input_name = (char *)input_name;

    node = cl_list_map(menu->items, find_submenu_to_replicate, &ld);

    if (NULL == node) {
        if (replicate(xpp, menu, copies, first_copy_index, input_name) < 0)
            return -1;

        return 0; /* ok */
    }

    cl_list_node_unref(node);

    return -1;
}

static struct xante_item *create_rme_item(struct xante_menu *menu,
    int item_index, const char *input_name)
{
    struct xante_item *item = NULL;

    item = xante_item_create();

    /* Create the required item's information */

    if (input_name != NULL)
        item->name = cl_string_create("%s", input_name);
    else if (menu->dynamic_names != NULL)
        item->name = cl_stringlist_get(menu->dynamic_names, item_index);
    else {
        item->name = cl_string_create("%s (%d)",
                                      cl_string_valueof(menu->name),
                                      item_index + 1);
    }

    item->object_id = cl_string_create("%s_%d",
                                       cl_string_valueof(menu->object_id),
                                       item_index);

    item->referenced_menu = cl_string_dup(item->object_id);
    item->type = cl_string_create(XANTE_STR_WIDGET_MENU);
    item->mode = XanteAccessView;
    item->widget_type = XANTE_WIDGET_MENU_REFERENCE;

    return item;
}

static void rme_create(struct xante_app *xpp, struct xante_menu *menu,
    int copies)
{
    struct xante_menu *rme = NULL;
    struct xante_item *rme_item = NULL;
    int i;

    rme = xante_menu_create(XANTE_MENU_CREATED_INTERNALLY);

    /* Create some required menu's information */
    rme->name = cl_string_dup(menu->name);
    rme->object_id = cl_string_dup(menu->object_id);
    rme->menu_type = XANTE_MENU_DEFAULT;

    /* Create items to point at every previously created (sub)menu */
    for (i = 0; i < copies; i++) {
        rme_item = create_rme_item(menu, i, NULL);
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

    if (menu->menu_type == XANTE_MENU_DEFAULT)
        return 0;

    /* Discover how many copies we'll have from this menu */
    copies = dm_find_number_of_copies(xpp, cfg_file, menu);

    /* Replicate menus (submenus and items) */
    if (dm_replicate(xpp, menu, copies, 0, NULL) < 0)
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

static void unreference_menus(struct xante_app *xpp)
{
    cl_list_set_filter(xpp->ui.menus, detect_unreferenced_menu);
    xpp->ui.unreferenced_menus = cl_list_filter(xpp->ui.menus, NULL);
}

static int find_dm(cl_list_node_t *node, void *a)
{
    struct xante_menu *menu = cl_list_node_content(node);
    struct xante_item *item = (struct xante_item *)a;

    if (menu->menu_type == XANTE_MENU_DEFAULT)
        return 0;

    if (menu->dynamic_names != NULL)
        return 0;

    if (menu->copies != -1)
        return 0;

    if ((cl_string_cmp(menu->dynamic_origin_block, item->config_block) == 0) &&
        (cl_string_cmp(menu->dynamic_origin_item, item->config_item) == 0))
    {
        return 1;
    }

    return 0;
}

static struct xante_menu *get_unref_menu(struct xante_app *xpp,
    struct xante_item *item)
{
    cl_list_node_t *node;
    struct xante_menu *menu;

    node = cl_list_map(xpp->ui.unreferenced_menus, find_dm, item);

    if (NULL == node)
        return NULL;

    menu = cl_list_node_content(node);
    cl_list_node_unref(node);

    return menu;
}

static void dm_remove(struct xante_menu *rme, int entries_to_remove)
{
    int i;
    cl_list_node_t *node;

    for (i = 0; i < entries_to_remove; i++) {
        node = cl_list_shift(rme->items);
        cl_list_node_unref(node);
    }
}

static void dm_add(struct xante_app *xpp, struct xante_menu *rme,
    struct xante_menu *unreferenced_menu, int entries_to_add,
    const char *input_name)
{
    int i, current_copies;
    struct xante_item *rme_item = NULL;

    current_copies = cl_list_size(rme->items);

    /* Replicate the unreferenced_menu */
    if (dm_replicate(xpp, unreferenced_menu, entries_to_add,
                     current_copies, input_name) < 0)
    {
        return;
    }

    /* Add entries to the RME menu */
    for (i = 0; i < entries_to_add; i++) {
        rme_item = create_rme_item(unreferenced_menu, i + current_copies,
                                   input_name);

        cl_list_unshift(rme->items, rme_item, -1);
    }
}

/*
 *
 * Internal API
 *
 */

/**
 * @name dm_init
 * @brief Initializes the dynamic menus.
 *
 * Here the library will search for all dynamic menus loaded from the JTF
 * and create all internal necessary menus, submenus and items for it.
 *
 * A dynamic menu will have N copies of a specific menu (the original one),
 * and will need a Reference Menu Entry (RME) where all recently created
 * menus will be called and will replace the original one.
 *
 * The original menu will be put in a secondary list. To serve as a base
 * for new copies on the fly.
 *
 * @param [in,out] xpp: The library main object.
 * @param [in] cfg_file: The cl_cfg_file_t pointer to the application
 *                       configurations.
 */
void dm_init(struct xante_app *xpp, cl_cfg_file_t *cfg_file)
{
    struct list_data ld = {
        .xpp = xpp,
        .cfg_file = cfg_file,
    };

    cl_list_map(xpp->ui.menus, dm_push_menu, &ld);
    unreference_menus(xpp);
}

/**
 * @name dm_uninit
 * @brief Clears out all memory allocated while creating the dynamic menus.
 *
 * @param [in,out] xpp: The library main object.
 */
void dm_uninit(struct xante_app *xpp)
{
    if (xpp->ui.unreferenced_menus != NULL)
        cl_list_destroy(xpp->ui.unreferenced_menus);
}

/**
 * @name dm_update
 * @brief Updates a dynamic menu pointed by an item.
 *
 * The function will remove from or insert items inside a dynamic menu
 * pointed by \a selected_item.
 *
 * @param [in,out] xpp: The library main object.
 * @param [in] selected_item: The item pointing to a dynamic menu.
 */
void dm_update(struct xante_app *xpp, struct xante_item *selected_item)
{
    struct xante_menu *unref_menu = NULL, *rme_menu = NULL;
    int expected_copies = -1, current_copies = -1;

    if (selected_item->widget_type != XANTE_WIDGET_INPUT_INT)
        return;

    expected_copies = CL_OBJECT_AS_INT(item_value(selected_item));
    unref_menu = get_unref_menu(xpp, selected_item);

    if (NULL == unref_menu) {
        // error msg
        return;
    }

    rme_menu =
        xante_menu_search_by_object_id(xpp->ui.menus,
                                       cl_string_valueof(unref_menu->object_id));

    if (NULL == rme_menu) {
        // error msg
        return;
    }

    current_copies = cl_list_size(rme_menu->items);

    if (expected_copies == current_copies)
        return;

    if (expected_copies > current_copies) {
        dm_add(xpp, rme_menu, unref_menu, abs(expected_copies - current_copies),
               NULL);
    } else
        dm_remove(rme_menu, abs(expected_copies - current_copies));
}

/**
 * @name dm_delete
 * @brief Deletes a dynamic menu from the internal menus.
 *
 * The function will remove a dynamic menu by removing a specific \a position
 * from the RME menu.
 *
 * @param [in,out] rme_menu: The RME menu.
 * @param [in] position: The item position to be removed.
 */
void dm_delete(struct xante_menu *rme_menu, int position)
{
    cl_list_delete_indexed(rme_menu->items, position);
}

/**
 * @name dm_insert
 * @brief Inserts an item inside a dynamic menu.
 *
 * The function will insert a new item at the end of the dynamic menu.
 *
 * @param [in,out] xpp: The main library object.
 * @param [in] item: The item to be used as a source to find the dynamic menu.
 * @param [in] new_entry_name: The name of the new entry inside the menu.
 *
 * @return Returns true if the item was successfully inserted or false
 *         otherwise.
 */
bool dm_insert(struct xante_app *xpp, struct xante_item *item,
    const char *new_entry_name)
{
    struct xante_menu *unref_menu = NULL, *rme_menu = NULL;
    const char *referenced_menu = cl_string_valueof(item->referenced_menu);

    unref_menu = xante_menu_search_by_object_id(xpp->ui.unreferenced_menus,
                                                referenced_menu);

    if (NULL == unref_menu) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                             cl_tr("The menu '%s' was not found!"), referenced_menu);

        return false;
    }

    rme_menu = xante_menu_search_by_object_id(xpp->ui.menus, referenced_menu);

    if (NULL == rme_menu) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                             cl_tr("A reference for the menu '%s' was not found!"),
                             referenced_menu);

        return false;
    }

    dm_add(xpp, rme_menu, unref_menu, 1, new_entry_name);

    return true;
}

