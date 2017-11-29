
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Mon Nov 27 15:10:13 2017
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

#define DEFAULT_STATUSBAR_TEXT      \
    "[ESC] Cancel [Enter] Confirm a selected option [Tab/Left/Right] Select an option [Up/Down] Select field"

#define DIALOG_WIDTH                55

struct dialog_properties {
    int width;
    int height;
    int form_height;
    int number_of_items;        //Number of items on screen
    int longest_name_length;
    int total_fields;
};

/*
 *
 * Internal functions
 *
 */

static cl_json_t *get_fields_node(const struct xante_item *item)
{
    return cl_json_get_object_item(item->mixedform_options, "fields");
}

static cl_string_t *get_title(const struct xante_item *item)
{
    cl_json_t *node;

    node = cl_json_get_object_item(item->mixedform_options, "title");

    if (NULL == node)
        return NULL;

    return cl_json_get_object_value(node);
}

static cl_string_t *get_string_field(const cl_json_t *field, const char *name)
{
    cl_json_t *node;

    node = cl_json_get_object_item(field, name);

    if (NULL == node)
        return NULL;

    return cl_json_get_object_value(node);
}

static cl_object_t *get_cl_object_field(const cl_json_t *field, const char *name)
{
    cl_json_t *node;

    node = cl_json_get_object_item(field, name);

    if (NULL == node)
        return NULL;

    /* Must unref after used */
    return cl_object_from_cstring(cl_json_get_object_value(node));
}

static bool get_boolean_field(const cl_json_t *field, const char *name)
{
    cl_json_t *node;
    bool value = false;

    node = cl_json_get_object_item(field, name);

    if (NULL == node)
        return NULL;

    if (cl_json_get_object_type(node) == CL_JSON_TRUE)
        value = true;

    return value;
}

static cl_string_t *get_element_name(const cl_json_t *field)
{
    return get_string_field(field, "name");
}

static bool get_element_password(const cl_json_t *field)
{
    return get_boolean_field(field, "password");
}

static bool get_element_read_only(const cl_json_t *field)
{
    return get_boolean_field(field, "read_only");
}

static cl_object_t *get_element_value(const cl_json_t *field)
{
    return get_cl_object_field(field, "value");
}

static cl_object_t *get_element_default_value(const cl_json_t *field)
{
    return get_cl_object_field(field, "default_value");
}

static char *mixedform_item_value(const cl_json_t *field)
{
    cl_object_t *value;
    char *ret;

    value = get_element_value(field);

    if (NULL == value)
        value = get_element_default_value(field);

    ret = CL_OBJECT_AS_STRING(value);
    cl_object_unref(value);

    return ret;
}

static void set_element(cl_json_t *field, const char *field_key,
    const char *new_value)
{
    cl_json_t *node;

    node = cl_json_create_string(new_value);

    if (cl_json_get_object_item(field, field_key) != NULL)
        cl_json_replace_item_in_object(field, field_key, node);
    else
        cl_json_add_item_to_object(field, field_key, node);
}

static void set_element_value(cl_json_t *field, const char *new_value)
{
    set_element(field, "value", new_value);
}

static int get_longest_element_name(const cl_json_t *fields, int total_fields)
{
    int i, max = 0, length;
    cl_json_t *node = NULL;

    for (i = 0; i < total_fields; i++) {
        node = cl_json_get_array_item(fields, i);

        length = cl_string_length(get_element_name(node));

        if (length > max)
            max = length;
    }

    return max;
}

static void release_dialog_content(DIALOG_FORMITEM *items, int total_items)
{
    int i;

    for (i = 0; i < total_items; i++) {
        free(items[i].name);
        free(items[i].text);
    }

    free(items);
}

