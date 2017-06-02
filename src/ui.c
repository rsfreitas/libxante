
/*
 * Description: Functions to handle User Interface.
 *
 * Author: Rodrigo Freitas
 * Created at: Tue May  2 21:58:11 2017
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

#include <stdlib.h>
#include <string.h>

#include "libxante.h"
#include "ui/ui_dialogs.h"

/*
 *
 * Internal functions
 *
 */

/*
 * The purpose of this function is to be used inside the cl_list_ API
 */
static void __destroy_xante_item(void *a)
{
    struct xante_item *item = (struct xante_item *)a;

    if (NULL == item)
        return;

    ui_xante_item_unref(item);
}

static void destroy_xante_item(const struct cl_ref_s *ref)
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
        cl_string_list_destroy(item->checklist_options);

    if (item->checklist_brief_options != NULL)
        cl_string_list_destroy(item->checklist_brief_options);

    if (item->value_spec != NULL)
        cl_spec_destroy(item->value_spec);

    if (item->events != NULL)
        cl_json_delete(item->events);

    free(item);
}

static void destroy_xante_menu(const struct cl_ref_s *ref)
{
    struct xante_menu *menu = cl_container_of(ref, struct xante_menu, ref);

    if (NULL == menu)
        return;

    if (menu->name != NULL)
        cl_string_unref(menu->name);

    if (menu->object_id != NULL)
        cl_string_unref(menu->object_id);

    if (menu->items != NULL)
        cl_list_destroy(menu->items);

    if (menu->type != NULL)
        cl_string_unref(menu->type);

    if (menu->dynamic_names != NULL)
        cl_string_list_destroy(menu->dynamic_names);

    if (menu->dynamic_block_prefix != NULL)
        cl_string_unref(menu->dynamic_block_prefix);

    if (menu->dynamic_origin_block != NULL)
        cl_string_unref(menu->dynamic_origin_block);

    if (menu->dynamic_origin_item != NULL)
        cl_string_unref(menu->dynamic_origin_item);

    if (menu->events != NULL)
        cl_json_delete(menu->events);

    free(menu);
}

/*
 * The purpose of this function is to be used inside the cl_list_ API
 */
static void __destroy_xante_menu(void *a)
{
    struct xante_menu *menu = (struct xante_menu *)a;

    if (NULL == menu)
        return;

    ui_xante_menu_unref(menu);
}

static int __search_menu_by_object_id(cl_list_node_t *node, void *a)
{
    struct xante_menu *menu = cl_list_node_content(node);
    char *object_id = (char *)a;

    if (strcmp(cl_string_valueof(menu->object_id), object_id) == 0)
        return 1;

    return 0;
}

/*
 *
 * Internal API
 *
 */

/**
 * @name ui_xante_item_ref
 * @brief Increases the reference count for a xante_item object.
 *
 * @param [in,out] item: The xante_item item.
 */
void ui_xante_item_ref(struct xante_item *item)
{
    if (NULL == item)
        return;

    cl_ref_inc(&item->ref);
}

/**
 * @name ui_xante_item_unref
 * @brief Decreases the reference count for a xante_item object.
 *
 * When its reference count drops to 0, the item is finalized (its memory
 * is freed).
 *
 * @param [in,out] item: The xante_item item.
 */
void ui_xante_item_unref(struct xante_item *item)
{
    if (NULL == item)
        return;

    cl_ref_dec(&item->ref);
}

/**
 * @name ui_new_xante_item
 * @brief Creates a new xante_item to hold informations about a menu item.
 *
 * To release an object of this type, one may call the 'ui_xante_item_unref'.
 *
 * @return On success returns a xante_item structure or NULL otherwise.
 */
struct xante_item *ui_new_xante_item(void)
{
    struct xante_item *item = NULL;

    item = calloc(1, sizeof(struct xante_item));

    if (NULL == item) {
        errno_set(XANTE_ERROR_NO_MEMORY);
        return NULL;
    }

