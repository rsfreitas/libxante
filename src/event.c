
/*
 * Description: API to handle our internal "module" support.
 *
 * Author: Rodrigo Freitas
 * Created at: Tue May  2 21:40:57 2017
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

#include <stdarg.h>

#include "libxante.h"

struct xante_event_argument {
    struct xante_app        *xpp;
    struct xante_menu       *menu;
    struct xante_item       *item;
    cl_cfg_file_t           *cfg_file;
    cl_object_t             *value;
    cl_list_t               *changes;
    void                    *data;
};

static const char *__mandatory_functions[] = {
    EV_INIT,
    EV_UNINIT,
    EV_CONFIG_LOAD,
    EV_CONFIG_UNLOAD,
    EV_CHANGES_SAVED
};

#define MANDATORY_FUNCTIONS         \
    (sizeof(__mandatory_functions) / sizeof(__mandatory_functions[0]))

/*
 *
 * Internal functions
 *
 */

static bool find_function(cl_stringlist_t *functions, const char *name)
{
    int i, t, ret;
    cl_string_t *f;
    bool found = false;

    t = cl_stringlist_size(functions);

    for (i = 0; i < t; i++) {
        f = cl_stringlist_get(functions, i);
        ret = strcmp(cl_string_valueof(f), name);
        cl_string_unref(f);

        if (ret == 0) {
            found = true;
            break;
        }
    }

    return found;
}

static bool validate_mandatory_events(struct xante_app *xpp)
{
    unsigned int i;

    for (i = 0; i < MANDATORY_FUNCTIONS; i++) {
        if (find_function(xpp->module.functions,
                          __mandatory_functions[i]) == false)
        {
            return false;
        }
    }

    return true;
}

static int ev_void(struct xante_app *xpp, const char *event_name)
{
    cl_object_t *ret = NULL;
    int event_return = 0;
    struct xante_event_argument arg = {
        .xpp = xpp,
        .menu = NULL,
        .item = NULL,
        .value = NULL,
        .cfg_file = NULL,
        .changes = NULL,
        .data = NULL,
    };

    ret = cl_plugin_call(xpp->module.module, event_name, "xpp_arg", &arg,
                         NULL);

    if (strcmp(event_name, EV_INIT) == 0)
        event_return = CL_OBJECT_AS_INT(ret);

    cl_object_unref(ret);

    return event_return;
}

static void ev_config(struct xante_app *xpp, const char *event_name, va_list ap)
{
    cl_object_t *ret = NULL;
    cl_cfg_file_t *cfg_file = NULL;
    struct xante_event_argument arg = {
        .xpp = xpp,
        .menu = NULL,
        .item = NULL,
        .value = NULL,
        .changes = NULL,
        .data = NULL,
    };

    cfg_file = va_arg(ap, void *);
    arg.cfg_file = cfg_file;
    ret = cl_plugin_call(xpp->module.module, event_name, "xpp_arg", &arg, NULL);
    cl_object_unref(ret);
}

static char *get_function_name(cl_json_t *events, const char *event_name)
{
    cl_json_t *event = NULL;
    cl_string_t *value = NULL;
    char *name = NULL;

    if (NULL == events)
        return NULL;

    event = cl_json_get_object_item(events, event_name);

    if (NULL == event)
        return NULL;

    value = cl_json_get_object_value(event);
    name = strdup(cl_string_valueof(value));

    return name;
}

