
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
    xante_runtime_set_force_config_file_saving(xpp, false);
    xante_runtime_set_ui_dialog_timeout(xpp, UI_DIALOG_TIMEOUT);
    xante_runtime_set_config_file_status(xpp, XANTE_CFG_ST_UNKNOWN);
    xante_runtime_set_show_config_saving_question(xpp, true);
    xante_runtime_set_accent_characters(xpp, false);
    xante_runtime_set_exit_value(xpp, 0);
    xante_runtime_set_close_ui(xpp, false);
    xante_runtime_set_ui_active(xpp, false);
    xante_runtime_set_user_authentication(xpp, true);
}

/*
 *
 * API
 *
 */

/**
 * @name xante_runtime_set_discard_changes
 * @brief Sets/Unsets the application to ignore internal changes
 *
 * @param [in] xpp: The library main object.
 * @param [in] discard_changes: The boolean value to ignore or not internal
 *                              changes.
 *
 * @return On success returns 0 or -1 otherwise.
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

/**
 * @name xante_runtime_discard_changes
 * @brief Retrieves if the application is going to ignore internal changes or
 *        not.
 *
 * @param [in] xpp: The library main object.
 *
 * @return Returns true/false if it's going to ignore.
 */
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

/**
 * @name xante_runtime_set_discard_changes_on_timeout
 * @brief Sets/Unsets an application to ignore changes on timeout.
 *
 * @param [in] xpp: The library main object.
 * @param [in] discard_changes: The boolean value to ignore or not.
 *
 * @return On success returns 0 or -1 otherwise.
 */
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

/**
 * @name xante_runtime_discard_changes_on_timeout
 * @brief Retrives if the application is going to ignore changes on timeout.
 *
 * @param [in] xpp: The library main object.
 *
 * @return Returns true/false if it's going to ignore.
 */
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

/**
 * @name xante_runtime_set_execute_plugin.
 * @brief Enables/disables the plugin from an application.
 *
 * @param [in] xpp: The library main object.
 * @param [in] execute_plugin: The boolean value to enable/disable.
 *
 * @return On success returns 0 or -1 otherwise.
 */
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

/**
 * @name xante_runtime_execute_plugin
 * @brief Retrieves if the application is working with a plugin or not.
 *
 * @param [in] xpp: The library main object.
 *
 * @return Returns true/false if the application is working or not.
 */
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

/**
 * @name xante_runtime_set_create_default_config_file
 * @brief Sets/Unsets if an application is going to save its configuration
 *        file with default values.
 *
 * @param [in] xpp: The library main object.
 * @param [in] create_default_config_file: The boolean value to create or not
 *                                         the file.
 *
 * @return On success returns 0 or -1 otherwise.
 */
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

/**
 * @name xante_runtime_create_default_config_file
 * @brief Retrives if an application is going to save its default configuration
 *        file.
 *
 * @param [in] xpp: The library main object.
 *
 * @return Returns true/false if it's going to.
 */
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

/**
 * @name xante_runtime_set_force_config_file_saving
 * @brief Sets/Unsets an application to save its configuration file when
 *        exiting even when no internal modification is made.
 *
 * @param [in] xpp: The library main object.
 * @param [in] force_saving: The boolean value to save or not.
 *
 * @return On success returns 0 or -1 otherwise.
 */
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

/**
 * @name xante_runtime_force_config_file_saving
 * @brief Retrives if an application is going to force save its configuration
 *        file.
 *
 * @param [in] xpp: The library main object.
 *
 * @return Returns true/false if it's going to.
 */
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

/**
 * @name xante_runtime_set_ui_dialog_timeout
 * @brief Sets the timeout to close an UI dialog.
 *
 * @param [in] xpp: The library main object.
 * @param [in] timeout: The timeout in seconds.
 *
 * @return On success returns 0 or -1 otherwise.
 */
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

/**
 * @name xante_runtime_ui_dialog_timeout
 * @brief Retrives the current timeout to close a dialog.
 *
 * @param [in] xpp: The library main object.
 *
 * @return On success returns the timeout or -1 otherwise.
 */
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

