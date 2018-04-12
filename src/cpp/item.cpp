
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
        (m_type == XanteItem::Type::Checklist) ||
        (m_type == XanteItem::Type::Buildlist))
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

    switch (m_type) {
        case XanteItem::Type::InputString:
        case XanteItem::Type::InputPasswd:
        case XanteItem::Type::Inputscroll:
            inputRanges[XANTE_JTF_STRING_LENGTH] = m_stringLength;
            break;

        case XanteItem::Type::InputInt:
        case XanteItem::Type::Range:
            inputRanges[XANTE_JTF_MAX_RANGE] = m_maxInputRange.toInt();
            inputRanges[XANTE_JTF_MIN_RANGE] = m_minInputRange.toInt();
            break;

        case XanteItem::Type::InputFloat:
            inputRanges[XANTE_JTF_MAX_RANGE] = m_maxInputRange.toFloat();
            inputRanges[XANTE_JTF_MIN_RANGE] = m_minInputRange.toFloat();
            break;

        case XanteItem::Type::UpdateObject:
        case XanteItem::Type::Progress:
        case XanteItem::Type::SpinnerSync:
        case XanteItem::Type::DotsSync:
            inputRanges[XANTE_JTF_MAX_RANGE] = m_maxInputRange.toInt();
            break;

        default:
            break;
    }

    return inputRanges;
}