static int ev_item(struct xante_app *xpp, const char *event_name, va_list ap)
{
    cl_object_t *ret = NULL;
    struct xante_item *item = NULL;
    char *function_name = NULL;
    int event_return = -1;
    struct xante_event_argument arg = {
        .xpp = xpp,
        .menu = NULL,
        .item = NULL,
        .value = NULL,
        .cfg_file = NULL,
        .changes = NULL,
        .data = NULL,
    };

    item = va_arg(ap, void *);
    arg.item = item;
    function_name = get_function_name(item->events, event_name);

    if (NULL == function_name) {
        xante_log_debug(cl_tr("Event function from event [%s] not found"),
                        event_name);

        return 0; /* Should we return an error? */
    }

    /*
     * We need to pass the custom data, otherwise this routine call (maybe)
     * won't result in something useful.
     */
    if ((strcmp(event_name, EV_UPDATE_ROUTINE) == 0) ||
        (strcmp(event_name, EV_SYNC_ROUTINE) == 0) ||
        (strcmp(event_name, EV_VALUE_CHECK) == 0) ||
        (strcmp(event_name, EV_VALUE_STRLEN) == 0))
    {
        arg.data = va_arg(ap, void *);
    }

    ret = cl_plugin_foreign_call(xpp->module.module, function_name, CL_INT,
                                 CL_PLUGIN_ARGS_POINTER, "xpp_arg", CL_POINTER,
                                 &arg, NULL);

    if (NULL == ret) {
        if ((strcmp(event_name, EV_CUSTOM) == 0) ||
            (strcmp(event_name, EV_EXTRA_BUTTON_PRESSED) == 0))
        {
            xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                                 "Event call error: %s",
                                 cl_strerror(cl_get_last_error()));
        }

        goto end_block;
    }

    /* These events have a return value */
    if ((strcmp(event_name, EV_ITEM_SELECTED) == 0) ||
        (strcmp(event_name, EV_SYNC_ROUTINE) == 0) ||
        (strcmp(event_name, EV_UPDATE_ROUTINE) == 0) ||
        (strcmp(event_name, EV_VALUE_CHECK) == 0) ||
        (strcmp(event_name, EV_VALUE_STRLEN) == 0))
    {
        event_return = CL_OBJECT_AS_INT(ret);
    }

    cl_object_unref(ret);

end_block:
    free(function_name);

    return event_return;
}

static int ev_menu(struct xante_app *xpp, const char *event_name, va_list ap)
{
    cl_object_t *ret = NULL;
    struct xante_menu *menu = NULL;
    char *function_name = NULL;
    int event_return = 0;
    struct xante_event_argument arg = {
        .xpp = xpp,
        .menu = NULL,
        .item = NULL,
        .value = NULL,
        .cfg_file = NULL,
        .changes = NULL,
        .data = NULL,
    };

    menu = va_arg(ap, void *);
    arg.menu = menu;
    function_name = get_function_name(menu->events, event_name);

    if (NULL == function_name)
        return 0; /* Should we return an error? */

    ret = cl_plugin_foreign_call(xpp->module.module, event_name, CL_INT,
                                 CL_PLUGIN_ARGS_POINTER, "xpp_arg", CL_POINTER,
                                 &arg, NULL);

    if (strcmp(event_name, EV_MENU_EXIT) == 0)
        event_return = CL_OBJECT_AS_INT(ret);

    cl_object_unref(ret);

    return event_return;
}

static int ev_item_value(struct xante_app *xpp, const char *event_name, va_list ap)
{
    cl_object_t *ret = NULL;
    struct xante_item *item = NULL;
    char *function_name = NULL;
    int event_return = 0;
    struct xante_event_argument arg = {
        .xpp = xpp,
        .menu = NULL,
        .item = NULL,
        .value = NULL,
        .cfg_file = NULL,
        .changes = NULL,
        .data = NULL,
    };

    item = va_arg(ap, void *);
    arg.item = item;
    function_name = get_function_name(item->events, event_name);

    if (NULL == function_name)
        return 0; /* Should we return an error? */

    /* Parse the item value */
    switch (item->widget_type) {
        case XANTE_WIDGET_INPUT_INT:
        case XANTE_WIDGET_CHECKLIST:
        case XANTE_WIDGET_RADIO_CHECKLIST:
        case XANTE_WIDGET_YES_NO:
            arg.value = cl_object_create(CL_INT, va_arg(ap, int));
            break;

        case XANTE_WIDGET_INPUT_FLOAT:
            arg.value = cl_object_create(CL_FLOAT, (float)va_arg(ap, double));
            break;

        case XANTE_WIDGET_INPUT_DATE:
        case XANTE_WIDGET_INPUT_STRING:
        case XANTE_WIDGET_INPUT_PASSWD:
        case XANTE_WIDGET_INPUT_TIME:
        case XANTE_WIDGET_CALENDAR:
        case XANTE_WIDGET_TIMEBOX:
            arg.value = cl_object_create(CL_STRING, va_arg(ap, char *));
            break;

        default:
            break;
    }

    ret = cl_plugin_foreign_call(xpp->module.module, event_name, CL_INT,
                                 CL_PLUGIN_ARGS_POINTER, "xpp_arg", CL_POINTER,
                                 &arg, NULL);

    if (strcmp(event_name, EV_ITEM_VALUE_CONFIRM) == 0)
        event_return = CL_OBJECT_AS_INT(ret);

    free(function_name);
    cl_object_unref(ret);

    if (arg.value != NULL)
        cl_object_unref(arg.value);

    return event_return;
}

