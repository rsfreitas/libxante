
/*
 * Description: API to manipulate xante_item structures from outside the
 *              library.
 *
 * Author: Rodrigo Freitas
 * Created at: Sat May 27 20:35:20 2017
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
 * Informations to be used inside functions called while traversing a list
 * of items.
 */
struct search_data {
    char    *name;
    char    *object_id;
    char    *block;
    char    *item;
    void    *found;
    int     (*compare)(cl_list_node_t *, void *);
};

/*
 *
 * Internal functions
 *
 */

static void __destroy_xante_item(const struct cl_ref_s *ref)
{
    struct xante_item *item = cl_container_of(ref, struct xante_item, ref);

    if (NULL == item)
        return;

    /* string */
    if (item->name != NULL)
        cl_string_unref(item->name);

    if (item->type != NULL)
        cl_string_unref(item->type);

    if (item->object_id != NULL)
        cl_string_unref(item->object_id);

    if (item->referenced_menu != NULL)
        cl_string_unref(item->referenced_menu);

    if (item->config_block != NULL)
        cl_string_unref(item->config_block);

    if (item->config_item != NULL)
        cl_string_unref(item->config_item);

    if (item->brief_help != NULL)
        cl_string_unref(item->brief_help);

    if (item->descriptive_help != NULL)
        cl_string_unref(item->descriptive_help);

    if (item->options != NULL)
        cl_string_unref(item->options);

    /* object */
    if (item->value != NULL)
        cl_object_unref(item->value);

    if (item->default_value != NULL)
        cl_object_unref(item->default_value);

    if (item->min != NULL)
        cl_object_unref(item->min);

    if (item->max != NULL)
        cl_object_unref(item->max);

    /* spec */
    if (item->value_spec != NULL)
        cl_spec_destroy(item->value_spec);

    /* json */
    if (item->events != NULL)
        cl_json_delete(item->events);

    if (item->form_options != NULL)
        cl_json_delete(item->form_options);

    /* stringlist */
    if (item->list_items != NULL)
        cl_stringlist_destroy(item->list_items);

    if (item->checklist_brief_options != NULL)
        cl_stringlist_destroy(item->checklist_brief_options);

    if (item->selected_items != NULL)
        cl_stringlist_destroy(item->selected_items);

    /* labels */
    if (item->label.ok != NULL)
        cl_string_unref(item->label.ok);

    if (item->label.cancel != NULL)
        cl_string_unref(item->label.cancel);

    if (item->label.extra != NULL)
        cl_string_unref(item->label.extra);

    if (item->label.help != NULL)
        cl_string_unref(item->label.help);

    if (item->label.title != NULL)
        cl_string_unref(item->label.title);

    free(item);
}

/*
 * Helper function to compare an item by its name.
 */
static int search_by_name(cl_list_node_t *node, void *a)
{
    struct xante_item *item = cl_list_node_content(node);
    struct search_data *sd = (struct search_data *)a;

    if (strcmp(cl_string_valueof(item->name), sd->name) == 0)
        return 1;

    return 0;
}

/*
 * Helper function to compare an item by its object_id.
 */
static int search_by_object_id(cl_list_node_t *node, void *a)
{
    struct xante_item *item = cl_list_node_content(node);
    struct search_data *sd = (struct search_data *)a;

    if (strcmp(cl_string_valueof(item->object_id), sd->object_id) == 0)
        return 1;

    return 0;
}

/*
 * Helper function to compare an item by its configuration information.
 */
static int search_by_config_name(cl_list_node_t *node, void *a)
{
    struct xante_item *item = cl_list_node_content(node);
    struct search_data *sd = (struct search_data *)a;

    if ((strcmp(cl_string_valueof(item->config_block), sd->block) == 0) &&
        (strcmp(cl_string_valueof(item->config_item), sd->item) == 0))
    {
        return 1;
    }

    return 0;
}

/*
 * Searches for an item inside a menu (node). If the item is found, the function
 * returns a positive value and a reference to it will live in sd->found.
 */
static int look_for_item_in_menu(cl_list_node_t *node, void *a)
{
    struct xante_menu *menu = cl_list_node_content(node);
    struct search_data *sd = (struct search_data *)a;
    cl_list_node_t *item_node = NULL;

    item_node = cl_list_map(menu->items, sd->compare, sd);

    if (item_node != NULL) {
        sd->found = cl_list_node_content(item_node);
        cl_list_node_unref(item_node);
        return 1;
    }

    return 0;
}

