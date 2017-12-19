
/*
 * Description: Functions to handle the User Interface mode.
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
#include <stdarg.h>

#include "libxante.h"

/*
 *
 * Internal functions
 *
 */

static void ui_init(struct xante_app *xpp)
{
    runtime_set_ui_active(xpp, true);
    dlgx_init(false);
    xante_dlg_set_backtitle(xpp);
}

static void ui_uninit(struct xante_app *xpp)
{
    xante_dlg_clear_backtitle(xpp);
    dlgx_uninit();
    runtime_set_ui_active(xpp, false);
}

static int ui_run_mjtf(struct xante_app *xpp, struct xante_mjtf *mjtf)
{
    char *btn_cancel_label = NULL;
    struct xante_menu *first = NULL;
    int ret_dialog = DLG_EXIT_CANCEL;

    /* Are we dealing with a complete menu? */
    if (mjtf->menus != NULL) {
        btn_cancel_label = strdup(cl_tr("Back"));
        first = xante_menu_head(mjtf->menus);
        ret_dialog = manager_run(xpp, mjtf->menus, first, btn_cancel_label);
        xante_menu_unref(first);
        free(btn_cancel_label);
    } else /* Or a single item? */
        ret_dialog = manager_run_dialog(xpp, NULL, mjtf->object);

    return ret_dialog;
}

static int __xante_dlg_messagebox(int width, int height,
    enum xante_msgbox_type type, const char *title, const char *text)
{
    int pause_opt = 1, ret_dialog = DLG_EXIT_OK;
    chtype dlg_color, dialog_attr_b, border_attr_b, title_attr_b,
           border2_attr_b;

    switch (type) {
        case XANTE_MSGBOX_INFO:
            dlg_color = dlg_color_pair(COLOR_BLACK, COLOR_GREEN);
            break;

        case XANTE_MSGBOX_WARNING:
            dlg_color = dlg_color_pair(COLOR_BLACK, COLOR_YELLOW);
            break;

        case XANTE_MSGBOX_ERROR:
            dlg_color = dlg_color_pair(COLOR_WHITE, COLOR_RED);
            break;
    }

    /* Saves libdialog dialog attr */
    dialog_attr_b = dialog_attr;
    border_attr_b = border_attr;
    title_attr_b = title_attr;
    border2_attr_b = border2_attr;

    /* Sets our new attr */
    dialog_attr = dlg_color;
    border_attr = dlg_color;
    title_attr = dlg_color;
    border2_attr = dlg_color;

    dlg_put_backtitle();
    ret_dialog = dialog_msgbox(title, text, height, width, pause_opt);

    /* Restores libdialog dialog attr */
    dialog_attr = dialog_attr_b;
    border_attr = border_attr_b;
    title_attr = title_attr_b;
    border2_attr = border2_attr_b;

    return ret_dialog;
}

/*
 *
 * Internal API
 *
 */

/**
 * @name ui_data_init
 * @brief Initializes everything related to UI informations.
 *
 * @param [in,out] xpp: The previously created xante_app structure.
 */
void ui_data_init(struct xante_app *xpp)
{
    if (NULL == xpp)
        return;

    xpp->ui.menus = cl_list_create(xante_menu_destroy, NULL, NULL, NULL);
}

/**
 * @name ui_data_uninit
 * @brief Releases all informations related to the UI.
 *
 * @param [in,out] xpp: The previously created xante_app structure.
 */
