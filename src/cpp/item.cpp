
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Jun 10 11:09:09 2017
 * Project: libxante
 *
 * Copyright (C) 2017 Rodrigo Freitas
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QMap>

#include "libxante.h"

/*
 *
 * XanteItem
 *
 */

void XanteItem::name(QString name)
{
    m_name = name;
    m_objectId = XanteJTF::objectIdCalc(m_applicationName, m_menuName, name);
}

void XanteItem::writeOptions(QJsonObject &root) const
{
    if ((m_type == XanteItem::Type::RadioChecklist) ||
        (m_type == XanteItem::Type::Checklist))
    {
        QJsonArray options;

        for (int i = 0; i < m_options.size(); i++)
            options.append(m_options.at(i));

        root[XANTE_JTF_OPTIONS] = options;
    } else
        root[XANTE_JTF_OPTIONS] = m_fixedOption;
}

QJsonObject XanteItem::writeInputRanges(void) const
{
    QJsonObject inputRanges;

    if (m_type == XanteItem::Type::InputString)
        inputRanges[XANTE_JTF_STRING_LENGTH] = m_stringLength;
    else {
        if (m_type == XanteItem::Type::InputInt) {
            inputRanges[XANTE_JTF_MAX_RANGE] = m_maxInputRange.toInt();
            inputRanges[XANTE_JTF_MIN_RANGE] = m_minInputRange.toInt();
        } else if (m_type == XanteItem::Type::InputFloat) {
            inputRanges[XANTE_JTF_MAX_RANGE] = m_maxInputRange.toFloat();
            inputRanges[XANTE_JTF_MIN_RANGE] = m_minInputRange.toFloat();
        }
    }

    return inputRanges;
}

QJsonObject XanteItem::writeConfig(void) const
{
    QJsonObject config;

    config[XANTE_JTF_BLOCK] = m_configBlock;
    config[XANTE_JTF_ITEM] = m_configItem;

    return config;
}

QJsonObject XanteItem::writeEvents(void) const
{
    QJsonObject events;
    QMap<QString, enum XanteItem::Event> evMap;

    evMap.insert(EV_ITEM_SELECTED, XanteItem::Event::Selected);
    evMap.insert(EV_ITEM_EXIT, XanteItem::Event::Exit);
    evMap.insert(EV_ITEM_VALUE_CONFIRM, XanteItem::Event::ValueConfirmed);
    evMap.insert(EV_ITEM_VALUE_UPDATED, XanteItem::Event::ValueChanged);

    QMapIterator<QString, enum XanteItem::Event> i(evMap);

    while (i.hasNext()) {
        i.next();
        QString evValue = m_events.value(i.value(), QString(""));

        if (evValue.isEmpty() == false)
            events[i.key()] = evValue;
    }

    return events;
}

QJsonObject XanteItem::writeHelp(void) const
{
    QJsonObject help;

    if (m_briefHelp.isEmpty() == false)
        help[XANTE_JTF_BRIEF] = m_briefHelp;

    if (m_descriptiveHelp.isEmpty() == false)
        help[XANTE_JTF_DESCRIPTION] = m_descriptiveHelp;

    if (((m_type == XanteItem::Type::Checklist) ||
         (m_type == XanteItem::Type::RadioChecklist)) &&
        m_helpOptions.size() != 0)
    {
        QJsonArray options;
        int i, t = m_helpOptions.size();

        for (i = 0; i < t; i++)
            options.append(m_helpOptions.at(i));

        help[XANTE_JTF_OPTIONS] = options;
    }

    return help;
}

/*
 * Writes the current content of a XanteItem object into a JSON object.
 */
