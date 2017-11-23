
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Wed Nov 22 21:20:02 2017
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

struct progress_thread {
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
 * This is the thread that updates the progress bar dialog.
 */
static void *make_progress(cl_thread_t *thread)
{
    struct progress_thread *progress = cl_thread_get_user_data(thread);
    struct xante_app *xpp = progress->xpp;
    struct xante_item *item = progress->item;
    int percent;

    cl_thread_set_state(thread, CL_THREAD_ST_CREATED);
    cl_thread_set_state(thread, CL_THREAD_ST_INITIALIZED);

    do {
        percent = event_update_routine(xpp, item, progress->data);
        dlgx_simple_progress(cl_string_valueof(item->name),
                             cl_string_valueof(item->options),
                             PROGRESS_DIALOG_HEIGHT, PROGRESS_DIALOG_WIDTH,
                             percent);

        cl_msleep(100);
    } while (((percent + 1) < 101) && (item->cancel_update == false));

    return NULL;
}

/*
 *
 * Internal API
 *
 */

/**
 * @name ui_dialog_progress
 * @brief Creates a dialog of progress type.
 *
 * This dialog runs a task with a specific ending delimiter and shows a
 * progress bar to ilustrate the evolution of it.
 *
 * The \a item must have two events filled:
 *
 * EV_UPDATE_ROUTINE_DATA: which must return a pointer to a specific data
 *                         which will passed to the update-routine.
 *
 * EV_UPDATE_ROUTINE: which must return the new stage to be updated to the
 *                    user.
 *
 * Anytime the user wants this routine may be cancelled by calling the
 * function 'xante_item_cancel_update'.
 *
 * @param [in] xpp: The main library object.
 * @param [in] item: The item to be used inside the dialog.
 *
 * @return Returns a ui_return_t value indicating if the item's value has been
 *         changed (true) or not (false) with the dialog selected button.
 */
ui_return_t ui_dialog_progress(struct xante_app *xpp, struct xante_item *item)
{
    ui_return_t ret;
    void *data;
    cl_thread_t *thread;
    struct progress_thread progress = {
        .xpp = xpp,
        .item = item
    };

    /* Assures that we will be able to, at least, start the progress */
    item->cancel_update = false;
    data = event_update_routine_data(xpp, item);
    progress.data = data;
    thread = cl_thread_spawn(CL_THREAD_JOINABLE, make_progress, &progress);

    if (NULL == thread) {
    }

    if (cl_thread_wait_startup(thread) != 0) {
    }

    /* Since there is a thread-join here it will wait for the thread to end. */
    cl_thread_destroy(thread);

    return ret;
}