/*
 * Here we don't use the 'call' function since it is a special event. It
 * doesn't receive any argument, just returns a pointer to some data used
 * inside the oficial update-routine.
 */
static void *ev_item_custom_data(struct xante_app *xpp,
    struct xante_item *item)
{
    char *function_name = NULL;
    cl_object_t *ret = NULL;
    void *data = NULL;

    function_name = get_function_name(item->events, EV_ITEM_CUSTOM_DATA);

    if (NULL == function_name)
        return NULL; /* Should we return an error? */

    ret = cl_plugin_foreign_call(xpp->module.module, function_name, CL_POINTER,
                                 CL_PLUGIN_ARGS_VOID, NULL);

    if (NULL == ret)
        return NULL;

    data = CL_OBJECT_AS_POINTER(ret);
    cl_object_unref(ret);

    return data;
}

static int ev_changes(struct xante_app *xpp)
{
    cl_object_t *ret = NULL;
    int event_return = 0;
    struct xante_event_argument arg = {
        .xpp = xpp,
        .menu = NULL,
        .item = NULL,
        .value = NULL,
        .cfg_file = NULL,
        .changes = xpp->changes.user_changes,
        .data = NULL,
    };

    ret = cl_plugin_call(xpp->module.module, EV_CHANGES_SAVED, "xpp_arg", &arg, NULL);
    event_return = CL_OBJECT_AS_INT(ret);
    cl_object_unref(ret);

    return event_return;
}

static int call(const char *event_name, struct xante_app *xpp, va_list ap)
{
    int ret = -1;

    if ((strcmp(event_name, EV_INIT) == 0) ||
        (strcmp(event_name, EV_UNINIT) == 0))
    {
        ret = ev_void(xpp, event_name);
    } else if ((strcmp(event_name, EV_CONFIG_LOAD) == 0) ||
               (strcmp(event_name, EV_CONFIG_UNLOAD) == 0))
    {
        ev_config(xpp, event_name, ap);
    } else if ((strcmp(event_name, EV_ITEM_SELECTED) == 0) ||
               (strcmp(event_name, EV_ITEM_VALUE_UPDATED) == 0) ||
               (strcmp(event_name, EV_ITEM_EXIT) == 0) ||
               (strcmp(event_name, EV_UPDATE_ROUTINE) == 0) ||
               (strcmp(event_name, EV_SYNC_ROUTINE) == 0) ||
               (strcmp(event_name, EV_CUSTOM) == 0) ||
               (strcmp(event_name, EV_VALUE_CHECK) == 0) ||
               (strcmp(event_name, EV_VALUE_STRLEN) == 0) ||
               (strcmp(event_name, EV_EXTRA_BUTTON_PRESSED) == 0))
    {
        ret = ev_item(xpp, event_name, ap);
    } else if (strcmp(event_name, EV_MENU_EXIT) == 0)
        ret = ev_menu(xpp, event_name, ap);
    else if (strcmp(event_name, EV_ITEM_VALUE_CONFIRM) == 0)
        ret = ev_item_value(xpp, event_name, ap);
    else if (strcmp(event_name, EV_CHANGES_SAVED) == 0)
        ret = ev_changes(xpp);

    return ret;
}

/*
 *
 * Internal API
 *
 */

