
/*
 * Description: Handles the dialog (window) manager.
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

/*
 *
 * Internal functions
 *
 */

/* A structure to be used while mapping lists */
struct list_data {
    int total;
    int name_size;
    int item_size;
};

static const char *ui_statusbar_text(enum xante_ui_dialog type, bool editable);

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
    switch (item->dialog_type) {
        /*
         * These are all dialogs that may have values and their values are
         * viewed beside its name inside a menu. So the user may know its
         * current value.
         */
        case XANTE_UI_DIALOG_INPUT_INT:
        case XANTE_UI_DIALOG_INPUT_FLOAT:
        case XANTE_UI_DIALOG_INPUT_STRING:
        case XANTE_UI_DIALOG_INPUT_DATE:
        case XANTE_UI_DIALOG_INPUT_TIME:
        case XANTE_UI_DIALOG_CALENDAR:
        case XANTE_UI_DIALOG_TIMEBOX:
        case XANTE_UI_DIALOG_RADIO_CHECKLIST:
        case XANTE_UI_DIALOG_YES_NO:
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
    ui_properties_t *properties)
{
    properties->litems = calloc(properties->number_of_items,
                                sizeof(DIALOG_LISTITEM));

    if (NULL == properties->litems) {
        errno_set(XANTE_ERROR_NO_MEMORY);
        return -1;
    }

    cl_list_map_indexed(menu->items, add_item_content, properties->litems);

    return 0;
}

static void build_properties(const struct xante_menu *menu,
    ui_properties_t *properties)
{
    properties->number_of_items = calc_menu_items(menu);

    /*
     * We don't have a default width because we need to display the full item
     * name to the user.
     */
    properties->width = (menu->geometry.width == 0) ? calc_menu_width(menu)
                                                    : menu->geometry.width;

    properties->height = (menu->geometry.height == 0) ? DEFAULT_HEIGHT
                                                      : menu->geometry.height;

    properties->displayed_items = properties->height - DIALOG_HEIGHT_WITHOUT_TEXT;

    /* Creates the UI content */
    prepare_content(menu, properties);
}

static void prepare_dialog_look(struct xante_app *xpp,
    const char *cancel_label)
{
    int timeout = -1;

    dlgx_set_backtitle(xpp);
    dlgx_update_ok_button_label(NULL);
    dialog_vars.cancel_label = strdup(cancel_label);
    timeout = xante_runtime_ui_dialog_timeout(xpp);

    if (timeout > 0)
        dialog_vars.timeout_secs = timeout;

    dlgx_put_statusbar(ui_statusbar_text(XANTE_UI_DIALOG_MENU, true));
}

static void release_dialog_labels(void)
{
    if (dialog_vars.cancel_label != NULL) {
        free(dialog_vars.cancel_label);
        dialog_vars.cancel_label = NULL;
    }
}

