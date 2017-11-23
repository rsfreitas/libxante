# MJTF Dialog

This document describes a JTF format specifically used to build and run
dialogs inside an already running application, called here as a
Minimum JTF (MJTF).

With this mechanism one may build custom dialogs to be called on an
active application.

Its format is very similar to the JTF file, but instead having all
specific application informations it only has the dialog part, with a
few modifications.

While in the JTF you must have all information about any accessible
option described in details and inside an array of menus, in this kind
of MJTF you can put only one specific kind of dialog to be built or
even a sequence of dialogs to be accessible through the first one.

## Details

### Specific dialog
```
{
    "item": {
        "name": string,
        "type": string,
        "mode": int,
        "menu_id": string,
        "default_value": string,
        "options": string/array of strings,
        "config": {
            "block": string,
            "item": string,
        },
        "help": {
            "brief": string,
            "description": string,
            "options": array of strings
        },
        "ranges": {
            "string_length": int,
            "max": int/float,
            "min": int/float,
        },
        "events": {
            "selected": string,
            "value_confirmed": string,
            "value_changed": string,
            "exit": string
        }
    }
}
```

Note that with a single item one does not need to configure the _object\_id_ of
an item.

### Sequnce of dialogs

**Notice** that this format is exactly the same of the JTF.

```
{
    "main_menu": string,
    "menus": [
        {
            "name": string,
            "object_id": string,
            "mode": int,
            "type": string,
            "dynamic": {
                "copies": int/array of strings,
                "block_prefix": string,
                "origin": {
                    "block": string,
                    "item": string,
                }
            },
            "events": {
                "selected": string,
                "exit": string,
            },
            "items": [
                {
                    "name": string,
                    "object_id": string,
                    "type": string,
                    "mode": int,
                    "menu_id": string,
                    "default_value": string,
                    "options": string/array of strings,
                    "config": {
                        "block": string,
                        "item": string,
                    },
                    "help": {
                        "brief": string,
                        "description": string,
                        "options": array of strings
                    },
                    "ranges": {
                        "string_length": int,
                        "max": int/float,
                        "min": int/float,
                    },
                    "events": {
                        "selected": string,
                        "value_confirmed": string,
                        "value_changed": string,
                        "exit": string
                    }
                }
            ]
        }
    ]
}
```