    /* Initialize reference count */
    item->ref.count = 1;
    item->ref.free = destroy_xante_item;

    return item;
}

/**
 * @name ui_xante_menu_ref
 * @brief Increases the reference count for a xante_menu object.
 *
 * @param [in,out] menu: The xante_menu item.
 */
void ui_xante_menu_ref(struct xante_menu *menu)
{
    if (NULL == menu)
        return;

    cl_ref_inc(&menu->ref);
}

/**
 * @name ui_xante_menu_unref
 * @brief Decreases the reference count for a xante_menu object.
 *
 * When its reference count drops to 0, the item is finalized (its memory
 * is freed).
 *
 * @param [in,out] menu: The xante_menu item.
 */
void ui_xante_menu_unref(struct xante_menu *menu)
{
    if (NULL == menu)
        return;

    cl_ref_dec(&menu->ref);
}

/**
 * @name ui_new_xante_menu
 * @brief Creates a new xante_menu to hold items and informations about an UI
 *        menu.
 *
 * This function creates a new xante_menu structure, holding some informations,
 * such as its name, its object_id and who is creating the menu.
 *
 * To release an object of this type, one may call the 'ui_xante_menu_unref'.
 *
 * @param [in] creator: An information to say who is creating the menu, it may
 *                      be created when loaded from a JTF file or created when
 *                      a dynamic menu is requested.
 *
 * @return On success returns a xante_menu structure or NULL otherwise.
 */
struct xante_menu *ui_new_xante_menu(enum xante_menu_creator creator)
{
    struct xante_menu *menu = NULL;

    menu = calloc(1, sizeof(struct xante_menu));

    if (NULL == menu) {
        errno_set(XANTE_ERROR_NO_MEMORY);
        return NULL;
    }

    menu->creator = creator;
    menu->move_to_be_released = false;
    menu->copies = -1;
    menu->items = cl_list_create(__destroy_xante_item, NULL, NULL, NULL);

    /* Initialize reference count */
    menu->ref.count = 1;
    menu->ref.free = destroy_xante_menu;

    return menu;
}

/**
 * @name ui_init
 * @brief Initializes everything related to UI informations.
 *
 * @param [in,out] xpp: The previously created xante_app structure.
 */
void ui_init(struct xante_app *xpp)
{
    if (NULL == xpp)
        return;

    xpp->ui.menus = cl_list_create(__destroy_xante_menu, NULL, NULL, NULL);
}

/**
 * @name ui_uninit
 * @brief Releases all informations related to the UI.
 *
 * @param [in,out] xpp: The previously created xante_app structure.
 */
void ui_uninit(struct xante_app *xpp)
{
    if (NULL == xpp)
        return;

    if (xpp->ui.main_menu_object_id != NULL)
        cl_string_unref(xpp->ui.main_menu_object_id);

    if (xpp->ui.menus != NULL)
        cl_list_destroy(xpp->ui.menus);

    dm_uninit(xpp);
}

/**
 * @name ui_adjusts_menu_info
 * @brief Do some adjustments inside a menu after its informations is
 *        completely loaded from the JTF file.
 *
 * @param [in,out] menu: Them menu to be adjusted.
 */
void ui_adjusts_menu_info(struct xante_menu *menu, void *copies)
{
    int i, t;
    cl_json_t *node = NULL;
    cl_string_t *value = NULL;

    menu->menu_type = translate_string_menu_type((menu->type != NULL)
                                                  ? cl_string_valueof(menu->type)
                                                  : NULL);

    if ((menu->menu_type == XANTE_UI_MENU_DYNAMIC) && (copies != NULL)) {
        /* If we don't have a block_prefix, we handle @copies as an array */
        if (NULL == menu->dynamic_block_prefix) {
            t = cl_json_get_array_size(copies);
            menu->dynamic_names = cl_string_list_create();

            for (i = 0; i < t; i++) {
                node = cl_json_get_array_item(copies, i);
                value = cl_json_get_object_value(node);
                cl_string_list_add(menu->dynamic_names, value);
            }
        } else
            menu->copies = *(int *)&copies;
    }
}

