
/*
 * Description: Functions to handle user access control inside an application.
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

#include <sqlite3.h>
#include <openssl/sha.h>

#include "libxante.h"

#define ENV_XANTE_DB_PATH           "XANTE_DB_PATH"
#define XANTE_DB_FILENAME           "auth.xdb"

/* JSON objects */
#define APPLICATION                 "application"
#define NAME                        "name"
#define REVISION                    "revision"
#define VERSION                     "version"
#define UI                          "ui"
#define ITEMS                       "items"
#define OBJECT_ID                   "object_id"
#define TYPE                        "type"

/*
 *
 * Internal functions
 *
 */

static void release_row_data(void *a)
{
    cl_string_list_t *row = (cl_string_list_t *)a;

    if (NULL == row)
        return;

    cl_string_list_destroy(row);
}

static cl_list_t *db_query(sqlite3 *db, const char *query)
{
    sqlite3_stmt *stmt = NULL;
    cl_list_t *query_data = NULL;
    cl_string_list_t *row_data = NULL;
    cl_string_t *data = NULL;
    const unsigned char *db_data;
    int i;

    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK)
        return NULL;

    query_data = cl_list_create(release_row_data, NULL, NULL, NULL);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        row_data = cl_string_list_create();
        i = 0;

        while ((db_data = sqlite3_column_text(stmt, i)) != NULL) {
            data = cl_string_create("%s", db_data);
            cl_string_list_add(row_data, data);
            cl_string_unref(data);
            i++;
        }

        cl_list_unshift(query_data, row_data, -1);
    }

    sqlite3_finalize(stmt);

    /* Nothing found */
    if (cl_list_size(query_data) < 1) {
        cl_list_destroy(query_data);
        query_data = NULL;
    }

    return query_data;
}

static void create_user_table(sqlite3 *db)
{
    char *query = NULL;

    asprintf(&query, "CREATE TABLE IF NOT EXISTS 'user' ("
                     "'id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                     "'login' CHAR(128) NOT NULL, "
                     "'name' CHAR(128) NOT NULL, "
                     "'password' CHAR(256) NOT NULL)");

    db_query(db, query);
    free(query);
}

static void create_group_table(sqlite3 *db)
{
    char *query = NULL;

    asprintf(&query, "CREATE TABLE IF NOT EXISTS 'group' ("
                     "'id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                     "'name' CHAR(128) NOT NULL)");

    db_query(db, query);
    free(query);
}

static void create_group_user_table(sqlite3 *db)
{
    char *query = NULL;

    asprintf(&query, "CREATE TABLE IF NOT EXISTS 'group_user_rel' ("
                     "'id_user' INTEGER NOT NULL, "
                     "'id_group' INTEGER NOT NULL)");

    db_query(db, query);
    free(query);
}

static void create_application_table(sqlite3 *db)
{
    char *query = NULL;

    asprintf(&query, "CREATE TABLE IF NOT EXISTS 'application' ("
                     "'id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                     "'name' CHAR(128) NOT NULL)");

    db_query(db, query);
    free(query);
}

static void create_tool_table(sqlite3 *db)
{
    char *query = NULL, *tools[] = { "web-central", "xante-auth" };
    int i, t;

    asprintf(&query, "CREATE TABLE IF NOT EXISTS 'tool' ("
                     "'id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                     "'name' CHAR(256) NOT NULL)");

    db_query(db, query);
    free(query);
    t = sizeof(tools) / sizeof(tools[0]);

    for (i = 0; i < t; i++) {
        asprintf(&query, "INSERT INTO 'tool' (name) VALUES ('%s')",
                 tools[i]);

        db_query(db, query);
        free(query);
    }
}

static void create_origin_table(sqlite3 *db)
{
    char *query = NULL, *origins[] = { "local", "ssh-session" };
    int i, t;

    asprintf(&query, "CREATE TABLE IF NOT EXISTS 'origin' ("
                     "'id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                     "'name' CHAR(256) NOT NULL)");

    db_query(db, query);
    free(query);
    t = sizeof(origins) / sizeof(origins[0]);

    for (i = 0; i < t; i++) {
        asprintf(&query, "INSERT INTO 'origin' (name) VALUES ('%s')",
                 origins[i]);

        db_query(db, query);
        free(query);
    }
}

