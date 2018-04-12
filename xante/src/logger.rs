#![allow(dead_code)]

///! Bindings for the logger interface
///
/// Author: Rodrigo Freitas
/// Created at: Qui Mar 22 08:40:52 -03 2018
///

extern crate libc;

use std::ffi::CString;

extern {
    fn xante_log_ex(level: i32, function: *const u8, line: i32, content: *const u8);
}

pub enum Level {
    Off,
    Emerg,
    Alert,
    Criti,
    Error,
    Warng,
    Notice,
    Info,
    Debug
}

pub fn emerg(message: &str) {
    unsafe {
        let content = CString::new(message).unwrap();
        let fname = CString::new("__function__").unwrap();
        xante_log_ex(Level::Emerg as i32, fname.as_ptr() as *const u8, 0,
                     content.as_ptr() as *const u8);
    }
}

pub fn alert(message: &str) {
    unsafe {
        let content = CString::new(message).unwrap();
        let fname = CString::new("__function__").unwrap();
        xante_log_ex(Level::Alert as i32, fname.as_ptr() as *const u8, 0,
                     content.as_ptr() as *const u8);
    }
}

pub fn criti(message: &str) {
    unsafe {
        let content = CString::new(message).unwrap();
        let fname = CString::new("__function__").unwrap();
        xante_log_ex(Level::Criti as i32, fname.as_ptr() as *const u8, 0,
                     content.as_ptr() as *const u8);
    }
}

pub fn warng(message: &str) {
    unsafe {
        let content = CString::new(message).unwrap();
        let fname = CString::new("__function__").unwrap();
        xante_log_ex(Level::Warng as i32, fname.as_ptr() as *const u8, 0,
                     content.as_ptr() as *const u8);
    }
}

pub fn notice(message: &str) {
    unsafe {
        let content = CString::new(message).unwrap();
        let fname = CString::new("__function__").unwrap();
        xante_log_ex(Level::Notice as i32, fname.as_ptr() as *const u8, 0,
                     content.as_ptr() as *const u8);
    }
}

pub fn info(message: &str) {
    unsafe {
        let content = CString::new(message).unwrap();
        let fname = CString::new("__function__").unwrap();
        xante_log_ex(Level::Info as i32, fname.as_ptr() as *const u8, 0,
                     content.as_ptr() as *const u8);
    }
}

pub fn error(message: &str) {
    unsafe {
        let content = CString::new(message).unwrap();
        let fname = CString::new("__function__").unwrap();
        xante_log_ex(Level::Error as i32, fname.as_ptr() as *const u8, 0,
                     content.as_ptr() as *const u8);
    }
}

pub fn debug(function: &str, message: &str) {
    unsafe {
        let content = CString::new(message).unwrap();
        let fname = CString::new(function).unwrap();
        xante_log_ex(Level::Debug as i32, fname.as_ptr() as *const u8, 0,
                     content.as_ptr() as *const u8);
    }
}