void XanteItem::write(QJsonObject &root) const
{
    root[XANTE_JTF_NAME] = m_name;
    root[XANTE_JTF_OBJECT_ID] = m_objectId;
    root[XANTE_JTF_MODE] = m_mode;
    root[XANTE_JTF_TYPE] = m_typeDescription.value(m_type);

    if (m_defaultValue.isEmpty() == false)
        root[XANTE_JTF_DEFAULT_VALUE] = m_defaultValue;

    if ((m_type == XanteItem::Type::Menu) ||
        (m_type == XanteItem::Type::DeleteDynamicMenu))
    {
        root[XANTE_JTF_REFERENCED_MENU] = m_menuReferenceId;
    }

    if (hasOptions())
        writeOptions(root);

    if (hasInputRanges())
        root[XANTE_JTF_RANGES] = writeInputRanges();

    if (hasConfig())
        root[XANTE_JTF_CONFIG] = writeConfig();

    if (hasEvents())
        root[XANTE_JTF_EVENTS] = writeEvents();

    if (hasHelp())
        root[XANTE_JTF_HELP] = writeHelp();
}

void XanteItem::preLoad(void)
{
    const char *item_name[] = {
        XANTE_STR_WIDGET_MENU,
        XANTE_STR_WIDGET_INPUT_INT,
        XANTE_STR_WIDGET_INPUT_FLOAT,
        XANTE_STR_WIDGET_INPUT_DATE,
        XANTE_STR_WIDGET_INPUT_TIME,
        XANTE_STR_WIDGET_INPUT_STRING,
        XANTE_STR_WIDGET_INPUT_PASSWD,
        XANTE_STR_WIDGET_CALENDAR,
        XANTE_STR_WIDGET_TIMEBOX,
        XANTE_STR_WIDGET_RADIO_CHECKLIST,
        XANTE_STR_WIDGET_CHECKLIST,
        XANTE_STR_WIDGET_YESNO,
        XANTE_STR_WIDGET_DYNAMIC_MENU,
        XANTE_STR_WIDGET_DELETE_DYNAMIC_MENU,
        XANTE_STR_WIDGET_ADD_DYNAMIC_MENU,
        XANTE_STR_WIDGET_CUSTOM,
        XANTE_STR_WIDGET_PROGRESS,
        XANTE_STR_WIDGET_SPINNER_SYNC,
        XANTE_STR_WIDGET_DOTS_SYNC,
        XANTE_STR_WIDGET_RANGE,
        XANTE_STR_WIDGET_FILE_SELECT,
        XANTE_STR_WIDGET_DIR_SELECT,
        XANTE_STR_WIDGET_FILE_VIEW,
        XANTE_STR_WIDGET_TAILBOX,
        XANTE_STR_WIDGET_SCROLLTEXT,
        XANTE_STR_WIDGET_UPDATE_OBJECT,
        XANTE_STR_WIDGET_INPUTSCROLL,
        XANTE_STR_WIDGET_MIXEDFORM,
        XANTE_STR_WIDGET_BUILDLIST,
        XANTE_STR_WIDGET_SPREADSHEET,
        XANTE_STR_GADGET_CLOCK,
    };

    for (int i = XanteItem::Type::Menu, j = 0; i < XanteItem::Type::MaxTypes; i++, j++)
        m_typeDescription.insert((enum XanteItem::Type)i, QString(item_name[j]));
}

/**
 * @name XanteItem
 * @brief A XanteItem object constructor.
 *
 * This constructor is used to create a XanteItem object when the user requires.
 *
 * @param [in] applicationName: The current application name.
 * @param [in] menuName: The menu name which this item will belong to.
 * @param [in] name: The item name.
 */
XanteItem::XanteItem(QString applicationName, QString menuName, QString name)
    : m_applicationName(applicationName), m_menuName(menuName), m_name(name)
{
    preLoad();

    /*
     * Altough we already initialized the @m_name, the name() function is called
     * here so the item objectId is also initialized.
     */
    this->name(name);
    mode(XanteAccessMode::XanteAccessEdit);
    type(XanteItem::Type::Menu);
}

enum XanteItem::Type XanteItem::toXanteItem(const QString &type)
{
    return m_typeDescription.key(type, XanteItem::Type::Unknown);
}

