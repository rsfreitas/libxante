
/*
 * Description: Handles the "window" manager.
 *
 * Author: Rodrigo Freitas
 * Created at: Fri May  5 21:08:15 2017
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

#define DEFAULT_HEIGHT                  \
    (DIALOG_HEIGHT_WITHOUT_TEXT + MAX_DLG_ITEMS)

/* A structure to be used while mapping lists */
struct list_data {
    int total;
    int name_size;
    int item_size;
};

static int manager_run_widget(struct xante_app *xpp, cl_list_t *menus,
                              struct xante_item *selected_item);

static int manager_run(struct xante_app *xpp, cl_list_t *menus,
                       const struct xante_menu *entry_menu,
                       const char *cancel_label);

/*
 *
 * Internal functions
 *
 */

static void ui_init(struct xante_app *xpp)
{
    runtime_set_ui_active(xpp, true);
    dlgx_init(false);
    dlgx_set_backtitle(xpp);
}

static void ui_uninit(struct xante_app *xpp)
{
    dlgx_uninit(xpp);
    runtime_set_ui_active(xpp, false);
}

static int ui_single_run(struct xante_app *xpp, struct xante_single_instance_jtf *si)
{
    char *btn_cancel_label = NULL;
    struct xante_menu *first = NULL;
    int ret_dialog = DLG_EXIT_CANCEL;

    /* Are we dealing with a complete menu? */
    if (si->menus != NULL) {
        btn_cancel_label = strdup(cl_tr("Back"));
        first = xante_menu_head(si->menus);
        ret_dialog = manager_run(xpp, si->menus, first, btn_cancel_label);
        xante_menu_unref(first);
        free(btn_cancel_label);
    } else /* Or a single item? */
        ret_dialog = manager_run_widget(xpp, NULL, si->object);

    return ret_dialog;
}

static int check_item_availability(cl_list_node_t *node, void *a)
{
    struct xante_item *item = cl_list_node_content(node);
    struct list_data *ld = (struct list_data *)a;

    if (is_item_available(item))
        ld->total++;

    return 0;
}

/*
 * We need to "manually" calculate the number of items of the menu because some
 * of them must have been hidden with the JTF mode attribute.
 */
static int calc_menu_items(const struct xante_menu *menu)
{
    struct list_data ld = {
        .total = 0,
    };

    cl_list_map(menu->items, check_item_availability, &ld);

    return ld.total;
}

static bool item_may_have_value(const struct xante_item *item)
{
    switch (item->widget_type) {
        /*
         * These are all objects that may have values and their values are
         * viewed beside its name inside a menu. So the user may know its
         * current value.
         */
        case XANTE_WIDGET_INPUT_INT:
        case XANTE_WIDGET_INPUT_FLOAT:
        case XANTE_WIDGET_INPUT_STRING:
        case XANTE_WIDGET_INPUT_DATE:
        case XANTE_WIDGET_INPUT_TIME:
        case XANTE_WIDGET_CALENDAR:
        case XANTE_WIDGET_TIMEBOX:
        case XANTE_WIDGET_RADIO_CHECKLIST:
        case XANTE_WIDGET_YES_NO:
            return true;

        default:
            break;
    }

    return false;
}

static int check_item_width(cl_list_node_t *node, void *a)
{
    struct xante_item *item = cl_list_node_content(node);
    struct list_data *ld = (struct list_data *)a;
    int length = 0;
    char *s = NULL;

    if (is_item_available(item) == false)
        return 0;

    length = cl_string_length(item->name);

    if (length > ld->name_size)
        ld->name_size = length;

    /* We only check items that may have values */
    if (item_may_have_value(item) == true) {
        s = dlgx_get_item_value_as_text(item);

        if (NULL == s)
            return 0;

        length = strlen(s);
        free(s);

        if (length > ld->item_size)
            ld->item_size = length;
    }

    return 0;
}