static void create_internal_tables(sqlite3 *db)
{
    char *query = NULL;

    /* Create tables with content */
    create_tool_table(db);
    create_origin_table(db);

    /* The database update history */
    asprintf(&query, "CREATE TABLE IF NOT EXISTS 'update_history' ("
                     "'id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                     "'update' DATETIME NOT NULL, "
                     "'id_tool' INTEGER NOT NULL)");

    db_query(db, query);
    free(query);

    /* The JXDBI files used to populate the database */
    asprintf(&query, "CREATE TABLE IF NOT EXISTS 'jxdbi_source' ("
                     "'id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                     "'version' INTEGER NOT NULL, "
                     "'filename' CHAR(256) NOT NULL, "
                     "'id_application' INTEGER NOT NULL)");

    db_query(db, query);
    free(query);

    /* Login history */
    asprintf(&query, "CREATE TABLE IF NOT EXISTS 'login_history' ("
                     "'id' INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                     "'id_user' INTEGER NOT NULL, "
                     "'id_origin' INTEGER NOT NULL, "
                     "'login' DATETIME NOT NULL, "
                     "'logout' DATETIME)");

    db_query(db, query);
    free(query);

    /* Active login */
    asprintf(&query, "CREATE TABLE IF NOT EXISTS 'active_login' ("
                     "'id_user' INTEGER NOT NULL, "
                     "'id_origin' INTEGER NOT NULL, "
                     "'login' DATETIME NOT NULL, "
                     "'logout' DATETIME)");

    db_query(db, query);
    free(query);
}

static int create_auth_tables(cl_string_t *db_filename)
{
    sqlite3 *db = NULL;

    if (sqlite3_open_v2(cl_string_valueof(db_filename), &db,
                        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                        NULL) != SQLITE_OK)
    {
        return -1;
    }

    if (NULL == db)
        return -1;

    /* user */
    create_user_table(db);

    /* group */
    create_group_table(db);

    /* group_user */
    create_group_user_table(db);

    /* application */
    create_application_table(db);

    /* internals */
    create_internal_tables(db);

    if (db != NULL)
        sqlite3_close(db);

    return 0;
}

static cl_string_t *auth_build_db_filename(const char *pathname)
{
    cl_string_t *db_pathname = NULL;
    char *env = NULL;

    if (NULL == pathname) {
        env = getenv(ENV_XANTE_DB_PATH);

        if (NULL == env) {
            errno_set(XANTE_ERROR_NO_ENV_DB_PATH);
            return NULL;
        }

        db_pathname = cl_string_create("%s/%s", env, XANTE_DB_FILENAME);
    } else
        db_pathname = cl_string_create("%s/%s", pathname, XANTE_DB_FILENAME);

    return db_pathname;
}

static bool auth_db_exists(cl_string_t *db_pathname)
{
    if (access(cl_string_valueof(db_pathname), 0x00) == -1) {
        errno_set(XANTE_ERROR_DB_ACCESS_FAILED);
        return false;
    }

    return true;
}

static void write_jxdbi_application_info(struct xante_app *xpp, cl_json_t *root)
{
    cl_json_t *application = NULL, *n = NULL;

    application = cl_json_create_object();

    if (NULL == application)
        return;

    n = cl_json_create_string(cl_string_valueof(xpp->info.application_name));
    cl_json_add_item_to_object(application, NAME, n);

    n = cl_json_create_string(cl_string_valueof(xpp->info.version));
    cl_json_add_item_to_object(application, VERSION, n);

    n = cl_json_create_number(xpp->info.revision);
    cl_json_add_item_to_object(application, REVISION, n);

    cl_json_add_item_to_object(root, APPLICATION, application);
}

static int write_jxdbi_item(cl_list_node_t *node, void *a)
{
    struct xante_item *item = cl_list_node_content(node);
    cl_json_t *array = (cl_json_t *)a;
    cl_json_t *i = NULL, *n = NULL;

    i = cl_json_create_object();

    if (NULL == i)
        return -1;

    n = cl_json_create_string(cl_string_valueof(item->name));
    cl_json_add_item_to_object(i, NAME, n);

    n = cl_json_create_string(cl_string_valueof(item->object_id));
    cl_json_add_item_to_object(i, OBJECT_ID, n);

    n = cl_json_create_string(cl_string_valueof(item->type));
    cl_json_add_item_to_object(i, TYPE, n);

    cl_json_add_item_to_array(array, i);

    return 0;
}

static int write_jxdbi_menu(cl_list_node_t *node, void *a)
{
    struct xante_menu *menu = cl_list_node_content(node);
    cl_json_t *ui = (cl_json_t *)a;
    cl_json_t *m = NULL, *n = NULL;

    m = cl_json_create_object();

    if (NULL == m)
        return -1;

    n = cl_json_create_string(cl_string_valueof(menu->name));
    cl_json_add_item_to_object(m, NAME, n);

    n = cl_json_create_string(cl_string_valueof(menu->object_id));
    cl_json_add_item_to_object(m, OBJECT_ID, n);

    /* items */
    n = cl_json_create_array();
    cl_list_map(menu->items, write_jxdbi_item, n);
    cl_json_add_item_to_object(m, ITEMS, n);

    cl_json_add_item_to_array(ui, m);

    return 0;
}

