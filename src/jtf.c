
/*
 * Description: Functions to handle the JSON file from an application.
 *
 * Author: Rodrigo Freitas
 * Created at: Tue May  2 21:03:18 2017
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

/* JTF object names */
#define GENERAL             "general"
#define APPLICATION_NAME    "name"
#define MODULE_NAME         "module"
#define CONFIG_PATHNAME     "config_pathname"
#define LOG_PATHNAME        "log_pathname"
#define LOG_LEVEL           "log_level"
#define JTF_VERSION         "version"
#define JTF_REVISION        "revision"
#define JTF_BUILD           "build"
#define JTF_BETA            "beta"
#define UI                  "ui"
#define MAIN_MENU           "main_menu"
#define MENUS               "menus"
#define NAME                "name"
#define OBJECT_ID           "object_id"
#define MODE                "mode"
#define TYPE                "type"
#define BLOCK               "block"
#define ITEM                "item"
#define HELP                "help"
#define ITEMS               "items"
#define OPTIONS             "options"
#define DEFAULT_VALUE       "default_value"
#define RANGES              "ranges"
#define STRING_LENGTH       "string_length"
#define MAX_RANGE           "max"
#define MIN_RANGE           "min"
#define CONFIG              "config"
#define INTERNAL            "internal"
#define APPLICATION         "application"
#define COMPANY             "company"
#define DYNAMIC             "dynamic"
#define COPIES              "copies"
#define BLOCK_PREFIX        "block_prefix"
#define ORIGIN              "origin"
#define EVENTS              "events"
#define DESCRIPTION         "description"
#define BRIEF               "brief"
#define REFERENCED_MENU     "referenced_menu"
#define BLOCKED_KEYS        "blocked_keys"
#define ESC_KEY             "esc"
#define SUSPEND_KEY         "suspend_key"
#define STOP_KEY            "stop_key"
#define GEOMETRY            "geometry"
#define WIDTH               "width"
#define HEIGHT              "height"
#define DATA                "data"
#define BTN_OK_LABEL        "ok"
#define BTN_CANCEL_LABEL    "cancel"
#define BTN_EXTRA_LABEL     "extra"
#define BTN_HELP_LABEL      "help"
#define BUTTONS             "buttons"
#define TITLE               "title"
#define BTN_OK              "btn_ok"
#define BTN_CANCEL          "btn_cancel"
#define BTN_EXTRA           "btn_extra"
#define BTN_HELP            "btn_help"
#define LABELS              "labels"

/*
 *
 * Internal functions
 *
 */

static inline bool object_type_check(enum cl_json_type type,
    enum cl_json_type expected_type)
{
    bool bcheck = false;

    if ((expected_type == CL_JSON_TRUE) || (expected_type == CL_JSON_FALSE))
        bcheck = true;

    if ((bcheck && ((type != CL_JSON_TRUE) && (type != CL_JSON_FALSE))) ||
        (!bcheck && (type != expected_type)))
    {
        return false;
    }

    return true;
}

/*
 * This function will try to parse and load a value from the JTF file. It will
 * convert the value according the object type.
 *
 * Not every object is mandatory inside a JTF file, therefore one may desire to
 * abort execution only if we parsed with the wrong expected content type.
 */
static int parse_object_value(const cl_json_t *node, const char *object_name,
    enum cl_json_type expected_type, bool required_object, ...)
{
    va_list ap;
    cl_json_t *subnode = NULL;
    cl_string_t *value = NULL, *s = NULL;
    enum cl_json_type type;
    bool *b, btrue = true, bfalse = false;
    int *i, i_tmp;
    float *f, f_tmp;
    void **pp;

    subnode = cl_json_get_object_item(node, object_name);

    if (NULL == subnode) {
        if (required_object) {
            errno_set(XANTE_ERROR_JTF_NO_INFO);
            errno_store_additional_content(object_name);
            return -1;
        }

        return 0;
    }

    va_start(ap, NULL);
    type = cl_json_get_object_type(subnode);

    /* If the object exists its type must be expected one. */
    if (object_type_check(type, expected_type) == false) {
        errno_set(XANTE_ERROR_JTF_WRONG_OBJECT_TYPE);
        errno_store_additional_content(object_name);
        errno_store_additional_content(cl_json_type_to_string(expected_type));
        return -1;
    }

    if (type == CL_JSON_TRUE) {
        b = va_arg(ap, bool *);
        memcpy(b, &btrue, sizeof(bool));
        goto end_block;
    } else if (type == CL_JSON_FALSE) {
        b = va_arg(ap, bool *);
        memcpy(b, &bfalse, sizeof(bool));
        goto end_block;
    } else if (type == CL_JSON_ARRAY) {
        pp = va_arg(ap, void **);
        *pp = subnode;
        goto end_block;
    }

