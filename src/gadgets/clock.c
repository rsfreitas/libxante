
/*
 * Description: The clock gadget.
 *
 * Author: Rodrigo Freitas
 * Created at: Fri Dec 22 22:59:52 2017
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

#define GADGET_PREFIX           "__gadget_clock__"

static const char *clock_sijtf = "\
{\
    \"item\": {\
        \"name\": \"Curent clock\",\
        \"type\": \"update-object\",\
        \"data\": {\
            \"options\": \"Current system date/time:\"\
        },\
        \"ui\": {\
            \"labels\": {\
                \"buttons\": {\
                    \"ok\": \"Change\"\
                }\
            },\
            \"geometry\": {\
                \"height\": 8,\
                \"width\": 40\
            }\
        },\
        \"events\": {\
            \"update-routine\": \"xante:gadget_clock_current_time\",\
            \"item-selected\": \"xante:gadget_clock_item_selected\",\
            \"item-value-confirm\": \"xante:gadget_clock_item_value_confirm\",\
            \"item-value-updated\": \"xante:gadget_clock_item_value_updated\",\
            \"item-exit\": \"xante:gadget_clock_item_exit\"\
        }\
    }\
}";

static const char *input_sijtf = "\
{\
    \"item\": {\
        \"name\": \"Change curent date/time\",\
        \"type\": \"input-string\",\
        \"data\": {\
            \"options\": \"Change current date/time\",\
            \"default_value\": \"$current_datetime\",\
            \"ranges\": {\
                \"string_length\": 19\
            }\
        },\
        \"events\": {\
            \"item-selected\": \"xante:gadget_clock_input_selected\",\
            \"item-value-confirm\": \"xante:gadget_clock_input_confirmed\",\
            \"item-value-updated\": \"xante:gadget_clock_updated\"\
        }\
    }\
}";

/*
 *
 * Internal functions
 *
 */

/*
 * The function does what it says. The @read_only flag is just for us to use
 * a \t into the output string to look nice on the screen for the user.
 */
static int fill_item_with_current_datetime(struct xante_item *item,
    bool read_only)
{
    cl_datetime_t *dt = NULL;
    cl_string_t *s = NULL;

    if (NULL == item)
        return -1;

    dt = cl_dt_localtime();

    if (NULL == dt)
        return -1;

    s = cl_dt_to_cstring(dt, "%F %T");
    xante_item_update_value(item, "%s%s", (read_only ? "\t" : ""),
                            cl_string_valueof(s));

    cl_string_unref(s);
    cl_dt_destroy(dt);

    return 0;
}

/*
 * Event called right after a user chooses to change date/time by selecting
 * the Change button.
 *
 * Here we just put the current date/time inside the dialog so the user may
 * change it.
 */
static int gadget_clock_input_selected(struct xante_app *xpp __attribute__((unused)),
    struct xante_item *item __attribute__((unused)),
    void *data __attribute__((unused)))
{
    return fill_item_with_current_datetime(item, false);
}

/*
 * Event called in the input-string widget when the current date/time is
 * successfully validated.
 *
 * Here we update the system's date/time.
 */
static int gadget_clock_updated(struct xante_app *xpp __attribute__((unused)),
    struct xante_item *item __attribute__((unused)),
    void *data __attribute__((unused)))
{
    // TODO
    return 0;
}

/*
 * Event called to confirm the data/time value entered by the user inside the
 * input-string widget.
 */
static int gadget_clock_input_confirmed(struct xante_app *xpp __attribute__((unused)),
    struct xante_item *item, void *data)
{
    cl_datetime_t *dt;
    cl_string_t *date;

    date = CL_OBJECT_AS_CSTRING(data);
    dt = cl_dt_mktime_from_cstring(date);
    cl_string_unref(date);

    if (NULL == dt) {
        fill_item_with_current_datetime(item, false);
        return -1;
    }

    cl_dt_destroy(dt);
    return 0;
}

/*
 * Event called to retrieve current date/time informations from SO and to
 * fill the item's content with its value, which will be used to be displayed
 * on screen for the user.
 */
static int gadget_clock_current_time(struct xante_app *xpp __attribute__((unused)),
    struct xante_item *item, void *data __attribute__((unused)))
{
    return fill_item_with_current_datetime(item, true);
}

static int gadget_clock_item_selected(struct xante_app *xpp,
    struct xante_item *item __attribute__((unused)), void *data)
{
    gadget_dispatch_run_user_event(xpp, GADGET_PREFIX, EV_ITEM_SELECTED, data);
    return 0;
}

static int gadget_clock_item_value_confirm(struct xante_app *xpp,
    struct xante_item *item __attribute__((unused)), void *data)
{
    gadget_dispatch_run_user_event(xpp, GADGET_PREFIX, EV_ITEM_VALUE_CONFIRM,
                                   data);

    return 0;
}

static int gadget_clock_item_value_updated(struct xante_app *xpp,
    struct xante_item *item __attribute__((unused)), void *data)
{
    gadget_dispatch_run_user_event(xpp, GADGET_PREFIX, EV_ITEM_VALUE_UPDATED,
                                   data);

    return 0;
}

static int gadget_clock_item_exit(struct xante_app *xpp,
    struct xante_item *item __attribute__((unused)), void *data)
{
    gadget_dispatch_run_user_event(xpp, GADGET_PREFIX, EV_ITEM_EXIT, data);
    return 0;
}

/*
 *
 * Internal API
 *
 */

/*
 * The function to register inside the internal dispatch table all events called
 * from the clock-gadget.
 */
void gadget_clock_register(void)
{
    gadget_dispatch_add("gadget_clock_updated", gadget_clock_updated);
    gadget_dispatch_add("gadget_clock_current_time", gadget_clock_current_time);
    gadget_dispatch_add("gadget_clock_input_selected",
                        gadget_clock_input_selected);

    gadget_dispatch_add("gadget_clock_input_confirmed",
                        gadget_clock_input_confirmed);

    gadget_dispatch_add("gadget_clock_item_selected",
                        gadget_clock_item_selected);

    gadget_dispatch_add("gadget_clock_item_value_confirm",
                        gadget_clock_item_value_confirm);

    gadget_dispatch_add("gadget_clock_item_value_updated",
                        gadget_clock_item_value_updated);

    gadget_dispatch_add("gadget_clock_item_exit", gadget_clock_item_exit);
}

/*
 * The function to put the clock-gadget to run.
 */
int gadget_clock(session_t *session)
{
    enum xante_return_value ret_dialog;

    /*
     * TODO: Save the original object events so we can call them later. One way
     *       to do this is save the session->item event's into the dispatch
     *       table, so we can use it later.
     */
    gadget_dispatch_register_user_events(session, GADGET_PREFIX);
    ret_dialog = xante_manager_single_run(session->xpp, clock_sijtf);

    return (ret_dialog == XANTE_RETURN_OK) ? DLG_EXIT_OK : DLG_EXIT_CANCEL;
}

/*
 * This is a cleverness tatic to call another widget and let the user change
 * the current date/time. To keep a nice UI experience to the user we always
 * return false from this function, so we return to the previoius gadget.
 */
bool gadget_clock_validate_result(session_t *session)
{
    xante_manager_single_run(session->xpp, input_sijtf);

    /* We return false to prevent the clock gadget from being closed */
    return false;
}