static void write_jxdbi_ui_info(struct xante_app *xpp, cl_json_t *root)
{
    cl_json_t *ui = NULL;

    ui = cl_json_create_array();

    if (NULL == ui)
        return;

    cl_list_map(xpp->ui.menus, write_jxdbi_menu, ui);
    cl_json_add_item_to_object(root, UI, ui);
}

static int create_jxdbi(struct xante_app *xpp, const char *filename)
{
    cl_json_t *root = NULL;
    int ret = -1;

    root = cl_json_create_object();

    if (NULL == root)
        goto end_block;

    write_jxdbi_application_info(xpp, root);
    write_jxdbi_ui_info(xpp, root);

    ret = cl_json_write_file(root, filename);
    cl_json_delete(root);

end_block:
    if (ret < 0)
        errno_set(XANTE_ERROR_UNABLE_TO_SAVE_JXDBI);

    return ret;
}

static char *password_to_db_password(const char *password)
{
    int i;
    unsigned char digest[SHA_DIGEST_LENGTH] = {0};
    char *hex_digest = NULL;
    cl_string_t *tmp;

    SHA1((const unsigned char *)password, strlen(password), digest);
    tmp = cl_string_create_empty(0);

    for (i = 0; i < SHA_DIGEST_LENGTH; i++)
        cl_string_cat(tmp, "%02x", digest[i]);

    cl_string_update_length(tmp);
    hex_digest = strdup(cl_string_valueof(tmp));
    cl_string_unref(tmp);

    return hex_digest;
}

static int get_group_id(struct xante_app *xpp, int id_user)
{
    char *query = NULL;
    int ret = -1;
    cl_list_t *db_data = NULL;
    cl_list_node_t *node = NULL;
    cl_string_list_t *row = NULL;
    cl_string_t *s = NULL;

    asprintf(&query, "SELECT id_group FROM 'group_user_rel' WHERE id_user = %d",
             id_user);

    db_data = db_query(xpp->auth.db, query);
    free(query);

    if (NULL == db_data) {
        errno_set(XANTE_ERROR_DB_GROUP_NOT_FOUND);
        return -1;
    }

    if (cl_list_size(db_data) == 1) {
       node = cl_list_at(db_data, 0);
       row = cl_list_node_content(node);
       s = cl_string_list_get(row, 0);
       xpp->auth.id_group = cl_string_to_int(s);
       cl_string_unref(s);
       cl_list_node_unref(node);

       /* Ok */
       ret = 0;
    } else
        errno_set(XANTE_ERROR_DB_MULTIPLE_GROUP_ENTRIES);

    cl_list_destroy(db_data);

    return ret;
}

static int validate_user_access(struct xante_app *xpp)
{
    char *query = NULL, *db_password = NULL;
    cl_list_t *db_data = NULL;
    cl_list_node_t *node = NULL;
    cl_string_list_t *row = NULL;
    cl_string_t *s = NULL;
    int id_user = -1, ret = -1;

    db_password = password_to_db_password(cl_string_valueof(xpp->auth.password));
    asprintf(&query, "SELECT name, id FROM 'user' WHERE login = '%s' AND "
                     "password = '%s'", cl_string_valueof(xpp->auth.username),
                     db_password);

    db_data = db_query(xpp->auth.db, query);
    free(db_password);
    free(query);

    if (NULL == db_data) {
        errno_set(XANTE_ERROR_DB_USER_NOT_FOUND);
        return -1;
    }

    /*
     * If we find the user we store its required informations, such as:
     *
     * - name (real name)
     * - group id
     */
    if (cl_list_size(db_data) == 1) {
        node = cl_list_at(db_data, 0);
        row = cl_list_node_content(node);
        s = cl_string_list_get(row, 0);

        /* -- name */
        xpp->auth.name = cl_string_dup(s);
        cl_string_unref(s);

        /* -- id */
        s = cl_string_list_get(row, 1);
        id_user = cl_string_to_int(s);
        cl_string_unref(s);
        cl_list_node_unref(node);

        /* Get groupd ID */
        if (get_group_id(xpp, id_user) == 0)
            ret = 0; /* Ok */
    } else
        errno_set(XANTE_ERROR_DB_MULTIPLE_USER_ENTRIES);

    cl_list_destroy(db_data);

    return ret;
}