    /*
     * If we just parsed a boolean or an array object we don't need to get here.
     */

    value = cl_json_get_object_value(subnode);

    if (NULL == value) {
        errno_set(XANTE_ERROR_JTF_INFO_WITHOUT_VALUE);
        errno_store_additional_content(object_name);
        return -1;
    }

    if (type == CL_JSON_STRING) {
        pp = va_arg(ap, void **);
        s = cl_string_ref(value);
        *pp = s;
    } else if (type == CL_JSON_NUMBER) {
        i = va_arg(ap, int *);
        i_tmp = cl_string_to_int(value);
        memcpy(i, &i_tmp, sizeof(int));
    } else if (type == CL_JSON_NUMBER_FLOAT) {
        f = va_arg(ap, float *);
        f_tmp = cl_string_to_float(value);
        memcpy(f, &f_tmp, sizeof(float));
    }

end_block:
    va_end(ap);
    return 0;
}

static int parse_application_version(cl_json_t *internal,
    struct xante_app *xpp)
{
    cl_json_t *app = NULL;
    cl_string_t *tmp = NULL;

    app = cl_json_get_object_item(internal, APPLICATION);

    if (NULL == app) {
        errno_set(XANTE_ERROR_JTF_NO_APPLICATION_OBJECT);
        return -1;
    }

    if (parse_object_value(app, JTF_VERSION, CL_JSON_STRING, true,
                           (void **)&tmp) < 0)
    {
        return -1;
    } else {
        xpp->info.version = strdup(cl_string_valueof(tmp));
        cl_string_unref(tmp);
    }

    if (parse_object_value(app, JTF_REVISION, CL_JSON_NUMBER, true,
                           (void **)&xpp->info.revision) < 0)
    {
        return -1;
    }

    if (parse_object_value(app, JTF_BUILD, CL_JSON_NUMBER, true,
                           (void **)&xpp->info.build) < 0)
    {
        return -1;
    }

    if (parse_object_value(app, JTF_BETA, CL_JSON_TRUE, true,
                           (void **)&xpp->info.beta) < 0)
    {
        return -1;
    }

    return 0;
}

static void parse_blocked_keys(const cl_json_t *general, struct xante_app *xpp)
{
    cl_json_t *blocked_keys;

    /*
     * Sets the default value of these key or keys combination inside the
     * application.
     */
    xpp->info.esc_key = false;
    xpp->info.suspend_key = false;
    xpp->info.stop_key = false;

    blocked_keys = cl_json_get_object_item(general, BLOCKED_KEYS);

    if (NULL == blocked_keys)
        return;

    if (parse_object_value(blocked_keys, ESC_KEY, CL_JSON_TRUE, false,
                           (void **)&xpp->info.esc_key) < 0)
    {
        return;
    }

    if (parse_object_value(blocked_keys, SUSPEND_KEY, CL_JSON_TRUE, false,
                           (void **)&xpp->info.suspend_key) < 0)
    {
        return;
    }

    if (parse_object_value(blocked_keys, STOP_KEY, CL_JSON_TRUE, false,
                           (void **)&xpp->info.stop_key) < 0)
    {
        return;
    }
}

static int parse_jtf_info(cl_json_t *jtf, struct xante_app *xpp)
{
    cl_json_t *general = NULL, *internal = NULL;
    cl_string_t *tmp = NULL;

    internal = cl_json_get_object_item(jtf, INTERNAL);

    if (NULL == internal) {
        errno_set(XANTE_ERROR_JTF_NO_INTERNAL_OBJECT);
        return -1;
    }

    if (parse_application_version(internal, xpp) < 0)
        return -1;

    general = cl_json_get_object_item(jtf, GENERAL);

    if (NULL == general) {
        errno_set(XANTE_ERROR_JTF_NO_GENERAL_OBJECT);
        return -1;
    }

    if (parse_object_value(general, APPLICATION_NAME, CL_JSON_STRING, true,
                           (void **)&tmp) < 0)
    {
        return -1;
    } else {
        xpp->info.application_name = strdup(cl_string_valueof(tmp));
        cl_string_unref(tmp);
    }

    if (parse_object_value(general, MODULE_NAME, CL_JSON_STRING, true,
                           (void **)&tmp) < 0)
    {
        return -1;
    } else {
        xpp->info.module_name = strdup(cl_string_valueof(tmp));
        cl_string_unref(tmp);
    }

    if (parse_object_value(general, CONFIG_PATHNAME, CL_JSON_STRING, true,
                           (void **)&tmp) < 0)
    {
        return -1;
    } else {
        xpp->info.cfg_pathname = strdup(cl_string_valueof(tmp));
        cl_string_unref(tmp);
    }

    if (parse_object_value(general, LOG_PATHNAME, CL_JSON_STRING, true,
                           (void **)&tmp) < 0)
    {
        return -1;
    } else {
        xpp->info.log_pathname = strdup(cl_string_valueof(tmp));
        cl_string_unref(tmp);
    }

    if (parse_object_value(general, LOG_LEVEL, CL_JSON_STRING, true,
                           (void **)&tmp) < 0)
    {
        return -1;
    } else {
        xpp->info.log_level = strdup(cl_string_valueof(tmp));
        cl_string_unref(tmp);
    }

    if (parse_object_value(general, COMPANY, CL_JSON_STRING, true,
                           (void **)&tmp) < 0)
    {
        return -1;
    } else {
        xpp->info.company = strdup(cl_string_valueof(tmp));
        cl_string_unref(tmp);
    }

    if (parse_object_value(general, DESCRIPTION, CL_JSON_STRING, true,
                           (void **)&tmp) < 0)
    {
        return -1;
    } else {
        xpp->info.description = strdup(cl_string_valueof(tmp));
        cl_string_unref(tmp);
    }

    parse_blocked_keys(general, xpp);

    return 0;
}