static int calc_menu_width(const struct xante_menu *menu)
{
    int w = 0, menu_width;
    struct list_data ld = {
        .name_size = 0,
        .item_size = 0,
    };

    cl_list_map(menu->items, check_item_width, &ld);
    w = (ld.name_size + ld.item_size) + WINDOW_BORDER_SIZE;
    menu_width = (menu->geometry.width == 0) ? DEFAULT_DIALOG_WIDTH
                                             : menu->geometry.width;

    if (w < menu_width)
        w = menu_width;

    return (w == 0) ? menu_width: w;
}

static int add_item_content(unsigned int index, cl_list_node_t *node, void *a)
{
    struct xante_item *item = cl_list_node_content(node);
    DIALOG_LISTITEM *litems = (DIALOG_LISTITEM *)a;
    DIALOG_LISTITEM *listitem = NULL;
    char *value = NULL;

    /* Ignore this item (and the index does not get incremented) */
    if (is_item_available(item) == false)
        return 1;

    /* Fills litems[index] with item content */
    listitem = &litems[index];
    value = dlgx_get_item_value_as_text(item);

    if (NULL == value)
        listitem->text = strdup("");
    else
        listitem->text = value;

    listitem->name = strdup(cl_string_valueof(item->name));
    listitem->help = strdup("");
    listitem->state = 0;

    return 0;
}

static int prepare_content(const struct xante_menu *menu,
    session_t *session)
{
    session->litems = calloc(session->number_of_items,
                                sizeof(DIALOG_LISTITEM));

    if (NULL == session->litems) {
        errno_set(XANTE_ERROR_NO_MEMORY);
        return -1;
    }

    cl_list_map_indexed(menu->items, add_item_content, session->litems);

    return 0;
}

static void build_session(const struct xante_menu *menu,
    session_t *session)
{
    session->number_of_items = calc_menu_items(menu);

    /*
     * We don't have a default width because we need to display the full item
     * name to the user.
     */
    session->width = (menu->geometry.width == 0) ? calc_menu_width(menu)
                                                    : menu->geometry.width;

    session->displayed_items = dlgx_get_dlg_items(session->number_of_items);
    session->height = (menu->geometry.height == 0)
                ? (session->displayed_items + DIALOG_HEIGHT_WITHOUT_TEXT)
                : menu->geometry.height;

    /* Creates the UI content */
    prepare_content(menu, session);
}

static void prepare_object_look(struct xante_app *xpp,
    const char *cancel_label)
{
    int timeout = -1;
    char *text = NULL;

    dlgx_set_backtitle(xpp);
    dlgx_update_ok_button_label(NULL);
    dialog_vars.cancel_label = strdup(cancel_label);
    timeout = xante_runtime_inactivity_timeout(xpp);

    if (timeout > 0)
        dialog_vars.timeout_secs = timeout;

    text = widget_statusbar_text(XANTE_WIDGET_MENU_REFERENCE, true,
                                 xante_runtime_esc_key(xpp));

    dlgx_put_statusbar(text);
    free(text);
}

static void release_object_labels(void)
{
    if (dialog_vars.cancel_label != NULL) {
        free(dialog_vars.cancel_label);
        dialog_vars.cancel_label = NULL;
    }
}

/* Rename menus to something better */
static widget_result_t call_menu_object(struct xante_app *xpp,
    cl_list_t *menus, struct xante_item *selected_item)
{
    struct xante_menu *referenced_menu = NULL;
    char *btn_cancel_label = NULL;
    widget_result_t ret;

    ret.selected_button = DLG_EXIT_OK;
    ret.updated_value = false;
    referenced_menu =
        xante_menu_search_by_object_id(menus,
                                       cl_string_valueof(selected_item->referenced_menu));

    if (NULL == referenced_menu) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                             cl_tr("No menu '%s' was found!"),
                             cl_string_valueof(selected_item->name));

        return ret;
    }

    btn_cancel_label = strdup(cl_tr("Back"));
    manager_run(xpp, menus, referenced_menu, btn_cancel_label);
    free(btn_cancel_label);

    return ret;
}

