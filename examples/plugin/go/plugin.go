// A little Go plugin example
package main

import "C"
import (
	"fmt"
	"unsafe"

	"xante/pkg/xante"
)

var logger = xante.NewLogger()

//
// Mandatory plugin functions
//

func main() {
	//
	// We need the main function otherwise the ELF shared object created will be
	// in the wrong format, as an AR (archive) file and not an ELF shared object.
	//
}

//export plugin_name
func plugin_name() *C.char {
	return C.CString("Go-Plugin")
}

//export plugin_version
func plugin_version() *C.char {
	return C.CString("0.1")
}

//export plugin_author
func plugin_author() *C.char {
	return C.CString("Rodrigo Freitas")
}

//export plugin_description
func plugin_description() *C.char {
	return C.CString("Go plugin example for libxante")
}

//
// Plugin functions
//

//export xapl_init
func xapl_init(arguments unsafe.Pointer) int {
	logger.Info("Hello from xapl_init")
	return 0
}

//export xapl_config_load
func xapl_config_load(arguments unsafe.Pointer) {
	evArguments, err := xante.LoadEvent(arguments)

	if err != nil {
		return
	}

	// read some settings
	inputInt := evArguments.CfgFile.Get("Main", "input_int")
	inputString := evArguments.CfgFile.Get("Main", "input_string")

	logger.Info("Hello from xapl_config_load")
	logger.Info(fmt.Sprintf("[Main]input_int = %s", inputInt))
	logger.Info(fmt.Sprintf("[Main]input_string = '%s'", inputString))
}

//export foo_custom
func foo_custom(arguments unsafe.Pointer) int {
	evArguments, err := xante.LoadEvent(arguments)

	if err != nil {
		return -1
	}

	evArguments.Xpp.MsgBox(xante.MsgInfo, "Go foo_custom",
		fmt.Sprintf("Custom event name: %s", evArguments.Item.Name))

	return 0
}
