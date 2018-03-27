
///! A libxante rust plugin example
///
/// Author: Rodrigo Freitas
/// Created at: Qua Mar 14 11:19:20 -03 2018
///

extern crate libc;
extern crate xante;

use xante::core::MessageBoxType;
use xante::core::XanteApp;
use xante::item::XanteItem;
use xante::logger;

/*
 *
 * Plugin information
 *
 */

#[no_mangle]
pub extern "C" fn plugin_name() -> *const u8 {
    "Rust Plugin\0".as_ptr()
}

#[no_mangle]
pub extern "C" fn plugin_version() -> *const u8 {
    "0.1\0".as_ptr()
}

#[no_mangle]
pub extern "C" fn plugin_author() -> *const u8 {
    "Rodrigo Freitas\0".as_ptr()
}

#[no_mangle]
pub extern "C" fn plugin_description() -> *const u8 {
    "Rust plugin example for libxante\0".as_ptr()
}

/*
 *
 * Plugin functions
 *
 */

#[no_mangle]
pub extern "C" fn xapl_init(_arguments: *const u8) -> i32 {
    logger::info("Hello from rust xapl_init");
    0
}

#[no_mangle]
pub extern "C" fn foo_custom(arguments: *const u8) -> i32 {
    let item = match XanteItem::new(arguments) {
        Ok(value) => value,
        Err(_) => return -2,
    };

    let xpp = match XanteApp::new(arguments) {
        Ok(value) => value,
        Err(_) => return -1,
    };

    xpp.message_box(MessageBoxType::Info, "Rust - foo_custom", s);
                    &format!("Custom event name: {}", item.name()));

    0
}