static int item_at(unsigned int index, cl_list_node_t *node, void *a)
{
    struct xante_item *item = cl_list_node_content(node);
    int searched_index = *(int *)a;

    /* Ignore this item (and the index does not get incremented) */
    if (is_item_available(item) == false)
        return 1;

    /* Item found */
    if ((int)index == searched_index)
        return -1;

    return 0;
}

static struct xante_item *get_item_at(cl_list_t *list, int index)
{
    cl_list_node_t *node = NULL;
    struct xante_item *item = NULL;

    node = cl_list_map_indexed(list, item_at, &index);

    if (NULL == node)
        return NULL;

    item = cl_list_node_content(node);
    cl_list_node_unref(node);

    return item;
}

#ifdef ALTERNATIVE_DIALOG
static void update_menu_item_brief(int current_item, void *a)
{
    struct xante_menu *menu = (struct xante_menu *)a;
    struct xante_item *item;
    char *text = NULL;

    item = get_item_at(menu->items, current_item);

    if ((NULL == item) || (NULL == item->brief_help))
        text = " ";
    else
        text = (char *)cl_string_valueof(item->brief_help);

    dlgx_put_item_brief(text);
}
#endif

static bool should_call_manager(struct xante_item *item)
{
    if ((item->widget_type == XANTE_WIDGET_MENU_REFERENCE) ||
        (item->widget_type == XANTE_WIDGET_DYNAMIC_MENU_REFERENCE))
    {
        return true;
    }

    return false;
}

/*
 * This function is where a internal object is called according a selected item
 * type.
 *
 * The selected button validation is made here, so the object functions don't
 * need to do it.
 *
 * There are a couple of functions that an object must provide to be correctly
 * executed and validated here:
 *
 * * run:
 *
 *     The function which calls the object. It must have the following
 *     prototype:
 *
 *     int foo_object(session_t *session);
 *
 *     Where the returned value must indicate which button was selected to
 *     close it.
 *
 *     And, in case o returning DLG_EXIT_OK (which is when the user selects
 *     the Ok button), the function must fill the object result inside the
 *     session->result variable.
 *
 * * validate_result:
 *
 *     A function to validate the session->result value and, case it is
 *     valid, allow to update the item's internal value.
 *
 *     The function must have the following prototype:
 *
 *     bool foo_validate(session_t *session);
 *
 *     Where the returned value must tell if the new value, that is contained
 *     in session->result, can replace the current item's value or not.
 *
 * * value_changed:
 *
 *     In case of session->result can successfully replace the item's value,
 *     this function must also update three @session properties, so this change
 *     may be inserted into the internal changes lists. The session properties
 *     are:
 *
 *     - session->change_item_name: the name which will identify the item
 *                                  inside the changes list.
 *
 *     - session->change_old_value: the item's old value.
 *
 *     - session->change_new_value: the item's new value.
 *
 *     This function must have the following prototype:
 *
 *     int value_changed(session_t *session);
 *
 * * update_value:
 *
 *      The function to replace current item's value with session->result
 *      content. Its prototype is the following:
 *
 *      void value_changed(session_t *session);
 */