/* Rename menus to something better */
static ui_return_t call_menu_dialog(struct xante_app *xpp,
    cl_list_t *menus, struct xante_item *selected_item)
{
    struct xante_menu *referenced_menu = NULL;
    char *btn_cancel_label = NULL;
    ui_return_t ret;

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

static const char *ui_statusbar_text(enum xante_ui_dialog type, bool editable)
{
    char *text = NULL;

    switch (type) {
        case XANTE_UI_DIALOG_MENU:
        case XANTE_UI_DIALOG_DYNAMIC_MENU:
            text = cl_tr("[ESC] Exit [Enter] Select an option [Up/Down] Move "
                         "[TAB/Left/Right] Choose button");

            break;

        case XANTE_UI_DIALOG_INPUT_INT:
        case XANTE_UI_DIALOG_INPUT_FLOAT:
        case XANTE_UI_DIALOG_INPUT_DATE:
        case XANTE_UI_DIALOG_INPUT_STRING:
        case XANTE_UI_DIALOG_INPUT_TIME:
        case XANTE_UI_DIALOG_INPUT_PASSWD:
        case XANTE_UI_DIALOG_INPUTSCROLL:
            text = cl_tr("[ESC] Cancel [Enter] Confirm a selected option "
                         "[Tab/Left/Right] Select an option");

            break;

        case XANTE_UI_DIALOG_RANGE:
            text = cl_tr("[ESC] Cancel [Enter] Confirm selected option "
                         "[Tab/Left/Right] Select an option "
                         "[Up/Down] Adjust range value");

            break;

        case XANTE_UI_DIALOG_CALENDAR:
            if (editable) {
                text = cl_tr("[ESC] Cancel [TAB] Select an option "
                             "[Enter] Confirm [Arrows] Change selected date");
            } else
                text = cl_tr("[ESC] Cancel [TAB] Select an option "
                             "[Enter] Confirm");

            break;

        case XANTE_UI_DIALOG_TIMEBOX:
            if (editable) {
                text = cl_tr("[ESC] Cancel [TAB] Select an option "
                             "[Enter] Confirm [Arrows] Change selected time");
            } else
                text = cl_tr("[ESC] Cancel [TAB/Arrows] Select an option "
                             "[Enter] Confirm");

            break;

        case XANTE_UI_DIALOG_RADIO_CHECKLIST:
        case XANTE_UI_DIALOG_CHECKLIST:
            if (editable) {
                text = cl_tr("[ESC] Cancel [Enter] Confirm a selection "
                             "[Up/Down] Move [TAB/Left/Right] Choose option "
                             "[Spacebar] Select option");
            } else
                text = cl_tr("[ESC] Cancel [Enter] Confirm a selection "
                             "[Up/Down] Move [TAB/Left/Right] Choose option");

            break;

        case XANTE_UI_DIALOG_YES_NO:
            text = cl_tr("[ESC] Cancel [Enter] Confirm a selected option "
                         "[Tab/Left/Right] Select an option");

            break;

        case XANTE_UI_DIALOG_DELETE_DYNAMIC_MENU_ITEM:
            if (editable) {
                text = cl_tr("[ESC] Cancel [Enter] Confirm a selection "
                             "[Up/Down] Move [TAB/Left/Right] Choose option "
                             "[Spacebar] Select option");
            } else
                text = cl_tr("[ESC] Cancel [Enter] Confirm a selection "
                             "[Up/Down] Move [TAB/Left/Right] Choose option");

            break;

        case XANTE_UI_DIALOG_ADD_DYNAMIC_MENU_ITEM:
            text = cl_tr("[ESC] Cancel [Enter] Confirm a selected option "
                         "[Tab/Left/Right] Select an option");

            break;

        case XANTE_UI_DIALOG_PROGRESS:
            text = cl_tr("Wait the process to end.");
            break;

        case XANTE_UI_DIALOG_SPINNER_SYNC:
        case XANTE_UI_DIALOG_DOTS_SYNC:
            text = cl_tr("A task is running in background. Wait for it to end.");
            break;

        case XANTE_UI_DIALOG_FILE_SELECT:
        case XANTE_UI_DIALOG_DIR_SELECT:
            text = cl_tr("[ESC] Cancel [Enter] Confirm selected button "
                         "[Tab/Arrows] Navigate through options "
                         "[Spacebar] Select dir/file");

            break;

        case XANTE_UI_DIALOG_FILE_VIEW:
            text = cl_tr("[ESC] Cancel [Enter] Confirm selected button "
                         "[Up/Down/Pg Up/Pg Down] Navigate through the text");

            break;

        case XANTE_UI_DIALOG_TAILBOX:
            text = cl_tr("[ESC] Cancel [Enter] Confirm selected button");
            break;

        case XANTE_UI_DIALOG_SCROLLTEXT:
            text = cl_tr("[ESC] Cancel [Enter] Confirm selected button "
                         "[Up/Down/Pg Up/Pg Down] Navigate through the text");

            break;

        case XANTE_UI_DIALOG_UPDATE_OBJECT:
            text = cl_tr("[ESC] Cancel [Enter] Confirm a selected option");
            break;

        case XANTE_UI_DIALOG_MIXEDFORM:
            text = cl_tr("[ESC] Cancel [Enter] Confirm a selected option "
                         "[Tab/Left/Right] Select an option "
                         "[Up/Down] Select field");

            break;

        case XANTE_UI_DIALOG_BUILDLIST:
            text = cl_tr("[ESC] Cancel [Enter] Confirm button "
                         "[Spacebar] Move option between lists "
                         "[^] Select origin [$] Select destination");

            break;

        case XANTE_UI_DIALOG_SPREADSHEET:
            text = cl_tr("[ESC] Cancel [Enter] Confirm selected button "
                         "[Tab] Navigate through cells");

            break;

        default:
            break;
    }

    return text;
}

static void start_dialog_internals(struct xante_app *xpp,
    struct xante_item *item, bool edit_value)
{
    const char *text = NULL;

    dlgx_set_backtitle(xpp);
    text = ui_statusbar_text(item->dialog_type, edit_value);

    if (text != NULL)
        dlgx_put_statusbar(text);

    if ((item->dialog_type == XANTE_UI_DIALOG_CALENDAR) ||
        (item->dialog_type == XANTE_UI_DIALOG_CALENDAR))
    {
        dlgx_alloc_input(64);
    }

    if (item->dialog_type == XANTE_UI_DIALOG_MIXEDFORM) {
        /* Since we may have a password item, enable '*' on screen */
        dialog_vars.insecure = 1;
    }

    /* Enables the help button */
    if (item->descriptive_help != NULL) {
        dialog_vars.help_button = 1;
        dlgx_update_help_button_label(item->label.help);
    }

    /* Enables the extra button */
    if (item->button.extra) {
        dlgx_update_extra_button_label(item->label.extra);
        dialog_vars.extra_button = 1;
    }

    dlgx_update_ok_button_label(item->label.ok);
    dlgx_update_cancel_button_label(item->label.cancel);
}

static void finish_dialog_internals(struct xante_item *item)
{
    /* Removes the Help button off the screen */
    if (item->descriptive_help != NULL)
        dialog_vars.help_button = 0;

    /* Removes the Extra button off the screen */
    if (item->button.extra)
        dialog_vars.extra_button = 0;

    if ((item->dialog_type == XANTE_UI_DIALOG_CALENDAR) ||
        (item->dialog_type == XANTE_UI_DIALOG_CALENDAR))
    {
        dlgx_free_input();
    }

    dialog_vars.insecure = 0;
}

/*
 *
 * Internal API
 *
 */

/**
 * @name manager_run_dialog
 * @brief Create and run a dialog according to the item passed as argument.
 *
 * @param [in,out] xpp: The library main object.
 * @param [in,out] menus: The list of menus accessible through internal objects.
 * @param [in,out] selected_item: The selected item object.
 *
 * @return Returns the button used by the user to end the dialog.
 */
int manager_run_dialog(struct xante_app *xpp, cl_list_t *menus,
    struct xante_item *selected_item)
{
    bool edit_item_value = true;
    ui_properties_t properties;
    ui_return_t ret_dialog;
    int ret = -1;

    if (NULL == selected_item) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                             cl_tr("No item was selected!"));

        return ret;
    }

    /* Are we leaving? */
    if (xante_runtime_close_ui(xpp) == true)
        return DLG_EXIT_OK;

    if (event_call(EV_ITEM_SELECTED, xpp, selected_item) < 0)
        return ret;

    /* Verify input edit */
    if (!(selected_item->mode & XANTE_ACCESS_EDIT))
        edit_item_value = false;

    /* Prepare dialog common properties */
    INIT_PROPERTIES(properties);
    start_dialog_internals(xpp, selected_item, edit_item_value);

    switch (selected_item->dialog_type) {
        case XANTE_UI_DIALOG_MENU:
        case XANTE_UI_DIALOG_DYNAMIC_MENU:
            ret_dialog = call_menu_dialog(xpp, menus, selected_item);
            break;

        case XANTE_UI_DIALOG_INPUT_INT:
        case XANTE_UI_DIALOG_INPUT_FLOAT:
        case XANTE_UI_DIALOG_INPUT_DATE:
        case XANTE_UI_DIALOG_INPUT_STRING:
        case XANTE_UI_DIALOG_INPUT_TIME:
        case XANTE_UI_DIALOG_INPUT_PASSWD:
        case XANTE_UI_DIALOG_RANGE:
        case XANTE_UI_DIALOG_INPUTSCROLL:
            ret_dialog = ui_input(xpp, selected_item, edit_item_value);
            break;

        case XANTE_UI_DIALOG_CALENDAR:
            ret_dialog = ui_calendar(xpp, selected_item, edit_item_value);
            break;

        case XANTE_UI_DIALOG_TIMEBOX:
            ret_dialog = ui_timebox(xpp, selected_item, edit_item_value);
            break;

        case XANTE_UI_DIALOG_RADIO_CHECKLIST:
        case XANTE_UI_DIALOG_CHECKLIST:
            ret_dialog = ui_checklist(xpp, selected_item, edit_item_value);
            break;

        case XANTE_UI_DIALOG_YES_NO:
            if (edit_item_value == true)
                ret_dialog = ui_yesno(xpp, selected_item);
            else {
                xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                                     cl_tr("Cannot change the value of this item!"));
            }

            break;

        case XANTE_UI_DIALOG_DELETE_DYNAMIC_MENU_ITEM:
            ui_delete_dm(xpp, selected_item, edit_item_value);
            break;

        case XANTE_UI_DIALOG_ADD_DYNAMIC_MENU_ITEM:
            if (edit_item_value == true)
                ui_add_dm(xpp, selected_item);
            else {
                xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                                     cl_tr("Cannot add item!"));
            }

            break;

        case XANTE_UI_DIALOG_CUSTOM:
            if (event_call(EV_CUSTOM, xpp, selected_item, NULL) < 0) {
                xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                                     cl_tr("Error calling custom object '%s'"),
                                     cl_string_valueof(selected_item->name));
            }

            break;

        case XANTE_UI_DIALOG_PROGRESS:
            ret_dialog = ui_progress(xpp, selected_item);
            break;

        case XANTE_UI_DIALOG_SPINNER_SYNC:
        case XANTE_UI_DIALOG_DOTS_SYNC:
            ret_dialog = ui_sync(xpp, selected_item);
            break;

        case XANTE_UI_DIALOG_FILE_SELECT:
        case XANTE_UI_DIALOG_DIR_SELECT:
            ret_dialog = ui_fselect(xpp, selected_item);
            break;

        case XANTE_UI_DIALOG_FILE_VIEW:
            ret_dialog = ui_file_view(xpp, selected_item);
            break;

        case XANTE_UI_DIALOG_TAILBOX:
            ret_dialog = ui_tailbox(xpp, selected_item);
            break;

        case XANTE_UI_DIALOG_SCROLLTEXT:
            ret_dialog = ui_scrolltext(xpp, selected_item);
            break;

        case XANTE_UI_DIALOG_UPDATE_OBJECT:
            ret_dialog = ui_update_object(xpp, selected_item);
            break;

        case XANTE_UI_DIALOG_MIXEDFORM:
            ret_dialog = ui_mixedform(xpp, selected_item);
            break;

        case XANTE_UI_DIALOG_BUILDLIST:
            ret_dialog = ui_buildlist(xpp, selected_item);
            break;

        case XANTE_UI_DIALOG_SPREADSHEET:
            ret_dialog = ui_spreadsheet(xpp, selected_item);
            break;

        default:
            break;
    }

    UNINIT_PROPERTIES(properties);
    finish_dialog_internals(selected_item);

    /*
     * We'll need to search if the item that has been updated is pointed by a
     * dynamic menu. If so, we're going to update its contents.
     */
    if (ret_dialog.updated_value == true) {
        dm_update(xpp, selected_item);
        event_call(EV_ITEM_VALUE_UPDATED, xpp, selected_item);
    }

    /* Run return event */
    event_call(EV_ITEM_EXIT, xpp, selected_item);

    return ret_dialog.selected_button;
}

