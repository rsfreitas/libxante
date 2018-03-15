
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

/*
 * A spreadsheet object must be configured inside a JTF with the following
 * content:
 * {
 *      "type": "spreadsheet",
 *      "name": "Name of the sheet",
 *      "object_id": "id of the object",
 *      "data": {
 *          "config": {
 *              "block": "Sheet_block_name"
 *          },
 *          "options": "Sheet data"
 *      }
 * }
 *
 * The options object defines the sheet size, labels and content. And must be
 * configured as a JSON string, like this:
 *
 * {
 *     "title": string,
 *     "sheet": [
 *          {
 *              "title": string,
 *              "column": [
 *               {
 *                  "title": string,
 *                  "value": string,
 *                  "default_value": string
 *               },
 *               {
 *                  "title": string,
 *                  "value": string,
 *                  "default_value": string
 *               }
 *              ]
 *          },
 *          {
 *              "title": string,
 *              "column": [
 *               {
 *                  "title": string,
 *                  "value": string,
 *                  "default_value": string
 *               },
 *               {
 *                  "title": string,
 *                  "value": string,
 *                  "default_value": string
 *               }
 *              ]
 *          }
 *     ]
 * }
 *
 * The cells content will be written to the configuration file using the config
 * block object name and some fixed string as suffixes, like this:
 *
 * "config": {
 *      "block":" "Block_name"
 *      "item": "Sheet"
 * }
 *
 * Assuming a sheet with 2 rows and 3 columns each, the data will looks like:
 *
 * [Block_name]
 * Sheet=2
 *
 * [Sheet_row_0]
 * column_0=
 * column_1=
 * column_2=
 *
 * [Sheet_row_1]
 * column_0=
 * column_1=
 * column_2=
 *
 * So every row will have the config->item object content as a prefix. followed
 * by a 'row_' suffix and the row number, starting from 0. And the columns will
 * have their names with 'column_' prefix followed by the column number, also
 * starting from 0.
 */

/*
 *
 * Internal functions
 *
 */

static cl_string_t *get_title(const cl_json_t *node)
{
    cl_json_t *n = NULL;

    n = cl_json_get_object_item(node, "title");

    if (NULL == n)
        return NULL;

    return cl_json_get_object_value(n);
}

