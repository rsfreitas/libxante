# Authentication database

This document describe the database used to authenticate users using an
application.

## Format

Sqlite is the database format used.

## Tables

* user

| Column name | Type |
| :---------- | :--- |
| id | int (primary key) |
| login | char (128) |
| name | char (128) |
| password | char (256) |

* group

| Column name | Type |
| :---------- | :--- |
| id | int (primary key) |
| name | char (128) |

* group\_user\_rel

| Column name | Type |
| :---------- | :--- |
| id\_user | int |
| id\_group | int |

* application

| Column name | Type |
| :---------- | :--- |
| id | int (primary key) |
| name | char (128) |

## Internal tables

* tool

| Column name | Type |
| :---------- | :--- |
| id | int |
| name | char (128) |

* source

| Column name | Type |
| :---------- | :--- |
| id | int |
| name | char (128) |

* session\_type

| Column name | Type |
| :---------- | :--- |
| id | int |
| name | char (128) |

* update\_history

| Column name | Type |
| :---------- | :--- |
| id | int (primary key) |
| update | datetime |
| id\_tool | int |

* jxdbi\_source

| Column name | Type |
| :---------- | :--- |
| id | int (primary key) |
| version |  int |
| filename | char (256) |
| id\_application | int |

* session\_history

| Column name | Type |
| :---------- | :--- |
| id | int (primary key) |
| id\_user | int |
| id\_source | int |
| id\_session\_type | int |
| login | datetime |
| logout | datetime |

* session

| Column name | Type |
| :---------- | :--- |
| id | int (primary key) |
| id\_user | int |
| id\_source | int |
| id\_session\_type | int |
| login | datetime |