static int validate_application_access_control(struct xante_app *xpp)
{
    char *query = NULL;
    cl_list_t *db_data = NULL;
    cl_list_node_t *node = NULL;
    cl_string_list_t *row = NULL;
    cl_string_t *name = NULL;
    int ret = -1;

    asprintf(&query, "SELECT id FROM 'application' WHERE name = '%s'",
             cl_string_valueof(xpp->info.application_name));

    db_data = db_query(xpp->auth.db, query);
    free(query);

    if (NULL == db_data) {
        errno_set(XANTE_ERROR_DB_APPLICATION_NOT_FOUND);
        return -1;
    }

    if (cl_list_size(db_data) == 1) {
        node = cl_list_at(db_data, 0);
        row = cl_list_node_content(node);
        name = cl_string_list_get(row, 0);

        /* Stores the application database ID */
        xpp->auth.id_application = cl_string_to_int(name);
        cl_string_unref(name);
        cl_list_node_unref(node);

        ret = 0;
    } else
        errno_set(XANTE_ERROR_DB_MULTIPLE_APPLICATION_ENTRIES);

    cl_list_destroy(db_data);

    return ret;
}

/*
 *
 * Internal API
 *
 */

/**
 * @name auth_init
 * @brief Starts the internal user access validation.
 *
 * @param [in,out] xpp: The main library object.
 * @param [in] use_auth: A boolean flag to indicate if we're going to use
 *                       user authentication or not.
 * @param [in] username: The username.
 * @param [in] password: The username password.
 *
 * @return On succcess returns 0 or -1 otherwise.
 */
int auth_init(struct xante_app *xpp, bool use_auth, const char *username,
    const char *password)
{
    cl_string_t *db_filename = NULL;
    int ret = -1;

    if (use_auth == false) {
        xante_runtime_set_user_authentication(xpp, false);
        return 0;
    }

    if ((NULL == username) || (NULL == password)) {
        /* TODO: Makes the user enter user/pass inside a dialog form */
    }

    xpp->auth.db = NULL;
    xpp->auth.username = cl_string_create("%s", username);
    xpp->auth.password = cl_string_create("%s", password);
    db_filename = auth_build_db_filename(NULL);

    if (NULL == db_filename)
        return -1;

    /* Checks if we have a database to validate */
    if (auth_db_exists(db_filename) == false)
        return -1;

    if (sqlite3_open_v2(cl_string_valueof(db_filename), &xpp->auth.db,
                        SQLITE_OPEN_READONLY, NULL) != SQLITE_OK)
    {
        errno_set(XANTE_ERROR_DB_OPEN);
        goto end_block;
    }

    if (NULL == xpp->auth.db) {
        errno_set(XANTE_ERROR_DB_OPEN);
        goto end_block;
    }

    /* Validate user/pass inside the DB */
    if (validate_user_access(xpp) < 0)
        goto end_block;

    /* Checks if this application has access control */
    if (validate_application_access_control(xpp) < 0)
        goto end_block;

    /* TODO: Add login information */

    ret = 0;

end_block:
    if ((ret < 0 ) && (xpp->auth.db != NULL))
        sqlite3_close(xpp->auth.db);

    if (db_filename != NULL)
        cl_string_unref(db_filename);

    return ret;
}

/**
 * @name auth_uninit
 * @brief Finishes the internal user validation.
 *
 * @param [in,out] xpp: The main library object.
 */
void auth_uninit(struct xante_app *xpp)
{
    /* TODO: Add logout information */
    /* TODO: Add history login information */

    if (xpp->auth.password != NULL)
        cl_string_unref(xpp->auth.password);

    if (xpp->auth.username != NULL)
        cl_string_unref(xpp->auth.username);

    if (xpp->auth.name != NULL)
        cl_string_unref(xpp->auth.name);

    if (xpp->auth.db != NULL)
        sqlite3_close(xpp->auth.db);
}

void auth_check_item_access(const struct xante_app *xpp,
    struct xante_item *item)
{
    if (xante_runtime_user_authentication((struct xante_app *)xpp) == false)
        return;
}

/*
 *
 * External API
 *
 */

__PUB_API__ int xante_auth_set_path(const char *pathname)
{
    errno_clear();

    if (NULL == pathname) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    setenv(ENV_XANTE_DB_PATH, pathname, 1);

    return 0;
}

__PUB_API__ int xante_auth_create_database(const char *pathname,
    bool overwrite)
{
    cl_string_t *db_filename = NULL;
    int ret = -1;

    errno_clear();

    if (NULL == pathname) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    db_filename = auth_build_db_filename(pathname);

    if (NULL == db_filename)
        return -1;

    if ((overwrite == true) && auth_db_exists(db_filename)) {
        /* TODO */
    }

    /* Create the database */
    ret = create_auth_tables(db_filename);

    if (db_filename != NULL)
        cl_string_unref(db_filename);

    return ret;
}

__PUB_API__ int xante_auth_export_jxdbi(xante_t *xpp, const char *filename)
{
    errno_clear();

    if ((NULL == xpp) || (NULL == filename)) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    return create_jxdbi(xpp, filename);
}

