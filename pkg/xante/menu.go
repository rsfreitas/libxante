//
// Description: Handles the XanteMenu object.
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
#include <libxante.h>
*/
import "C"
import (
	"unsafe"
)

// XanteMenu is the Go version of a xante_menu_t object.
type XanteMenu struct {
	data     unsafe.Pointer
	Name     string
	ObjectId string
	Type     uint32
}

// LoadXanteMenu is responsible to load basic information related to a
// xante_menu_t.
func LoadXanteMenu(data unsafe.Pointer) *XanteMenu {
	return &XanteMenu{
		data:     data,
		Name:     C.GoString(C.xante_menu_name(data)),
		ObjectId: C.GoString(C.xante_menu_object_id(data)),
		Type:     C.xante_menu_type(data),
	}
}