static int parse_item_ranges(const cl_json_t *item, struct xante_item *i)
{
    cl_json_t *ranges = NULL;
    bool input_string = false, max_range = false, min_range = false;
    enum cl_json_type expected_type;

    /*
     * Dont't need to parse if we're not an input item.
     *
     * XXX: We just need to remember that the 'type' object must be previously
     *      loaded.
     */
    if (item_has_ranges(i->widget_type) == false)
        return 0;

    ranges = cl_json_get_object_item(item, RANGES);

    if (NULL == ranges) {
        errno_set(XANTE_ERROR_JTF_NO_RANGES);
        return -1;
    }

    /* Decide what ranges we're goind to parse */
    switch (i->widget_type) {
        case XANTE_WIDGET_INPUT_STRING:
        case XANTE_WIDGET_INPUT_PASSWD:
        case XANTE_WIDGET_INPUTSCROLL:
            input_string = true;
            break;

        case XANTE_WIDGET_INPUT_INT:
        case XANTE_WIDGET_INPUT_FLOAT:
        case XANTE_WIDGET_RANGE:
            max_range = true;
            min_range = true;
            break;

        case XANTE_WIDGET_PROGRESS:
        case XANTE_WIDGET_SPINNER_SYNC:
        case XANTE_WIDGET_DOTS_SYNC:
            max_range = true;
            break;

        default:
            break;
     }

    if ((parse_object_value(ranges, STRING_LENGTH, CL_JSON_NUMBER, false,
                            (void **)&i->string_length) < 0) &&
        input_string)
    {
        return -1;
    }

    expected_type = (i->widget_type == XANTE_WIDGET_INPUT_FLOAT) ? CL_JSON_NUMBER_FLOAT
                                                                    : CL_JSON_NUMBER;

    if ((parse_object_value(ranges, MIN_RANGE, expected_type, false,
                            (void **)&i->__helper.min) < 0) && min_range)
    {
        return -1;
    }

    if ((parse_object_value(ranges, MAX_RANGE, expected_type, false,
                            (void **)&i->__helper.max) < 0) && max_range)
    {
        return -1;
    }

    return 0;
}

static int parse_item_config(const cl_json_t *item, struct xante_item *it,
    bool single_instance)
{
    cl_json_t *config = NULL;

    /* The item does not have configuration */
    if (it->flags.config == false)
        return 0;

    config = cl_json_get_object_item(item, CONFIG);

    if (NULL == config) {
        if (single_instance == false) {
            errno_set(XANTE_ERROR_JTF_NO_CONFIG_OBJECT);
            return -1;
        } else {
            /*
             * If we're a single instance JTF we don't need to return an error
             */
            return 0;
        }
    }

    if (parse_object_value(config, BLOCK, CL_JSON_STRING, true,
                           (void **)&it->config_block) < 0)
    {
        return -1;
    }

    if (parse_object_value(config, ITEM, CL_JSON_STRING, true,
                           (void **)&it->config_item) < 0)
    {
        return -1;
    }

    return 0;
}

static int parse_item_help(const cl_json_t *item, struct xante_item *it,
    void **brief_options_help)
{
    cl_json_t *help = NULL;

    help = cl_json_get_object_item(item, HELP);

    if (NULL == help)
        return 0;

    if (parse_object_value(help, BRIEF, CL_JSON_STRING, false,
                           (void **)&it->brief_help) < 0)
    {
        return -1;
    }

    if (parse_object_value(help, DESCRIPTION, CL_JSON_STRING, false,
                           (void **)&it->descriptive_help) < 0)
    {
        return -1;
    }

    if (parse_object_value(help, OPTIONS, CL_JSON_ARRAY, false,
                           brief_options_help) < 0)
    {
        return -1;
    }

    return 0;
}

