
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sun May 21 12:31:50 2017
 * Project: main.c
 *
 * Copyright (c) 2017 All rights reserved
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

#include <string.h>
#include <unistd.h>

#include <collections.h>
#include <libxante.h>

#define API     "{\
    \"API\": [\
        { \"name\": \"xapl_init\", \"return_type\": \"int\",\
            \"arguments\": [\
                { \"name\": \"xpp_arg\", \"type\": \"pointer\" }\
            ]\
        },\
        { \"name\": \"xapl_uninit\", \"return_type\": \"void\",\
            \"arguments\": [\
                { \"name\": \"xpp_arg\", \"type\": \"pointer\" }\
            ]\
        },\
        { \"name\": \"xapl_config_load\", \"return_type\": \"void\",\
            \"arguments\": [\
                { \"name\": \"xpp_arg\", \"type\": \"pointer\" }\
            ]\
        },\
        { \"name\": \"xapl_config_unload\", \"return_type\": \"void\",\
            \"arguments\": [\
                { \"name\": \"xpp_arg\", \"type\": \"pointer\" }\
            ]\
        },\
        { \"name\": \"xapl_changes_saved\", \"return_type\": \"int\",\
            \"arguments\": [\
                { \"name\": \"xpp_arg\", \"type\": \"pointer\" }\
            ]\
        },\
        { \"name\": \"yesno_selected\", \"return_type\": \"int\",\
            \"arguments\": [\
                { \"name\": \"xpp_arg\", \"type\": \"pointer\" }\
            ]\
        },\
        { \"name\": \"yesno_confirm\", \"return_type\": \"int\",\
            \"arguments\": [\
                { \"name\": \"xpp_arg\", \"type\": \"pointer\" }\
            ]\
        },\
        { \"name\": \"yesno_updated\", \"return_type\": \"void\",\
            \"arguments\": [\
                { \"name\": \"xpp_arg\", \"type\": \"pointer\" }\
            ]\
        },\
        { \"name\": \"yesno_exit\", \"return_type\": \"void\",\
            \"arguments\": [\
                { \"name\": \"xpp_arg\", \"type\": \"pointer\" }\
            ]\
        }\
    ]\
}"

CL_PLUGIN_INIT()
{
    return 0;
}

CL_PLUGIN_UNINIT()
{
}

CL_PLUGIN_SET_INFO(
    test,
    "0.1",
    "Rodrigo Freitas",
    "Libxante's config-test module",
    API
)

/**
 *
 * Mandatory API
 *
 */

CL_PLUGIN_OBJECT_PTR_ONLY(int, xapl_init)
{
    xante_event_arg_t *arg = CL_PLUGIN_PTR_ARGUMENT();
    (void)arg;

    xante_log_info("%s", __FUNCTION__);

    return 0;
}

CL_PLUGIN_OBJECT_PTR_ONLY(void, xapl_uninit)
{
    xante_event_arg_t *arg = CL_PLUGIN_PTR_ARGUMENT();
    (void)arg;

    xante_log_info("%s", __FUNCTION__);
}

CL_PLUGIN_OBJECT_PTR_ONLY(void, xapl_config_load)
{
    xante_event_arg_t *arg = CL_PLUGIN_PTR_ARGUMENT();
    (void)arg;

    xante_log_info("%s", __FUNCTION__);
}

CL_PLUGIN_OBJECT_PTR_ONLY(void, xapl_config_unload)
{
    xante_event_arg_t *arg = CL_PLUGIN_PTR_ARGUMENT();
    (void)arg;

    xante_log_info("%s", __FUNCTION__);
}

CL_PLUGIN_OBJECT_PTR_ONLY(int, xapl_changes_saved)
{
    xante_event_arg_t *arg = CL_PLUGIN_PTR_ARGUMENT();
    (void)arg;

    xante_log_info("%s", __FUNCTION__);
    return 0;
}

/*
 *
 * Yesno events
 *
 */

CL_PLUGIN_OBJECT_PTR_ONLY(int, yesno_selected)
{
    xante_event_arg_t *arg = CL_PLUGIN_PTR_ARGUMENT();
    (void)arg;

    xante_log_info("%s", __FUNCTION__);
    return 0;
}

CL_PLUGIN_OBJECT_PTR_ONLY(int, yesno_confirm)
{
    xante_event_arg_t *arg = CL_PLUGIN_PTR_ARGUMENT();
    (void)arg;

    xante_log_info("%s", __FUNCTION__);
    return 0;
}