QJsonObject XanteItem::writeConfig(void) const
{
    QJsonObject config;

    config[XANTE_JTF_BLOCK] = m_configBlock;
    config[XANTE_JTF_ITEM] = m_configEntry;

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

QJsonObject XanteItem::writeData(void) const
{
    QJsonObject data;

    if (hasInputRanges())
        data[XANTE_JTF_RANGES] = writeInputRanges();

    if (hasConfig())
        data[XANTE_JTF_CONFIG] = writeConfig();

    if (hasOptions())
        writeOptions(data);

    if (m_defaultValue.isEmpty() == false)
        data[XANTE_JTF_DEFAULT_VALUE] = m_defaultValue;

    if ((m_type == XanteItem::Type::Menu) ||
        (m_type == XanteItem::Type::DeleteDynamicMenu))
    {
        data[XANTE_JTF_REFERENCED_MENU] = m_menuReferenceId;
    }

    return data;
}

QJsonObject XanteItem::writeButtons(void) const
{
    QJsonObject buttons;

    if (m_btnOk.isEmpty() == false)
        buttons[XANTE_JTF_BTN_OK] = m_btnOk;

    if (m_btnCancel.isEmpty() == false)
        buttons[XANTE_JTF_BTN_CANCEL] = m_btnCancel;

    if (m_btnExtra.isEmpty() == false)
        buttons[XANTE_JTF_BTN_EXTRA] = m_btnExtra;

    if (m_btnHelp.isEmpty() == false)
        buttons[XANTE_JTF_BTN_HELP] = m_btnHelp;

    return buttons;
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

QJsonObject XanteItem::writeLabels(void) const
{
    QJsonObject labels;

    if (hasButtons())
        labels[XANTE_JTF_BUTTONS] = writeButtons();

    if (hasHelp())
        labels[XANTE_JTF_HELP] = writeHelp();

    if (m_title.isEmpty() == false)
        labels[XANTE_JTF_TITLE] = m_title;

    return labels;
}

QJsonObject XanteItem::writeGeometry(void) const
{
    QJsonObject geometry;

    if (m_width != -1)
        geometry[XANTE_JTF_WIDTH] = m_width;

    if (m_height != -1)
        geometry[XANTE_JTF_HEIGHT] = m_height;

    return geometry;
}

QJsonObject XanteItem::writeUi(void) const
{
    QJsonObject ui;

    if (hasLabels())
        ui[XANTE_JTF_LABELS] = writeLabels();

    if (hasGeometry())
        ui[XANTE_JTF_GEOMETRY] = writeGeometry();

    if (m_btnExtraEnabled)
        ui[XANTE_JTF_BTN_EXTRA] = m_btnExtraEnabled;

    return ui;
}

/*
 * Writes the current content of a XanteItem object into a JSON object.
 */
void XanteItem::write(QJsonObject &root) const
{
#ifdef DEBUG
    qDebug() << debug();
#endif

    root[XANTE_JTF_NAME] = m_name;
    root[XANTE_JTF_OBJECT_ID] = m_objectId;
    root[XANTE_JTF_MODE] = m_mode;
    root[XANTE_JTF_TYPE] = m_typeDescription.value(m_type);

    if (hasData())
        root[XANTE_JTF_DATA] = writeData();

    if (hasUi())
        root[XANTE_JTF_UI] = writeUi();

    if (hasEvents())
        root[XANTE_JTF_EVENTS] = writeEvents();
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
        "gadget:" XANTE_STR_GADGET_CLOCK,
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
}

enum XanteItem::Type XanteItem::toXanteItem(const QString &type)
{
    return m_typeDescription.key(type, XanteItem::Type::Unknown);
}

/*
 * Parses the main objects from an item.
 */
void XanteItem::parseCommonData(QJsonObject item)
{
    int tmp;

    /* Load item from JSON */
    m_name = item[XANTE_JTF_NAME].toString();
    m_objectId = item[XANTE_JTF_OBJECT_ID].toString();

    if (m_objectId.isEmpty())
        m_objectId = XanteJTF::objectIdCalc(m_applicationName, m_menuName);

    tmp = item[XANTE_JTF_MODE].toInt();
    m_mode = (enum XanteAccessMode)tmp;
    m_type = toXanteItem(item[XANTE_JTF_TYPE].toString());
}

/*
 * Parses the "events" object from an item.
 */
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

/*
 * Parses the "config" object from the "data" object inside an item.
 */
void XanteItem::parseConfigData(QJsonObject data)
{
    QJsonValue value = data[XANTE_JTF_CONFIG];

    if (value.type() != QJsonValue::Object)
        return;

    QJsonObject config = value.toObject();

    m_configBlock = config[XANTE_JTF_BLOCK].toString();
    m_configEntry = config[XANTE_JTF_ITEM].toString();
}

/*
 * Parses the "ranges" object from the "data" object inside an item.
 */
void XanteItem::parseRangesData(QJsonObject data)
{
    QJsonValue value = data[XANTE_JTF_RANGES];

    if (value.type() != QJsonValue::Object)
        return;

    QJsonObject inputRanges = value.toObject();

    m_stringLength = inputRanges[XANTE_JTF_STRING_LENGTH].toInt();
    m_minInputRange = QVariant(inputRanges[XANTE_JTF_MIN_RANGE].toDouble());
    m_maxInputRange = QVariant(inputRanges[XANTE_JTF_MAX_RANGE].toDouble());
}

/*
 * Parses the "data" object from an item.
 */
void XanteItem::parseData(QJsonObject item)
{
    QJsonValue value = item[XANTE_JTF_DATA];

    if (value.isObject() == false)
        return; // nothing to parse

    QJsonObject data = value.toObject();

    m_menuReferenceId = data[XANTE_JTF_REFERENCED_MENU].toString();
    m_defaultValue = data[XANTE_JTF_DEFAULT_VALUE].toString();
    value = data[XANTE_JTF_OPTIONS];

    if (value.type() == QJsonValue::String)
        m_fixedOption = value.toString();
    else {
        QJsonArray options = value.toArray();

        foreach(const QJsonValue &v, options)
            m_options.append(v.toString());
    }

    parseConfigData(data);
    parseRangesData(data);
}

/*
 * Parses the "buttons" object from the "labels" object inside an item.
 */
void XanteItem::parseButtonsData(QJsonObject labels)
{
    QJsonValue value = labels[XANTE_JTF_LABELS];

    if (value.isObject() == false)
        return; // nothing to parse

    QJsonObject buttons = value.toObject();
    m_btnOk = buttons[XANTE_JTF_BTN_OK_LABEL].toString();
    m_btnCancel = buttons[XANTE_JTF_BTN_CANCEL_LABEL].toString();
    m_btnExtra = buttons[XANTE_JTF_BTN_EXTRA_LABEL].toString();
    m_btnHelp = buttons[XANTE_JTF_BTN_HELP_LABEL].toString();
}

/*
 * Parses the "help" object from the "labels" object inside an item.
 */
void XanteItem::parseHelpData(QJsonObject labels)
{
    QJsonValue value = labels[XANTE_JTF_HELP];

    if (value.type() != QJsonValue::Object)
        return;

    QJsonObject help = value.toObject();

    m_briefHelp = help[XANTE_JTF_BRIEF].toString();
    m_descriptiveHelp = help[XANTE_JTF_DESCRIPTION].toString();

    QJsonArray options = help[XANTE_JTF_OPTIONS].toArray();

    foreach(const QJsonValue &v, options)
        m_helpOptions.append(v.toString());
}

/*
 * Parses the "labels" object from an "ui" object inside an item.
 */
void XanteItem::parseLabelsData(QJsonObject ui)
{
    QJsonValue value = ui[XANTE_JTF_LABELS];

    if (value.isObject() == false)
        return; // nothing to parse

    QJsonObject labels = value.toObject();
    m_title = labels[XANTE_JTF_TITLE].toString();

    parseButtonsData(labels);
    parseHelpData(labels);
}

/*
 * Parses the "geometry" object inside a menu.
 */
void XanteItem::parseGeometryData(QJsonObject ui)
{
    QJsonValue value = ui[XANTE_JTF_GEOMETRY];

    if (value.isObject() == false)
        return; // nothing to parse

    QJsonObject geometry = value.toObject();
    m_width = geometry[XANTE_JTF_WIDTH].toInt();
    m_height = geometry[XANTE_JTF_HEIGHT].toInt();
}

/*
 * Parses the "ui" object from an item.
 */
void XanteItem::parseUiData(QJsonObject item)
{
    QJsonValue value = item[XANTE_JTF_UI];

    if (value.isObject() == false)
        return; // nothing to parse

    QJsonObject ui = value.toObject();
    m_btnExtraEnabled = ui[XANTE_JTF_BTN_EXTRA].toBool();

    parseLabelsData(ui);
    parseGeometryData(ui);
}

void XanteItem::parse(QJsonObject item)
{
    parseCommonData(item);
    parseEventsData(item);
    parseData(item);
    parseUiData(item);
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

bool XanteItem::needsSettings(enum XanteItem::Type type)
{
    bool ret = false;

    switch (type) {
        case XanteItem::Type::Spreadsheet:
        case XanteItem::Type::InputInt:
        case XanteItem::Type::InputFloat:
        case XanteItem::Type::InputDate:
        case XanteItem::Type::InputTime:
        case XanteItem::Type::InputString:
        case XanteItem::Type::InputPasswd:
        case XanteItem::Type::Calendar:
        case XanteItem::Type::Timebox:
        case XanteItem::Type::YesNo:
        case XanteItem::Type::Checklist:
        case XanteItem::Type::RadioChecklist:
        case XanteItem::Type::Range:
        case XanteItem::Type::Buildlist:
        case XanteItem::Type::Inputscroll:
            ret = true;
            break;

        default:
            break;
    }

    return ret;
}

bool XanteItem::needsMinRange(enum XanteItem::Type type)
{
    bool ret = false;

    switch (type) {
        case XanteItem::Type::Range:
        case XanteItem::Type::InputInt:
        case XanteItem::Type::InputFloat:
            ret = true;
            break;

        default:
            break;
    }

    return ret;
}

bool XanteItem::needsMaxRange(enum XanteItem::Type type)
{
    bool ret = false;

    switch (type) {
        case XanteItem::Type::UpdateObject:
        case XanteItem::Type::Progress:
        case XanteItem::Type::SpinnerSync:
        case XanteItem::Type::DotsSync:
        case XanteItem::Type::Range:
        case XanteItem::Type::InputInt:
        case XanteItem::Type::InputFloat:
            ret = true;
            break;

        default:
            break;
    }

    return ret;
}

bool XanteItem::needsStringLengthRange(enum XanteItem::Type type)
{
    bool ret = false;

    switch (type) {
        case XanteItem::Type::InputString:
        case XanteItem::Type::InputPasswd:
        case XanteItem::Type::Inputscroll:
            ret = true;
            break;

        default:
            break;
    }

    return ret;
}

bool XanteItem::needsDescription(enum XanteItem::Type type)
{
    bool ret = false;

    switch (type) {
        case XanteItem::Type::Spreadsheet:
        case XanteItem::Type::Range:
        case XanteItem::Type::Mixedform:
        case XanteItem::Type::FileSelect:
        case XanteItem::Type::DirSelect:
        case XanteItem::Type::Scrolltext:
        case XanteItem::Type::Inputscroll:
        case XanteItem::Type::SpinnerSync:
        case XanteItem::Type::DotsSync:
        case XanteItem::Type::UpdateObject:
        case XanteItem::Type::Progress:
        case XanteItem::Type::InputInt:
        case XanteItem::Type::InputFloat:
        case XanteItem::Type::InputDate:
        case XanteItem::Type::InputTime:
        case XanteItem::Type::InputString:
        case XanteItem::Type::InputPasswd:
        case XanteItem::Type::Calendar:
        case XanteItem::Type::Timebox:
        case XanteItem::Type::YesNo:
        case XanteItem::Type::DeleteDynamicMenu:
            ret = true;
            break;

        default:
            break;
    }

    return ret;
}

bool XanteItem::needsOptions(enum XanteItem::Type type)
{
    bool ret = false;

    switch (type) {
        case XanteItem::Type::Buildlist:
        case XanteItem::Type::Checklist:
        case XanteItem::Type::RadioChecklist:
            ret = true;
            break;

        default:
            break;
    }

    return ret;
}

bool XanteItem::needsDefaultValue(enum XanteItem::Type type)
{
    bool ret = false;

    switch (type) {
        case XanteItem::Type::Scrolltext:
        case XanteItem::Type::Tailbox:
        case XanteItem::Type::Inputscroll:
        case XanteItem::Type::Buildlist:
        case XanteItem::Type::InputInt:
        case XanteItem::Type::InputFloat:
        case XanteItem::Type::InputDate:
        case XanteItem::Type::InputTime:
        case XanteItem::Type::InputString:
        case XanteItem::Type::InputPasswd:
        case XanteItem::Type::Calendar:
        case XanteItem::Type::Timebox:
        case XanteItem::Type::YesNo:
        case XanteItem::Type::Checklist:
        case XanteItem::Type::RadioChecklist:
            ret = true;
            break;

        default:
            break;
    }

    return ret;
}

bool XanteItem::needsMenuReference(enum XanteItem::Type type)
{
    bool ret = false;

    switch (type) {
        case XanteItem::Type::Menu:
            ret = true;
            break;

        default:
            break;
    }

    return ret;
}

