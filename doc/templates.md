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
        "revision": int
        "language": string
    },
    "general": {
        "name": string
        "plugin": string
        "config_pathname": string
        "log_pathname": string
        "log_level": string
        "version": string
        "revision": int
        "build": int
        "beta": bool
    },
    "ui": {
        "main_menu": string
        "menus": [
            {
                "name": string
                "object_id": string
                "mode": int
                "items": [
                    {
                        "mode": int
                        "default_value": string
                        "config_block": string
                        "config_name": string
                        "type": string
                        "options": string
                        "name": string
                    }
                ]
            },
            {
                "name": string
                "object_id": string
                "items": [
                    {
                        "mode": int
                        "default_value": string
                        "config_block": string
                        "config_name": string
                        "type": string
                        "options": string
                        "name": string
                    }
                ]
            },
            {
                "name": string
                "object_id": string
                "items": [
                    {
                        "mode": int
                        "default_value": string
                        "config_block": string
                        "config_name": string
                        "type": string
                        "options": string
                        "name": string
                    }
                ]
            }
        ]
    }
}
```
