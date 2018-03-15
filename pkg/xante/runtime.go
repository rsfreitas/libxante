//
// Description: Runtime library information.
// Author: Rodrigo Freitas
// Created at: Dom Jan 21 19:48:11 -02 2018
//
// Copyright (C) 2017 Rodrigo Freitas
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
// USA
//
package xante

/*
#cgo CFLAGS: -I/usr/local/include/xante
#cgo LDFLAGS: -lxante
#include <stdlib.h>
#include <libxante.h>
*/
import "C"

// SetDiscardChanges enables or disables the application to ignore all setting
// changes made.
func (x *XanteApp) SetDiscardChanges(discardChanges bool) int {
	return int(C.xante_runtime_set_discard_changes(x.data, C._Bool(discardChanges)))
}

// DiscardChanges retrieves if the all settings changed will be discarded or
// not when application ends.
func (x *XanteApp) DiscardChanges() bool {
	return bool(C.xante_runtime_discard_changes(x.data))
}

// SetDiscardChangesOnTimeout enables or disables the feature of the application
// discarding all changes made by an user when leaving due to inactivity.
func (x *XanteApp) SetDiscardChangesOnTimeout(discardChanges bool) int {
	return int(C.xante_runtime_set_discard_changes_on_timeout(x.data, C._Bool(discardChanges)))
}

// DiscardChangesOnTimeout retrieves information if any changes made by the
// user on any setting will be discarded in case application ends due user
// inactivity.
func (x *XanteApp) DiscardChangesOnTimeout() bool {
	return bool(C.xante_runtime_discard_changes_on_timeout(x.data))
}

// ExecuteModule retrieves the information if the application is working with
// a module or not.
func (x *XanteApp) ExecuteModule() bool {
	return bool(C.xante_runtime_execute_module(x.data))
}

// SetForceConfigFileSaving enables os disables the application to save its
// settings into a file whether they have changes or not.
func (x *XanteApp) SetForceConfigFileSaving(forceSaving bool) int {
	return int(C.xante_runtime_set_force_config_file_saving(x.data, C._Bool(forceSaving)))
}

// ForceConfigFileSaving retrieves information if the application is going to
// save all its settings whether they have changes or not.
func (x *XanteApp) ForceConfigFileSaving() bool {
	return bool(C.xante_runtime_force_config_file_saving(x.data))
}

// SetInactivityTimeout sets the timeout (in seconds) to close UI interfaces
// without user activity.
func (x *XanteApp) SetInactivityTimeout(timeout uint) int {
	return int(C.xante_runtime_set_inactivity_timeout(x.data, C.uint(timeout)))
}

// InactivityTimeout retrieves the current timeout (in seconds) to close the
// active interface.
func (x *XanteApp) InactivityTimeout() uint {
	return uint(C.xante_runtime_inactivity_timeout(x.data))
}

// SetShowConfigSavingQuestion enables or disables the message box question
// asking, the user to save or not current modifications made into internal
// settings, when application exits.
func (x *XanteApp) SetShowConfigSavingQuestion(showQuestion bool) int {
	return int(C.xante_runtime_set_show_config_saving_question(x.data, C._Bool(showQuestion)))
}

// ShowConfigSavingQuestion retrieves the information if the message box
// question, where the user answers if the settings are going be saved
// or not when application exits, is going to be displayed or not.
func (x *XanteApp) ShowConfigSavingQuestion() bool {
	return bool(C.xante_runtime_show_config_saving_question(x.data))
}

// SetAccentCharacters enables or disables the internal support for accent
// characters.
func (x *XanteApp) SetAccentCharacters(useAccents bool) int {
	return int(C.xante_runtime_set_accent_characters(x.data, C._Bool(useAccents)))
}

// ExitValue retrieves the real exit value from the application, which will
// be returned to the Operational System.
func (x *XanteApp) ExitValue() int {
	return int(C.xante_runtime_exit_value(x.data))
}

// SetCloseUi enables or disables the necessity of closing the UI.
func (x *XanteApp) SetCloseUi(close bool) int {
	return int(C.xante_runtime_set_close_ui(x.data, C._Bool(close)))
}

// CloseUi retrieves information if the UI is going to be closed or not.
func (x *XanteApp) CloseUi() bool {
	return bool(C.xante_runtime_close_ui(x.data))
}

// UiActive gives the current status if the application is in the UI mode or
// not.
func (x *XanteApp) UiActive() bool {
	return bool(C.xante_runtime_ui_active(x.data))
}

// UserAuthentication retrieves information if the application is using the
// database user access authentication feature.
func (x *XanteApp) UserAuthentication() bool {
	return bool(C.xante_runtime_user_authentication(x.data))
}

// CallerName gives the parent application name.
func (x *XanteApp) CallerName() string {
	return C.GoString(C.xante_runtime_caller_name(x.data))
}

// EscKey retrieves information if the ESC key is blocked or not.
func (x *XanteApp) EscKey() bool {
	return bool(C.xante_runtime_esc_key(x.data))
}

// SuspendKey retrieves the information if the application can be suspended
// using a key combination (ctrl + D).
func (x *XanteApp) SuspendKey() bool {
	return bool(C.xante_runtime_suspend_key(x.data))
}

// StopKey retrieves the information if the application can be suspended
// using a key combination (ctrl + C).
func (x *XanteApp) StopKey() bool {
	return bool(C.xante_runtime_stop_key(x.data))
}
