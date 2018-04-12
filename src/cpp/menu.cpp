
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
 * XanteMenu
 *
 */

void XanteMenu::name(QString name)
{
    m_name = name;
    m_objectId = XanteJTF::objectIdCalc(m_applicationName, name);
}

void XanteMenu::preLoad(void)
{
    m_typeDescription.insert(XanteMenu::Type::Dynamic,
                             QString(XANTE_STR_DYNAMIC_MENU));

    m_typeDescription.insert(XanteMenu::Type::Default,
                             QString(XANTE_STR_DEFAULT_MENU));
}

/*
 * Parses the main objects of a menu.
 */
void XanteMenu::parseCommonData(QJsonObject menu)
{
    int tmp;

    m_name = menu[XANTE_JTF_NAME].toString();
    m_objectId = menu[XANTE_JTF_OBJECT_ID].toString();

    if (m_objectId.isEmpty())
        m_objectId = XanteJTF::objectIdCalc(m_applicationName, m_name);

    tmp = menu[XANTE_JTF_MODE].toInt();
    m_mode = (enum XanteAccessMode)tmp;
}

/*
 * Parses the "events" object inside a menu.
 */
void XanteMenu::parseEventsData(QJsonObject menu)
{
    QJsonValue value = menu[XANTE_JTF_EVENTS];

    if ((value.type() != QJsonValue::Undefined) &&
        (value.type() != QJsonValue::Null))
    {
        QJsonObject events = value.toObject();
        QMap<QString, enum XanteMenu::Event> evMap;

        evMap.insert(EV_MENU_EXIT, XanteMenu::Event::Exit);
        QMapIterator<QString, enum XanteMenu::Event> i(evMap);

        while (i.hasNext()) {
            i.next();
            value = events[i.key()];

            if (value.type() == QJsonValue::String)
                m_events.insert(i.value(), value.toString());
        }
    }
}

/*
 * Parses the "dynamic" object inside a menu.
 */
void XanteMenu::parseDynamicData(QJsonObject menu)
{
    QJsonValue value = menu[XANTE_JTF_TYPE];

    if ((value.type() == QJsonValue::Undefined) ||
        (value.type() == QJsonValue::Null))
    {
        m_type = XanteMenu::Type::Default;
    } else {
        QString tmp = value.toString();

        if (tmp == XANTE_JTF_DYNAMIC) {
            QJsonObject dynamic = menu[XANTE_JTF_DYNAMIC].toObject();

            m_type = XanteMenu::Type::Dynamic;
            value = dynamic[XANTE_JTF_COPIES];

            if (value.type() == QJsonValue::Double) {
                m_dynamicType = XanteMenu::FixedSize;
                m_dynamicCopies = value.toInt();
            } else if (value.type() == QJsonValue::Array) {
                QJsonArray copies = value.toArray();
                m_dynamicType = XanteMenu::FixedOptions;

                foreach(const QJsonValue &v, copies)
                    m_copies.append(v.toString());
            } else {
                QJsonObject origin = dynamic[XANTE_JTF_ORIGIN].toObject();

                m_dynamicType = XanteMenu::DynamicOptions;
                m_dynamicOriginBlock = origin[XANTE_JTF_BLOCK].toString();
                m_dynamicOriginItem = origin[XANTE_JTF_ITEM].toString();
                m_dynamicBlockPrefix = dynamic[XANTE_JTF_BLOCK_PREFIX].toString();
            }
        } else
            m_type = XanteMenu::Type::Default;
    }
}

/*
 * Parses the "geometry" object inside a menu.
 */
void XanteMenu::parseGeometryData(QJsonObject menu)
{
    QJsonValue value = menu[XANTE_JTF_GEOMETRY];

    if (value.isObject() == false)
        return; // nothing to parse

    QJsonObject geometry = value.toObject();
    m_width = geometry[XANTE_JTF_WIDTH].toInt();
    m_height = geometry[XANTE_JTF_HEIGHT].toInt();
}

/*
 * Parses the "item" object from a menu.
 */
