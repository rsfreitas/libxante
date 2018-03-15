//
// Description: Core library declarations.
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
#cgo CFLAGS: -D_GO_API_ -I/usr/local/include -fgnu89-inline
#cgo LDFLAGS: -L/usr/local/lib -lxante -lcollections
#include <stdlib.h>
#include <xante/libxante.h>
*/
import "C"
import (
	"unsafe"
)

// Supported objects
const (
	MenuReference = 1 + iota
	InputInt
	InputFloat
	InputDate
	InputString
	InputPasswd
	InputTime
	Calendar
	Timebox
	RadioChecklist
	Checklist
	YesNo
	DynamicMenuReference
	DeleteDynameMenuItem
	AddDynamicMenuItem
	Custom
	Progress
	SpinnerSync
	SpinnerDots
	Range
	FileSelect
	DirSelect
	FileView
	Tailbox
	Scrolltext
	UpdateObject
	Inputscroll
	Mixedform
	Buildlist
	Spreadsheet
	Clock
)

// Supported menus
const (
	DefaultMenu = 1 + iota
	DynamicMenu
)

// Type of message displayed inside on a message box.
type MsgType int

// Supported types of message displayed inside on a message box.
const (
	MsgInfo MsgType = 0 + iota
	MsgWarning
	MsgError
)

// XanteApp is the Go version of a xante_t object.
type XanteApp struct {
	data unsafe.Pointer
}

// LoadXanteApp is responsible to populate a XanteApp structure with @data.
func LoadXanteApp(data unsafe.Pointer) *XanteApp {
	return &XanteApp{
		data: data,
	}
}

// MsgBox runs a message box dialog to show a message to the user.
func (x *XanteApp) MsgBox(msgType MsgType, title string, message string) {
	cTitle := C.CString(title)
	defer C.free(unsafe.Pointer(cTitle))

	cMessage := C.CString(message)
	defer C.free(unsafe.Pointer(cMessage))

	C.xante_dlg_messagebox_ex(x.data, uint32(msgType), cTitle, cMessage)
}

// Dialog creates and runs a user custom dialog when called. It is important
// to remember that @dialog must be a JSON string understood by libxante with
// the desired dialog.
func (x *XanteApp) Dialog(dialog string) {
	cDialog := C.CString(dialog)
	defer C.free(unsafe.Pointer(cDialog))

	C.xante_manager_single_run(x.data, cDialog)
}
