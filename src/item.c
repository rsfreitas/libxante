
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
 *
 * Internal functions
 *
 */

static void __destroy_xante_item(const struct cl_ref_s *ref)
{
    struct xante_item *item = cl_container_of(ref, struct xante_item, ref);

    if (NULL == item)
        return;

    if (item->name != NULL)
        cl_string_unref(item->name);

    if (item->type != NULL)
        cl_string_unref(item->type);

    if (item->object_id != NULL)
        cl_string_unref(item->object_id);

    if (item->menu_id != NULL)
        cl_string_unref(item->menu_id);

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

    if (item->value != NULL)
        cl_object_unref(item->value);

    if (item->default_value != NULL)
        cl_object_unref(item->default_value);

    if (item->min != NULL)
        cl_object_unref(item->min);

    if (item->max != NULL)
        cl_object_unref(item->max);

    if (item->checklist_options != NULL)
        cl_stringlist_destroy(item->checklist_options);

    if (item->checklist_brief_options != NULL)
        cl_stringlist_destroy(item->checklist_brief_options);

    if (item->value_spec != NULL)
        cl_spec_destroy(item->value_spec);

    if (item->events != NULL)
        cl_json_delete(item->events);

    free(item);
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
    item->mode = XANTE_ACCESS_EDIT;

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

__PUB_API__ enum xante_access_mode xante_item_access_mode(const xante_item_t *item)
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

__PUB_API__ enum xante_ui_dialog xante_item_dialog_type(const xante_item_t *item)
{
    struct xante_item *i = (struct xante_item *)item;

    errno_clear();

    if (NULL == item) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    return i->dialog_type;
}

__PUB_API__ int xante_item_checklist_type(const xante_item_t *item)
{
    struct xante_item *i = (struct xante_item *)item;

    errno_clear();

    if (NULL == item) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    return i->dialog_checklist_type;
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