void XanteItem::parseCommonData(QJsonObject item)
{
    int tmp;

    /* Load item from JSON */

    m_name = item[XANTE_JTF_NAME].toString();
    m_objectId = item[XANTE_JTF_OBJECT_ID].toString();
    m_defaultValue = item[XANTE_JTF_DEFAULT_VALUE].toString();
    m_menuReferenceId = item[XANTE_JTF_REFERENCED_MENU].toString();

    tmp = item[XANTE_JTF_MODE].toInt();
    m_mode = (enum XanteAccessMode)tmp;
    m_type = toXanteItem(item[XANTE_JTF_TYPE].toString());

    QJsonValue value = item[XANTE_JTF_OPTIONS];

    if (value.type() == QJsonValue::String)
        m_fixedOption = value.toString();
    else {
        QJsonArray options = value.toArray();

        foreach(const QJsonValue &v, options)
            m_options.append(v.toString());
    }
}

void XanteItem::parseEventsData(QJsonObject item)
{
    QJsonValue value = item[XANTE_JTF_EVENTS];

    if ((value.type() != QJsonValue::Undefined) &&
        (value.type() != QJsonValue::Null))
    {
        QJsonObject events = value.toObject();
        QMap<QString, enum XanteItem::Event> evMap;

        evMap.insert(EV_ITEM_SELECTED, XanteItem::Event::Selected);
        evMap.insert(EV_ITEM_EXIT, XanteItem::Event::Exit);
        evMap.insert(EV_ITEM_VALUE_CONFIRM, XanteItem::Event::ValueConfirmed);
        evMap.insert(EV_ITEM_VALUE_UPDATED, XanteItem::Event::ValueChanged);

        QMapIterator<QString, enum XanteItem::Event> i(evMap);

        while (i.hasNext()) {
            i.next();
            value = events[i.key()];

            if (value.type() == QJsonValue::String)
                m_events.insert(i.value(), value.toString());
        }
    }
}

void XanteItem::parseConfigData(QJsonObject item)
{
    QJsonValue value = item[XANTE_JTF_CONFIG];

    if (value.type() != QJsonValue::Object)
        return;

    QJsonObject config = value.toObject();

    m_configBlock = config[XANTE_JTF_BLOCK].toString();
    m_configItem = config[XANTE_JTF_ITEM].toString();
}

void XanteItem::parseRangesData(QJsonObject item)
{
    QJsonValue value = item[XANTE_JTF_RANGES];

    if (value.type() != QJsonValue::Object)
        return;

    QJsonObject inputRanges = value.toObject();

    m_stringLength = inputRanges[XANTE_JTF_STRING_LENGTH].toInt();
    m_minInputRange = QVariant(inputRanges[XANTE_JTF_MIN_RANGE].toDouble());
    m_maxInputRange = QVariant(inputRanges[XANTE_JTF_MAX_RANGE].toDouble());
}

void XanteItem::parseHelpData(QJsonObject item)
{
    QJsonValue value = item[XANTE_JTF_HELP];

    if (value.type() != QJsonValue::Object)
        return;

    QJsonObject help = value.toObject();

    m_briefHelp = help[XANTE_JTF_BRIEF].toString();
    m_descriptiveHelp = help[XANTE_JTF_DESCRIPTION].toString();

    QJsonArray options = help[XANTE_JTF_OPTIONS].toArray();

    foreach(const QJsonValue &v, options)
        m_helpOptions.append(v.toString());
}

void XanteItem::parse(QJsonObject item)
{
    parseCommonData(item);
    parseEventsData(item);
    parseConfigData(item);
    parseRangesData(item);
    parseHelpData(item);
}

/**
 * @name XanteItem
 * @brief A XanteItem object constructor.
 *
 * This constructor is used to create a XanteItem object when the user requires.
 *
 * @param [in] applicationName: The current application name.
 * @param [in] menuName: The menu name which this item will belong to.
 * @param [in] item: The item information in JSON format.
 */
XanteItem::XanteItem(QString applicationName, QString menuName,
    QJsonObject item)
    : m_applicationName(applicationName), m_menuName(menuName)
{
    preLoad();
    parse(item);
}