static widget_result_t run_selected_object(session_t *session)
{
    struct xante_app *xpp = session->xpp;
    struct xante_item *item = session->item;
    widget_result_t ret_dialog;
    bool loop = true, value_changed = false;

    /* Default return value */
    ret_dialog.selected_button = DLG_EXIT_OK;
    ret_dialog.updated_value = false;

    /*
     * The only kind of object which we don't need to handle its return
     * value is XANTE_WIDGET_CUSTOM, since the function can do preety
     * much anything in it and it is its responsability to handle it.
     */
    if (item->widget_type == XANTE_WIDGET_CUSTOM) {
        event_call(EV_CUSTOM, xpp, item, NULL);
        return ret_dialog;
    }

    if (NULL == session->run) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                             cl_tr("The selected item doesn't have a function "
                                   "to run its object!"));

        return ret_dialog;
    }

    do {
        ret_dialog.selected_button = (session->run)(session);

        switch (ret_dialog.selected_button) {
            case DLG_EXIT_OK:
                /*
                 * We do nothing if we can't edit the item's value.
                 */
                if (session->editable_value == false) {
                    loop = false;
                    break;
                }

                if (session->validate_result != NULL)
                    if ((session->validate_result)(session) == false)
                        break;

                /*
                 * Call event to allow the module confirm if the session->result
                 * is valid or not.
                 */
                if (event_call(EV_ITEM_VALUE_CONFIRM, xpp, item,
                               cl_string_valueof(session->result)) < 0)
                {
                    break;
                }

                if (session->value_changed != NULL)
                    value_changed = (session->value_changed)(session);

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
                /* Don't let the user close the object */
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

    ret_dialog.updated_value = value_changed;

    xante_log_info("%s: %d, %d", __FUNCTION__,
            ret_dialog.selected_button,
            ret_dialog.updated_value);

    return ret_dialog;
}

/**
 * @name manager_run_widget
 * @brief Create and run an object according to the item passed as argument.
 *
 * @param [in,out] xpp: The library main object.
 * @param [in,out] menus: The list of menus accessible through internal objects.
 * @param [in,out] selected_item: The selected item object.
 *
 * @return Returns the button used by the user to end the object.
 */
static int manager_run_widget(struct xante_app *xpp, cl_list_t *menus,
    struct xante_item *selected_item)
{
    session_t session;
    widget_result_t ret_dialog;

    if (NULL == selected_item) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                             cl_tr("No item was selected!"));

        return DLG_EXIT_CANCEL;
    }

    /* Are we leaving? */
    if (xante_runtime_close_ui(xpp) == true)
        return DLG_EXIT_OK;

    if (event_call(EV_ITEM_SELECTED, xpp, selected_item) < 0)
        return DLG_EXIT_CANCEL;

    /* Prepare object common session */
    session_init(xpp, selected_item, &session);
    dlgx_session_init(xpp, selected_item, session.editable_value);

    if (should_call_manager(selected_item))
        ret_dialog = call_menu_object(xpp, menus, selected_item);
    else
        ret_dialog = run_selected_object(&session);

    xante_log_info("%s: %d, %d", __FUNCTION__,
            ret_dialog.selected_button,
            ret_dialog.updated_value);

    /*
     * We'll need to search if the item that has been updated is pointed by a
     * dynamic menu. If so, we're going to update its contents.
     *
     * Also here we insert the occourred modification into the internal changes
     * list.
     */
    if (ret_dialog.updated_value == true) {
        if (session.update_value != NULL)
            (session.update_value)(&session);

        /*
         * If the selected item is one of these types we already saved their
         * changes and don't need to do it again or they type don't hold an
         * internal change.
         */
        if ((selected_item->widget_type != XANTE_WIDGET_MIXEDFORM) ||
            (selected_item->widget_type != XANTE_WIDGET_SPREADSHEET) ||
            !is_gadget(selected_item->widget_type))
        {
            change_add(xpp, cl_string_valueof(session.change_item_name),
                       cl_string_valueof(session.change_old_value),
                       cl_string_valueof(session.change_new_value));
        }

        dm_update(xpp, selected_item);
        event_call(EV_ITEM_VALUE_UPDATED, xpp, selected_item);
    }

    /* Run return event */
    event_call(EV_ITEM_EXIT, xpp, selected_item);

    session_uninit(&session);
    dlgx_session_uninit(selected_item);

    return ret_dialog.selected_button;
}

