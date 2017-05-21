# JSON template files

This document describe the specifications of a JSON Template File (JTF),
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
        "plugin": string,
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
                        "mode": int,
                        "default_value": string,
                        "config": {
                            "block": string,
                            "item": string,
                        },
                        "type": string,
                        "options": string/array of strings,
                        "name": string,
                        "help": {
                            "brief": string,
                            "description": string,
                            "options": array of strings
                        },
                        "input_ranges": {
                            "string_length": int,
                            "max": int/float,
                            "min": int/float,
                        },
                        "events": {
                            "selected": string,
                            "value_cofirmed": string,
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