CL_PLUGIN_OBJECT_PTR_ONLY(void, yesno_updated)
{
    xante_event_arg_t *arg = CL_PLUGIN_PTR_ARGUMENT();
    (void)arg;

    xante_log_info("%s", __FUNCTION__);
}

CL_PLUGIN_OBJECT_PTR_ONLY(void, yesno_exit)
{
    xante_event_arg_t *arg = CL_PLUGIN_PTR_ARGUMENT();
    (void)arg;

    xante_log_info("%s", __FUNCTION__);
}

CL_PLUGIN_OBJECT_PTR_ONLY(int, foo_custom)
{
    xante_event_arg_t *arg = CL_PLUGIN_PTR_ARGUMENT();
    xante_t *xpp = xante_event_argument(arg, XANTE_EVENT_DATA_XANTE_T);
    xante_item_t *item = xante_event_argument(arg, XANTE_EVENT_DATA_XANTE_ITEM_T);

    xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, __FUNCTION__,
                         "Custom event name: %s", xante_item_name(item));

    return 0;
}

struct progress_data {
    int x;
    int y;
};

static int percent = 0;

static struct progress_data pb = {
    .x = 42,
    .y = 10001,
};

CL_PLUGIN_OBJECT_PTR_ONLY(void, *pb_data)
{
    xante_event_arg_t *arg = CL_PLUGIN_PTR_ARGUMENT();
    (void)arg;

    percent = 0;

    return &pb;
}

CL_PLUGIN_OBJECT_PTR_ONLY(int, progress)
{
    xante_event_arg_t *arg = CL_PLUGIN_PTR_ARGUMENT();
    struct progress_data *p = (struct progress_data *)xante_event_argument(arg, XANTE_EVENT_DATA_CUSTOM);

    sleep(1);
    percent += 10;
    xante_log_info("%s: %d-%d", __FUNCTION__, p->x, p->y);

    return percent;
}

CL_PLUGIN_OBJECT_PTR_ONLY(bool, foo_sync)
{
    xante_event_arg_t *arg = CL_PLUGIN_PTR_ARGUMENT();
    xante_item_t *item = xante_event_argument(arg, XANTE_EVENT_DATA_XANTE_ITEM_T);

    percent += 5;
    xante_log_info("%s: percent = %d", __FUNCTION__, percent);

    if (percent > 100) {
        xante_log_info("%s: lock for 20 seconds", __FUNCTION__);
        sleep(20);
        return false;
    }

    sleep(1);
    xante_item_update_value(item, ": percent = %d", percent);
    return true;
}

CL_PLUGIN_OBJECT_PTR_ONLY(int, clock_module)
{
    xante_event_arg_t *arg = CL_PLUGIN_PTR_ARGUMENT();
    xante_item_t *item = xante_event_argument(arg, XANTE_EVENT_DATA_XANTE_ITEM_T);

    cl_datetime_t *dt;
    cl_string_t *s = NULL;

    dt = cl_dt_localtime();
    s = cl_dt_to_cstring(dt, "%F %T");
    cl_dt_destroy(dt);

    xante_item_update_value(item, "%s", cl_string_valueof(s));
    cl_string_unref(s);

    return 0;
}

CL_PLUGIN_OBJECT_PTR_ONLY(void, *scroll_content)
{
    xante_event_arg_t *arg = CL_PLUGIN_PTR_ARGUMENT();
    char *content = NULL;

    (void)arg;
    asprintf(&content, "This is the content\nwhich the module just\ncreated.\n\n\nAnd this is to show\nthat we can\nscroll the content.\nTo a\nfew\nmore\nlines.");

    return content;
}

CL_PLUGIN_OBJECT_PTR_ONLY(int, scroll_strlen)
{
    xante_event_arg_t *arg = CL_PLUGIN_PTR_ARGUMENT();
    char *input = (char *)xante_event_argument(arg, XANTE_EVENT_DATA_CUSTOM);

    xante_log_info("%s: input = '%s'", __FUNCTION__, input);
    return strlen(input);
}

CL_PLUGIN_OBJECT_PTR_ONLY(int, scroll_check)
{
    xante_event_arg_t *arg = CL_PLUGIN_PTR_ARGUMENT();
    char *input = (char *)xante_event_argument(arg, XANTE_EVENT_DATA_CUSTOM);

    xante_log_info("%s: input = '%s'", __FUNCTION__, input);

    return 1; /* ok */
}