static struct xante_item *look_for_item(const struct xante_app *xpp,
    struct search_data *data)
{
    cl_list_node_t *node = NULL;
    struct xante_item *item = NULL;

    node = cl_list_map(xpp->ui.menus, look_for_item_in_menu, data);

    if (NULL == node)
        return NULL;

    item = data->found;
    cl_list_node_unref(node);

    return item;
}

static struct xante_item *xante_item_search_by_name(const struct xante_app *xpp,
    va_list ap)
{
    struct search_data sd;
    char *menu_name = NULL, *item_name = NULL;
    struct xante_menu *menu = NULL;

    menu_name = va_arg(ap, char *);
    item_name = va_arg(ap, char *);

    if ((NULL == menu_name) || (NULL == item_name)) {
        errno_set(XANTE_ERROR_INVALID_ARG);
        return NULL;
    }

    menu = xante_menu_search_by_name(xpp->ui.menus, menu_name);

    if (NULL == menu) {
        errno_set(XANTE_ERROR_MENU_NOT_FOUND);
        errno_store_additional_content(menu_name);
        return NULL;
    }

    sd.name = item_name;
    sd.compare = search_by_name;

    if (look_for_item_in_menu(menu, &sd) == 0) {
        errno_set(XANTE_ERROR_ITEM_NOT_FOUND);
        errno_store_additional_content(item_name);
        return NULL;
    }

    return (struct xante_item *)sd.found;
}

static struct xante_item *xante_item_search_by_object_id(const struct xante_app *xpp,
    va_list ap)
{
    struct search_data sd;
    char *object_id = NULL;
    struct xante_item *item = NULL;

    object_id = va_arg(ap, char *);

    if (NULL == object_id) {
        errno_set(XANTE_ERROR_INVALID_ARG);
        return NULL;
    }

    sd.object_id = object_id;
    sd.compare = search_by_object_id;
    item = look_for_item(xpp, &sd);

    if (NULL == item) {
        errno_set(XANTE_ERROR_ITEM_NOT_FOUND);
        errno_store_additional_content(object_id);
        return NULL;
    }

    return item;
}

static struct xante_item *xante_item_search_by_config_name(const struct xante_app *xpp,
    va_list ap)
{
    struct search_data sd;
    char *config_block = NULL, *config_item = NULL, *tmp = NULL;
    struct xante_item *item = NULL;

    config_block = va_arg(ap, char *);
    config_item = va_arg(ap, char *);

    if ((NULL == config_block) || (NULL == config_item)) {
        errno_set(XANTE_ERROR_INVALID_ARG);
        return NULL;
    }

    sd.block = config_block;
    sd.item = config_item;
    sd.compare = search_by_config_name;
    item = look_for_item(xpp, &sd);

    if (NULL == item) {
        asprintf(&tmp, "%s - %s", config_block, config_item);
        errno_set(XANTE_ERROR_ITEM_NOT_FOUND);
        errno_store_additional_content(tmp);
        free(tmp);
        return NULL;
    }

    return item;
}

/*
 *
 * Internal API
 *
 */

/*
 * The purpose of this function is to be used inside the cl_list_ API
 */
void xante_item_destroy(void *a)
{
    struct xante_item *item = (struct xante_item *)a;

    if (NULL == item)
        return;

    xante_item_unref(item);
}

/**
 * @name xante_item_ref
 * @brief Increases the reference count for a xante_item object.
 *
 * @param [in,out] item: The xante_item item.
 */
void xante_item_ref(struct xante_item *item)
{
    if (NULL == item)
        return;

    cl_ref_inc(&item->ref);
}

/**
 * @name xante_item_unref
 * @brief Decreases the reference count for a xante_item object.
 *
 * When its reference count drops to 0, the item is finalized (its memory
 * is freed).
 *
 * @param [in,out] item: The xante_item item.
 */
void xante_item_unref(struct xante_item *item)
{
    if (NULL == item)
        return;

    cl_ref_dec(&item->ref);
}

/**
 * @name new_xante_item
 * @brief Creates a new xante_item to hold informations about a menu item.
 *
 * To release an object of this type, one may call the 'xante_item_unref'.
 *
 * @return On success returns a xante_item structure or NULL otherwise.
 */
struct xante_item *xante_item_create(void)
{
    struct xante_item *item = NULL;

    item = calloc(1, sizeof(struct xante_item));

    if (NULL == item) {
        errno_set(XANTE_ERROR_NO_MEMORY);
        return NULL;
    }

