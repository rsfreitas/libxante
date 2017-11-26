# JXDBI - JSON Xanter Database Information

This document describes de specfications of a JSON Xanter Database Information
file (JXDBI), used to populate a libxanter's database with information from
a specific application.

## Format

A JXDBI file is a JSON file with specific fields, extracted from an
application JTF file.

## Details
```
{
    "application": {
        "name": string,
        "version": string,
        "revision": int
    },
    "ui": [
        {
            "name": string,
            "object_id": string,
            "items": [
                "name": string,
                "object_id": string,
                "type": string
            ]
        }
    ]
}
```

