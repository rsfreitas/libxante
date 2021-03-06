
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Tue May  2 22:54:08 2017
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

#ifndef _LIBXANTE_API_RUNTIME_H
#define _LIBXANTE_API_RUNTIME_H

#ifndef LIBXANTE_COMPILE
# ifndef _LIBXANTE_H
#  error "Never use <runtime.h> directly; include <libxante.h> instead."
# endif
#endif

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
int xante_runtime_set_discard_changes(xante_t *xpp, bool discard_changes);

/**
 * @name xante_runtime_discard_changes
 * @brief Retrieves if the application is going to ignore internal changes or
 *        not.
 *
 * @param [in] xpp: The library main object.
 *
 * @return Returns true/false if it's going to ignore.
 */
bool xante_runtime_discard_changes(const xante_t *xpp);

/**
 * @name xante_runtime_set_discard_changes_on_timeout
 * @brief Sets/Unsets an application to ignore changes on timeout.
 *
 * @param [in] xpp: The library main object.
 * @param [in] discard_changes: The boolean value to ignore or not.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int xante_runtime_set_discard_changes_on_timeout(xante_t *xpp,
                                                 bool discard_changes);

/**
 * @name xante_runtime_discard_changes_on_timeout
 * @brief Retrives if the application is going to ignore changes on timeout.
 *
 * @param [in] xpp: The library main object.
 *
 * @return Returns true/false if it's going to ignore.
 */
bool xante_runtime_discard_changes_on_timeout(const xante_t *xpp);

/**
 * @name xante_runtime_execute_module
 * @brief Retrieves if the application is working with a module or not.
 *
 * @param [in] xpp: The library main object.
 *
 * @return Returns true/false if the application is working or not.
 */
bool xante_runtime_execute_module(const xante_t *xpp);

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
int xante_runtime_set_force_config_file_saving(xante_t *xpp, bool force_saving);

/**
 * @name xante_runtime_force_config_file_saving
 * @brief Retrives if an application is going to force save its configuration
 *        file.
 *
 * @param [in] xpp: The library main object.
 *
 * @return Returns true/false if it's going to.
 */
bool xante_runtime_force_config_file_saving(const xante_t *xpp);

/**
 * @name xante_runtime_set_inactivity_timeout
 * @brief Sets the timeout to close an UI object.
 *
 * @param [in] xpp: The library main object.
 * @param [in] timeout: The timeout in seconds.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int xante_runtime_set_inactivity_timeout(xante_t *xpp, unsigned int timeout);

/**
 * @name xante_runtime_inactivity_timeout
 * @brief Retrives the current timeout to close an object.
 *
 * @param [in] xpp: The library main object.
 *
 * @return On success returns the timeout or -1 otherwise.
 */
int xante_runtime_inactivity_timeout(const xante_t *xpp);

/**
 * @name xante_runtime_set_show_config_saving_question.
 * @brief Enables/Disables the question to choose if the configuration file
 *        will be saved or not.
 *
 * @param [in] xpp: The library main object.
 * @param [in] show_question: The boolean value to enable/disable the object.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int xante_runtime_set_show_config_saving_question(xante_t *xpp,
                                                  bool show_question);

/**
 * @name xante_runtime_show_config_saving_question
 * @brief Retrieves if the configuration file object question is going to be
 *        displayed or not.
 *
 * @param [in] xpp: The library main object.
 *
 * @return Returns true/false if it's going to.
 */
bool xante_runtime_show_config_saving_question(const xante_t *xpp);

/**
 * @name xante_runtime_set_accent_characters
 * @brief Enables/Disables accent characters in the input object.
 *
 * @param [in] xpp: The library main object.
 * @param [in] use_accents: The boolean value to enable/disable.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int xante_runtime_set_accent_characters(xante_t *xpp, bool use_accents);

/**
 * @name xante_runtime_accent_characters
 * @brief Retrieves if the application is using accent characters or not.
 *
 * @param [in] xpp: The library main object.
 *
 * @return Returns true/false if it's using or not.
 */
bool xante_runtime_accent_characters(const xante_t *xpp);

/**
 * @name xante_runtime_exit_value
 * @brief Retrieves the exit value of a libxante application.
 *
 * @param [in] xpp: The library main object.
 *
 * @return On success returns the exit code or -1 otherwise.
 */
enum xante_return_value xante_runtime_exit_value(const xante_t *xpp);

/**
 * @name xante_runtime_set_close_ui
 * @brief Sets/Unsets the application to close its UI.
 *
 * @param [in] xpp: The library main object.
 * @param [in] close_ui: The boolean value to close or not the UI.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int xante_runtime_set_close_ui(xante_t *xpp, bool close_ui);

/**
 * @name xante_runtime_close_ui
 * @brief Retrieves if the application needs to close its UI or not.
 *
 * @param [in] xpp: The library main object.
 *
 * @return Returns true/false if the application needs to close its UI.
 */
bool xante_runtime_close_ui(const xante_t *xpp);

/**
 * @name xante_runtime_ui_active
 * @brief Retrives if the application is in the UI mode.
 *
 * @param [in] xpp: The library main object.
 *
 * @return Returns true/false if it's in the UI mode.
 */
bool xante_runtime_ui_active(const xante_t *xpp);

/**
 * @name xante_runtime_user_authentication
 * @brief Retrieves if the application is using access authentication.
 *
 * @param [in] xpp: The library main object.
 *
 * @return Returns true/false if it is using.
 */
bool xante_runtime_user_authentication(const xante_t *xpp);

/**
 * @name xante_runtime_caller_name
 * @brief Retrieves the application name which got us running.
 *
 * @param [in] xpp: The library main object.
 *
 * @return On success returns the caller name or NULL otherwise.
 */
const char *xante_runtime_caller_name(const xante_t *xpp);

/**
 * @name xante_runtime_esc_key
 * @brief Retrieves if the ESC key is blocked or not.
 *
 * @param [in] xpp: The library main object.
 *
 * @return Returns true/false if the ESC key is blocked or not.
 */
bool xante_runtime_esc_key(const xante_t *xpp);

/**
 * @name xante_runtime_suspend_key
 * @brief Retrieves if the application can be suspended through a key
 *        combination.
 *
 * @param [in] xpp: The library main object.
 *
 * @return Returns true/false if the application can be suspended or not.
 */
bool xante_runtime_suspend_key(const xante_t *xpp);

/**
 * @name xante_runtime_suspend_key
 * @brief Retrieves if the application can be suspended through a key
 *        combination.
 *
 * @param [in] xpp: The library main object.
 *
 * @return Returns true/false if the application can be suspended or not.
 */
bool xante_runtime_stop_key(const xante_t *xpp);

#endif

