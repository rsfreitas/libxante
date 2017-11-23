# JTF - JSON template files

This document describes the specifications of a JSON Template File (JTF),
used by **libxante** to build an application.

## Format

A JTF is, as its own name says, a JSON file, with specific fields so that
it may reference an application skeleton.

## Details

```
{
    "internal": {
        "revision": int,
        "language": string,
        "application": {
            "version": string,
            "revision": int,
            "build": int,
            "beta": bool
        }
    },
    "general": {
        "name": string,
        "description": string,
        "module": string,
        "config_pathname": string,
        "log_pathname": string,
        "log_level": string,
        "company": string
    },
    "ui": {
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
}
```