/*
 * This function is responsible for initializations with properties of an
 * item, so it may be correctly parsed and validated.
 */
static void pre_adjust_item_info(struct xante_item *item)
{
    item->widget_type =
        translate_string_widget_type(cl_string_valueof(item->type));

    /* Which properties are mandatory to an item? */
    if ((item->widget_type == XANTE_WIDGET_MENU_REFERENCE) ||
        (item->widget_type == XANTE_WIDGET_DYNAMIC_MENU_REFERENCE))
    {
        item->flags.referenced_menu = true;
    } else {
        if ((item->widget_type == XANTE_WIDGET_INPUT_INT) ||
            (item->widget_type == XANTE_WIDGET_INPUT_FLOAT) ||
            (item->widget_type == XANTE_WIDGET_INPUT_DATE) ||
            (item->widget_type == XANTE_WIDGET_INPUT_TIME) ||
            (item->widget_type == XANTE_WIDGET_INPUT_STRING) ||
            (item->widget_type == XANTE_WIDGET_CALENDAR) ||
            (item->widget_type == XANTE_WIDGET_TIMEBOX) ||
            (item->widget_type == XANTE_WIDGET_CHECKLIST) ||
            (item->widget_type == XANTE_WIDGET_RADIO_CHECKLIST) ||
            (item->widget_type == XANTE_WIDGET_YES_NO) ||
            (item->widget_type == XANTE_WIDGET_RANGE) ||
            (item->widget_type == XANTE_WIDGET_INPUTSCROLL) ||
            (item->widget_type == XANTE_WIDGET_BUILDLIST) ||
            (item->widget_type == XANTE_WIDGET_SPREADSHEET))
        {
            item->flags.config = true;
        }

        /* Almost every item needs to have the "options" object */
        if (item->widget_type != XANTE_WIDGET_CUSTOM)
            item->flags.options = true;
    }

    item->flags.ranges = item_has_ranges(item->widget_type);
}

/**
 * Do some adjustments inside an item after its informations is
 * completely loaded from the JTF file.
 *
 * For example, here we must split a checklist options into a cl_stringlist_t
 * object.
 *
 * @param [in,out] item: The item to be adjusted.
 */
static int adjusts_item_info(struct xante_item *item)
{
    int i, t, i_max, i_min;
    float f_max, f_min;
    cl_json_t *node = NULL;
    cl_string_t *value = NULL;

    /*
     * We hold the item's default-value as an cl_object_t since we can have
     * different types of values.
     */
    if (item->__helper.default_value != NULL) {
        item->default_value = cl_object_from_cstring(item->__helper.default_value);
        cl_string_unref(item->__helper.default_value);
    }

    switch (item->widget_type) {
        case XANTE_WIDGET_RADIO_CHECKLIST:
        case XANTE_WIDGET_CHECKLIST:
        case XANTE_WIDGET_BUILDLIST:
            if (item->__helper.options != NULL) {
                t = cl_json_get_array_size(item->__helper.options);
                item->list_items = cl_stringlist_create();

                for (i = 0; i < t; i++) {
                    node = cl_json_get_array_item(item->__helper.options, i);
                    value = cl_json_get_object_value(node);
                    cl_stringlist_add(item->list_items, value);
                }
            }

            /* There's nothing more to do if we're a buildlist object. */
            if (item->widget_type == XANTE_WIDGET_BUILDLIST) {
                break;
            }

            item->widget_checklist_type =
                (item->widget_type == XANTE_WIDGET_CHECKLIST) ? FLAG_CHECK
                                                                 : FLAG_RADIO;

            if (item->__helper.brief_options_help != NULL) {
                t = cl_json_get_array_size(item->__helper.brief_options_help);
                item->checklist_brief_options = cl_stringlist_create();

                for (i = 0; i < t; i++) {
                    node = cl_json_get_array_item(item->__helper.brief_options_help, i);
                    value = cl_json_get_object_value(node);
                    cl_stringlist_add(item->checklist_brief_options, value);
                }
            }

            break;

        case XANTE_WIDGET_INPUT_DATE:
            item->string_length = DATE_MAX_INPUT_LENGTH;
            break;

        case XANTE_WIDGET_INPUT_TIME:
            item->string_length = TIME_MAX_INPUT_LENGTH;
            break;

        case XANTE_WIDGET_INPUT_INT:
        case XANTE_WIDGET_RANGE:
            i_min = *(int *)&item->__helper.min;
            i_max = *(int *)&item->__helper.max;
            item->min = cl_object_create(CL_INT, i_min);
            item->max = cl_object_create(CL_INT, i_max);
            break;

        case XANTE_WIDGET_PROGRESS:
        case XANTE_WIDGET_SPINNER_SYNC:
        case XANTE_WIDGET_DOTS_SYNC:
            i_max = *(int *)&item->__helper.max;
            item->max = cl_object_create(CL_INT, i_max);
            break;

        case XANTE_WIDGET_INPUT_FLOAT:
            f_min = *(float *)&item->__helper.min;
            f_max = *(float *)&item->__helper.max;
            item->min = cl_object_create(CL_FLOAT, f_min);
            item->max = cl_object_create(CL_FLOAT, f_max);
            break;

        case XANTE_WIDGET_MIXEDFORM:
        case XANTE_WIDGET_SPREADSHEET:
            item->form_options = cl_json_parse(item->__helper.options);

            if (NULL == item->form_options) {
                errno_set(XANTE_ERROR_INVALID_FORM_JSON);
                errno_store_additional_content(cl_string_valueof(item->name));
                return -1;
            }

            break;

        default:
            break;
    }

    if ((item->widget_type != XANTE_WIDGET_CHECKLIST) &&
        (item->widget_type != XANTE_WIDGET_RADIO_CHECKLIST) &&
        (item->widget_type != XANTE_WIDGET_MIXEDFORM) &&
        (item->widget_type != XANTE_WIDGET_SPREADSHEET))
    {
        if (item->__helper.options != NULL) {
            item->options = cl_string_ref(item->__helper.options);
            cl_string_unref(item->__helper.options);
        }
    } else
        if (item->__helper.options != NULL)
            cl_string_unref(item->__helper.options);

    if (item_has_ranges(item->widget_type) == true)
        item->value_spec = cl_spec_create(CL_READABLE | CL_WRITABLE, item->min,
                                          item->max, item->string_length);

    return 0;
}

