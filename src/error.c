
/*
 * Description: Functions to handle the library internal error.
 *
 * Author: Rodrigo Freitas
 * Created at: Tue May  2 20:25:58 2017
 * Project: libxante
 *
 * Copyright (C) 2017 Rodrigo Freitas
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 */

#include <pthread.h>

#include "libxante.h"

#define MAX_ADDITIONAL_CONTENT          8
#define MAX_CONTENT_SIZE                32

/*
 * We use a struct to hold the library internal error code and a buffer to
 * store an error message in case the current error code has a secondary one.
 */
struct xante_storage_error {
    int     code;
    char    message[512];

    /*
     * These are used to add more information into a descriptive error message.
     *
     * @additional_content stores the number of contents to be concatenated.
     * @content stores the content.
     */
    int     additional_content;
    char    content[MAX_ADDITIONAL_CONTENT][MAX_CONTENT_SIZE];
};

/*
 * Messages with an asterisk has one or more contents to be concatenated with
 * it.
 */
static const char *__description[] = {
    cl_tr_noop("Ok"),
    cl_tr_noop("No internal memory available"),
    cl_tr_noop("A NULL argument was passed"),
    cl_tr_noop("An invalid argument was passed"),
    cl_tr_noop("Unable to parse JTF file. Wrong format."),
    cl_tr_noop("JTF file has no 'general' object"),
    cl_tr_noop("JTF object was not found"),                             //*
    cl_tr_noop("JTF object has no value"),                              //*
    cl_tr_noop("JTF file has no 'items' object"),
    cl_tr_noop("JTF file has no 'menu' object"),
    cl_tr_noop("JTF file has no 'ui' object"),
    cl_tr_noop("UI has not been initialized"),
    cl_tr_noop("JTF file has no 'input_ranges' object"),
    cl_tr_noop("JTF file has no 'config' object"),
    cl_tr_noop("JTF file has no 'internal' object"),
    cl_tr_noop("JTF file has no 'application' object"),
    cl_tr_noop("Plugin initialization failed"),
    cl_tr_noop("Plugin without internal info"),
    cl_tr_noop("Plugin without internal API"),
    cl_tr_noop("Plugin without mandatory function"),
    cl_tr_noop("Plugin EV_INIT error"),
    cl_tr_noop("No environment database variable found"),
    cl_tr_noop("Unable to access database file"),
    cl_tr_noop("Unable to save JXDBI file"),
    cl_tr_noop("Unable to open database file"),
    cl_tr_noop("User not found in the database"),
    cl_tr_noop("Group not found in the database"),
    cl_tr_noop("Application not found in the database"),
    cl_tr_noop("Multiple user entries found in the database"),
    cl_tr_noop("Multiple group entries found in the database"),
    cl_tr_noop("Multiple application entries found in the database"),
    cl_tr_noop("Unable to retrieve login information"),
    cl_tr_noop("Database already exists"),
    cl_tr_noop("JTF menu has no 'dynamic' object"),
    cl_tr_noop("JTF menu has no 'origin' object"),
    cl_tr_noop("Wrong JTF object type"),                                //*
    cl_tr_noop("Unknown event data type"),
    cl_tr_noop("An instance of this application is already running"),
    cl_tr_noop("Internal invalid data conversion"),
    cl_tr_noop("The MJTF has no object to be loaded"),
    cl_tr_noop("Menu was not found"),                                   //*
    cl_tr_noop("The head of menu list was not found"),
    cl_tr_noop("Item was not found"),                                   //*
    cl_tr_noop("Invalid form JSON"),                                    //*
    cl_tr_noop("Item has no data object"),                              //*
};

static const char *__unknown_error = cl_tr_noop("Unknown error");

cl_error_storage_declare(__storage__, sizeof(struct xante_storage_error))
#define __errno     (cl_errno_storage(&__storage__))
/*
 *
 * Internal API
 *
 */

/**
 * @name errno_clear
 * @brief Clears the library internal error code.
 */
void errno_clear(void)
{
    struct xante_storage_error *ste = (struct xante_storage_error *)__errno;

    ste->code = XANTE_NO_ERROR;
    ste->additional_content = 0;
}

/**
 * @name errno_set
 * @brief Sets the current value of the library internal error code.
 *
 * @param [in] code: The new errorcode.
 */
void errno_set(enum xante_error_code code)
{
    struct xante_storage_error *ste = (struct xante_storage_error *)__errno;

    ste->code = code;
}

/**
 * @name errno_store_additional_content
 * @brief Adds a content to be concatenated into the error message.
 *
 * @param [in] content: The additional content.
 */
void errno_store_additional_content(const char *content)
{
    struct xante_storage_error *ste = (struct xante_storage_error *)__errno;
    char *ptr = ste->content[ste->additional_content];

    /* We reached the limit */
    if (ste->additional_content == (MAX_ADDITIONAL_CONTENT - 1))
        return;

    memset(ptr, 0, MAX_CONTENT_SIZE);
    strncpy(ptr, content, min(strlen(content), MAX_CONTENT_SIZE));
    ste->additional_content++;
}

/*
 *
 * API
 *
 */

__PUB_API__ enum xante_error_code xante_get_last_error(void)
{
    struct xante_storage_error *ste = (struct xante_storage_error *)__errno;

    return ste->code;
}

__PUB_API__ const char *xante_strerror(enum xante_error_code code)
{
    struct xante_storage_error *ste = (struct xante_storage_error *)__errno;
    int i;

    if (code >= XANTE_MAX_ERROR_CODE)
        return __unknown_error;

    if (ste->additional_content > 0) {
        /* We use the storage error struct to hold the message */
        memset(ste->message, 0, sizeof(ste->message));
        strcpy(ste->message, __description[code]);

        for (i = 0; i < ste->additional_content; i++) {
            strcat(ste->message, ": ");
            strcat(ste->message, ste->content[i]);
        }

        return ste->message;
    }

    return __description[code];
}

