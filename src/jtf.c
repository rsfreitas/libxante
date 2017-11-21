
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
#define PLUGIN_NAME         "plugin"
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
#define INPUT_RANGES        "input_ranges"
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
#define MENU_ID             "menu_id"

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

    if (parse_object_value(general, PLUGIN_NAME, CL_JSON_STRING, true,
                           (void **)&tmp) < 0)
    {
        return -1;
    } else {
        xpp->info.plugin_name = strdup(cl_string_valueof(tmp));
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

    return 0;
}

static int parse_item_input_ranges(const cl_json_t *item, struct xante_item *i,
    void **max, void **min)
{
    cl_json_t *input_range = NULL;
    bool input_string = true;
    enum cl_json_type expected_type;

    /*
     * Dont't need to parse if we're not an input item.
     *
     * XXX: We just need to remember that the 'type' object must be previously
     *      loaded.
     */
    if (is_input_item(i->dialog_type) == false)
        return 0;

    input_range = cl_json_get_object_item(item, INPUT_RANGES);

    if (NULL == input_range) {
        errno_set(XANTE_ERROR_JTF_NO_INPUT_RANGES);
        return -1;
    }

    input_string = ((i->dialog_type == XANTE_UI_DIALOG_INPUT_INT) ||
                    (i->dialog_type == XANTE_UI_DIALOG_INPUT_FLOAT)) ? false
                                                                     : true;

    if ((parse_object_value(input_range, STRING_LENGTH, CL_JSON_NUMBER, false,
                            (void **)&i->string_length) < 0) &&
        input_string)
    {
        return -1;
    }

    expected_type = (i->dialog_type == XANTE_UI_DIALOG_INPUT_INT) ? CL_JSON_NUMBER
                                                                  : CL_JSON_NUMBER_FLOAT;

    if ((parse_object_value(input_range, MIN_RANGE, expected_type, false,
                            min) < 0) &&
        (input_string == false))
    {
        return -1;
    }

    if ((parse_object_value(input_range, MAX_RANGE, expected_type, false,
                            max) < 0) &&
        (input_string == false))
    {
        return -1;
    }

    return 0;
}

