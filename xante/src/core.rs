
///! Core library functions.
///
/// Author: Rodrigo Freitas
/// Created at: Qui Mar 22 08:43:29 -03 2018
///

use std::ffi::CString;
use std::os::raw::c_char;
use rcollections::arguments;
use rcollections::cfg::ClCfgFile;
use rcollections::c_void;
use rcollections::object::ClObject;

extern {
    fn xante_get_last_error() -> i32;
    fn xante_manager_single_run(xpp: *const u8, dialog: *const c_char) -> i32;
    fn xante_dlg_messagebox_ex(xpp: *const u8, msg_type: i32, title: *const c_char,
                               message: *const c_char) -> i32;
}

pub enum MessageBoxType {
    Info,
    Warning,
    Error
}

pub fn get_last_error() -> i32 {
    unsafe {
        xante_get_last_error()
    }
}

pub fn load_cfg(arguments: *const c_void) -> Result<ClCfgFile, i32> {
    let c = match arguments::retrieve_pointer_argument(arguments, "cfg-file") {
        Ok(value) => value,
        Err(err) => return Err(err),
    };

    Ok(ClCfgFile::new(c))
}

pub fn custom_data(arguments: *const c_void) -> Result<*const u8, i32> {
    arguments::retrieve_pointer_argument(arguments, "data")
}

pub fn event_value(arguments: *const c_void) -> Result<ClObject, i32> {
    let v = match arguments::retrieve_pointer_argument(arguments, "value") {
        Ok(value) => value,
        Err(err) => return Err(err),
    };

    Ok(ClObject::new(v as *mut u8))
}

pub struct XanteApp {
    data: *const u8,
}

impl XanteApp {
    pub fn message_box(&self, msg_type: MessageBoxType, title: &str, message: &str) {
        let t = CString::new(title).unwrap();
        let m = CString::new(message).unwrap();

        unsafe {
            xante_dlg_messagebox_ex(self.data, msg_type as i32,
                                    t.as_ptr(),
                                    m.as_ptr());
        }
    }

    pub fn dialog(&self, dialog: &str) -> i32 {
        let d = CString::new(dialog).unwrap();

        unsafe {
            xante_manager_single_run(self.data, d.as_ptr())
        }
    }

    pub fn new(arguments: *const c_void) -> Result<XanteApp, i32> {
        let xpp = match arguments::retrieve_pointer_argument(arguments, "xpp") {
            Ok(value) => value,
            Err(err) => return Err(err),
        };

        /* FIXME: This will need to get out... */
        format!("{:p}", xpp);

        Ok(XanteApp{ data: xpp })
    }
}