static DIALOG_FORMITEM *prepare_dialog_content(const cl_json_t *fields,
    const struct dialog_properties *properties)
{
    DIALOG_FORMITEM *fitems = NULL, *item = NULL;
    int i;
    char fmt[8] = {0};
    cl_json_t *field;

    fitems = calloc(properties->total_fields, sizeof(DIALOG_FORMITEM));

    if (NULL == fitems) {
        errno_set(XANTE_ERROR_NO_MEMORY);
        return NULL;
    }

    sprintf(fmt, "%%-%ds:", properties->longest_name_length);

    for (i = 0; i < properties->total_fields; i++) {
        item = &fitems[i];
        field = cl_json_get_array_item(fields, i);

        asprintf(&item->name, fmt,
                 cl_string_valueof(get_element_name(field)));

        item->name_len = strlen(item->name);

        item->text = mixedform_item_value(field);
        item->text_len = strlen(item->text);
        item->text_free = true;

        item->name_x = 0;
        item->name_y = i;
        item->text_x = properties->longest_name_length + 2;
        item->text_y = i;

        if (get_element_read_only(field))
            item->type = 2;
        else
            item->type = 0;

        if (get_element_password(field))
            item->type = 1;

        /* XXX: Maybe we can leave these available through JSON */
        item->text_ilen = 64;
        item->text_flen = 30;
    }

    return fitems;
}

static void calc_dialog_limits(const cl_json_t *fields,
    struct dialog_properties *properties)
{
    properties->width = DIALOG_WIDTH;
    properties->total_fields = cl_json_get_array_size(fields);
    properties->longest_name_length = get_longest_element_name(fields,
                                                               properties->total_fields);

    properties->form_height = cl_json_get_array_size(fields);
    properties->number_of_items = dialog_get_dlg_items(properties->form_height);

    /* XXX: This 1 is from the dialog text message. */
    properties->height = properties->number_of_items +
                         DIALOG_HEIGHT_WITHOUT_TEXT + 1;
}

static char *get_current_result(DIALOG_FORMITEM *items, int number_of_items)
{
    DIALOG_FORMITEM *item;
    cl_string_t *result = NULL, *name;
    char *tmp = NULL;
    int i;

    result = cl_string_create("{\"fields\":[");

    for (i = 0; i < number_of_items; i++) {
        item = &items[i];
        name = cl_string_create("%s", item->name);
        cl_string_truncate(name, -1);
        cl_string_rtrim(name);
        cl_string_cat(result, "{\"name\":\"%s\",\"value\":\"%s\"},",
                      cl_string_valueof(name), item->text);

        cl_string_unref(name);
    }

    cl_string_truncate(result, -1);
    cl_string_cat(result, "]}");
    tmp = strdup(cl_string_valueof(result));
    cl_string_unref(result);

    return tmp;
}

static cl_json_t *get_field_by_name(cl_json_t *fields, int total_fields,
    cl_string_t *name)
{
    int i;
    cl_json_t *node = NULL;

    for (i = 0; i < total_fields; i++) {
        node = cl_json_get_array_item(fields, i);

        if (cl_string_cmp(name, get_element_name(node)) == 0)
            break;
    }

    return node;
}

static bool compare_elements(struct xante_app *xpp, struct xante_item *item,
    cl_json_t *old_element, const cl_json_t *new_element)
{
    cl_object_t *old_value, *new_value;
    cl_string_t *o, *n;
    bool changed = false;

    old_value = get_element_value(old_element);
    new_value = get_element_value(new_element);

    /*
     * Since we don't hold CL_POINTERs inside the values we can compare
     * them using its own API.
     */
    if (cl_object_equals(old_value, new_value) == false) {
        changed = true;
        o = cl_object_to_cstring(old_value);
        n = cl_object_to_cstring(new_value);
        set_element_value(old_element, cl_string_valueof(n));
        change_add(xpp, cl_string_valueof(item->name), cl_string_valueof(o),
                   cl_string_valueof(n));

        cl_string_unref(n);
        cl_string_unref(o);
    }

    cl_object_unref(new_value);
    cl_object_unref(old_value);

    return changed;
}

static bool item_values_have_changed(struct xante_app *xpp,
    struct xante_item *item, cl_json_t *fields, int total_fields,
    const char *result)
{
    bool changed = false;
    int i;
    cl_json_t *field = NULL, *jresult = NULL, *result_fields = NULL,
              *rfield = NULL;

    /* Gets the result in JSON format to ease our job below */
    jresult = cl_json_parse_string(result);

    if (NULL == jresult)
        goto end_block;

    result_fields = cl_json_get_object_item(jresult, "fields");

    if (NULL == result_fields)
        goto end_block;

    /* We compare each element to look for changes */
    for (i = 0; i < total_fields; i++) {
        rfield = cl_json_get_array_item(result_fields, i);
        field = get_field_by_name(fields, total_fields,
                                  get_element_name(rfield));

        changed = compare_elements(xpp, item, field, rfield);
    }

end_block:
    if (jresult != NULL)
        cl_json_delete(jresult);

    return changed;
}

