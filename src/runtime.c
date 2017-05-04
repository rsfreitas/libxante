
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
 * Internal API
 *
 */

/**
 * @name runtime_start
 * @brief Initialize all internal runtime flags to its default values.
 *
 * @param [in,out] xpp: A previously created xante_app structure.
 */
void runtime_start(struct xante_app *xpp)
{
    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return;
    }

    /* Sets the default values from our runtime flags */
    xante_runtime_set_discard_changes(xpp, false);
    xante_runtime_set_discard_changes_on_timeout(xpp, true);
    xante_runtime_set_execute_plugin(xpp, true);
    xante_runtime_set_create_default_config_file(xpp, false);
    xante_runtime_set_force_config_file_saving(xpp, true);
    xante_runtime_set_ui_dialog_timeout(xpp, UI_DIALOG_TIMEOUT);
    xante_runtime_set_config_file_status(xpp, XANTE_CFG_ST_UNKNOWN);
    xante_runtime_set_show_config_saving_question(xpp, true);
    xante_runtime_set_accent_characters(xpp, false);
    xante_runtime_set_exit_value(xpp, 0);
    xante_runtime_set_close_ui(xpp, false);
    xante_runtime_set_ui_active(xpp, false);
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

__PUB_API__ bool xante_runtime_discard_changes(xante_t *xpp)
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

__PUB_API__ bool xante_runtime_discard_changes_on_timeout(xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return false;
    }

    return x->runtime.discard_changes_on_timeout;
}

__PUB_API__ int xante_runtime_set_execute_plugin(xante_t *xpp,
    bool execute_plugin)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    x->runtime.execute_plugin = execute_plugin;

    return 0;
}

__PUB_API__ bool xante_runtime_execute_plugin(xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return false;
    }

    return x->runtime.execute_plugin;
}

__PUB_API__ int xante_runtime_set_create_default_config_file(xante_t *xpp,
    bool create_default_config_file)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    x->runtime.create_default_config_file = create_default_config_file;

    return 0;
}

__PUB_API__ bool xante_runtime_create_default_config_file(xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return false;
    }

    return x->runtime.create_default_config_file;
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

__PUB_API__ bool xante_runtime_force_config_file_saving(xante_t *xpp)
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

__PUB_API__ int xante_runtime_ui_dialog_timeout(xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    return x->runtime.ui_dialog_timeout;
}

__PUB_API__ int xante_runtime_set_config_file_status(xante_t *xpp,
    enum xante_config_file_status status)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    if (is_valid_config_file_status(status) == false) {
        errno_set(XANTE_ERROR_INVALID_ARG);
        return -1;
    }

    x->runtime.config_file_status = status;

    return 0;
}

__PUB_API__ int xante_runtime_config_file_status(xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    return x->runtime.config_file_status;
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

__PUB_API__ bool xante_runtime_show_config_saving_question(xante_t *xpp)
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

__PUB_API__ bool xante_runtime_accent_characters(xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return false;
    }

    return x->runtime.accent_characters;
}

__PUB_API__ int xante_runtime_set_exit_value(xante_t *xpp, int exit_value)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    x->runtime.exit_value = exit_value;

    return 0;
}

__PUB_API__ int xante_runtime_exit_value(xante_t *xpp)
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

__PUB_API__ bool xante_runtime_close_ui(xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return false;
    }

    return x->runtime.close_ui;
}

__PUB_API__ int xante_runtime_set_ui_active(xante_t *xpp, bool ui_active)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    x->runtime.ui_active = ui_active;

    return 0;
}

__PUB_API__ bool xante_runtime_ui_active(xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return false;
    }

    return x->runtime.ui_active;
}