static int parse_dynamic_menu_properties(const cl_json_t *menu,
    struct xante_menu *m, void **copies)
{
    cl_json_t *dynamic = NULL, *origin = NULL;
    enum cl_json_type expected_type;

    /* We don't need to continue if we're not a dynamic menu */
    if (m->menu_type != XANTE_MENU_DYNAMIC)
        return 0;

    dynamic = cl_json_get_object_item(menu, DYNAMIC);

    if (NULL == dynamic) {
        errno_set(XANTE_ERROR_JTF_NO_DYNAMIC_OBJECT);
        return -1;
    }

    if (parse_object_value(dynamic, BLOCK_PREFIX, CL_JSON_STRING, false,
                           (void **)&m->dynamic_block_prefix) < 0)
    {
        return -1;
    }

    expected_type = (NULL == m->dynamic_block_prefix) ? CL_JSON_ARRAY
                                                      : CL_JSON_NUMBER;

    if (parse_object_value(dynamic, COPIES, expected_type, false, copies) < 0)
        return -1;

    origin = cl_json_get_object_item(dynamic, ORIGIN);

    if (NULL == origin) {
        errno_set(XANTE_ERROR_JTF_NO_ORIGIN_OBJECT);
        return -1;
    }

    if (parse_object_value(origin, BLOCK, CL_JSON_STRING, false,
                           (void **)&m->dynamic_origin_block) < 0)
    {
        return -1;
    }

    if (parse_object_value(origin, ITEM, CL_JSON_STRING, false,
                           (void **)&m->dynamic_origin_item) < 0)
    {
        return -1;
    }

    return 0;
}

/**
 * Do some pre adjustments inside a menu after some of its informations
 * was loaded from the JTF file.
 *
 * @param [in,out] menu: Them menu to be adjusted.
 */
static void pre_adjust_menu_info(struct xante_menu *menu)
{
    menu->menu_type = translate_string_menu_type((menu->type != NULL)
                                                  ? cl_string_valueof(menu->type)
                                                  : NULL);
}

/**
 * Do some adjustments inside a menu after its informations is
 * completely loaded from the JTF file.
 *
 * @param [in,out] menu: Them menu to be adjusted.
 * @param [in] copies: A pointer to some specific properties loaded.
 */
static void adjusts_menu_info(struct xante_menu *menu, const void *copies)
{
    int i, t;
    cl_json_t *node = NULL;
    cl_string_t *value = NULL;

    if ((menu->menu_type == XANTE_MENU_DYNAMIC) && (copies != NULL)) {
        /* If we don't have a block_prefix, we handle @copies as an array */
        if (NULL == menu->dynamic_block_prefix) {
            t = cl_json_get_array_size(copies);
            menu->dynamic_names = cl_stringlist_create();

            for (i = 0; i < t; i++) {
                node = cl_json_get_array_item(copies, i);
                value = cl_json_get_object_value(node);
                cl_stringlist_add(menu->dynamic_names, value);
            }
        } else
            menu->copies = *(int *)&copies;
    }
}