/**
 * @name event_call
 * @brief Calls an application event function from within the module.
 *
 * All events called with this function return an int value.
 *
 * @param [in] event_name: The name of the event which will be called.
 * @param [in] xpp: The library main object.
 * @param [in] ...: Variadic arguments according the event called.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int event_call(const char *event_name, struct xante_app *xpp, ...)
{
    va_list ap;
    int ret;

    if (xante_runtime_execute_module(xpp) == false)
        return 0;

    va_start(ap, NULL);
    ret = call(event_name, xpp, ap);
    va_end(ap);

    return ret;
}

/**
 * @name event_init
 * @brief Initialize the application module.
 *
 * The function calls the EV_INIT event function after the module is loaded.
 *
 * @param [in,out] xpp: The library main object.
 * @param [in] use_module: A boolean flag to enable/disable the application
 *                         module.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int event_init(struct xante_app *xpp, bool use_module)
{
    if (use_module == false) {
        runtime_set_execute_module(xpp, false);
        return 0;
    }

    xpp->module.module = cl_plugin_load(xpp->info.module_name);

    if (NULL == xpp->module.module) {
        // DEBUG
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, "ERROR",
                "%s", cl_strerror(cl_get_last_error()));
        errno_set(XANTE_ERROR_PLUGIN_LOAD_ERROR);
        return -1;
    }

    xpp->module.info = cl_plugin_info(xpp->module.module);

    if (NULL == xpp->module.info) {
        errno_set(XANTE_ERROR_PLUGIN_WITHOUT_INFO);
        return -1;
    }

    xpp->module.functions = cl_plugin_functions(xpp->module.info);

    if (NULL == xpp->module.functions) {
        errno_set(XANTE_ERROR_PLUGIN_WITHOUT_API);
        return -1;
    }

    if (validate_mandatory_events(xpp) == false) {
        errno_set(XANTE_ERROR_PLUGIN_WITHOUT_MANDATORY_FUNCTION);
        return -1;
    }

    if (event_call(EV_INIT, xpp, NULL) < 0) {
        errno_set(XANTE_ERROR_PLUGIN_INIT_ERROR);
        return -1;
    }

    return 0;
}

/**
 * @name event_uninit
 * @brief Closes the application module.
 *
 * Before closing the module, the function calls the EV_UNINIT event function.
 *
 * @param [in,out] xpp: The library main object.
 */
void event_uninit(struct xante_app *xpp)
{
    if (NULL == xpp)
        return;

    if (xpp->module.module != NULL) {
        event_call(EV_UNINIT, xpp, NULL);
        cl_plugin_info_unref(xpp->module.info);
        cl_stringlist_destroy(xpp->module.functions);
        cl_plugin_unload(xpp->module.module);
    }
}

/**
 * @name event_item_custom_data
 * @brief Calls the EV_ITEM_CUSTOM_DATA from the item to get a reference to
 *        some data.
 *
 * This data may be used inside the EV_UPDATE_ROUTINE event.
 *
 * @param [in,out] xpp: The library main object.
 * @param [in,out] item: The item which will have its event called.
 *
 * @return On success returns a pointer to the data ou NULL otherwise.
 */
void *event_item_custom_data(struct xante_app *xpp, struct xante_item *item)
{
    return ev_item_custom_data(xpp, item);
}

/*
 *
 * API
 *
 */

__PUB_API__ void *xante_event_argument(xante_event_arg_t *arg,
    enum xante_event_argument_type data_type)
{
    struct xante_event_argument *evt_arg = (struct xante_event_argument *)arg;

    errno_clear();

    if (NULL == arg) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return NULL;
    }

    switch (data_type) {
        case XANTE_EVENT_DATA_XANTE_T:
            return evt_arg->xpp;

        case XANTE_EVENT_DATA_XANTE_MENU_T:
            return evt_arg->menu;

        case XANTE_EVENT_DATA_XANTE_ITEM_T:
            return evt_arg->item;

        case XANTE_EVENT_DATA_XANTE_ITEM_VALUE:
            return evt_arg->value;

        case XANTE_EVENT_DATA_XANTE_CONFIG:
            return evt_arg->cfg_file;

        case XANTE_EVENT_DATA_XANTE_CHANGES_LIST:
            return evt_arg->changes;

        case XANTE_EVENT_DATA_CUSTOM:
            return evt_arg->data;

        default:
            break;
    }

    errno_set(XANTE_ERROR_UNKNOWN_EVENT_DATA_TYPE);

    return NULL;
}

