# libxante

A library to build (dialog based) applications using JSON templates and
plugins.

## License

LGPLv2

## Overview

This library is intended to build and run dialog based applications.

## Templates

## Plugins

### Plugin events

## Dependencies

* [libcollections](https://github.com/rsfreitas/libcollections)
* [libdialog](http://invisible-island.net/dialog/)
* [sqlite](http://www.sqlite.org)

### Alternative libdialog

We may choose to use an alternative version of libdialog by setting the
option **ALTERNATIVE\_DIALOG** in the make command.
```
make ALTERNATIVE_DIALOG=1
```

## Bindings

## Recomendations

- Every internal libdialog based function must start, i.e., must have the
same prefix **dlgx\_**.