static int parse_ui(cl_json_t *jtf, struct xante_app *xpp)
{
    cl_json_t *ui, *node;
    struct xante_menu *menu = NULL;
    int i, t;

    ui = cl_json_get_object_item(jtf, UI);

    if (NULL == ui) {
        errno_set(XANTE_ERROR_JTF_NO_UI_OBJECT);
        return -1;
    }

    /* menus */
    node = cl_json_get_object_item(ui, MENUS);

    if (NULL == node) {
        errno_set(XANTE_ERROR_JTF_NO_MENUS_OBJECT);
        return -1;
    }

    t = cl_json_get_array_size(node);

    for (i = 0; i < t; i++) {
        menu = jtf_parse_menu(cl_json_get_array_item(node, i));

        if (NULL == menu)
            return -1;

        cl_list_unshift(xpp->ui.menus, menu, -1);
    }

    /* main menu */
    if (parse_object_value(ui, MAIN_MENU, CL_JSON_STRING, true,
                           (void **)&xpp->ui.main_menu_object_id) < 0)
    {
        return -1;
    }

    return 0;
}

static int parse_geometry(const cl_json_t *node, struct geometry *geometry)
{
    cl_json_t *jgeometry = NULL;

    jgeometry = cl_json_get_object_item(node, GEOMETRY);

    if (NULL == jgeometry)
        return 0;

    if (parse_object_value(jgeometry, WIDTH, CL_JSON_NUMBER, false,
                           (void **)&geometry->width) < 0)
    {
        return -1;
    }

    if (parse_object_value(jgeometry, HEIGHT, CL_JSON_NUMBER, false,
                           (void **)&geometry->height) < 0)
    {
        return -1;
    }

    return 0;
}

/*
 * Checks if an item must have a data object according its loaded
 * informations so far.
 */
static bool data_object_must_exist(const struct xante_item *item)
{
    bool must = false;

    if (item->flags.options ||
        item->flags.referenced_menu ||
        item->flags.config)
    {
        must = true;
    }

    return must;
}

/*
 * Here we parse the "data" object inside an item.
 */
static int parse_item_data(const cl_json_t *root, struct xante_item *item,
    bool single_instance)
{
    cl_json_t *data = NULL;
    enum cl_json_type expected_option = CL_JSON_STRING;

    data = cl_json_get_object_item(root, DATA);

    if (data_object_must_exist(item) && (NULL == data)) {
        errno_set(XANTE_ERROR_JTF_NO_DATA_OBJECT);
        errno_store_additional_content(cl_string_valueof(item->name));
        return -1;
    }

    if (parse_object_value(data, DEFAULT_VALUE, CL_JSON_STRING, false,
                           (void **)&item->__helper.default_value) < 0)
    {
        return -1;
    }

    /* Some widgets require that the options object to be an array. */
    if ((item->widget_type == XANTE_WIDGET_CHECKLIST) ||
        (item->widget_type == XANTE_WIDGET_RADIO_CHECKLIST) ||
        (item->widget_type == XANTE_WIDGET_BUILDLIST))
    {
        expected_option = CL_JSON_ARRAY;
    }

    if ((parse_object_value(data, OPTIONS, expected_option, false,
                            (void **)&item->__helper.options) < 0) &&
        item->flags.options)
    {
        return -1;
    }

    if ((parse_object_value(data, REFERENCED_MENU, CL_JSON_STRING, false,
                            (void **)&item->referenced_menu) < 0) &&
        item->flags.referenced_menu)
    {
        return -1;
    }

    if (parse_item_config(data, item, single_instance) < 0)
        return -1;

    if (parse_item_ranges(data, item) < 0)
        return -1;

    return 0;
}

static int parse_item_button_labels(const cl_json_t *labels,
    struct xante_item *item)
{
    cl_json_t *buttons = NULL;

    buttons = cl_json_get_object_item(labels, BUTTONS);

    if (NULL == buttons)
        return 0;

    if (parse_object_value(buttons, BTN_OK_LABEL, CL_JSON_STRING, false,
                           (void **)&item->label.ok) < 0)
    {
        return -1;
    }

    if (parse_object_value(buttons, BTN_CANCEL_LABEL, CL_JSON_STRING, false,
                           (void **)&item->label.cancel) < 0)
    {
        return -1;
    }

    if (parse_object_value(buttons, BTN_EXTRA_LABEL, CL_JSON_STRING, false,
                           (void **)&item->label.extra) < 0)
    {
        return -1;
    }

    if (parse_object_value(buttons, BTN_HELP_LABEL, CL_JSON_STRING, false,
                           (void **)&item->label.help) < 0)
    {
        return -1;
    }

    return 0;
}