    /*
     * Our default behavior is to let every new item in edit mode. If one
     * desires to block any the JTF must be properly configured.
     */
    item->mode = XanteAccessEdit;

    /*
     * Every item has, at least, the Ok and the Cancel buttons enabled.
     */
    item->button.ok = true;
    item->button.cancel = true;

    /*
     * Ensures that every item has its helper variables correctly
     * initialized.
     */
    item->__helper.min = NULL;
    item->__helper.max = NULL;
    item->__helper.brief_options_help = NULL;
    item->__helper.options = NULL;
    item->__helper.default_value = NULL;

    /* Initialize reference count */
    item->ref.count = 1;
    item->ref.free = __destroy_xante_item;

    return item;
}

/*
 *
 * API
 *
 */

__PUB_API__ const char *xante_item_name(const xante_item_t *item)
{
    struct xante_item *i = (struct xante_item *)item;

    errno_clear();

    if (NULL == item) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return NULL;
    }

    return cl_string_valueof(i->name);
}

__PUB_API__ const char *xante_item_object_id(const xante_item_t *item)
{
    struct xante_item *i = (struct xante_item *)item;

    errno_clear();

    if (NULL == item) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return NULL;
    }

    return cl_string_valueof(i->object_id);
}

__PUB_API__ enum XanteAccessMode xante_item_access_mode(const xante_item_t *item)
{
    struct xante_item *i = (struct xante_item *)item;

    errno_clear();

    if (NULL == item) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    return i->mode;
}

__PUB_API__ cl_object_t *xante_item_default_value(const xante_item_t *item)
{
    struct xante_item *i = (struct xante_item *)item;

    errno_clear();

    if (NULL == item) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return NULL;
    }

    return cl_object_ref(i->default_value);
}

__PUB_API__ cl_object_t *xante_item_value(const xante_item_t *item)
{
    struct xante_item *i = (struct xante_item *)item;

    errno_clear();

    if (NULL == item) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return NULL;
    }

    return cl_object_ref(i->value);
}

__PUB_API__ enum xante_widget xante_item_widget_type(const xante_item_t *item)
{
    struct xante_item *i = (struct xante_item *)item;

    errno_clear();

    if (NULL == item) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    return i->widget_type;
}

__PUB_API__ int xante_item_checklist_type(const xante_item_t *item)
{
    struct xante_item *i = (struct xante_item *)item;

    errno_clear();

    if (NULL == item) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    return i->widget_checklist_type;
}

__PUB_API__ int xante_item_update_value(xante_item_t *item, const char *fmt, ...)
{
    struct xante_item *i = (struct xante_item *)item;
    cl_string_t *tmp = NULL;
    cl_object_t *data = NULL;
    char *buf = NULL;
    va_list ap;

    errno_clear();

    if ((NULL == item) || (NULL == fmt)) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    va_start(ap, fmt);
    vasprintf(&buf, fmt, ap);
    va_end(ap);

    tmp = cl_string_create("%s", buf);
    free(buf);

    if (NULL == tmp) {
        errno_set(XANTE_ERROR_INVALID_INTERNAL_CONVERSION);
        return -1;
    }

    data = cl_object_from_cstring(tmp);

    if (NULL == data) {
        errno_set(XANTE_ERROR_INVALID_INTERNAL_CONVERSION);
        return -1;
    }

    cl_string_unref(tmp);
    cl_object_unref(i->value);
    i->value = data;

    return 0;
}

__PUB_API__ int xante_item_cancel_update(xante_item_t *item)
{
    struct xante_item *i = (struct xante_item *)item;

    errno_clear();

    if (NULL == item) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    /*
     * If any update routine is running for this item it will stop right
     * after this change.
     */
    i->cancel_update = true;

    return 0;
}

__PUB_API__ xante_item_t *xante_item_search(const xante_t *xpp,
    enum xante_item_search_mode mode, ...)
{
    va_list ap;
    struct xante_item *item = NULL;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return NULL;
    }

    va_start(ap, NULL);

    switch (mode) {
        case XANTE_ITEM_SEARCH_BY_NAME:
            item = xante_item_search_by_name(xpp, ap);
            break;

        case XANTE_ITEM_SEARCH_BY_CONFIG_NAME:
            item = xante_item_search_by_config_name(xpp, ap);
            break;

        case XANTE_ITEM_SEARCH_BY_OBJECT_ID:
            item = xante_item_search_by_object_id(xpp, ap);
            break;

        default:
            errno_set(XANTE_ERROR_INVALID_ARG);
            return NULL;
    }

    va_end(ap);

    return item;
}

