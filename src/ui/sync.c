
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Thu Nov 23 19:15:23 BRST 2017
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

/* The dialog size onto the screen */
#define PROGRESS_DIALOG_HEIGHT          7
#define PROGRESS_DIALOG_WIDTH           60

struct sync_thread {
    struct xante_app    *xpp;
    struct xante_item   *item;
    void                *data;
};

/*
 *
 * Internal functions
 *
 */

/*
 * This is the thread that updates the sync bar dialog.
 */
static void *make_sync(cl_thread_t *thread)
{
    struct sync_thread *sync = cl_thread_get_user_data(thread);
    struct xante_app *xpp = sync->xpp;
    struct xante_item *item = sync->item;
    int percent;

    cl_thread_set_state(thread, CL_THREAD_ST_CREATED);
    cl_thread_set_state(thread, CL_THREAD_ST_INITIALIZED);

    do {
        percent = event_update_routine(xpp, item, sync->data);
        dlgx_simple_sync(cl_string_valueof(item->name),
                             cl_string_valueof(item->options),
                             PROGRESS_DIALOG_HEIGHT, PROGRESS_DIALOG_WIDTH,
                             percent);

        cl_msleep(100);
    } while (((percent + 1) < 101) && (item->cancel_sync == false));

    return NULL;
}

/*
 *
 * Internal API
 *
 */

/**
 * @name ui_dialog_sync
 * @brief Creates a dialog of sync type.
 *
 * This dialog runs a task with a specific ending delimiter and shows a
 * sync bar to ilustrate the evolution of it.
 *
 * The \a item must have two events filled
 * @param [in] xpp: The main library object.
 * @param [in] item: The item to be used inside the dialog.
 *
 * @return Returns a ui_return_t value indicating if the item's value has been
 *         changed (true) or not (false) with the dialog selected button.
 */
ui_return_t ui_dialog_sync(struct xante_app *xpp, struct xante_item *item)
{
    ui_return_t ret;
    void *data;
    cl_thread_t *thread;
    struct sync_thread sync = {
        .xpp = xpp,
        .item = item
    };

    // run ev-update-routine-data
    //
    // creates the thread which will update the dialog
    //
    // wait for thread to end

    /* Assures that we will be able to, at least, start the sync */
    item->cancel_sync = false;
    data = event_update_routine_data(xpp, item);
    sync.data = data;
    thread = cl_thread_spawn(CL_THREAD_JOINABLE, make_sync, &sync);

    if (NULL == thread) {
    }

    if (cl_thread_wait_startup(thread) != 0) {
    }

    /* Since there is a thread-join here it will wait for the thread to end. */
    cl_thread_destroy(thread);

    return ret;
}

