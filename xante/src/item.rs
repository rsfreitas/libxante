
///! Handles the xante_item_t object.
///
/// Author: Rodrigo Freitas
/// Created at: Qui Mar 22 09:15:52 -03 2018
///

use std::ffi::CStr;
use std::ffi::CString;
use rcollections::c_char;
use rcollections::c_void;
use rcollections::arguments;
use rcollections::object::ClObject;

extern {
    fn xante_item_name(item: *const u8) -> *const c_char;
    fn xante_item_object_id(item: *const u8) -> *const c_char;
    fn xante_item_value(item: *const u8) -> *mut u8;
    fn xante_item_update_value_ex(item: *const u8, content: *const u8) -> i32;
    fn xante_item_object_type(item: *const u8) -> i32;
}

pub struct XanteItem {
    item: *const u8,
}

pub enum XanteObject {
	MenuReference,
	InputInt,
	InputFloat,
	InputDate,
	InputString,
	InputPasswd,
	InputTime,
	Calendar,
	Timebox,
	RadioChecklist,
	Checklist,
	YesNo,
	DynamicMenuReference,
	DeleteDynameMenuItem,
	AddDynamicMenuItem,
	Custom,
	Progress,
	SpinnerSync,
	SpinnerDots,
	Range,
	FileSelect,
	DirSelect,
	FileView,
	Tailbox,
	Scrolltext,
	UpdateObject,
	Inputscroll,
	Mixedform,
	Buildlist,
	Spreadsheet,
	Clock,
}

impl XanteItem {
    pub fn name(&self) -> &str {
        let c = unsafe {
            let s = xante_item_name(self.item);
            CStr::from_ptr(s)
        };

        c.to_str().unwrap()
    }

    pub fn object_id(&self) -> &str {
        let c = unsafe {
            let s = xante_item_object_id(self.item);
            CStr::from_ptr(s)
        };

        c.to_str().unwrap()
    }

    pub fn value(&self) -> String {
        let o = unsafe { xante_item_value(self.item) };
        let cl_object = ClObject::new(o);
        cl_object.get()
    }

    pub fn set_value(&self, content: &str) {
        let c = CString::new(content).unwrap();

        unsafe {
            xante_item_update_value_ex(self.item, c.as_ptr() as *const u8);
        }
    }

    pub fn object_type(&self) -> Option<XanteObject> {
        let t = unsafe {
            xante_item_object_type(self.item)
        };

        match t {
            1 => Some(XanteObject::MenuReference),
            2 => Some(XanteObject::InputInt),
            3 => Some(XanteObject::InputFloat),
            4 => Some(XanteObject::InputDate),
            5 => Some(XanteObject::InputString),
            6 => Some(XanteObject::InputPasswd),
            7 => Some(XanteObject::InputTime),
            8 => Some(XanteObject::Calendar),
            9 => Some(XanteObject::Timebox),
            10 => Some(XanteObject::RadioChecklist),
            11 => Some(XanteObject::Checklist),
            12 => Some(XanteObject::YesNo),
            13 => Some(XanteObject::DynamicMenuReference),
            14 => Some(XanteObject::DeleteDynameMenuItem),
            15 => Some(XanteObject::AddDynamicMenuItem),
            16 => Some(XanteObject::Custom),
            17 => Some(XanteObject::Progress),
            18 => Some(XanteObject::SpinnerSync),
            19 => Some(XanteObject::SpinnerDots),
            20 => Some(XanteObject::Range),
            21 => Some(XanteObject::FileSelect),
            22 => Some(XanteObject::DirSelect),
            23 => Some(XanteObject::FileView),
            24 => Some(XanteObject::Tailbox),
            25 => Some(XanteObject::Scrolltext),
            26 => Some(XanteObject::UpdateObject),
            27 => Some(XanteObject::Inputscroll),
            28 => Some(XanteObject::Mixedform),
            29 => Some(XanteObject::Buildlist),
            30 => Some(XanteObject::Spreadsheet),
            31 => Some(XanteObject::Clock),
            _ => None,
        }
    }

    pub fn new(arguments: *const c_void) -> Result<XanteItem, i32> {
        let item = match arguments::retrieve_pointer_argument(arguments, "item") {
            Ok(value) => value,
            Err(err) => return Err(err),
        };

        Ok(XanteItem{ item: item })
    }
}