void XanteMenu::parseItems(QJsonObject menu)
{
    QJsonArray jitems = menu[XANTE_JTF_ITEMS].toArray();

    foreach(const QJsonValue &v, jitems) {
        XanteItem it(m_applicationName, m_name, v.toObject());
        m_items.append(it);
    }
}

/*
 * Parses a menu object, an item from its array.
 */
void XanteMenu::parse(QJsonObject menu)
{
    parseCommonData(menu);
    parseEventsData(menu);
    parseDynamicData(menu);
    parseGeometryData(menu);
    parseItems(menu);
}

XanteMenu::XanteMenu(QString applicationName, QJsonObject menu)
    : m_applicationName(applicationName)
{
    preLoad();
    parse(menu);
}

XanteMenu::XanteMenu(QString applicationName, QString name)
    : m_applicationName(applicationName), m_name(name)
{
    preLoad();
    mode(XanteAccessMode::XanteAccessEdit);
    this->name(name);
    type(XanteMenu::Type::Default);

    /* We always create an empty item for a new menu */
    XanteItem it(applicationName, name, QString("Item"));
    it.type(XanteItem::Type::Menu);
    it.referencedMenu(XanteJTF::objectIdCalc(applicationName, name));
    m_items.append(it);
}

QJsonObject XanteMenu::writeEvents(void) const
{
    QJsonObject events;
    QMap<QString, enum XanteMenu::Event> evMap;

    evMap.insert(EV_MENU_EXIT, XanteMenu::Event::Exit);
    QMapIterator<QString, enum XanteMenu::Event> i(evMap);

    while (i.hasNext()) {
        i.next();
        QString evValue = m_events.value(i.value(), QString(""));

        if (evValue.isEmpty() == false)
            events[i.key()] = evValue;
    }

    return events;
}

QJsonObject XanteMenu::writeDynamic(void) const
{
    QJsonArray optArray;
    QJsonObject origin, dynamic;

    switch (m_dynamicType) {
        case XanteMenu::DynamicType::FixedSize:
            dynamic[XANTE_JTF_COPIES] = m_dynamicCopies;
            break;

        case XanteMenu::DynamicType::FixedOptions:
            for (int i = 0; i < m_copies.size(); i++)
                optArray.append(m_copies.at(i));

            dynamic[XANTE_JTF_COPIES] = optArray;
            break;

        case XanteMenu::DynamicType::DynamicOptions:
            origin[XANTE_JTF_BLOCK] = m_dynamicOriginBlock;
            origin[XANTE_JTF_ITEM] = m_dynamicOriginItem;
            dynamic[XANTE_JTF_BLOCK_PREFIX] = m_dynamicBlockPrefix;
            dynamic[XANTE_JTF_ORIGIN] = origin;
            break;

        default:
            break;
    }

    return dynamic;
}

QJsonObject XanteMenu::writeGeometry(void) const
{
    QJsonObject geometry;

    geometry[XANTE_JTF_WIDTH] = m_width;
    geometry[XANTE_JTF_HEIGHT] = m_height;

    return geometry;
}

void XanteMenu::write(QJsonObject &root) const
{
    QJsonArray jitems;

    foreach(const XanteItem item, m_items) {
        QJsonObject it;
        item.write(it);
        jitems.append(it);
    }

    root[XANTE_JTF_MODE] = m_mode;
    root[XANTE_JTF_NAME] = m_name;
    root[XANTE_JTF_OBJECT_ID] = m_objectId;
    root[XANTE_JTF_TYPE] = m_typeDescription.value(m_type);

    if (hasEvents())
        root[XANTE_JTF_EVENTS] = writeEvents();

    if (m_type == XanteMenu::Type::Dynamic)
        root[XANTE_JTF_DYNAMIC] = writeDynamic();

    if ((m_width != -1) || (m_height != -1))
        root[XANTE_JTF_GEOMETRY] = writeGeometry();

    root[XANTE_JTF_ITEMS] = jitems;
}

XanteItem &XanteMenu::itemAt(int index)
{
    if ((index < 0) || (index > m_items.size()))
        throw std::out_of_range(QObject::tr("Item not found.").toLocal8Bit().data());

    return m_items[index];
}

