
/*
 * Description: Functions to handle User Interface information.
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

/*
 *
 * Internal functions
 *
 */

static int __search_menu_by_object_id(cl_list_node_t *node, void *a)
{
    struct xante_menu *menu = cl_list_node_content(node);
    char *object_id = (char *)a;

    if (strcmp(cl_string_valueof(menu->object_id), object_id) == 0)
        return 1;

    return 0;
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

/*
 *
 * Internal API
 *
 */

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

    xpp->ui.menus = cl_list_create(xante_menu_destroy, NULL, NULL, NULL);
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

/**
 * @name xante_ui_run
 * @brief Puts a libxante application to run.
 *
 * It is important to remember that once we call this function we only leave it
 * when closing the application from the UI.
 *
 * @param [in,out] xpp: The library main object.
 *
 * @return Return an exit value indicating what happened inside (see enum
 *         xante_return_value declaration).
 */
__PUB_API__ enum xante_return_value xante_ui_run(xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;
    struct xante_menu *root = NULL;
    char *btn_cancel_label = NULL;
    enum xante_return_value exit_status;
    int ret_dialog;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return XANTE_RETURN_ERROR;
    }

    runtime_set_ui_active(xpp, true);
    dlgx_init(false);
    xante_dlg_set_backtitle(xpp);
    btn_cancel_label = strdup(cl_tr(MAIN_MENU_CANCEL_LABEL));
    root = ui_search_menu_by_object_id(xpp,
                                       cl_string_valueof(x->ui.main_menu_object_id));


    if (NULL == root) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                             cl_tr("The menu '%s' was not found!"),
                             cl_string_valueof(x->ui.main_menu_object_id));

        goto end_block;
    }

    ret_dialog = ui_dialog_menu(xpp, root, btn_cancel_label);

end_block:
    if (btn_cancel_label != NULL)
        free(btn_cancel_label);

    xante_dlg_clear_backtitle(xpp);
    dlgx_uninit();

#ifdef ALTERNATIVE_DIALOG
    exit_status = (ret_dialog == DLG_EXIT_TIMEOUT) ? XANTE_RETURN_TIMEOUT
                                                   : ((ret_dialog == DLG_EXIT_OK)
                                                        ? XANTE_RETURN_OK
                                                        : XANTE_RETURN_ERROR);
#else
    exit_status = (ret_dialog == DLG_EXIT_OK) ? XANTE_RETURN_OK
                                              : XANTE_RETURN_ERROR;
#endif

    runtime_set_ui_active(xpp, false);
    runtime_set_exit_value(xpp, exit_status);

    return exit_status;
}

__PUB_API__ void xante_ui_suspend(void)
{
}

__PUB_API__ void xante_ui_continue(void)
{
}

