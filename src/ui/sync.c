
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

#define DEFAULT_STATUSBAR_TEXT      \
    "A task is running in background. Wait for it to end."

/* The dialog size onto the screen */
#define DIALOG_HEIGHT               5
#define DIALOG_WIDTH                60

struct sync_thread {
    struct xante_app    *xpp;
    struct xante_item   *item;
    void                *data;
    cl_timeout_t        *tm_task;
};

struct sync {
    enum xante_ui_dialog    type;
    int                     step_limit;
};

static struct sync __sync[] = {
    { XANTE_UI_DIALOG_SPINNER_SYNC, 4 },
    { XANTE_UI_DIALOG_DOTS_SYNC,    6 }
};

#define MAX_SYNC        (sizeof(__sync) / sizeof(__sync[0]))

/*
 *
 * Internal functions
 *
 */

static void task_signal(int signum)
{
    if (signum == SIGUSR1) {
        xante_log_info("The sync task seemed to be freeze. We're going to end it.");
    }
}

static struct sync *get_sync(enum xante_ui_dialog sync)
{
    unsigned int i;

    for (i = 0; i < MAX_SYNC; i++)
        if (__sync[i].type == sync)
            return &__sync[i];

    return NULL;
}

static void update_synctext(cl_string_t *text, int step, enum xante_ui_dialog sync)
{
    char c;
    int i;

    if (sync == XANTE_UI_DIALOG_SPINNER_SYNC) {
        if (step == 0)
            c = '-';
        else if (step == 1)
            c = '\\';
        else if (step == 2)
            c = '|';
        else
            c = '/';

        cl_string_cat(text, " %c", c);
    } else if (sync == XANTE_UI_DIALOG_DOTS_SYNC) {
        for (i = 0; i < step; i++)
            cl_string_cat(text, ".");
    }
}

static void *call_task(cl_thread_t *thread)
{
    struct sync_thread *sync = cl_thread_get_user_data(thread);
    struct xante_app *xpp = sync->xpp;
    struct xante_item *item = sync->item;
    bool loop = false;

    cl_thread_set_state(thread, CL_THREAD_ST_CREATED);

    /*
     * We monitor the SIGUSR1 signal so if this task gets blocked and reaches
     * the timeout adjusted in its item->max attribute we can killed it without
     * killing the all application.
     */
    cl_trap(SIGUSR1, task_signal);
    cl_thread_set_state(thread, CL_THREAD_ST_INITIALIZED);
    xante_log_debug("%s: starting...", __FUNCTION__);

    do {
        loop = event_call(EV_SYNC_ROUTINE, xpp, item, sync->data);

        /*
         * At one time the event function _must_ return a false value. Otherwise
         * we're going to freeze.
         */
        if (loop == false)
            break;

        cl_timeout_reset(sync->tm_task, CL_OBJECT_AS_INT(item->max),
                         CL_TM_SECONDS);
    } while (loop);

    item->cancel_update = true;
    xante_log_debug("%s: finishing...", __FUNCTION__);

    return NULL;
}

/*
 * This is the thread that updates the sync bar dialog.
 */
static void *make_sync(cl_thread_t *thread)
{
    struct sync_thread *sync = cl_thread_get_user_data(thread);
    struct xante_app *xpp = sync->xpp;
    struct xante_item *item = sync->item;
    struct sync *sync_model;
    cl_thread_t *task = NULL;
    cl_string_t *text = NULL;
    ui_properties_t properties;
    int stepbar = 0;
    char *tmp = NULL;

    cl_thread_set_state(thread, CL_THREAD_ST_CREATED);
    INIT_PROPERTIES(properties);
    sync_model = get_sync(item->dialog_type);

    /* Creates a thread to run the user task (event) */
    task = cl_thread_spawn(CL_THREAD_JOINABLE, call_task, sync);

    if (NULL == task) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                             cl_tr("Sync thread creation error: %s"),
                             cl_strerror(cl_get_last_error()));

        cl_thread_set_state(task, CL_THREAD_ST_INITIALIZED);
        return NULL;
    }

    if (cl_thread_wait_startup(task) != 0) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                             cl_tr("Error waiting for thread to initialize: %s"),
                             cl_strerror(cl_get_last_error()));

        cl_thread_set_state(task, CL_THREAD_ST_INITIALIZED);
        return NULL;
    }

    sync->tm_task = cl_timeout_create(CL_OBJECT_AS_INT(item->max),
                                      CL_TM_SECONDS);

    properties.width = (item->geometry.width == 0) ? DIALOG_WIDTH
                                                   : item->geometry.width;

    properties.height = (item->geometry.height == 0) ? DIALOG_HEIGHT
                                                     : item->geometry.height;

    cl_thread_set_state(thread, CL_THREAD_ST_INITIALIZED);
    xante_log_debug("%s: started sync thread", __FUNCTION__);

    do {
        text = cl_string_dup(item->options);

        /*
         * Allow the user to insert a custom message from within the sync-event
         * function inside the module.
         */
        if (item->value != NULL) {
            tmp = CL_OBJECT_AS_STRING(item->value);
            cl_string_cat(text, "%s", tmp);
            free(tmp);
        }

        update_synctext(text, stepbar, sync_model->type);
        dialog_msgbox(cl_string_valueof(item->name), cl_string_valueof(text),
                      properties.height, properties.width, 0);

        stepbar++;

        if (stepbar >= sync_model->step_limit)
            stepbar = 0;

        cl_string_unref(text);
        cl_msleep(500);

        /*
         * Checks if the task is consuming more time than the item's maximum
         * timeout. If it is, we ended it.
         */
        if (cl_timeout_expired(sync->tm_task)) {
            cl_thread_force_finish(task);
            item->cancel_update = true;
            xante_dlg_messagebox(xpp, XANTE_MSGBOX_WARNING, cl_tr("Warning"),
                                 cl_tr("The task running in background seems "
                                       "to be blocked. We're killing it!"));
        }
    } while (item->cancel_update == false);

    cl_thread_destroy(task);
    UNINIT_PROPERTIES(properties);
    xante_log_debug("%s: finish sync thread", __FUNCTION__);

    return NULL;
}

/*
 *
 * Internal API
 *
 */

/**
 * @name ui_sync
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
ui_return_t ui_sync(struct xante_app *xpp, struct xante_item *item)
{
    ui_return_t ret;
    void *data;
    cl_thread_t *thread;
    struct sync_thread sync = {
        .xpp = xpp,
        .item = item
    };

    /* Prepare dialog */
    dlgx_set_backtitle(xpp);
    dlgx_put_statusbar(DEFAULT_STATUSBAR_TEXT);

    /* Assures that we will be able to, at least, start the sync */
    item->cancel_update = false;
    data = event_item_custom_data(xpp, item);
    sync.data = data;
    thread = cl_thread_spawn(CL_THREAD_JOINABLE, make_sync, &sync);

    if (NULL == thread) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                             cl_tr("Sync thread creation error: %s"),
                             cl_strerror(cl_get_last_error()));

        goto end_block;
    }

    if (cl_thread_wait_startup(thread) != 0) {
        xante_dlg_messagebox(xpp, XANTE_MSGBOX_ERROR, cl_tr("Error"),
                             cl_tr("Error waiting for thread to initialize: %s"),
                             cl_strerror(cl_get_last_error()));

        goto end_block;
    }

    /* Since there is a thread-join here it will wait for the thread to end. */
    cl_thread_destroy(thread);

end_block:
    ret.selected_button = DLG_EXIT_OK;
    ret.updated_value = false;

    return ret;
}