/**
 * @name ui_adjusts_item_info
 * @brief Do some adjustments inside an item after its informations is
 *        completely loaded from the JTF file.
 *
 * For example, here we must split a checklist options into a cl_string_list_t
 * object.
 *
 * @param [in,out] item: The item to be adjusted.
 * @param [in] default_value: A default value loaded from the JTF.
 * @param [in] options: The options value loaded from the JTF.
 * @param [in] max_range: The maximum range of an input item.
 * @param [in] min_range: The minimum range of an input item.
 * @param [in] brief_options_help: A JSON object with checklist options brief
 *                                 help.
 */
void ui_adjusts_item_info(struct xante_item *item, cl_string_t *default_value,
    void *options, void *max_range, void *min_range, void *brief_options_help)
{
    int i, t, i_max, i_min;
    float f_max, f_min;
    cl_json_t *node = NULL;
    cl_string_t *value = NULL;

    item->dialog_type =
        translate_string_dialog_type(cl_string_valueof(item->type));

    if (default_value != NULL)
        item->default_value = cl_object_from_cstring(default_value);

    switch (item->dialog_type) {
        case XANTE_UI_DIALOG_RADIO_CHECKLIST:
        case XANTE_UI_DIALOG_CHECKLIST:
            if (options != NULL) {
                t = cl_json_get_array_size(options);
                item->checklist_options = cl_string_list_create();

                for (i = 0; i < t; i++) {
                    node = cl_json_get_array_item(options, i);
                    value = cl_json_get_object_value(node);
                    cl_string_list_add(item->checklist_options, value);
                }
            }

            item->dialog_checklist_type =
                (item->dialog_type == XANTE_UI_DIALOG_CHECKLIST) ? FLAG_CHECK
                                                                 : FLAG_RADIO;

            if (brief_options_help != NULL) {
                t = cl_json_get_array_size(brief_options_help);
                item->checklist_brief_options = cl_string_list_create();

                for (i = 0; i < t; i++) {
                    node = cl_json_get_array_item(brief_options_help, i);
                    value = cl_json_get_object_value(node);
                    cl_string_list_add(item->checklist_brief_options, value);
                }
            }

            break;

        case XANTE_UI_DIALOG_INPUT_DATE:
            item->string_length = DATE_MAX_INPUT_LENGTH;
            break;

        case XANTE_UI_DIALOG_INPUT_TIME:
            item->string_length = TIME_MAX_INPUT_LENGTH;
            break;

        case XANTE_UI_DIALOG_INPUT_INT:
            i_min = *(int *)&min_range;
            i_max = *(int *)&max_range;
            item->min = cl_object_create(CL_INT, i_min);
            item->max = cl_object_create(CL_INT, i_max);
            break;

        case XANTE_UI_DIALOG_INPUT_FLOAT:
            f_min = *(float *)&min_range;
            f_max = *(float *)&max_range;
            item->min = cl_object_create(CL_FLOAT, f_min);
            item->max = cl_object_create(CL_FLOAT, f_max);
            break;

        default:
            break;
    }

    if ((item->dialog_type != XANTE_UI_DIALOG_CHECKLIST) &&
        (item->dialog_type != XANTE_UI_DIALOG_RADIO_CHECKLIST))
    {
        if (options != NULL)
            item->options = options;
    }

    if (is_input_item(item->type) == true)
        item->value_spec = cl_spec_create(CL_READABLE | CL_WRITABLE, item->min,
                                          item->max, item->string_length);
}

static struct xante_menu *search_menu_by_object_id(const cl_list_t *menus,
    const char *object_id_to_search)
{
    cl_list_node_t *node = NULL;
    struct xante_menu *menu = NULL;

    node = cl_list_map(menus, __search_menu_by_object_id,
                       (void *)object_id_to_search);

    if (NULL == node)
        return NULL;

    menu = cl_list_node_content(node);
    cl_list_node_unref(node);

    return menu;
}