/**
 * @name manager_run
 * @brief Creates a dialog of menu type.
 *
 * @param [in,out] xpp: The main library object.
 * @param [in] menus: The list of menus accessible through internal objects.
 * @param [in] entry_menu: The menu used as the entry point of all the others.
 * @param [in] cancel_label: A string label of the cancel button.
 *
 * @return Returns the libdialog type of return value.
 */
int manager_run(struct xante_app *xpp, cl_list_t *menus,
    const struct xante_menu *entry_menu, const char *cancel_label)
{
    bool loop = true;
    ui_properties_t properties;
    int ret_dialog = DLG_EXIT_OK, selected_index = -1;

    release_dialog_labels();

    do {
        if (xante_runtime_close_ui(xpp) == true)
            break;

        INIT_PROPERTIES(properties);
        build_properties(entry_menu, &properties);
        prepare_dialog_look(xpp, cancel_label);

#ifdef ALTERNATIVE_DIALOG
        ret_dialog = dlg_menu(cl_string_valueof(entry_menu->name), "",
                              properties.height, properties.width,
                              properties.displayed_items,
                              properties.number_of_items,
                              properties.litems, &selected_index, NULL,
                              update_menu_item_brief, (void *)entry_menu);
#else
        ret_dialog = dlg_menu(cl_string_valueof(entry_menu->name), "",
                              properties.height, properties.width,
                              properties.displayed_items,
                              properties.number_of_items,
                              properties.litems, &selected_index, NULL);
#endif

        switch (ret_dialog) {
            case DLG_EXIT_OK:
                manager_run_dialog(xpp, menus, get_item_at(entry_menu->items,
                                                           selected_index));

                break;

#ifdef ALTERNATIVE_DIALOG
            case DLG_EXIT_TIMEOUT:
                xante_log_info(cl_tr("Internal timeout reached... Leaving..."));
                loop = false;
                break;
#endif

            case DLG_EXIT_ESC:
                /* Don't let the user close the dialog */
                if (xante_runtime_esc_key(xpp))
                    break;

            case DLG_EXIT_CANCEL:
                /* Are we leaving the application? */
                if (strcmp(cancel_label, MAIN_MENU_CANCEL_LABEL) == 0) {
                    if (dlgx_question(xpp, cl_tr("Closing"),
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

        release_dialog_labels();
        UNINIT_PROPERTIES(properties);
    } while (loop);

    return ret_dialog;
}

