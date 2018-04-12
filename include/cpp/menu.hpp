
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Jun 10 11:09:52 2017
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

#ifndef _LIBXANTE_MENU_HPP
#define _LIBXANTE_MENU_HPP

#include <QList>
#include <QJsonObject>
#include <QMap>

class XanteMenu
{
    public:
        enum Type {
            Default,
            Dynamic
        };

        enum Event {
            Exit,

            MaxEvents
        };

        enum DynamicType {
            FixedSize,
            FixedOptions,
            DynamicOptions,

            MaxDynamicMenuType
        };

        XanteMenu() {}
        XanteMenu(QString applicationName, QJsonObject menu);
        XanteMenu(QString applicationName, QString name);
        XanteMenu(QString objectId) : m_objectId(objectId) {}

        XanteItem &itemAt(int index);
        int totalItems(void) { return m_items.size(); }
        void itemMove(int from, int to) { m_items.move(from, to); }
        void addItem(XanteItem item) { m_items.append(item); }
        void removeItem(int index) { m_items.removeAt(index); }

        void write(QJsonObject &root) const;
        bool hasEvents(void) const { return m_events.size() != 0; }

        /*
         * The purpose of this function is to be used by the method indexOf of
         * a QList<XanteMenu>.
         *
         * We search for a XanteMenu only by its objectId attribute.
         */
        bool operator ==(const XanteMenu &other) const {
            return m_objectId.compare(other.m_objectId) == 0;
        }

        /*
         * This one is actually used to compare two XanteMenu objects.
         */
        bool operator !=(const XanteMenu &other) const {
            return (m_applicationName != other.m_applicationName) ||
                (m_name != other.m_name) ||
                (m_objectId != other.m_objectId) ||
                (m_dynamicBlockPrefix != other.m_dynamicBlockPrefix) ||
                (m_dynamicOriginBlock != other.m_dynamicOriginBlock) ||
                (m_dynamicOriginItem != other.m_dynamicOriginItem) ||
                (m_mode != other.m_mode) ||
                (m_type != other.m_type) ||
                (m_dynamicType != other.m_dynamicType) ||
                (m_dynamicCopies != other.m_dynamicCopies) ||
                (m_copies != other.m_copies) ||
                (m_events != other.m_events);
        }

        XanteMenu &operator =(const XanteMenu &other) {
            if (&other == this)
                return *this;

            m_applicationName = other.m_applicationName;
            m_name = other.m_name;
            m_objectId = other.m_objectId;
            m_dynamicBlockPrefix = other.m_dynamicBlockPrefix;
            m_dynamicOriginBlock = other.m_dynamicOriginBlock;
            m_dynamicOriginItem = other.m_dynamicOriginItem;

            m_mode = other.m_mode;
            m_type = other.m_type;
            m_dynamicType = other.m_dynamicType;
            m_dynamicCopies = other.m_dynamicCopies;
            m_copies = other.m_copies;
            m_typeDescription = other.m_typeDescription;
            m_events = other.m_events;

            m_items = other.m_items;

            return *this;
        }

        QString debug(bool printItems) const {
            QString info = QString("{applicationName: '%1', "
                    "name: '%2', objectId: '%3', dynamicBlockPrefix: '%4', "
                    "dynamicOriginBlock: '%5', dynamicOriginItem: '%6', "
                    "mode: '%7', type: '%8', dynamicType: '%9', "
                    "dynamicCopies: '%10'")
                .arg(m_applicationName)
                .arg(m_name)
                .arg(m_objectId)
                .arg(m_dynamicBlockPrefix)
                .arg(m_dynamicOriginBlock)
                .arg(m_dynamicOriginItem)
                .arg(m_mode)
                .arg(m_type)
                .arg(m_dynamicType)
                .arg(m_dynamicCopies);

            if (m_events.size() > 0) {
                QMap<enum XanteMenu::Event, QString>::const_iterator it;
                info += QString(", events: {");

                for (it = m_events.constBegin(); it != m_events.constEnd(); it++)
                    info += QString("%1: '%2' ").arg(it.key()).arg(it.value());

                info += QString("}");
            }

            if (printItems && (m_items.size() > 0)) {
                info += QString(", items: [");

                for (int i = 0; i < m_items.size(); i++) {
                    XanteItem it = m_items.at(i);
                    info += it.debug();
                }

                info += QString("]");
            } else
                info += QString("}");

            return info;
        }