void ui_data_uninit(struct xante_app *xpp)
{
    if (NULL == xpp)
        return;

    if (xpp->ui.main_menu_object_id != NULL)
        cl_string_unref(xpp->ui.main_menu_object_id);

    if (xpp->ui.menus != NULL)
        cl_list_destroy(xpp->ui.menus);

    dm_uninit(xpp);
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

__PUB_API__ int xante_dlg_set_backtitle(xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;
    cl_string_t *title = NULL, *left = NULL, *format = NULL, *right = NULL;
    unsigned int screen_width = dlg_box_x_ordinate(0) * 2;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    if (xante_runtime_ui_active(xpp) == false) {
        errno_set(XANTE_ERROR_UI_NOT_INITIALIZED);
        return -1;
    }

    if (change_has_occourred(xpp)) {
        right = cl_string_create("[%s*] %s",
                                 (x->auth.name != NULL)
                                    ? cl_string_valueof(x->auth.login_and_source)
                                    : "",
                                 x->info.company);
    } else
        right = cl_string_create("[%s] %s",
                                 (x->auth.name != NULL)
                                    ? cl_string_valueof(x->auth.login_and_source)
                                    : "",
                                 x->info.company);

    left = cl_string_create(cl_tr("%s - Version %s.%d Build %d %s"),
                            x->info.application_name,
                            x->info.version,
                            x->info.revision, x->info.build,
                            (x->info.beta == true) ? "BETA" : "");

    if (screen_width  > (unsigned int)(cl_string_length(left) +
                                       cl_string_length(right)))
    {
        format = cl_string_create("%%s%%%ds",
                                  (screen_width - cl_string_length(left)));
    } else
        format = cl_string_create("%%s - %%s");

    title = cl_string_create(cl_string_valueof(format),
                             cl_string_valueof(left),
                             cl_string_valueof(right));

    if (dialog_vars.backtitle != NULL)
        free(dialog_vars.backtitle);

    dialog_vars.backtitle = strdup(cl_string_valueof(title));

    if (title != NULL)
        cl_string_unref(title);

    if (left != NULL)
        cl_string_unref(left);

    if (right != NULL)
        cl_string_unref(right);

    if (format != NULL)
        cl_string_unref(format);

    return 0;
}

__PUB_API__ int xante_dlg_clear_backtitle(xante_t *xpp)
{
    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    if (xante_runtime_ui_active(xpp) == false) {
        errno_set(XANTE_ERROR_UI_NOT_INITIALIZED);
        return -1;
    }

    free(dialog_vars.backtitle);
    dialog_vars.backtitle = NULL;

    return 0;
}

__PUB_API__ int xante_dlg_messagebox(struct xante_app *xpp,
    enum xante_msgbox_type type, const char *title, const char *message, ...)
{
    va_list ap;
    char *msg = NULL;
    enum xante_buttons key;
    int height = 0, ret_dialog;
    cl_string_t *real_msg = NULL;
    bool dialog_needs_close = false, restore_help_button = false,
         restore_extra_button = false;

    va_start(ap, message);
    vasprintf(&msg, message, ap);
    va_end(ap);

    if ((NULL == xpp) ||
        ((xpp != NULL) && xante_runtime_ui_active(xpp) == false))
    {
        dlgx_init(true);
        dialog_needs_close = true;
    }

    if (dialog_vars.help_button) {
        restore_help_button = true;
        dialog_vars.help_button = 0;
    }

    if (dialog_vars.extra_button) {
        restore_extra_button = true;
        dialog_vars.extra_button = 0;
    }

    real_msg = cl_string_create("%s", msg);
    cl_string_rplchr(real_msg, XANTE_STR_LINE_BREAK, '\n');
    height = dlgx_count_lines(msg, DEFAULT_DIALOG_WIDTH);

    if (xpp != NULL)
        xante_log_info("MSGBOX: %s", msg);

    ret_dialog = __xante_dlg_messagebox(DEFAULT_DIALOG_WIDTH, height, type,
                                        title, cl_string_valueof(real_msg));

    if (restore_help_button == true)
        dialog_vars.help_button = 1;

    if (restore_extra_button == true)
        dialog_vars.extra_button = 1;

    if (dialog_needs_close == true)
        dlgx_uninit();

    if (real_msg != NULL)
        cl_string_unref(real_msg);

    if (msg != NULL)
        free(msg);

    switch (ret_dialog) {
        case DLG_EXIT_OK:
            key = XANTE_BTN_OK;
            break;

        case DLG_EXIT_CANCEL:
#ifdef ALTERNATIVE_DIALOG
        case DLG_EXIT_TIMEOUT:
#endif
            key = XANTE_BTN_CANCEL;
            break;

        case DLG_EXIT_HELP:
            key = XANTE_BTN_HELP;
            break;

        case DLG_EXIT_EXTRA:
            key = XANTE_BTN_INFO;
            break;
    }

    return key;
}

__PUB_API__ enum xante_return_value xante_ui_run(xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;
    struct xante_menu *root = NULL;
    char *btn_cancel_label = NULL;
    enum xante_return_value exit_status;
    int ret_dialog = DLG_EXIT_CANCEL;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return XANTE_RETURN_ERROR;
    }

    ui_init(xpp);
    btn_cancel_label = strdup(cl_tr(MAIN_MENU_CANCEL_LABEL));
    root = xante_menu_search_by_object_id(x->ui.menus,
                                          cl_string_valueof(x->ui.main_menu_object_id));

    if (NULL == root) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                             cl_tr("The menu '%s' was not found!"),
                             cl_string_valueof(x->ui.main_menu_object_id));

        goto end_block;
    }

    ret_dialog = manager_run(xpp, x->ui.menus, root, btn_cancel_label);

end_block:
    if (btn_cancel_label != NULL)
        free(btn_cancel_label);

    ui_uninit(xpp);

#ifdef ALTERNATIVE_DIALOG
    exit_status = (ret_dialog == DLG_EXIT_TIMEOUT) ? XANTE_RETURN_TIMEOUT
                                                   : ((ret_dialog == DLG_EXIT_OK)
                                                        ? XANTE_RETURN_OK
                                                        : XANTE_RETURN_ERROR);
#else
    exit_status = (ret_dialog == DLG_EXIT_OK) ? XANTE_RETURN_OK
                                              : XANTE_RETURN_ERROR;
#endif

    runtime_set_exit_value(xpp, exit_status);

    return exit_status;
}

__PUB_API__ enum xante_return_value xante_ui_run_mjtf(xante_t *xpp,
    const char *raw_mjtf)
{
    enum xante_return_value exit_status = XANTE_RETURN_OK;
    int ret_dialog = DLG_EXIT_CANCEL;
    bool close_libdialog = false;
    struct xante_mjtf *mjtf = NULL;

    errno_clear();

    if ((NULL == xpp) || (NULL == raw_mjtf)) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return XANTE_RETURN_ERROR;
    }

    mjtf = mjtf_load(raw_mjtf);

    if (NULL == mjtf)
        return XANTE_RETURN_ERROR;

    if (xante_runtime_ui_active(xpp) == false) {
        ui_init(xpp);
        close_libdialog = true;
    }

    ret_dialog = ui_run_mjtf(xpp, mjtf);

    if (mjtf != NULL)
        mjtf_unload(mjtf);

#ifdef ALTERNATIVE_DIALOG
    exit_status = (ret_dialog == DLG_EXIT_TIMEOUT) ? XANTE_RETURN_TIMEOUT
                                                   : ((ret_dialog == DLG_EXIT_OK)
                                                        ? XANTE_RETURN_OK
                                                        : XANTE_RETURN_ERROR);
#else
    exit_status = (ret_dialog == DLG_EXIT_OK) ? XANTE_RETURN_OK
                                              : XANTE_RETURN_ERROR;
#endif

    if (close_libdialog == true)
        ui_uninit(xpp);

    return exit_status;
}

__PUB_API__ void xante_ui_suspend(void)
{
}

__PUB_API__ void xante_ui_continue(void)
{
}

