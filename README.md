# libxante (eXtendable ApplicatioN TEmplate)

A library to build (dialog based) applications using a specification file.

## License

LGPLv2

## Overview

This library is intended to build and run dialog based applications, where one
dialog, as called, overlaps the older one.

It is heavily dependent on libdialog to control terminal settings and some standard
dialogs. Such as:

* Menu
* Calendar input
* Timebox input
* File viewer
* File viewer with update
* File selector
* Directory selector
* Range selector
* Yes/No selector
* Checklist
* Buildlist
* Progressbar
* Mixedforms

With some internal created custom dialogs, such as:

* Spreadsheet
* Syncing tasks
* Data input with scrollable descriptive text

### Features

* INI configuration files
* Dynamic menus
* Database authentication
* Plugin support to build custom applications

## Dependencies

* [libcollections](https://github.com/rsfreitas/libcollections)
* [libdialog](http://invisible-island.net/dialog/)
* [sqlite](http://www.sqlite.org)

## Compiling

In order to compile the project you must have at least the following programs
installed;

* C compiler
* CMake, at least version 2.8

After installing them you can compile the project using the commands:

* mkdir build
* cd build
* cmake ..
* make

### Alternative libdialog

We may choose to use an alternative version of libdialog by setting the
option **ALTERNATIVE\_DIALOG** in the cmake command.
```
cmake -DALTERNATIVE_DIALOG=ON ..
```

