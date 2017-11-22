
/*
 * Description: Functions to handle runtime application flags and status.
 *
 * Author: Rodrigo Freitas
 * Created at: Tue May  2 22:17:07 2017
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
 *
 * Internal functions
 *
 */
static void runtime_set_caller_name(struct xante_app *xpp, const char *caller_name)
{
    char *bname, *tmp;

    if (NULL == caller_name) {
        xpp->runtime.caller_name = NULL;
        return;
    }

    tmp = strdup(caller_name);
    bname = basename(tmp);
    xpp->runtime.caller_name = strdup(bname);
    free(tmp);
}


/*
 *
 * Internal API
 *
 */

/**
 * @name runtime_start
 * @brief Initialize all internal runtime flags to its default values.
 *
 * @param [in,out] xpp: A previously created xante_app structure.
 * @param [in] caller_name: The name of the application which is calling us.
 */
void runtime_start(struct xante_app *xpp, const char *caller_name)
{
    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return;
    }

    /* Sets the default values from our runtime flags */
    xante_runtime_set_discard_changes(xpp, false);
    xante_runtime_set_discard_changes_on_timeout(xpp, true);
    xante_runtime_set_force_config_file_saving(xpp, false);
    xante_runtime_set_ui_dialog_timeout(xpp, UI_DIALOG_TIMEOUT);
    xante_runtime_set_show_config_saving_question(xpp, true);
    xante_runtime_set_accent_characters(xpp, false);
    xante_runtime_set_close_ui(xpp, false);
    runtime_set_execute_module(xpp, true);
    runtime_set_exit_value(xpp, XANTE_RETURN_OK);
    runtime_set_ui_active(xpp, false);
    runtime_set_user_authentication(xpp, true);
    runtime_set_caller_name(xpp, caller_name);
}

void runtime_stop(struct xante_app *xpp)
{
    if (xpp->runtime.caller_name != NULL)
        free(xpp->runtime.caller_name);
}

/**
 * @name runtime_set_exit_value
 * @brief Sets the exit value of a libxante application.
 *
 * @param [in] xpp: The library main object.
 * @param [in] exit_value: The exit value.
 */
void runtime_set_exit_value(struct xante_app *xpp, int exit_value)
{
    xpp->runtime.exit_value = exit_value;
}

/**
 * @name runtime_set_ui_active
 * @brief Set/Unsets if the application is in the UI mode or not.
 *
 * @param [in] xpp: The library main object.
 * @param [in] ui_active: The boolean value to set/unset the UI.
 */
void runtime_set_ui_active(struct xante_app *xpp, bool ui_active)
{
    xpp->runtime.ui_active = ui_active;
}

/**
 * @name runtime_set_execute_module.
 * @brief Enables/disables the module from an application.
 *
 * @param [in] xpp: The library main object.
 * @param [in] execute_module: The boolean value to enable/disable.
 */
void runtime_set_execute_module(struct xante_app *xpp, bool execute_module)
{
    xpp->runtime.execute_module = execute_module;
}

/**
 * @name runtime_set_user_authentication
 * @brief Sets/Unsets if the application is going to use internal user access
 *        authentication.
 *
 * @param [in] xpp: The library main object.
 * @param [in] auth: The boolean flag to set/unset the authentication.
 */
void runtime_set_user_authentication(struct xante_app *xpp, bool auth)
{
    xpp->runtime.user_authentication = auth;
}

/*
 *
 * API
 *
 */

__PUB_API__ int xante_runtime_set_discard_changes(xante_t *xpp,
    bool discard_changes)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    x->runtime.discard_changes = discard_changes;

    return 0;
}

__PUB_API__ bool xante_runtime_discard_changes(const xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return false;
    }

    return x->runtime.discard_changes;
}

__PUB_API__ int xante_runtime_set_discard_changes_on_timeout(xante_t *xpp,
    bool discard_changes)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    x->runtime.discard_changes_on_timeout = discard_changes;

    return 0;
}

__PUB_API__ bool xante_runtime_discard_changes_on_timeout(const xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return false;
    }

    return x->runtime.discard_changes_on_timeout;
}

__PUB_API__ bool xante_runtime_execute_module(const xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return false;
    }

    return x->runtime.execute_module;
}

__PUB_API__ int xante_runtime_set_force_config_file_saving(xante_t *xpp,
    bool force_saving)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    x->runtime.force_config_file_saving = force_saving;

    return 0;
}

__PUB_API__ bool xante_runtime_force_config_file_saving(const xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return false;
    }

    return x->runtime.force_config_file_saving;
}

__PUB_API__ int xante_runtime_set_ui_dialog_timeout(xante_t *xpp,
    unsigned int timeout)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    x->runtime.ui_dialog_timeout = timeout;

    return 0;
}

__PUB_API__ int xante_runtime_ui_dialog_timeout(const xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    return x->runtime.ui_dialog_timeout;
}

__PUB_API__ int xante_runtime_set_show_config_saving_question(xante_t *xpp,
    bool show_question)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    x->runtime.show_config_saving_question = show_question;

    return 0;
}

__PUB_API__ bool xante_runtime_show_config_saving_question(const xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return false;
    }

    return x->runtime.show_config_saving_question;
}

__PUB_API__ int xante_runtime_set_accent_characters(xante_t *xpp,
    bool use_accents)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    x->runtime.accent_characters = use_accents;

    return 0;
}

__PUB_API__ bool xante_runtime_accent_characters(const xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return false;
    }

    return x->runtime.accent_characters;
}

__PUB_API__ enum xante_return_value xante_runtime_exit_value(const xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    return x->runtime.exit_value;
}

__PUB_API__ int xante_runtime_set_close_ui(xante_t *xpp, bool close_ui)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    x->runtime.close_ui = close_ui;

    return 0;
}

__PUB_API__ bool xante_runtime_close_ui(const xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return false;
    }

    return x->runtime.close_ui;
}

__PUB_API__ bool xante_runtime_ui_active(const xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return false;
    }

    return x->runtime.ui_active;
}

__PUB_API__ bool xante_runtime_user_authentication(const xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return false;
    }

    return x->runtime.user_authentication;
}

__PUB_API__ const char *xante_runtime_caller_name(const xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return NULL;
    }

    return x->runtime.caller_name;
}