static int parse_item_config(const cl_json_t *item, struct xante_item *it)
{
    cl_json_t *config = NULL;

    /* The item does not have configuration */
    if (it->flags.config == false)
        return 0;

    config = cl_json_get_object_item(item, CONFIG);

    if (NULL == config) {
        errno_set(XANTE_ERROR_JTF_NO_CONFIG_OBJECT);
        return -1;
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
    item->dialog_type =
        translate_string_dialog_type(cl_string_valueof(item->type));

    /* Which properties are mandatory to an item? */
    if ((item->dialog_type == XANTE_UI_DIALOG_MENU) ||
        (item->dialog_type == XANTE_UI_DIALOG_DYNAMIC_MENU))
    {
        item->flags.menu_id = true;
    } else {
        if ((item->dialog_type != XANTE_UI_DIALOG_DELETE_DYNAMIC_MENU_ITEM) &&
            (item->dialog_type != XANTE_UI_DIALOG_ADD_DYNAMIC_MENU_ITEM))
        {
            item->flags.config = true;
        }

        item->flags.options = true;
    }
}

/**
 * Do some adjustments inside an item after its informations is
 * completely loaded from the JTF file.
 *
 * For example, here we must split a checklist options into a cl_stringlist_t
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
static void adjusts_item_info(struct xante_item *item, cl_string_t *default_value,
    void *options, void *max_range, void *min_range, void *brief_options_help)
{
    int i, t, i_max, i_min;
    float f_max, f_min;
    cl_json_t *node = NULL;
    cl_string_t *value = NULL;

    if (default_value != NULL)
        item->default_value = cl_object_from_cstring(default_value);

    switch (item->dialog_type) {
        case XANTE_UI_DIALOG_RADIO_CHECKLIST:
        case XANTE_UI_DIALOG_CHECKLIST:
            if (options != NULL) {
                t = cl_json_get_array_size(options);
                item->checklist_options = cl_stringlist_create();

                for (i = 0; i < t; i++) {
                    node = cl_json_get_array_item(options, i);
                    value = cl_json_get_object_value(node);
                    cl_stringlist_add(item->checklist_options, value);
                }
            }

            item->dialog_checklist_type =
                (item->dialog_type == XANTE_UI_DIALOG_CHECKLIST) ? FLAG_CHECK
                                                                 : FLAG_RADIO;

            if (brief_options_help != NULL) {
                t = cl_json_get_array_size(brief_options_help);
                item->checklist_brief_options = cl_stringlist_create();

                for (i = 0; i < t; i++) {
                    node = cl_json_get_array_item(brief_options_help, i);
                    value = cl_json_get_object_value(node);
                    cl_stringlist_add(item->checklist_brief_options, value);
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

    if (is_input_item(item->dialog_type) == true)
        item->value_spec = cl_spec_create(CL_READABLE | CL_WRITABLE, item->min,
                                          item->max, item->string_length);
}

static int parse_dynamic_menu_properties(const cl_json_t *menu,
    struct xante_menu *m, void **copies)
{
    cl_json_t *dynamic = NULL, *origin = NULL;
    enum cl_json_type expected_type;

    /* We don't need to continue if we're not a dynamic menu */
    if (m->menu_type != XANTE_UI_MENU_DYNAMIC)
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

    if ((menu->menu_type == XANTE_UI_MENU_DYNAMIC) && (copies != NULL)) {
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

/*
 *
 * Internal API
 *
 */

/**
 * @name jtf_parse_item
 * @brief Parses a JTF item from a JSON node.
 *
 * @param [in] item: The item as a JSON node.
 *
 * @return On success returns a struct xante_item with the parsed info or
 *         NULL otherwise.
 */
struct xante_item *jtf_parse_item(const cl_json_t *item)
{
    struct xante_item *i;
    cl_string_t *default_value = NULL;
    void *options = NULL, *max = NULL, *min = NULL, *brief_options_help = NULL;
    enum cl_json_type expected_option = CL_JSON_STRING;

    i = xante_item_create();

    if (NULL == i)
        return NULL;

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

    if (parse_object_value(item, MODE, CL_JSON_NUMBER, true,
                           (void **)&i->mode) < 0)
    {
        return NULL;
    }

    if (parse_object_value(item, OBJECT_ID, CL_JSON_STRING, true,
                           (void **)&i->object_id) < 0)
    {
        return NULL;
    }

    /*
     * Before continue parsing the item we must adjust some internal properties
     * to correctly parse the next ones.
     */
    pre_adjust_item_info(i);

    if (parse_object_value(item, DEFAULT_VALUE, CL_JSON_STRING, false,
                           (void **)&default_value) < 0)
    {
        return NULL;
    }

    if ((i->dialog_type == XANTE_UI_DIALOG_CHECKLIST) ||
        (i->dialog_type == XANTE_UI_DIALOG_RADIO_CHECKLIST))
    {
        expected_option = CL_JSON_ARRAY;
    }

    if ((parse_object_value(item, OPTIONS, expected_option, false,
                            (void **)&options) < 0) &&
        i->flags.options)
    {
        return NULL;
    }

    if ((parse_object_value(item, MENU_ID, CL_JSON_STRING, false,
                            (void **)&i->menu_id) < 0) &&
        i->flags.menu_id)
    {
        return NULL;
    }

    i->events = cl_json_dup(cl_json_get_object_item(item, EVENTS));

    if (parse_item_help(item, i, &brief_options_help) < 0)
        return NULL;

    if (parse_item_config(item, i) < 0)
        return NULL;

    if (parse_item_input_ranges(item, i, &max, &min) < 0)
        return NULL;

    /* Set up specific item properties */
    adjusts_item_info(i, default_value, options, max, min,
                      brief_options_help);

    if (default_value != NULL)
        cl_string_unref(default_value);

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
        it = jtf_parse_item(cl_json_get_array_item(items, i));

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

    if (xpp->info.plugin_name != NULL)
        free(xpp->info.plugin_name);

    if (xpp->info.version != NULL)
        free(xpp->info.version);

    if (xpp->info.company != NULL)
        free(xpp->info.company);

    if (xpp->info.description != NULL)
        free(xpp->info.description);
}

