
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Fri Dec 22 20:57:29 2017
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

#include "libxante.h"

/*
 *
 * Internal API
 *
 */

char *widget_statusbar_text(enum xante_object type, bool editable,
    bool block_esc_key)
{
    cl_string_t *t = NULL;
    char *text = NULL;

    if (block_esc_key)
        t = cl_string_create_empty(0);
    else {
        switch (type) {
            case XANTE_WIDGET_MENU_REFERENCE:
            case XANTE_WIDGET_DYNAMIC_MENU_REFERENCE:
                t = cl_string_create("%s", cl_tr("[ESC] Exit "));
                break;

            case XANTE_WIDGET_PROGRESS:
            case XANTE_WIDGET_SPINNER_SYNC:
            case XANTE_WIDGET_DOTS_SYNC:
                t = cl_string_create_empty(0);
                break;

            default:
                t = cl_string_create("%s", cl_tr("[ESC] Cancel "));
                break;
        }
    }

    switch (type) {
        case XANTE_WIDGET_MENU_REFERENCE:
        case XANTE_WIDGET_DYNAMIC_MENU_REFERENCE:
            cl_string_cat(t, "%s",
                          cl_tr("[Enter] Select an option "
                                "[Up/Down] Move [TAB/Left/Right] Choose button"));

            break;

        case XANTE_WIDGET_INPUT_INT:
        case XANTE_WIDGET_INPUT_FLOAT:
        case XANTE_WIDGET_INPUT_DATE:
        case XANTE_WIDGET_INPUT_STRING:
        case XANTE_WIDGET_INPUT_TIME:
        case XANTE_WIDGET_INPUT_PASSWD:
        case XANTE_WIDGET_INPUTSCROLL:
            cl_string_cat(t, "%s",
                          cl_tr("[Enter] Confirm a selected option "
                                "[Tab/Left/Right] Select an option"));

            break;

        case XANTE_WIDGET_RANGE:
            cl_string_cat(t, "%s",
                          cl_tr("[Enter] Confirm selected option "
                                "[Tab/Left/Right] Select an option "
                                "[Up/Down] Adjust range value"));

            break;

        case XANTE_WIDGET_CALENDAR:
            if (editable) {
                cl_string_cat(t, "%s",
                              cl_tr("[TAB] Select an option "
                                    "[Enter] Confirm [Arrows] Change selected date"));
            } else
                cl_string_cat(t, "%s",
                              cl_tr("[TAB] Select an option "
                                    "[Enter] Confirm"));

            break;

        case XANTE_WIDGET_TIMEBOX:
            if (editable) {
                cl_string_cat(t, "%s",
                              cl_tr("[TAB] Select an option "
                                    "[Enter] Confirm [Arrows] Change selected time"));
            } else
                cl_string_cat(t, "%s",
                              cl_tr("[TAB/Arrows] Select an option "
                                    "[Enter] Confirm"));

            break;

        case XANTE_WIDGET_RADIO_CHECKLIST:
        case XANTE_WIDGET_CHECKLIST:
            if (editable) {
                cl_string_cat(t, "%s",
                              cl_tr("[Enter] Confirm a selection "
                                    "[Up/Down] Move [TAB/Left/Right] Choose option "
                                    "[Spacebar] Select option"));
            } else
                cl_string_cat(t, "%s",
                              cl_tr("[Enter] Confirm a selection "
                                    "[Up/Down] Move [TAB/Left/Right] Choose option"));

            break;

        case XANTE_WIDGET_YES_NO:
            cl_string_cat(t, "%s",
                          cl_tr("[Enter] Confirm a selected option "
                                "[Tab/Left/Right] Select an option"));

            break;

        case XANTE_WIDGET_DELETE_DYNAMIC_MENU_ITEM:
            if (editable) {
                cl_string_cat(t, "%s",
                              cl_tr("[Enter] Confirm a selection "
                                    "[Up/Down] Move [TAB/Left/Right] Choose option "
                                    "[Spacebar] Select option"));
            } else
                cl_string_cat(t, "%s",
                              cl_tr("[Enter] Confirm a selection "
                                    "[Up/Down] Move [TAB/Left/Right] Choose option"));

            break;

        case XANTE_WIDGET_ADD_DYNAMIC_MENU_ITEM:
            cl_string_cat(t, "%s",
                          cl_tr("[Enter] Confirm a selected option "
                                "[Tab/Left/Right] Select an option"));

            break;

        case XANTE_WIDGET_PROGRESS:
            cl_string_cat(t, "%s", cl_tr("Wait the process to end."));
            break;

        case XANTE_WIDGET_SPINNER_SYNC:
        case XANTE_WIDGET_DOTS_SYNC:
            cl_string_cat(t, "%s",
                          cl_tr("A task is running in background. Wait for it to end."));

            break;

        case XANTE_WIDGET_FILE_SELECT:
        case XANTE_WIDGET_DIR_SELECT:
            cl_string_cat(t, "%s",
                          cl_tr("[Enter] Confirm selected button "
                                "[Tab/Arrows] Navigate through options "
                                "[Spacebar] Select dir/file"));

            break;

        case XANTE_WIDGET_FILE_VIEW:
            cl_string_cat(t, "%s",
                          cl_tr("[Enter] Confirm selected button "
                                "[Up/Down/Pg Up/Pg Down] Navigate through the text"));

            break;

        case XANTE_WIDGET_TAILBOX:
            cl_string_cat(t, "%s",
                          cl_tr("[Enter] Confirm selected button"));

            break;

        case XANTE_WIDGET_SCROLLTEXT:
            cl_string_cat(t, "%s",
                          cl_tr("[Enter] Confirm selected button "
                                "[Up/Down/Pg Up/Pg Down] Navigate through the text"));

            break;

        case XANTE_WIDGET_UPDATE_OBJECT:
            cl_string_cat(t, "%s",
                          cl_tr("[Enter] Confirm a selected option"));

            break;

        case XANTE_WIDGET_MIXEDFORM:
            cl_string_cat(t, "%s",
                          cl_tr("[Enter] Confirm a selected option "
                                "[Tab/Left/Right] Select an option "
                                "[Up/Down] Select field"));

            break;

        case XANTE_WIDGET_BUILDLIST:
            cl_string_cat(t, "%s",
                          cl_tr("[Enter] Confirm button "
                                "[Spacebar] Move option between lists "
                                "[^] Select origin [$] Select destination"));

            break;

        case XANTE_WIDGET_SPREADSHEET:
            cl_string_cat(t, "%s",
                          cl_tr("[Enter] Confirm selected button "
                                "[Tab] Navigate through cells"));

            break;

        default:
            break;
    }

    text = strdup(cl_string_valueof(t));
    cl_string_unref(t);

    return text;
}