static int parse_item_labels(const cl_json_t *ui, struct xante_item *item)
{
    cl_json_t *labels = NULL;

    labels = cl_json_get_object_item(ui, LABELS);

    if (NULL == labels)
        return 0;

    if (parse_object_value(labels, TITLE, CL_JSON_STRING, false,
                           (void **)&item->label.title) < 0)
    {
        return -1;
    }

    if (parse_item_help(labels, item, &item->__helper.brief_options_help) < 0)
        return -1;

    if (parse_item_button_labels(labels, item) < 0)
        return -1;

    return 0;
}

/*
 * Parse flags to enable/disable buttons in the widget.
 */
static int parse_item_buttons(const cl_json_t *ui, struct xante_item *item)
{
    cl_json_t *buttons = NULL;

    buttons = cl_json_get_object_item(ui, BUTTONS);

    if (NULL == buttons)
        return 0;

    if (parse_object_value(buttons, BTN_OK_LABEL, CL_JSON_TRUE, false,
                           (void **)&item->button.ok) < 0)
    {
        return -1;
    }

    if (parse_object_value(buttons, BTN_CANCEL_LABEL, CL_JSON_TRUE, false,
                           (void **)&item->button.cancel) < 0)
    {
        return -1;
    }

    if (parse_object_value(buttons, BTN_EXTRA_LABEL, CL_JSON_TRUE, false,
                           (void **)&item->button.extra) < 0)
    {
        return -1;
    }

    if (parse_object_value(buttons, BTN_HELP_LABEL, CL_JSON_TRUE, false,
                           (void **)&item->button.help) < 0)
    {
        return -1;
    }

    return 0;
}

/*
 * Here all informations related the item's UI are parsed.
 */
static int parse_item_ui(const cl_json_t *root, struct xante_item *item)
{
    cl_json_t *ui = NULL;

    ui = cl_json_get_object_item(root, UI);

    if (NULL == ui) {
        /*
         * All ui (object) related informations are not required, so if the
         * object does not exist we don't care and just return OK.
         */
        return 0;
    }

    if (parse_object_value(ui, BTN_OK, CL_JSON_TRUE, false,
                           (void **)&item->button.ok) < 0)
    {
        return -1;
    }

    if (parse_object_value(ui, BTN_CANCEL, CL_JSON_TRUE, false,
                           (void **)&item->button.cancel) < 0)
    {
        return -1;
    }

    if (parse_object_value(ui, BTN_EXTRA, CL_JSON_TRUE, false,
                           (void **)&item->button.extra) < 0)
    {
        return -1;
    }

    if (parse_object_value(ui, BTN_HELP, CL_JSON_TRUE, false,
                           (void **)&item->button.help) < 0)
    {
        return -1;
    }

    if (parse_geometry(ui, &item->geometry) < 0)
        return -1;

    if (parse_item_labels(ui, item) < 0)
        return -1;

    if (parse_item_buttons(ui, item) < 0)
        return -1;

    return 0;
}

/*
 *
 * Internal API
 *
 */

/**
 * @name jtf_parse_item
 * @brief Parses a JTF item from a JSON node.
 *
 * With the \a single_instance as true we make the item parse flexible, ignoring
 * its needed for the object_id and configurations objects to exist.
 *
 * @param [in] item: The item as a JSON node.
 * @param [in] single_instance: A boolean flag to tell if we're parsing the main
 *                              application JTF file or a Single Instance JTF.
 *
 * @return On success returns a struct xante_item with the parsed info or
 *         NULL otherwise.
 */
struct xante_item *jtf_parse_item(const cl_json_t *item, bool single_instance)
{
    struct xante_item *i;

    i = xante_item_create();

    if (NULL == i)
        return NULL;

    /* Objects loaded before pre-adjusment are (almost all) mandatory */
    if (parse_object_value(item, NAME, CL_JSON_STRING, true,
                           (void **)&i->name) < 0)
    {
        return NULL;
    }

    if (parse_object_value(item, TYPE, CL_JSON_STRING, true,
                           (void **)&i->type) < 0)
    {
        return NULL;
    }

    if (parse_object_value(item, MODE, CL_JSON_NUMBER, false,
                           (void **)&i->mode) < 0)
    {
        return NULL;
    }

    if (parse_object_value(item, OBJECT_ID, CL_JSON_STRING, !single_instance,
                           (void **)&i->object_id) < 0)
    {
        return NULL;
    }

    /*
     * Before continue parsing the item we must adjust some internal properties
     * to correctly parse the next ones.
     */
    pre_adjust_item_info(i);

    if (parse_item_data(item, i, single_instance) < 0)
        return NULL;

    i->events = cl_json_dup(cl_json_get_object_item(item, EVENTS));

    if (parse_item_ui(item, i) < 0)
        return NULL;

    /* Set up specific item properties */
    if (adjusts_item_info(i))
        return NULL;

    return i;
}

