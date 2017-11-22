
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

    xante_info("%s", __FUNCTION__);

    return 0;
}

CL_PLUGIN_OBJECT_PTR_ONLY(void, xapl_uninit)
{
    xante_event_arg_t *arg = CL_PLUGIN_PTR_ARGUMENT();
    (void)arg;

    xante_info("%s", __FUNCTION__);
}

CL_PLUGIN_OBJECT_PTR_ONLY(void, xapl_config_load)
{
    xante_event_arg_t *arg = CL_PLUGIN_PTR_ARGUMENT();
    (void)arg;

    xante_info("%s", __FUNCTION__);
}

CL_PLUGIN_OBJECT_PTR_ONLY(void, xapl_config_unload)
{
    xante_event_arg_t *arg = CL_PLUGIN_PTR_ARGUMENT();
    (void)arg;

    xante_info("%s", __FUNCTION__);
}

CL_PLUGIN_OBJECT_PTR_ONLY(int, xapl_changes_saved)
{
    xante_event_arg_t *arg = CL_PLUGIN_PTR_ARGUMENT();
    (void)arg;

    xante_info("%s", __FUNCTION__);
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

    xante_info("%s", __FUNCTION__);
    return 0;
}

CL_PLUGIN_OBJECT_PTR_ONLY(int, yesno_confirm)
{
    xante_event_arg_t *arg = CL_PLUGIN_PTR_ARGUMENT();
    (void)arg;

    xante_info("%s", __FUNCTION__);
    return 0;
}

CL_PLUGIN_OBJECT_PTR_ONLY(void, yesno_updated)
{
    xante_event_arg_t *arg = CL_PLUGIN_PTR_ARGUMENT();
    (void)arg;

    xante_info("%s", __FUNCTION__);
}

CL_PLUGIN_OBJECT_PTR_ONLY(void, yesno_exit)
{
    xante_event_arg_t *arg = CL_PLUGIN_PTR_ARGUMENT();
    (void)arg;

    xante_info("%s", __FUNCTION__);
}

