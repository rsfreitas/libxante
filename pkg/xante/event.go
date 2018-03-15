//
// Description:
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

import (
	"errors"
	"unsafe"

	"collections/pkg/collections"
)

// XanteEvent represents every possible argument received by an exported plugin
// function.
type XanteEvent struct {
	Xpp        *XanteApp
	Menu       *XanteMenu
	Item       *XanteItem
	CfgFile    *collections.ClConfigFile
	EventValue unsafe.Pointer
	Changes    unsafe.Pointer
	Data       unsafe.Pointer
}

// LoadEvent is responsible to create a XanteEvent structure with @data as
// content.
func LoadEvent(arguments unsafe.Pointer) (*XanteEvent, error) {
	args, err := collections.LoadArguments(arguments)

	if err != nil {
		return nil, errors.New("Unable to load function arguments")
	}

	return &XanteEvent{
		Xpp:        LoadXanteApp(args.Pointer("xpp")),
		Menu:       LoadXanteMenu(args.Pointer("menu")),
		Item:       LoadXanteItem(args.Pointer("item")),
		CfgFile:    collections.NewConfigFile(args.Pointer("cfg-file")),
		EventValue: args.Pointer("value"),
		Changes:    args.Pointer("changes"),
		Data:       args.Pointer("data"),
	}, nil
}
