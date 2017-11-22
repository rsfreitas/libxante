# Application module

This document describes the specifications of the module to be used inside
an application using libxante.

## Module

A module is an outside application (shared object) with functions which are
called from inside an application.

### Format

A libxante's module must be written using the **libcollections** plugin
API.

### Mandatory functions

* xapl\_init

* xapl\_uninit

* xapl\_config\_load

* xapl\_config\_unload

### JTF callback functions

Supported callback events:

* item-selected

* item-value-confirm

* item-value-changed

* item-exit

* menu-exit

### Events

* init

* uninit

* config-load

* config-unload

* changes-saved

