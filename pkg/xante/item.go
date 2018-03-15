//
// Description: Handles XanteItem object
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
import (
	"unsafe"

	"collections/pkg/collections"
)

// XanteItem is the Go version of a xante_item_t object.
type XanteItem struct {
	data       unsafe.Pointer
	Type       uint32
	Name       string
	ObjectId   string
	AccessMode uint32
}

// Value gets the current item value as a string.
func (i *XanteItem) Value() string {
	value := C.xante_item_value(i.data)

	if C.xante_get_last_error() != 0 {
		return ""
	}

	object := collections.NewObject(value)

	return object.Get()
}

// SetValue updates the item current value.
func (i *XanteItem) SetValue(content string) {
	cs := C.CString(content)
	defer C.free(unsafe.Pointer(cs))

	C.xante_item_update_value_ex(i.data, cs)
}

// LoadXanteItem creates a XanteItem structure from @data.
func LoadXanteItem(data unsafe.Pointer) *XanteItem {
	return &XanteItem{
		data:       data,
		Type:       C.xante_item_object_type(data),
		Name:       C.GoString(C.xante_item_name(data)),
		ObjectId:   C.GoString(C.xante_item_object_id(data)),
		AccessMode: C.xante_item_access_mode(data),
	}
}