/*
 *
 * Internal API
 *
 */

/**
 * @name ui_mixedform_load_and_set_value
 * @brief Loads current configured value of internal elements of a mixedform
 *        object.
 *
 * Since a mixedform object can have several internal elements with each one
 * of them having a specific value (configured), this function needs to search
 * and load every one of them from the configuration file \a cfg.
 *
 * @param [in,out] item: The mixedform item object.
 * @param [in] cfg: The configuration file object.
 */
void ui_mixedform_load_and_set_value(struct xante_item *item, cl_cfg_file_t *cfg)
{
    cl_json_t *fields = NULL, *field;
    cl_string_t *config_block, *config_item;
    cl_cfg_entry_t *entry;
    cl_object_t *value;
    char *tmp;
    int i, total_fields;

    fields = get_fields_node(item);

    if (NULL == fields)
        return;

    total_fields = cl_json_get_array_size(fields);

    for (i = 0; i < total_fields; i++) {
        field = cl_json_get_array_item(fields, i);
        config_block = get_string_field(field, "config_block");
        config_item = get_string_field(field, "config_item");

        entry = cl_cfg_entry(cfg, cl_string_valueof(config_block),
                             cl_string_valueof(config_item));

        if (NULL == entry)
            continue;

        value = cl_cfg_entry_value(entry);
        tmp = CL_OBJECT_AS_STRING(value);
        set_element_value(field, tmp);
        cl_object_unref(value);
        free(tmp);
    }
}

ui_return_t ui_dialog_mixedform(struct xante_app *xpp, struct xante_item *item)
{
    bool value_changed = false, loop = true;
    struct dialog_properties properties;
    int ret_dialog = DLG_EXIT_OK, selected_item = 0;
    cl_string_t *form_title = NULL;
    DIALOG_FORMITEM *items = NULL;
    cl_json_t *fields = NULL;
    char *result = NULL;
    ui_return_t ret;

    /* Prepare dialog */
    dlgx_set_backtitle(xpp);
    dlgx_update_cancel_button_label();
    dlgx_put_statusbar(DEFAULT_STATUSBAR_TEXT);

    /* Since we may have a password item, enable '*' on screen */
    dialog_vars.insecure = 1;

    /* Prepares dialog content */
    fields = get_fields_node(item);
    form_title = get_title(item);
    calc_dialog_limits(fields, &properties);
    items = prepare_dialog_content(fields, &properties);

    /* Enables the help button */
    if (item->descriptive_help != NULL)
        dialog_vars.help_button = 1;

    do {
        if (result != NULL) {
            free(result);
            result = NULL;
        }

        ret_dialog = dlg_form(cl_string_valueof(item->name),
                              cl_string_valueof(form_title),
                              properties.height, properties.width,
                              properties.form_height,
                              properties.number_of_items, items,
                              &selected_item);

        switch (ret_dialog) {
            case DLG_EXIT_OK:
                result = get_current_result(items, properties.total_fields);

                if (event_call(EV_ITEM_VALUE_CONFIRM, xpp, item, result) < 0)
                    break;

                value_changed = item_values_have_changed(xpp, item, fields,
                                                         properties.total_fields,
                                                         result);

                loop = false;
                break;

#ifdef ALTERNATIVE_DIALOG
            case DLG_EXIT_TIMEOUT:
                loop = false;
                break;
#endif

            case DLG_EXIT_ESC:
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

    if (item->descriptive_help != NULL)
        dialog_vars.help_button = 0;

    if (result != NULL)
        free(result);

    dialog_vars.insecure = 0;
    release_dialog_content(items, properties.total_fields);

    ret.selected_button = ret_dialog;
    ret.updated_value = value_changed;

    return ret;
}

