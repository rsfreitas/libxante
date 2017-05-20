# Libxante's plugin

This document describe the specifications of a plugin to be used inside
an application using libxante.

## Plugin

A plugin is an outside application with functions which may be called
from within an application.

### Format

A libxante's plugin must be written with the **libcollections** plugin
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

