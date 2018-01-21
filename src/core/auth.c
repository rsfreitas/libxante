
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

#include <unistd.h>
#include <utmp.h>

#include <sqlite3.h>
#include <openssl/sha.h>

#include "libxante.h"

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

struct column {
    char    *name;
    char    *type;
    bool    null;
    bool    primary_key;
};

struct table {
    char    *name;
    void    *columns;
    int     total_columns;
};

/*
 *
 * Internal functions
 *
 */

static void release_row_data(void *a)
{
    cl_stringlist_t *row = (cl_stringlist_t *)a;

    if (NULL == row)
        return;

    cl_stringlist_destroy(row);
}

static cl_list_t *db_query(sqlite3 *db, const char *query)
{
    sqlite3_stmt *stmt = NULL;
    cl_list_t *query_data = NULL;
    cl_stringlist_t *row_data = NULL;
    cl_string_t *data = NULL;
    const unsigned char *db_data;
    int i;

    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK)
        return NULL;

    query_data = cl_list_create(release_row_data, NULL, NULL, NULL);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        row_data = cl_stringlist_create();
        i = 0;

        while ((db_data = sqlite3_column_text(stmt, i)) != NULL) {
            data = cl_string_create("%s", db_data);
            cl_stringlist_add(row_data, data);
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

static void db_create_table(sqlite3 *db, const struct table *table)
{
    cl_string_t *query = NULL;
    struct column *column = NULL, *columns;
    int i;

    query = cl_string_create("CREATE TABLE IF NOT EXISTS '%s' (", table->name);

    for (i = 0; i < table->total_columns; i++) {
        columns = (struct column *)table->columns;
        column = &columns[i];
        cl_string_cat(query, "'%s' %s %s %s", column->name, column->type,
                      (column->primary_key == true) ? "PRIMARY KEY AUTOINCREMENT"
                                                    : "",
                      (column->null == false) ? "NOT NULL" : "");

        if (i < (table->total_columns - 1))
            cl_string_cat(query, ",");
    }

    cl_string_cat(query, ")");
    db_query(db, cl_string_valueof(query));
    cl_string_unref(query);
}

static void insert_auth_data(sqlite3 *db)
{
    struct column_data {
        char    *name;
        int     id;
    };

    cl_string_t *query = NULL;
    int i, j, t;
    struct column_data *column, *columns, tools[] = {
        { "web-central", 1 },
        { "xante-auth", 2  }
    }, sources[] = {
        { "localhost", XANTE_SESSION_LOCALHOST },
        { "ssh-session", XANTE_SESSION_SSH }
    }, session_type[] = {
        { "continuous", XANTE_SESSION_CONTINUOUS },
        { "single", XANTE_SESSION_SINGLE }
    }, levels[] = {
        { "editable", XanteAccessEdit },
        { "view-only", XanteAccessView },
        { "blocked", XanteAccessHidden }
    };

    struct table tables[] = {
        {
            .name = "tool",
            .columns = tools,
            .total_columns = sizeof(tools) / sizeof(tools[0])
        },

        {
            .name = "source",
            .columns = sources,
            .total_columns = sizeof(sources) / sizeof(sources[0])
        },

        {
            .name = "session_type",
            .columns = session_type,
            .total_columns = sizeof(session_type) / sizeof(session_type[0])
        },

        {
            .name = "level",
            .columns = levels,
            .total_columns = sizeof(levels) / sizeof(levels[0])
        }
    };

    t = sizeof(tables) / sizeof(tables[0]);

    for (i = 0; i < t; i++) {
        for (j = 0; j < tables[i].total_columns; j++) {
            columns = (struct column_data *)tables[i].columns;
            column = &columns[j];
            query = cl_string_create("INSERT INTO '%s' (id, name) VALUES "
                                     "(%d, '%s')", tables[i].name, column->id,
                                     column->name);

            db_query(db, cl_string_valueof(query));
            cl_string_unref(query);
        }
    }
}

static int create_auth_tables(cl_string_t *db_filename)
{
    sqlite3 *db = NULL;
    int i, t;

    struct column user[] = {
        { "id", "INTEGER", false, true },
        { "login", "CHAR(128)", false, false },
        { "name", "CHAR(128)", false, false },
        { "password", "CHAR(256)", false, false }
    };

    struct column common_pk[] = {
        { "id", "INTEGER", false, true },
        { "name", "CHAR(256)", false, false }
    };

    struct column common[] = {
        { "id", "INTEGER", false, false},
        { "name", "CHAR(256)", false, false }
    };

    struct column group_user[] = {
        { "id_user", "INTEGER", false, false },
        { "id_group", "INTEGER", false, false }
    };

    struct column jxdbi_source[] = {
        { "id", "INTEGER", false, true },
        { "version", "INTEGER", false, false },
        { "id_application", "INTEGER", false, false },
        { "filename", "CHAR(256)", false, false }
    };

    struct column update_history[] = {
        { "id", "INTEGER", false, true },
        { "id_tool", "INTEGER", false, false },
        { "update", "DATETIME", false, false }
    };

    struct column session_history[] = {
        { "id", "INTEGER", false, true },
        { "id_user", "INTEGER", false, false },
        { "id_source", "INTEGER", false, false },
        { "id_session_type", "INTEGER", false, false },
        { "login", "DATETIME", false, false },
        { "logout", "DATETIME", false, false }
    };

    struct column session[] = {
        { "id", "INTEGER", false, true },
        { "id_user", "INTEGER", false, false },
        { "id_source", "INTEGER", false, false },
        { "id_session_type", "INTEGER", false, false },
        { "login", "DATETIME", false, false },
        { "pid", "INTEGER", false, false }
    };

    struct column item_application[] = {
        { "id", "INTEGER", false, true },
        { "id_application", "INTEGER", false, false },
        { "name", "CHAR(256)", false, false },
        { "object_id", "CHAR(256)", false, false },
        { "type", "INTEGER", false, false }
    };

    struct column profile[] = {
        { "id_group", "INTEGER", false, false },
        { "id_item_application", "INTEGER", false, false },
        { "id_level", "INTEGER", false, false },
    };

    struct table tables[] = {
        /* user */
        {
            .name = "user",
            .columns = user,
            .total_columns = sizeof(user) / sizeof(user[0])
        },

        /* group */
        {
            .name = "group",
            .columns = common_pk,
            .total_columns = sizeof(common_pk) / sizeof(common_pk[0])
        },

        /* group_user_rel */
        {
            .name = "group_user_rel",
            .columns = group_user,
            .total_columns = sizeof(group_user) / sizeof(group_user[0])
        },

        /* application */
        {
            .name = "application",
            .columns = common_pk,
            .total_columns = sizeof(common_pk) / sizeof(common_pk[0])
        },

        /* tool */
        {
            .name = "tool",
            .columns = common,
            .total_columns = sizeof(common) / sizeof(common[0])
        },

        /* source */
        {
            .name = "source",
            .columns = common,
            .total_columns = sizeof(common) / sizeof(common[0])
        },

        /* session_type */
        {
            .name = "session_type",
            .columns = common,
            .total_columns = sizeof(common) / sizeof(common[0])
        },

        /* update_history */
        {
            .name = "update_history",
            .columns = update_history,
            .total_columns = sizeof(update_history) / sizeof(update_history[0])
        },

        /* jxdbi_source */
        {
            .name = "jxdbi_source",
            .columns = jxdbi_source,
            .total_columns = sizeof(jxdbi_source) / sizeof(jxdbi_source[0])
        },

        /* session_history */
        {
            .name = "session_history",
            .columns = session_history,
            .total_columns = sizeof(session_history) / sizeof(session_history[0])
        },

        /* session */
        {
            .name = "session",
            .columns = session,
            .total_columns = sizeof(session) / sizeof(session[0])
        },

        /* level */
        {
            .name = "level",
            .columns = common,
            .total_columns = sizeof(common) / sizeof(common[0])
        },

        /* application items */
        {
            .name = "item_application",
            .columns = item_application,
            .total_columns = sizeof(item_application) / sizeof(item_application[0])
        },

        /* profile */
        {
            .name = "profile",
            .columns = profile,
            .total_columns = sizeof(profile) / sizeof(profile[0])
        }
    };

    if (sqlite3_open_v2(cl_string_valueof(db_filename), &db,
                        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                        NULL) != SQLITE_OK)
    {
        return -1;
    }

    if (NULL == db)
        return -1;

    /* create tables */
    t = sizeof(tables) / sizeof(tables[0]);

    for (i = 0; i < t; i++)
        db_create_table(db, &tables[i]);

    insert_auth_data(db);

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
    if (file_exists(cl_string_valueof(db_pathname)) == false) {
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

    n = cl_json_create_string(xpp->info.application_name);
    cl_json_add_item_to_object(application, NAME, n);

    n = cl_json_create_string(xpp->info.version);
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
    cl_stringlist_t *row = NULL;
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
       s = cl_stringlist_get(row, 0);
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
    cl_stringlist_t *row = NULL;
    cl_string_t *s = NULL;
    int ret = -1;

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
        s = cl_stringlist_get(row, 0);

        /* -- name */
        xpp->auth.name = cl_string_dup(s);
        cl_string_unref(s);

        /* -- id */
        s = cl_stringlist_get(row, 1);
        xpp->auth.id_user = cl_string_to_int(s);
        cl_string_unref(s);
        cl_list_node_unref(node);

        /* Get groupd ID */
        if (get_group_id(xpp, xpp->auth.id_user) == 0)
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
    cl_stringlist_t *row = NULL;
    cl_string_t *name = NULL;
    int ret = -1;

    asprintf(&query, "SELECT id FROM 'application' WHERE name = '%s'",
             xpp->info.application_name);

    db_data = db_query(xpp->auth.db, query);
    free(query);

    if (NULL == db_data) {
        errno_set(XANTE_ERROR_DB_APPLICATION_NOT_FOUND);
        return -1;
    }

    if (cl_list_size(db_data) == 1) {
        node = cl_list_at(db_data, 0);
        row = cl_list_node_content(node);
        name = cl_stringlist_get(row, 0);

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

static int get_session_source(struct xante_app *xpp)
{
    char login[32] = {0}, tty[128] = {0}, *tmp;
    struct utmp *u = NULL;

    xpp->auth.session_pid = -1;
    getlogin_r(login, sizeof(login) - 1);

    if (ttyname_r(STDIN_FILENO, tty, sizeof(tty) - 1) != 0)
        return -1;

    tmp = tty + strlen("/dev/");
    setutent();
    u = getutent();

    while (u != NULL) {
        if ((u->ut_type == USER_PROCESS) &&
            (strcmp(u->ut_user, login) == 0) &&
            (strcmp(u->ut_line, tmp) == 0))
        {
            xpp->auth.session_pid = u->ut_pid;

            if (strlen(u->ut_host) > 2) {
                xpp->auth.session_source = XANTE_SESSION_SSH;
                xpp->auth.source_description = cl_string_create("%s",
                                                                u->ut_host);
            } else {
                xpp->auth.session_source = XANTE_SESSION_LOCALHOST;
                xpp->auth.source_description = cl_string_create("localhost");
            }

            break;
        }

        u = getutent();
    }

    endutent();

    return 0;
}

static cl_list_t *get_active_session(struct xante_app *xpp,
    enum xante_session type)
{
    char *query = NULL;
    cl_list_t *session = NULL;

    asprintf(&query, "SELECT id, login FROM 'session' WHERE id_user = %d AND "
                     "id_session_type = %d", xpp->auth.id_user, type);

    session = db_query(xpp->auth.db, query);
    free(query);

    return session;
}

static void close_session(struct xante_app *xpp, cl_list_t *session)
{
    cl_list_node_t *node = NULL;
    cl_stringlist_t *row = NULL;
    cl_datetime_t *dt = NULL;
    cl_string_t *dt_str, *data = NULL;
    char *query = NULL;

    if ((NULL == session) || (cl_list_size(session) > 1))
        return;

    node = cl_list_at(session, 0);
    row = cl_list_node_content(node);
    data = cl_stringlist_get(row, 1);
    dt = cl_dt_localtime();
    dt_str = cl_dt_to_cstring(dt, "%F %T");
    asprintf(&query, "INSERT INTO 'session_history' (id_user, id_session_type, "
                     "id_source, login, logout) VALUES (%d, %d, %d, '%s', '%s')",
                     xpp->auth.id_user, xpp->auth.session_type,
                     xpp->auth.session_source, cl_string_valueof(data),
                     cl_string_valueof(dt_str));

    db_query(xpp->auth.db, query);
    cl_string_unref(data);
    free(query);

    data = cl_stringlist_get(row, 0);
    asprintf(&query, "DELETE FROM 'session' WHERE id = %d",
             cl_string_to_int(data));

    db_query(xpp->auth.db, query);
    cl_string_unref(data);
    free(query);

    cl_string_unref(dt_str);
    cl_dt_destroy(dt);
    cl_list_node_unref(node);
}

static void add_active_session(struct xante_app *xpp, enum xante_session type)
{
    cl_datetime_t *dt = NULL;
    cl_string_t *dt_str = NULL;
    char *query = NULL;

    dt = cl_dt_localtime();
    dt_str = cl_dt_to_cstring(dt, "%F %T");
    asprintf(&query, "INSERT INTO 'session' (id_user, id_session_type, "
                     "id_source, login, pid) VALUES (%d, %d, %d, '%s', %d)",
                     xpp->auth.id_user, type, xpp->auth.session_source,
                     cl_string_valueof(dt_str), xpp->auth.session_pid);

    db_query(xpp->auth.db, query);
    free(query);
    cl_string_unref(dt_str);
    cl_dt_destroy(dt);
}

static void continuous_login(struct xante_app *xpp)
{
    cl_list_t *val = NULL;

    val = get_active_session(xpp, XANTE_SESSION_CONTINUOUS);

    if (NULL == val) {
        /* We don't have an active session. Create one... */
        add_active_session(xpp, XANTE_SESSION_CONTINUOUS);
        return;
    }

    /* We do have an active session. Life goes on... */
    cl_list_destroy(val);
}

static void single_login(struct xante_app *xpp)
{
    cl_list_t *session;

    session = get_active_session(xpp, XANTE_SESSION_SINGLE);

    if (session != NULL) {
        close_session(xpp, session);
        cl_list_destroy(session);
    }

    /* Add a new session */
    add_active_session(xpp, XANTE_SESSION_SINGLE);
}

static int auth_login(struct xante_app *xpp)
{
    if (get_session_source(xpp) < 0) {
        errno_set(XANTE_ERROR_UNABLE_TO_RETRIEVE_LOGIN_INFO);
        return -1;
    }

    switch (xpp->auth.session_type) {
        case XANTE_SESSION_CONTINUOUS:
            continuous_login(xpp);
            break;

        case XANTE_SESSION_SINGLE:
            single_login(xpp);
            break;
    }

    xpp->auth.login_and_source =
            cl_string_create("%s@%s", cl_string_valueof(xpp->auth.username),
                             cl_string_valueof(xpp->auth.source_description));

    return 0;
}

static void auth_logout(struct xante_app *xpp)
{
    cl_list_t *session;

    session = get_active_session(xpp, xpp->auth.session_type);

    if (session != NULL) {
        close_session(xpp, session);
        cl_list_destroy(session);
    }
}

static int get_item_access_level(const struct xante_app *xpp,
    const struct xante_item *item)
{
    char *query = NULL;
    cl_list_t *val = NULL;
    cl_list_node_t *node = NULL;
    cl_stringlist_t *row = NULL;
    cl_string_t *db_level = NULL;
    int level = XanteAccessEdit;

    if (NULL == item->object_id)
        return level;

    asprintf(&query, "SELECT id_level FROM 'profile' WHERE id_group = %d AND "
                     "id_item_application IN (SELECT id FROM item_application "
                     "WHERE id_application = %d AND object_id = '%s')",
                     xpp->auth.id_group, xpp->auth.id_application,
                     cl_string_valueof(item->object_id));

    val = db_query(xpp->auth.db, query);
    free(query);

    if ((NULL == val) || (cl_list_size(val) > 1))
        goto end_block;

    node = cl_list_at(val, 0);
    row = cl_list_node_content(node);
    db_level = cl_stringlist_get(row, 0);
    level = cl_string_to_int(db_level);
    cl_string_unref(db_level);
    cl_list_node_unref(node);

end_block:
    if (val != NULL)
        cl_list_destroy(val);

    return level;
}

static int update_item_access(cl_list_node_t *node, void *a)
{
    struct xante_item *item = cl_list_node_content(node);
    struct xante_app *xpp = (struct xante_app *)a;

    item->mode = auth_get_access_level(xpp, item);

    return 0;
}

static int update_menu_access(cl_list_node_t *node, void *a)
{
    struct xante_menu *menu = cl_list_node_content(node);

    cl_list_map(menu->items, update_item_access, a);

    return 0;
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
 * @param [in] session: The session type.
 * @param [in] username: The username.
 * @param [in] password: The username password.
 *
 * @return On succcess returns 0 or -1 otherwise.
 */
int auth_init(struct xante_app *xpp, bool use_auth, enum xante_session session,
    const char *username, const char *password)
{
    cl_string_t *db_filename = NULL;
    int ret = -1;

    if (use_auth == false) {
        runtime_set_user_authentication(xpp, false);
        return 0;
    }

    if ((NULL == username) || (NULL == password)) {
        /* TODO: Makes the user enter user/pass inside a gadget */
    }

    xpp->auth.db = NULL;
    xpp->auth.session_type = session;
    xpp->auth.username = cl_string_create("%s", username);
    xpp->auth.password = cl_string_create("%s", password);
    db_filename = auth_build_db_filename(NULL);

    if (NULL == db_filename)
        return -1;

    /* Checks if we have a database to validate */
    if (auth_db_exists(db_filename) == false)
        return -1;

    if (sqlite3_open_v2(cl_string_valueof(db_filename), &xpp->auth.db,
                        SQLITE_OPEN_READWRITE, NULL) != SQLITE_OK)
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

    if (auth_login(xpp) < 0)
        goto end_block;

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
    auth_logout(xpp);

    if (xpp->auth.password != NULL)
        cl_string_unref(xpp->auth.password);

    if (xpp->auth.username != NULL)
        cl_string_unref(xpp->auth.username);

    if (xpp->auth.name != NULL)
        cl_string_unref(xpp->auth.name);

    if (xpp->auth.source_description != NULL)
        cl_string_unref(xpp->auth.source_description);

    if (xpp->auth.login_and_source != NULL)
        cl_string_unref(xpp->auth.login_and_source);

    if (xpp->auth.db != NULL)
        sqlite3_close(xpp->auth.db);
}

/**
 * @name auth_application_init
 * @brief Checks if the current application has database access control.
 *
 * If the application have access control, every item has its access mode
 * updated.
 *
 * @param [in,out] xpp: The library main object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int auth_application_init(struct xante_app *xpp)
{
    /* We're running without authentication */
    if (xante_runtime_user_authentication(xpp) == false)
        return 0;

    /* Checks if this application has access control */
    if (validate_application_access_control(xpp) < 0)
        return -1;

    cl_list_map(xpp->ui.menus, update_menu_access, xpp);

    return 0;
}

/**
 * @name auth_get_access_level
 * @brief Gets the current user level access to a specific UI item.
 *
 * @param [in] xpp: The library main object.
 * @param [in] item: The item to be validated.
 *
 * @returns Returns the access level.
 */
enum XanteAccessMode auth_get_access_level(const struct xante_app *xpp,
    const struct xante_item *item)
{
    int mode = -1;

    /* We're running without authentication */
    if (xante_runtime_user_authentication((struct xante_app *)xpp) == false)
        return XanteAccessEdit;

    mode = get_item_access_level(xpp, item);

    if (mode < 0)
        mode = XanteAccessEdit;

    return mode;
}

/**
 * @name auth_check_item_access
 * @brief Checks if the current user access to a specific UI item.
 *
 * @param [in] xpp: The library main object.
 * @param [in] item: The item to be validated.
 *
 * @returns Returns true if the current user can access (view/edit) the item or
 *          false if the item is blocked.
 */
bool auth_check_item_access(const struct xante_app *xpp,
    const struct xante_item *item)
{
    enum XanteAccessMode mode;

    mode = auth_get_access_level(xpp, item);

    if (mode == XanteAccessHidden)
        return false;

    return true;
}

/*
 *
 * API
 *
 */

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

    if (auth_db_exists(db_filename)) {
        if (overwrite == false) {
            errno_set(XANTE_ERROR_DB_EXISTS);
            return -1;
        }

        remove(cl_string_valueof(db_filename));
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

