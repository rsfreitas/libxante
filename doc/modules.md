# Application module

This document describes the specifications of modules to be used inside
an application using libxante.

## Module

A module is an outside application (shared object) with functions which are
called from inside an application.

### Format

A libxante's module must be written using the **libcollections** plugin
API. And, by using it as a dependency a module may be written in C, C++,
Go and Rust.

As a libcollection's module a libxante's module must provide its information
such as: name, author name, version and description. Startup and shutdown
functions are optional.

### Events, callbacks and arguments

Libxante has some events in which callbacks may be called so a module can
do pretty much anything.

These events are:

* Initialization: This event is triggered when an application is also being
initialized, inside the function **xante\_init**. After internal library
adjustments a callback is called, so a module may also initialize anything
needed. This callback is called **xapl\_init** and it must return an integer
value (as 0 being success).

* Ending: This event is triggered when an application is being closed. When
the function **xante\_uninit** is called. The callback name is **xapl\_uninit**.

* Settings: Two events are triggered when an application settings is being
loaded and unloaded. The callbacks in this case are **xapl\_config\_load** and
**xapl\_config\_unload**. And they receive a pointer to the current settings
file (using the libcollection API).

* Changes: When a change is made in an application and the user chooses to
save them on the disc (in a settings file) this event is triggered and a
list of all changes made is passed as argument to the **xapl\_changes\_saved**
callback. This callback function must return an integer value.

#### Item callbacks

As well as library events, items also have specific events that call some
callback functions. They are directly related to a JTF object. The following
callbacks are avaliable:

* **item-selected**
* **item-value-confirm**
* **item-value-updated**
* **item-exit**
* **item-custom-data**
* **menu-exit**
* **custom-event**
* **update-routine**
* **sync-routine**
* **value-strlen**
* **value-check**
* **extra-button-pressed**

#### Arguments

Every event or item callback function receives as argument pointers to internal
structures or information (which must be retrieved using libcollection's plugin
API) to allow it access data, change UI behaviour and display new widgets.

These arguments are:

* **xpp**: A pointer to the **xante\_t** object. This argument is passed in
every event or item callback.

* **menu**: A pointer to the current menu, a **xante\_menu\_t** object. This
argument is passed only in item callback functions.

* **item**: A pointer to the current item, a **xante\_item\_t** object. This
argument is passed only in item callback functions.

* **cfg-file**: A pointer to a **cl\_cfg\_file\_t** object, allowing access to
the entire application settings. This argument is passed to the **xapl\_config\_load**
and **xapl\_config\_unload** events.

* **value**: A pointer to a **cl\_object\_t** object. This argument is passed
only in item callbacks.

* **changes**: A pointer to a **cl\_list\_t** object, with all current changes
made by the user inside the application. This argument is only passed to the
**xapl\_changes\_saved** event and only C/C++ plugins can handle it.

* **data**: A pointer to some data which may vay according the event. This
argument is passed only in item callbacks.