static cl_string_t *get_cell_value(const cl_json_t *cell)
{
    cl_json_t *n = NULL;

    n = cl_json_get_object_item(cell, "value");

    if (NULL == n)
        n = cl_json_get_object_item(cell, "default_value");

    return cl_json_get_object_value(n);
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

static void set_cell_value(cl_json_t *cell, const char *new_value)
{
    set_element(cell, "value", new_value);
}

static int build_sheet(const struct xante_item *item,
    session_t *session)
{
    cl_json_t *sheet, *row, *column, *cell;
    cl_string_t *value;
    int i, rows = 0, j, columns = 0;

    sheet = cl_json_get_object_item(item->form_options, "sheet");

    if (NULL == sheet)
        return -1;

    rows = cl_json_get_array_size(sheet);
    session->row_title = cl_string_create_empty(0);
    session->column_title = cl_string_create_empty(0);

    /* First we build the sheet row and column titles */
    for (i = 0; i < rows; i++) {
        row = cl_json_get_array_item(sheet, i);

        /* We only use the first row to build the title */
        if (i == 0) {
            column = cl_json_get_object_item(row, "column");
            columns = cl_json_get_array_size(column);

            /* Column title */
            for (j = 0; j < columns; j++) {
                cell = cl_json_get_array_item(column, j);
                cl_string_cat(session->column_title, "%s,",
                              cl_string_valueof(get_title(cell)));
            }
        }

        /* Row column */
        cl_string_cat(session->row_title, "%s,",
                      cl_string_valueof(get_title(row)));
    }

    cl_string_truncate(session->row_title, -1);
    cl_string_truncate(session->column_title, -1);
    session->sheet = spreadsheet_st_init(cl_string_valueof(session->row_title),
                                            cl_string_valueof(session->column_title));

    if (NULL == session->sheet)
        return -1;

    /* Now we can store the value of each cell */
    for (i = 0; i < rows; i++) {
        row = cl_json_get_array_item(sheet, i);
        column = cl_json_get_object_item(row, "column");
        columns = cl_json_get_array_size(column);

        for (j = 0; j < columns; j++) {
            cell = cl_json_get_array_item(column, j);
            value = get_cell_value(cell);
            spreadsheet_st_add_data(session->sheet, (i * columns) + j,
                                    "%s", cl_string_valueof(value));
        }
    }

    return 0;
}

static void build_session(session_t *session)
{
    struct xante_item *item = session->item;
    cl_json_t *node;

    /* Window title */
    node = cl_json_get_object_item(item->form_options, "title");

    if (NULL == node)
        return;

    session->title = cl_string_ref(cl_json_get_object_value(node));

    /* Sheet */
    if (build_sheet(item, session) < 0)
        return;
}

static int get_form_number_of_rows(const struct xante_item *item)
{
    cl_json_t *sheet = NULL;

    sheet = cl_json_get_object_item(item->form_options, "sheet");

    if (NULL == sheet)
        return 0;

    return cl_json_get_array_size(sheet);
}

static cl_json_t *get_form_row(const struct xante_item *item, int row)
{
    cl_json_t *sheet = NULL;

    sheet = cl_json_get_object_item(item->form_options, "sheet");

    if (NULL == sheet)
        return 0;

    return cl_json_get_array_item(sheet, row);
}

static int get_row_number_of_columns(const cl_json_t *row)
{
    cl_json_t *c = NULL;

    c = cl_json_get_object_item(row, "column");

    if (NULL == c)
        return 0;

    return cl_json_get_array_size(c);
}

static int get_form_number_of_columns(const struct xante_item *item)
{
    cl_json_t *row = NULL;

    row = get_form_row(item, 0);

    if (NULL == row)
        return 0;

    return get_row_number_of_columns(row);
}

static cl_json_t *get_cell_from_row(const cl_json_t *row, int column)
{
    cl_json_t *c = NULL;

    c = cl_json_get_object_item(row, "column");

    if (NULL == c)
        return NULL;

    return cl_json_get_array_item(c, column);
}

static void load_configured_column(struct xante_item *item,
    const cl_cfg_file_t *cfg, const cl_json_t *jrow, int row, int column)
{
    cl_json_t *cell = NULL;
    cl_cfg_entry_t *entry = NULL;
    cl_object_t *value = NULL;
    cl_string_t *config_block = NULL, *config_item = NULL, *tmp = NULL;

    cell = get_cell_from_row(jrow, column);

    if (NULL == cell)
        return;

    config_block = cl_string_create("%s_row_%d",
                                    cl_string_valueof(item->config_item), row);

    if (NULL == config_block)
        return;

    config_item = cl_string_create("column_%d", column);

    if (NULL == config_item)
        return;

    entry = cl_cfg_entry(cfg, cl_string_valueof(config_block),
                         cl_string_valueof(config_item));

    if (NULL == entry)
        /* Uses the default-value */
        return;

    value = cl_cfg_entry_value(entry);

    switch (cl_object_type(value)) {
        case CL_INT:
            tmp = cl_string_create("%d", CL_OBJECT_AS_INT(value));
            break;

        case CL_FLOAT:
            tmp = cl_string_create("%f", CL_OBJECT_AS_FLOAT(value));
            break;

        default:
            /*
             * We handle as default here but the value will (possibly) be of
             * a CL_STRING type.
             */
            tmp = CL_OBJECT_AS_CSTRING(value);
            break;
    }

    set_cell_value(cell, cl_string_valueof(tmp));
    cl_string_unref(tmp);
    cl_object_unref(value);
    cl_string_unref(config_item);
    cl_string_unref(config_block);
}

static void load_configured_row(struct xante_item *item,
    const cl_cfg_file_t *cfg, int row)
{
    cl_json_t *jrow = NULL;
    int columns = 0, i;

    jrow = get_form_row(item, row);

    if (NULL == jrow)
        return;

    columns = get_row_number_of_columns(jrow);

    for (i = 0; i < columns; i++)
        load_configured_column(item, cfg, jrow, row, i);
}

static void save_row(struct xante_app *xpp, struct xante_item *item, int row)
{
    cl_json_t *jrow = NULL, *cell = NULL;
    cl_string_t *config_block = NULL, *config_item = NULL, *value = NULL;
    int columns = 0, i;

    jrow = get_form_row(item, row);

    if (NULL == jrow)
        return;

    columns = get_row_number_of_columns(jrow);
    config_block = cl_string_create("%s_row_%d",
                                    cl_string_valueof(item->config_item),
                                    row);

    // For every cell, get its value and save it
    for (i = 0; i < columns; i++) {
        cell = get_cell_from_row(jrow, i);
        value = get_cell_value(cell);
        config_item = cl_string_create("column_%d", i);
        cl_cfg_set_value(xpp->config.cfg_file,
                         cl_string_valueof(config_block),
                         cl_string_valueof(config_item),
                         "%s", cl_string_valueof(value));

        cl_string_unref(config_item);
    }

    cl_string_unref(config_block);
}

/*
 * This function builds an intermediate JSON containing only the cell values
 * with the following format:
 *
 * {
 *      "sheet": [
 *          // Array of rows
 *          [ // Array of column values ],
 *          [ // Array of column values ],
 *          [ // Array of column values ],
 *          [ // Array of column values ]
 *      ]
 * }
 *
 * It should be used to compare with current stored values to know if any change
 * has been made. It also should be passed to the confirm event.
 */
static cl_string_t *get_current_result(const struct xante_item *item,
    session_t *session)
{
    int rows = 0, columns = 0, i = 0, j = 0;
    cl_string_t *result = NULL;
    char *cell_data = NULL;

    rows = get_form_number_of_rows(item);
    columns = get_form_number_of_columns(item);
    result = cl_string_create("{\"sheet\":[");

    for (i = 0; i < rows; i++) {
        cl_string_cat(result, "[");

        for (j = 0; j < columns; j++) {
            cell_data = (char *)spreadsheet_st_get_data(session->sheet,
                                                        (i * columns) + j);

            cl_string_cat(result, "\"%s\",", cell_data);
        }

        cl_string_truncate(result, -1);
        cl_string_cat(result, "],");
    }

    cl_string_truncate(result, -1);
    cl_string_cat(result, "]}");

    return result;
}

static bool compare_cells(struct xante_app *xpp, struct xante_item *item,
    cl_json_t *old_cell, const cl_json_t *new_cell, int row_index,
    int column_index)
{
    cl_string_t *new_value, *old_value, *item_name;
    bool changed = false;

    old_value = cl_string_dup(get_cell_value(old_cell));
    new_value = cl_json_get_object_value(new_cell);
    item_name = cl_string_create("%s:row_%d:cell_%d",
                                 cl_string_valueof(item->name),
                                 row_index, column_index);

    if (cl_string_cmp(old_value, new_value) != 0) {
        changed = true;
        set_cell_value(old_cell, cl_string_valueof(new_value));

        /*
         * Since a spreadsheet may have several values we add a notification
         * about a change here, for each different cell.
         */
        change_add(xpp, cl_string_valueof(item_name),
                   cl_string_valueof(old_value),
                   cl_string_valueof(new_value));
    }

    cl_string_unref(item_name);
    cl_string_unref(old_value);

    return changed;
}

static bool compare_rows(struct xante_app *xpp, struct xante_item *item,
    cl_json_t *old_row, const cl_json_t *new_row, int row_index)
{
    int i, columns;
    bool changed = false;
    cl_json_t *old_cell, *new_cell, *jcolumn;

    jcolumn = cl_json_get_object_item(old_row, "column");

    if (NULL == jcolumn)
        return changed;

    columns = cl_json_get_array_size(jcolumn);

    for (i = 0; i < columns; i++) {
        old_cell = cl_json_get_array_item(jcolumn, i);
        new_cell = cl_json_get_array_item(new_row, i);

        if (compare_cells(xpp, item, old_cell, new_cell, row_index, i))
            changed = true;
    }

    return changed;
}

/*
 *
 * Internal API
 *
 */

bool spreadsheet_value_changed(session_t *session)
{
    struct xante_item *item = session->item;
    bool changed = false;
    cl_json_t *jresult = NULL, *sheet = NULL, *result_sheet = NULL,
              *sheet_row = NULL, *result_row = NULL;
    int rows, i;

    rows = get_form_number_of_rows(item);
    jresult = cl_json_parse(session->result);

    if (NULL == jresult)
        goto end_block;

    result_sheet = cl_json_get_object_item(jresult, "sheet");

    if (NULL == result_sheet)
        goto end_block;

    sheet = cl_json_get_object_item(item->form_options, "sheet");

    if (NULL == sheet)
        goto end_block;

    for (i = 0; i < rows; i++) {
        sheet_row = cl_json_get_array_item(sheet, i);
        result_row = cl_json_get_array_item(result_sheet, i);

        if (compare_rows(session->xpp, item, sheet_row, result_row, i))
            changed = true;
    }

end_block:
    if (jresult != NULL)
        cl_json_delete(jresult);

    return changed;
}

/**
 * @name ui_spreadsheet_load_and_set_value
 * @brief Loads values to the spreadsheet celss from the configuration file.
 *
 * @param [in,out] item: The item which is being loaded.
 * @param [in] cfg: A pointer to the configuration file data.
 */
void ui_spreadsheet_load_and_set_value(struct xante_item *item,
    const cl_cfg_file_t *cfg)
{
    int cfg_rows, i;

    // First we get the number of rows
    cfg_rows = get_form_number_of_rows(item);

    // For each row we get and set the column value
    for (i = 0; i < cfg_rows; i++)
        load_configured_row(item, cfg, i);
}

/**
 * @name ui_save_spreadsheet
 * @brief Saves the spreadsheet data into the configuration file.
 *
 * Besides saving the value of each spreadsheet cell this function needs to
 * save the number of rows and columns inside the configuration file, so one
 * may know these and be able to load all cell values.
 *
 * Assuming we have a spreadsheet with 2 rows with 3 columns each, the saved
 * configuration will look like this:
 *
 * [item->config_block]
 * item->config_item=2,3
 *
 * [item->config_item_0]
 * column_0=a
 * column_1=b
 * column_2=c
 *
 * [item->config_item_1]
 * column_0=1
 * column_1=2
 * column_2=3
 *
 * @param [in,out] xpp: The library main object.
 * @param [in,out] item: The item which is being saved.
 */
void ui_save_spreadsheet_item(struct xante_app *xpp, struct xante_item *item)
{
    int rows, i, columns;

    // First we get the number of form rows
    rows = get_form_number_of_rows(item);

    // Save each row
    for (i = 0; i < rows; i++)
        save_row(xpp, item, i);

    // Write the number of rows and columns
    columns = get_form_number_of_columns(item);
    cl_cfg_set_value(xpp->config.cfg_file,
                     cl_string_valueof(item->config_block),
                     cl_string_valueof(item->config_item),
                     "%d,%d", rows, columns);
}

int spreadsheet(session_t *session)
{
    struct xante_item *item = session->item;
    int ret_dialog = DLG_EXIT_OK;

    /* Prepares object content */
    build_session(session);

    ret_dialog = dlgx_spreadsheet(cl_string_valueof(item->name),
                                  cl_string_valueof(session->title),
                                  session->sheet);

    if (ret_dialog == DLG_EXIT_OK)
        session->result = get_current_result(item, session);

    return ret_dialog;
}