/**
 * @name xante_runtime_set_config_file_status
 * @brief Sets the current status of the application configuration file.
 *
 * @param [in] xpp: The library main object.
 * @param [in] status: The new configuration file status.
 *
 * @return On success returns 0 or -1 otherwise.
 */
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

/**
 * @name xante_runtime_config_file_status
 * @brief Retrieves the current configuration file status.
 *
 * @param [in] xpp: The library main object.
 *
 * @return On success returns the configuration file status or -1 otherwise.
 */
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

/**
 * @name xante_runtime_set_show_config_saving_question.
 * @brief Enables/Disables the question to choose if the configuration file
 *        will be saved or not.
 *
 * @param [in] xpp: The library main object.
 * @param [in] show_question: The boolean value to enable/disable the dialog.
 *
 * @return On success returns 0 or -1 otherwise.
 */
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

/**
 * @name xante_runtime_show_config_saving_question
 * @brief Retrieves if the configuration file dialog question is going to be
 *        used or not.
 *
 * @param [in] xpp: The library main object.
 *
 * @return Returns true/false if it's going to.
 */
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

/**
 * @name xante_runtime_set_accent_characters
 * @brief Enable/Disable accent characters in the input dialog.
 *
 * @param [in] xpp: The library main object.
 * @param [in] use_accents: The boolean value to enable/disable.
 *
 * @return On success returns 0 or -1 otherwise.
 */
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

/**
 * @name xante_runtime_accent_characters
 * @brief Retrieves if the application is using accent characters or not.
 *
 * @param [in] xpp: The library main object.
 *
 * @return Returns true/false if it's using or not.
 */
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

/**
 * @name xante_runtime_set_exit_value
 * @brief Sets the exit value of a libxante application.
 *
 * @param [in] xpp: The library main object.
 * @param [in] exit_value: The exit value.
 *
 * @return On success returns 0 or -1 otherwise.
 */
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

/**
 * @name xante_runtime_exit_value
 * @brief Retrieves the exit value of a libxante application.
 *
 * @param [in] xpp: The library main object.
 *
 * @return On success returns the exit code or -1 otherwise.
 */
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

/**
 * @name xante_runtime_set_close_ui
 * @brief Sets/Unsets the application to close its UI.
 *
 * @param [in] xpp: The library main object.
 * @param [in] close_ui: The boolean value to close or not the UI.
 *
 * @return On success returns 0 or -1 otherwise.
 */
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

/**
 * @name xante_runtime_close_ui
 * @brief Retrieves if the application needs to close its UI or not.
 *
 * @param [in] xpp: The library main object.
 *
 * @return Returns true/false if the application needs to close its UI.
 */
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

/**
 * @name xante_runtime_set_ui_active
 * @brief Set/Unsets if the application is in the UI mode or not.
 *
 * @param [in] xpp: The library main object.
 * @param [in] ui_active: The boolean value to set/unset the UI.
 *
 * @return On success returns 0 or -1 otherwise.
 */
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

/**
 * @name xante_runtime_ui_active
 * @brief Retrives if the application is in the UI mode.
 *
 * @param [in] xpp: The library main object.
 *
 * @return Returns true/false if it's in the UI mode.
 */
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

/**
 * @name xante_runtime_set_user_authentication
 * @brief Sets/Unsets if the application is going to use internal user access
 *        authentication.
 *
 * @param [in] xpp: The library main object.
 * @param [in] auth: The boolean flag to set/unset the authentication.
 *
 * @return On success returns 0 or -1 otherwise.
 */
__PUB_API__ int xante_runtime_set_user_authentication(xante_t *xpp, bool auth)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return -1;
    }

    x->runtime.user_authentication = auth;

    return 0;
}

/**
 * @name xante_runtime_user_authentication
 * @brief Retrieves if the application is using access authentication.
 *
 * @param [in] xpp: The library main object.
 *
 * @return Returns true/false if it is using.
 */
__PUB_API__ bool xante_runtime_user_authentication(xante_t *xpp)
{
    struct xante_app *x = (struct xante_app *)xpp;

    errno_clear();

    if (NULL == xpp) {
        errno_set(XANTE_ERROR_NULL_ARG);
        return false;
    }

    return x->runtime.user_authentication;
}