/**
 * @name manager_run
 * @brief Creates an object of menu type.
 *
 * @param [in,out] xpp: The main library object.
 * @param [in] menus: The list of menus accessible through internal objects.
 * @param [in] entry_menu: The menu used as the entry point of all the others.
 * @param [in] cancel_label: A string label of the cancel button.
 *
 * @return Returns the libdialog type of return value.
 */
static int manager_run(struct xante_app *xpp, cl_list_t *menus,
    const struct xante_menu *entry_menu, const char *cancel_label)
{
    bool loop = true;
    session_t session;
    int ret_dialog = DLG_EXIT_OK, selected_index = -1;

    release_object_labels();

    do {
        if (xante_runtime_close_ui(xpp) == true)
            break;

        session_init(xpp, NULL, &session);
        build_session(entry_menu, &session);
        prepare_object_look(xpp, cancel_label);

#ifdef ALTERNATIVE_DIALOG
        ret_dialog = dlg_menu(cl_string_valueof(entry_menu->name), "",
                              session.height, session.width,
                              session.displayed_items,
                              session.number_of_items,
                              session.litems, &selected_index, NULL,
                              update_menu_item_brief, (void *)entry_menu);
#else
        ret_dialog = dlg_menu(cl_string_valueof(entry_menu->name), "",
                              session.height, session.width,
                              session.displayed_items,
                              session.number_of_items,
                              session.litems, &selected_index, NULL);
#endif

        switch (ret_dialog) {
            case DLG_EXIT_OK:
                manager_run_widget(xpp, menus, get_item_at(entry_menu->items,
                                                           selected_index));

                break;

#ifdef ALTERNATIVE_DIALOG
            case DLG_EXIT_TIMEOUT:
                xante_log_info(cl_tr("Internal timeout reached... Leaving..."));
                loop = false;
                break;
#endif

            case DLG_EXIT_ESC:
                /* Don't let the user close the object */
                if (xante_runtime_esc_key(xpp))
                    break;

            case DLG_EXIT_CANCEL:
                /* Are we leaving the application? */
                if (strcmp(cancel_label, MAIN_MENU_CANCEL_LABEL) == 0) {
                    if (gadget_question(xpp, cl_tr("Closing"),
                                        cl_tr("Do you really want to quit?"),
                                        cl_tr("Yes"), cl_tr("No"), NULL) == true)
                    {
                        loop = false;
                    }
                } else {
                    if (event_call(EV_MENU_EXIT, xpp, entry_menu) == 0)
                        loop = false;
                }

                break;

            case DLG_EXIT_HELP:
                /*
                 * TODO: Think better of this, we don't have this info for
                 *       menus yet.
                 */
                break;
        }

        release_object_labels();
        session_uninit(&session);
    } while (loop);

    return ret_dialog;
}

/*
 *
 * Internal API
 *
 */

/**
 * @name ui_data_init
 * @brief Initializes everything related to UI information.
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
 * @brief Releases all information related to the UI.
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

__PUB_API__ enum xante_return_value xante_manager_run(xante_t *xpp)
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

__PUB_API__ enum xante_return_value xante_manager_single_run(xante_t *xpp,
    const char *raw_si)
{
    enum xante_return_value exit_status = XANTE_RETURN_OK;
    int ret_dialog = DLG_EXIT_CANCEL;
    bool close_libdialog = false;
    struct xante_single_instance_jtf *si = NULL;

    errno_clear();

    if ((NULL == xpp) || (NULL == raw_si)) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return XANTE_RETURN_ERROR;
    }

    si = si_jtf_load(raw_si);

    if (NULL == si)
        return XANTE_RETURN_ERROR;

    if (xante_runtime_ui_active(xpp) == false) {
        ui_init(xpp);
        close_libdialog = true;
    }

    ret_dialog = ui_single_run(xpp, si);

    if (si != NULL)
        si_jtf_unload(si);

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
    // TODO
}

__PUB_API__ void xante_ui_continue(void)
{
    // TODO
}