/**
 * @name jtf_parse_menu
 * @brief Parses a JTF menu from a JSON node.
 *
 * @param [in] menu: The menu as a JSON node.
 *
 * @return On success returns a struct xante_menu with the parsed info or
 *         NULL otherwise.
 */
struct xante_menu *jtf_parse_menu(const cl_json_t *menu)
{
    cl_json_t *items;
    struct xante_menu *m = NULL;
    struct xante_item *it = NULL;
    void *copies = NULL;
    int i, t;

    m = xante_menu_create(XANTE_MENU_CREATED_FROM_JTF);

    if (NULL == m)
        return NULL;

    if (parse_object_value(menu, NAME, CL_JSON_STRING, true,
                           (void **)&m->name) < 0)
    {
        return NULL;
    }

    if (parse_object_value(menu, OBJECT_ID, CL_JSON_STRING, true,
                           (void **)&m->object_id) < 0)
    {
        return NULL;
    }

    if (parse_object_value(menu, TYPE, CL_JSON_STRING, false,
                           (void **)&m->type) < 0)
    {
        return NULL;
    }

    /*
     * Before continue parsing the menu properties we must adjust some
     * internal informations.
     */
    pre_adjust_menu_info(m);

    if (parse_dynamic_menu_properties(menu, m, &copies) < 0)
        return NULL;

    if (parse_geometry(menu, &m->geometry) < 0)
        return NULL;

    m->events = cl_json_dup(cl_json_get_object_item(menu, EVENTS));

    /*
     * We must adjust some internal menu informations before loading its
     * items.
     */
    adjusts_menu_info(m, copies);
    items = cl_json_get_object_item(menu, ITEMS);

    if (NULL == items) {
        errno_set(XANTE_ERROR_JTF_NO_ITEMS_OBJECT);
        return NULL;
    }

    t = cl_json_get_array_size(items);

    for (i = 0; i < t; i++) {
        it = jtf_parse_item(cl_json_get_array_item(items, i), false);

        if (NULL == it)
            return NULL;

        cl_list_unshift(m->items, it, -1);
    }

    return m;
}

/**
 * @name jtf_parse_application_info
 * @brief Parses only the application relevant informations.
 *
 * @param [in] pathname: The JTF pathname.
 * @param [in,out] xpp: The previously created xante_app structure.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int jtf_parse_application_info(const char *pathname, struct xante_app *xpp)
{
    cl_json_t *jtf = NULL;
    int ret = -1;

    if ((NULL == pathname) || (NULL == xpp)) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    /*
     * We need to initialize libcollections here, since we're using its JSON
     * API, and we need to have some relevant informations to validate the
     * application initialization process.
     */
    cl_init(NULL);
    jtf = cl_json_read_file(pathname);

    if (NULL == jtf) {
        errno_set(XANTE_ERROR_WRONG_JTF_FORMAT);
        cl_uninit();
        return -1;
    }

    /* Parse main JTF informations */
    ret = parse_jtf_info(jtf, xpp);
    cl_json_delete(jtf);
    cl_uninit();

    return ret;
}

/**
 * @name jtf_parse_application
 * @brief Parses the whole application data to the xante_app structure.
 *
 * This function is responsible to parse all application information from
 * within a JTF file and put them inside a xante_app structure.
 *
 * @param [in] pathname: The JTF pathname.
 * @param [in,out] xpp: The previously created xante_app structure.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int jtf_parse_application(const char *pathname, struct xante_app *xpp)
{
    cl_json_t *jtf = NULL;
    int ret = -1;

    if ((NULL == pathname) || (NULL == xpp)) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    jtf = cl_json_read_file(pathname);

    if (NULL == jtf) {
        errno_set(XANTE_ERROR_WRONG_JTF_FORMAT);
        return -1;
    }

    /* Parse the interface (menus and its items) */
    ui_data_init(xpp);
    ret = parse_ui(jtf, xpp);
    cl_json_delete(jtf);

    return ret;
}

/**
 * @name jtf_release_info
 * @brief Releases all previously loaded information when parsing the JTF file.
 *
 * @param [in,out] xpp: The previously filled xante_app structure.
 */
void jtf_release_info(struct xante_app *xpp)
{
    if (NULL == xpp)
        return;

    if (xpp->info.cfg_pathname != NULL)
        free(xpp->info.cfg_pathname);

    if (xpp->info.log_pathname != NULL)
        free(xpp->info.log_pathname);

    if (xpp->info.log_level != NULL)
        free(xpp->info.log_level);

    if (xpp->info.application_name != NULL)
        free(xpp->info.application_name);

    if (xpp->info.module_name != NULL)
        free(xpp->info.module_name);

    if (xpp->info.version != NULL)
        free(xpp->info.version);

    if (xpp->info.company != NULL)
        free(xpp->info.company);

    if (xpp->info.description != NULL)
        free(xpp->info.description);
}

