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
        "revision":,
        "language":
    },
    "general": {
        "name":
        "plugin":
        "config_pathname":
        "version":
        "revision":
        "build":
        "beta":
    },
    "main_menu": "object_id",
    "ui": {
        "menus": [
            {
                "name": "Main",
                "object_id": "",
                "id": 9999,
                "itens": [
                    {
                        "default_value":,
                        "config_block":,
                        "config_name":,
                        "type":
                    }
                ]
            },
            {
                "name": "Debug",
                "object_id":
                "id":
                "itens": [
                    {
                        "default_value":,
                        "config_block":,
                        "config_name":,
                        "type":
                    }
                ]
            },
            {
                "name": "Help",
                "object_id":
                "id":
                "itens": [
                    {
                        "default_value":,
                        "config_block":,
                        "config_name":,
                        "type":
                    }
                ]
            }
        ]
    }
}
```
