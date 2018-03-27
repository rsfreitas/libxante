
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sun May 21 12:31:50 2017
 * Project: C plugin example
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

#include <collections/collections.h>
#include <xante/libxante.h>

CL_PLUGIN_INIT()
{
    return 0;
}

CL_PLUGIN_UNINIT()
{
}

CL_PLUGIN_SET_INFO(
    "C Plugin",
    "0.1",
    "Rodrigo Freitas",
    "Libxante's config-test module"
)

/**
 *
 * Mandatory API
 *
 */

CL_PLUGIN_FUNCTION(int, xapl_init)
{
    CL_PLUGIN_IGNORE_ARGUMENTS();

    xante_log_info("%s", __FUNCTION__);
    return 0;
}

CL_PLUGIN_FUNCTION(void, xapl_uninit)
{
    CL_PLUGIN_IGNORE_ARGUMENTS();

    xante_log_info("%s", __FUNCTION__);
}

CL_PLUGIN_FUNCTION(void, xapl_config_load)
{
    CL_PLUGIN_IGNORE_ARGUMENTS();

    xante_log_info("%s", __FUNCTION__);
}

CL_PLUGIN_FUNCTION(void, xapl_config_unload)
{
    CL_PLUGIN_IGNORE_ARGUMENTS();

    xante_log_info("%s", __FUNCTION__);
}

CL_PLUGIN_FUNCTION(int, xapl_changes_saved)
{
    CL_PLUGIN_IGNORE_ARGUMENTS();

    xante_log_info("%s", __FUNCTION__);
    return 0;
}

/*
 *
 * Yesno events
 *
 */

CL_PLUGIN_FUNCTION(int, yesno_selected)
{
    CL_PLUGIN_IGNORE_ARGUMENTS();

    xante_log_info("%s", __FUNCTION__);
    return 0;
}

CL_PLUGIN_FUNCTION(int, yesno_confirm)
{
    CL_PLUGIN_IGNORE_ARGUMENTS();

    xante_log_info("%s", __FUNCTION__);
    return 0;
}

CL_PLUGIN_FUNCTION(void, yesno_updated)
{
    CL_PLUGIN_IGNORE_ARGUMENTS();

    xante_log_info("%s", __FUNCTION__);
}

CL_PLUGIN_FUNCTION(void, yesno_exit)
{
    CL_PLUGIN_IGNORE_ARGUMENTS();

    xante_log_info("%s", __FUNCTION__);
}

CL_PLUGIN_FUNCTION(int, foo_custom)
{
    xante_t *xpp = NULL;
    xante_item_t *item = NULL;

    xpp = xante_event_argument(XANTE_ARG_XANTE_APP);
    item = xante_event_argument(XANTE_ARG_XANTE_ITEM);
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

CL_PLUGIN_FUNCTION(void, *pb_data)
{
    CL_PLUGIN_IGNORE_ARGUMENTS();

    percent = 0;

    return &pb;
}

CL_PLUGIN_FUNCTION(int, progress)
{
    struct progress_data *p = NULL;

    p = xante_event_argument(XANTE_ARG_DATA);
    sleep(1);
    percent += 10;
    xante_log_info("%s: %d-%d", __FUNCTION__, p->x, p->y);

    return percent;
}

CL_PLUGIN_FUNCTION(bool, foo_sync)
{
    xante_item_t *item = NULL;

    item = xante_event_argument(XANTE_ARG_XANTE_ITEM);

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

CL_PLUGIN_FUNCTION(int, clock_module)
{
    xante_item_t *item = NULL;

    item = xante_event_argument(XANTE_ARG_XANTE_ITEM);

    cl_datetime_t *dt;
    cl_string_t *s = NULL;

    dt = cl_dt_localtime();
    s = cl_dt_to_cstring(dt, "%F %T");
    cl_dt_destroy(dt);

    xante_item_update_value(item, "%s", cl_string_valueof(s));
    cl_string_unref(s);

    return 0;
}

CL_PLUGIN_FUNCTION(void, *scroll_content)
{
    CL_PLUGIN_IGNORE_ARGUMENTS();
    char *content = NULL;

    asprintf(&content, "This is the content\n"
                       "which the module just\n"
                       "created.\n\n\n"
                       "And this is to show\n"
                       "that we can\n"
                       "scroll the content.\n"
                       "To a\n"
                       "few\n"
                       "more\n"
                       "lines.");

    return content;
}

CL_PLUGIN_FUNCTION(int, scroll_strlen)
{
    char *input = NULL;

    input = xante_event_argument(XANTE_ARG_DATA);
    xante_log_info("%s: input = '%s'", __FUNCTION__, input);

    return strlen(input);
}

CL_PLUGIN_FUNCTION(int, scroll_check)
{
    char *input = NULL;

    input = xante_event_argument(XANTE_ARG_DATA);
    xante_log_info("%s: input = '%s'", __FUNCTION__, input);

    return 1; /* ok */
}

