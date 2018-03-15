//
// Description: File log functions.
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

// XanteLog represents the available logger.
type XanteLog struct{}

// NewLogger creates a new XanteLog structure.
func NewLogger() *XanteLog {
	return &XanteLog{}
}

// Info writes a message into the log file as an information type.
func (l *XanteLog) Info(content string) {
	f := C.CString("function")
	defer C.free(unsafe.Pointer(f))

	c := C.CString(content)
	defer C.free(unsafe.Pointer(c))

	C.xante_log_ex(C.CL_LOG_INFO, f, C.int(0), c)
}

// Debug writes a message into the log file as a debug type.
func (l *XanteLog) Debug(function, content string) {
	f := C.CString(function)
	defer C.free(unsafe.Pointer(f))

	c := C.CString(content)
	defer C.free(unsafe.Pointer(c))

	// TODO: Adjust the source line number
	C.xante_log_ex(C.CL_LOG_DEBUG, f, C.int(0), c)
}

// Warn writes a message into the log file as a warning type.
func (l *XanteLog) Warn(content string) {
	f := C.CString("function")
	defer C.free(unsafe.Pointer(f))

	c := C.CString(content)
	defer C.free(unsafe.Pointer(c))

	C.xante_log_ex(C.CL_LOG_WARNG, f, C.int(0), c)
}

// Error writes a message into the log file as an error type.
func (l *XanteLog) Error(content string) {
	f := C.CString("function")
	defer C.free(unsafe.Pointer(f))

	c := C.CString(content)
	defer C.free(unsafe.Pointer(c))

	C.xante_log_ex(C.CL_LOG_ERROR, f, C.int(0), c)
}