        /* Menu properties */
        const QString name(void) const { return m_name; }
        void name(QString name);

        const QString objectId(void) const { return m_objectId; }
        void objectId(QString objectId) { m_objectId = objectId; }

        enum XanteMenu::Type type(void) const { return m_type; }
        void type(enum XanteMenu::Type type) { m_type = type; }

        enum XanteMenu::DynamicType dynamicType(void) const { return m_dynamicType; }
        void dynamicType(enum XanteMenu::DynamicType dynamicType) { m_dynamicType = dynamicType; }

        enum XanteAccessMode mode(void) const { return m_mode; }
        void mode(enum XanteAccessMode mode) { m_mode = mode; }

        const QString event(enum XanteMenu::Event event) const {
            QMap<enum XanteMenu::Event, QString>::const_iterator it;

            for (it = m_events.constBegin(); it != m_events.constEnd(); it++)
                if (it.key() == event)
                    return it.value();

            return QString("");
        }

        void event(QString event, enum XanteMenu::Event eventId) {
            m_events.insert(eventId, event);
        }

        int dynamicCopies(void) const { return m_dynamicCopies; }
        void dynamicCopies(int dynamicCopies) {
            m_dynamicCopies = dynamicCopies;
            m_dynamicType = XanteMenu::DynamicType::FixedSize;
        }

        const QStringList dynamicOptions(void) const { return m_copies; }
        void dynamicOptions(QString copy) {
            if (m_copies.contains(copy) == false)
                m_copies.append(copy);

            m_dynamicType = XanteMenu::DynamicType::FixedOptions;
        }

        const QPair<QString, QPair<QString, QString>> dynamicOrigin(void) {
            return qMakePair(m_dynamicBlockPrefix,
                             qMakePair(m_dynamicOriginBlock,
                                       m_dynamicOriginItem));
        }

        void dynamicOrigin(QString blockPrefix, QString originBlock, QString originItem) {
            m_dynamicBlockPrefix = blockPrefix;
            m_dynamicOriginBlock = originBlock;
            m_dynamicOriginItem = originItem;
            m_dynamicType = XanteMenu::DynamicType::DynamicOptions;
        }

        void size(QSize size) { m_width = size.width(); m_height = size.height(); }
        QSize size(void) const { return QSize(m_width, m_height); }

    private:
        QString m_applicationName, m_name, m_objectId, m_dynamicBlockPrefix,
                m_dynamicOriginBlock, m_dynamicOriginItem;

        enum XanteAccessMode m_mode;
        enum XanteMenu::Type m_type;
        enum XanteMenu::DynamicType m_dynamicType = XanteMenu::DynamicType::FixedSize;
        int m_dynamicCopies = 0, m_width = -1, m_height = -1;
        QStringList m_copies;
        QList<XanteItem> m_items;
        QMap<enum XanteMenu::Type, QString> m_typeDescription;
        QMap<enum XanteMenu::Event, QString> m_events;

        void preLoad(void);
        void parse(QJsonObject menu);
        void parseCommonData(QJsonObject menu);
        void parseEventsData(QJsonObject menu);
        void parseDynamicData(QJsonObject menu);
        void parseGeometryData(QJsonObject menu);
        void parseItems(QJsonObject menu);

        QJsonObject writeEvents(void) const;
        QJsonObject writeDynamic(void) const;
        QJsonObject writeGeometry(void) const;
};

#endif