/**
 * @name ui_search_menu_by_object_id
 * @brief Searches a xante_menu structure inside the main menu list which have
 *        a specific object_id.
 *
 * Remember, when searching a menu pointed by an item, its object_id is located
 * inside the menu_id.
 *
 * @param [in] xpp: The main library object.
 * @param [in] object_it_to_search: The menu object_id which will be used to
 *                                  search.
 *
 * @return On success, i.e, the menu is found, returns a pointer to its
 *         xante_menu structure or NULL otherwise.
 */
struct xante_menu *ui_search_menu_by_object_id(const struct xante_app *xpp,
    const char *object_id_to_search)
{
    return search_menu_by_object_id(xpp->ui.menus, object_id_to_search);
}

/**
 * @name ui_search_unref_menu_by_object_id
 * @brief Searches a xante_menu structure inside the unreferenced menus list
 *        which have a specific object_id.
 *
 * Remember, when searching a menu pointed by an item, its object_id is located
 * inside the menu_id.
 *
 * @param [in] xpp: The main library object.
 * @param [in] object_it_to_search: The menu object_id which will be used to
 *                                  search.
 *
 * @return On success, i.e, the menu is found, returns a pointer to its
 *         xante_menu structure or NULL otherwise.
 */
struct xante_menu *ui_search_unref_menu_by_object_id(const struct xante_app *xpp,
    const char *object_id_to_search)
{
    return search_menu_by_object_id(xpp->ui.unreferenced_menus,
                                    object_id_to_search);
}

// DEBUG
static int print_item(cl_list_node_t *node, void *a __attribute__((unused)))
{
    struct xante_item *item= cl_list_node_content(node);

    printf("\t'%s' -> '%s', '%s', '%s'\n", cl_string_valueof(item->name),
            cl_string_valueof(item->object_id), cl_string_valueof(item->config_block),
            cl_string_valueof(item->config_item));

    return 0;
}

// DEBUG
static int print_menu(cl_list_node_t *node, void *a __attribute__((unused)))
{
    struct xante_menu *menu = cl_list_node_content(node);

    printf("'%s' -> '%s'\n", cl_string_valueof(menu->name), cl_string_valueof(menu->object_id));
    cl_list_map(menu->items, print_item, NULL);

    return 0;
}

// DEBUG
void ui_print_menu_tree(struct xante_app *xpp)
{
    cl_list_map(xpp->ui.menus, print_menu, NULL);
}

/*
 *
 * API
 *
 */

/*
 * XXX: Remember once we call this function we only leave it when closing the
 *      application from the UI.
 */
/**
 * @name xante_ui_run
 * @brief Puts a libxante application to run.
 *
 * @param [in,out] xpp: The library main object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
__PUB_API__ int xante_ui_run(xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;
    struct xante_menu *root = NULL;
    char *btn_cancel_label = NULL;
    int ret_dialog;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    xante_runtime_set_ui_active(xpp, true);
    dialog_init(false);
    xante_dlg_set_backtitle(xpp);
    btn_cancel_label = strdup(cl_tr(MAIN_MENU_CANCEL_LABEL));
    root = ui_search_menu_by_object_id(xpp,
                                       cl_string_valueof(x->ui.main_menu_object_id));


    if (NULL == root) {
        goto end_block;
    }

    ret_dialog = ui_dialog_menu(xpp, root, btn_cancel_label);

end_block:
    if (btn_cancel_label != NULL)
        free(btn_cancel_label);

    xante_dlg_clear_backtitle(xpp);
    dialog_uninit();
    xante_runtime_set_ui_active(xpp, false);

    return ret_dialog;
}

__PUB_API__ void xante_ui_suspend(void)
{
}

__PUB_API__ void xante_ui_continue(void)
{
}

