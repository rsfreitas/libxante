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
            "beta": boolean
        }
    },
    "general": {
        "name": string,
        "description": string,
        "module": string,
        "config_pathname": string,
        "log_pathname": string,
        "log_level": string,
        "company": string,
        "blocked_keys": {
            "esc": boolean,
            "stop_key": boolean,
            "suspend_key": boolean
        }
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
                        "item": string
                    }
                },
                "events": {
                    "selected": string,
                    "exit": string
                },
                "geometry": {
                    "width": int,
                    "height": int
                },
                "items": [
                    {
                        "name": string,
                        "object_id": string,
                        "type": string,
                        "mode": int,
                        "data": {
                            "referenced_menu": string,
                            "options": string/array of strings,
                            "default_value": string
                            "ranges": {
                                "string_length": int,
                                "max": int/float,
                                "min": int/float
                            },
                            "config": {
                                "block": string,
                                "item": string
                            }
                        },
                        "ui": {
                            "btn_extra": boolean,
                            "labels": {
                                "title": string,
                                "buttons": {
                                    "ok": string,
                                    "cancel": string,
                                    "help": string,
                                    "extra": string
                                },
                                "help": {
                                    "brief": string,
                                    "description": string,
                                    "options": array of strings
                                }
                            },
                            "geometry": {
                                "width": int,
                                "height": int
                            }
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
