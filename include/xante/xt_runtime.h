
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

#ifndef _LIBXANTE_XT_RUNTIME_H
#define _LIBXANTE_XT_RUNTIME_H          1

#ifndef LIBXANTE_COMPILE
# ifndef _LIBXANTE_H
#  error "Never use <xt_runtime.h> directly; include <libxante.h> instead."
# endif
#else

/* Internal library declarations */
void runtime_start(struct xante_app *xpp);

#endif

/**
 * @name
 * @brief
 *
 * @param [in] xpp:
 * @param [in] discard_changes:
 *
 * @return
 */
int xante_runtime_set_discard_changes(xante_t *xpp, bool discard_changes);

/**
 * @name
 * @brief
 *
 * @param [in] xpp:
 *
 * @return
 */
bool xante_runtime_discard_changes(xante_t *xpp);

/**
 * @name
 * @brief
 *
 * @param [in] xpp:
 * @param [in] discard_changes:
 *
 * @return
 */
int xante_runtime_set_discard_changes_on_timeout(xante_t *xpp,
                                                 bool discard_changes);

/**
 * @name
 * @brief
 *
 * @param [in] xpp:
 *
 * @return
 */
bool xante_runtime_discard_changes_on_timeout(xante_t *xpp);

/**
 * @name
 * @brief
 *
 * @param [in] xpp:
 * @param [in] execute_plugin:
 *
 * @return
 */
int xante_runtime_set_execute_plugin(xante_t *xpp, bool execute_plugin);

/**
 * @name
 * @brief
 *
 * @param [in] xpp:
 *
 * @return
 */
bool xante_runtime_execute_plugin(xante_t *xpp);

/**
 * @name
 * @brief
 *
 * @param [in] xpp:
 * @param [in] create_default_config_file:
 *
 * @return
 */
int xante_runtime_set_create_default_config_file(xante_t *xpp,
                                                 bool create_default_config_file);

/**
 * @name
 * @brief
 *
 * @param [in] xpp:
 *
 * @return
 */
bool xante_runtime_create_default_config_file(xante_t *xpp);

/**
 * @name
 * @brief
 *
 * @param [in] xpp:
 * @param [in] force_saving:
 *
 * @return
 */
int xante_runtime_set_force_config_file_saving(xante_t *xpp, bool force_saving);

/**
 * @name
 * @brief
 *
 * @param [in] xpp:
 *
 * @return
 */
bool xante_runtime_force_config_file_saving(xante_t *xpp);

/**
 * @name
 * @brief
 *
 * @param [in] xpp:
 * @param [in] timeout:
 *
 * @return
 */
int xante_runtime_set_ui_dialog_timeout(xante_t *xpp, unsigned int timeout);

/**
 * @name
 * @brief
 *
 * @param [in] xpp:
 *
 * @return
 */
int xante_runtime_ui_dialog_timeout(xante_t *xpp);

/**
 * @name
 * @brief
 *
 * @param [in] xpp:
 * @param [in] status:
 *
 * @return
 */
int xante_runtime_set_config_file_status(xante_t *xpp,
                                         enum xante_config_file_status status);

/**
 * @name
 * @brief
 *
 * @param [in] xpp:
 *
 * @return
 */
int xante_runtime_config_file_status(xante_t *xpp);

/**
 * @name
 * @brief
 *
 * @param [in] xpp:
 * @param [in] show_question:
 *
 * @return
 */
int xante_runtime_set_show_config_saving_question(xante_t *xpp,
                                                  bool show_question);

/**
 * @name
 * @brief
 *
 * @param [in] xpp:
 *
 * @return
 */
bool xante_runtime_show_config_saving_question(xante_t *xpp);

/**
 * @name
 * @brief
 *
 * @param [in] xpp:
 * @param [in] use_accents:
 *
 * @return
 */
int xante_runtime_set_accent_characters(xante_t *xpp, bool use_accents);

/**
 * @name
 * @brief
 *
 * @param [in] xpp:
 *
 * @return
 */
bool xante_runtime_accent_characters(xante_t *xpp);

/**
 * @name
 * @brief
 *
 * @param [in] xpp:
 * @param [in] exit_value:
 *
 * @return
 */
int xante_runtime_set_exit_value(xante_t *xpp, int exit_value);

/**
 * @name
 * @brief
 *
 * @param [in] xpp:
 *
 * @return
 */
int xante_runtime_exit_value(xante_t *xpp);

/**
 * @name
 * @brief
 *
 * @param [in] xpp:
 * @param [in] close_ui:
 *
 * @return
 */
int xante_runtime_set_close_ui(xante_t *xpp, bool close_ui);

/**
 * @name
 * @brief
 *
 * @param [in] xpp:
 *
 * @return
 */
bool xante_runtime_close_ui(xante_t *xpp);

/**
 * @name
 * @brief
 *
 * @param [in] xpp:
 * @param [in] ui_active:
 *
 * @return
 */
int xante_runtime_set_ui_active(xante_t *xpp, bool ui_active);

/**
 * @name
 * @brief
 *
 * @param [in] xpp:
 *
 * @return
 */
bool xante_runtime_ui_active(xante_t *xpp);

#endif

