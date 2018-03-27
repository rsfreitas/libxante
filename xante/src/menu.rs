
///! Handles the xante_menu_t object.
///
/// Author: Rodrigo Freitas
/// Created at: Ter Mar 27 11:21:56 -03 2018
///

use rcollections::arguments;

extern {
    fn xante_menu_name(menu: *const u8) -> *const c_char;
    fn xante_menu_object_id(menu: *const u8) -> *const c_char;
    fn xante_menu_type(menu: *const u8) -> i32;
}

pub struct XanteMenu {
    menu: *const u8
}

pub enum XanteMenuType {
    Default,
    Dynamic,
}

impl XanteMenu {
    pub fn name(&self) -> &str {
        let c = unsafe {
            let s = xante_menu_name(self.menu);
            CStr::from_ptr(s)
        };

        c.to_str().unwrap()
    }

    pub fn object_id(&self) -> &str {
        let c = unsafe {
            let s = xante_menu_object_id(self.menu);
            CStr::from_ptr(s)
        };

        c.to_str().unwrap()
    }

    pub fn object_type(&self) -> Option<XanteMenuType> {
        let t = unsafe {
            xante_menu_type(self.menu)
        };

        match t {
            0 => Some(XanteMenuType::Default),
            1 => Some(XanteMenuType::Dynamic),
            _ => None,
        }
    }

    pub fn new(arguments: *const u8) -> Result<XanteMenu, i32> {
        let menu = match arguments::retrieve_pointer_argument(arguments, "menu") {
            Ok(value) => value,
            Err(err) => return Err(err),
        };

        Ok(XanteMenu{ menu: menu })
    }
}

