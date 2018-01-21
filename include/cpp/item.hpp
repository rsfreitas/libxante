
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

#ifndef _LIBXANTE_ITEM_HPP
#define _LIBXANTE_ITEM_HPP

#include <QList>
#include <QJsonObject>
#include <QMap>

class QVariant;

class XanteItem
{
    public:
        enum Type {
            Unknown = -1,
            Menu,
            InputInt,
            InputFloat,
            InputDate,
            InputTime,
            InputString,
            InputPasswd,
            Calendar,
            Timebox,
            RadioChecklist,
            Checklist,
            YesNo,
            DynamicMenu,
            DeleteDynamicMenu,
            AddDynamicMenu,

            MaxTypes
        };

        enum Event {
            Selected,
            Exit,
            ValueConfirmed,
            ValueChanged,

            MaxEvents
        };

        XanteItem() {}
        XanteItem(QString applicationName, QString menuName, QJsonObject item);
        XanteItem(QString applicationName, QString menuName, QString name);

        void write(QJsonObject &root) const;
        bool hasEvents(void) const { return m_events.size() != 0; }
        bool hasOptions(void) const { return (m_type >= XanteItem::Type::InputInt) &&
                                             (m_type <= XanteItem::Type::DeleteDynamicMenu); }

        bool hasInputRanges(void) const { return (m_type >= XanteItem::Type::InputInt) &&
                                                 (m_type <= XanteItem::Type::InputPasswd); }

        bool hasConfig(void) const { return (m_type >= XanteItem::Type::InputInt) &&
                                            (m_type <= XanteItem::Type::YesNo); }

        bool hasHelp(void) const {
            return (m_briefHelp.isEmpty() == false) ||
                   (m_descriptiveHelp.isEmpty() == false) ||
                   (((m_type == XanteItem::Type::Checklist) ||
                     (m_type == XanteItem::Type::RadioChecklist)) &&
                    m_helpOptions.size() != 0);
        }

        bool operator ==(const XanteItem &other) const {
            return (m_applicationName == other.m_applicationName) &&
                (m_menuName == other.m_menuName) &&
                (m_name == other.m_name) &&
                (m_objectId == other.m_objectId) &&
                (m_briefHelp == other.m_briefHelp) &&
                (m_descriptiveHelp == other.m_descriptiveHelp) &&
                (m_configBlock == other.m_configBlock) &&
                (m_configItem == other.m_configItem) &&
                (m_fixedOption == other.m_fixedOption) &&
                (m_defaultValue == other.m_defaultValue) &&
                (m_menuReferenceId == other.m_menuReferenceId) &&
                (m_minInputRange == other.m_minInputRange) &&
                (m_maxInputRange == other.m_maxInputRange) &&
                (m_options == other.m_options) &&
                (m_helpOptions == other.m_helpOptions) &&
                (m_events == other.m_events) &&
                (m_mode == other.m_mode) &&
                (m_type == other.m_type) &&
                (m_stringLength == other.m_stringLength);
        }

        bool operator !=(const XanteItem &other) const {
            return !(*this == other);
        }

        XanteItem &operator =(const XanteItem &other) {
            if (&other == this)
                return *this;

            m_applicationName = other.m_applicationName;
            m_menuName = other.m_menuName;
            m_name = other.m_name;
            m_objectId = other.m_objectId;
            m_briefHelp = other.m_briefHelp;
            m_descriptiveHelp = other.m_descriptiveHelp;
            m_configBlock = other.m_configBlock;
            m_configItem = other.m_configItem;
            m_fixedOption = other.m_fixedOption;
            m_defaultValue = other.m_defaultValue;
            m_menuReferenceId = other.m_menuReferenceId;
            m_minInputRange = other.m_minInputRange;
            m_maxInputRange = other.m_maxInputRange;
            m_options = other.m_options;
            m_helpOptions = other.m_helpOptions;
            m_events = other.m_events;
            m_mode = other.m_mode;
            m_type = other.m_type;
            m_stringLength = other.m_stringLength;
            m_typeDescription = other.m_typeDescription;

            return *this;
        }

        QString debug(void) const {
            QString info = QString(
                    "{applicationName: '%1', menuName: '%2', name: '%3', "
                    "objectId: '%4', briefHelp: '%5', descriptiveHelp: '%6', "
                    "configBlock: '%7', configItem: '%8', fixedOption: '%9', "
                    "defaultValue: '%10', menuReferenceId: '%11', mode: '%12', "
                    "type: '%13', min: '%14', max: '%15', stringLength: '%16'")
                .arg(m_applicationName)
                .arg(m_menuName)
                .arg(m_name)
                .arg(m_objectId)
                .arg(m_briefHelp)
                .arg(m_descriptiveHelp)
                .arg(m_configBlock)
                .arg(m_configItem)
                .arg(m_fixedOption)
                .arg(m_defaultValue)
                .arg(m_menuReferenceId)
                .arg(m_mode)
                .arg(m_type)
                .arg((m_type == XanteItem::Type::InputFloat) ? m_minInputRange.toFloat()
                                                             : m_minInputRange.toInt())
                .arg((m_type == XanteItem::Type::InputFloat) ? m_maxInputRange.toFloat()
                                                             : m_maxInputRange.toInt())
                .arg(m_stringLength);

            if (m_events.size() > 0) {
                QMap<enum XanteItem::Event, QString>::const_iterator it;
                info += QString(", events: {");

                for (it = m_events.constBegin(); it != m_events.constEnd(); it++)
                    info += QString("%1: '%2' ").arg(it.key()).arg(it.value());

                info += QString("}");
            }

            if (m_options.size() > 0) {
                info += QString(", options: [");

                for (int i = 0; i < m_options.size(); i++)
                    info += QString("'%1' ").arg(m_options.at(i));

                info += QString("]");
            }

            if (m_helpOptions.size() > 0) {
                info += QString(", helpOptions: [");

                for (int i = 0; i < m_helpOptions.size(); i++)
                    info += QString("'%1' ").arg(m_helpOptions.at(i));

                info += QString("]");
            }

            info += QString("}");

            return info;
        }

        /* Item properties */
        const QString name(void) const { return m_name; }
        void name(QString name);

        const QString objectId(void) const { return m_objectId; }
        void objectId(QString objectId) { m_objectId = objectId; }

        const QString briefHelp(void) const { return m_briefHelp; }
        void briefHelp(QString briefHelp) { m_briefHelp = briefHelp; }

        const QString descriptiveHelp(void) const { return m_descriptiveHelp; }
        void descriptiveHelp(QString descriptiveHelp) { m_descriptiveHelp = descriptiveHelp; }

        const QString configBlock(void) const { return m_configBlock; }
        void configBlock(QString configBlock) { m_configBlock = configBlock; }

        const QString configItem(void) const { return m_configItem; }
        void configItem(QString configItem) { m_configItem = configItem; }

        enum XanteItem::Type type(void) const { return m_type; }
        void type(enum XanteItem::Type type) { m_type = type; }

        enum XanteAccessMode mode(void) const { return m_mode; }
        void mode(enum XanteAccessMode mode) { m_mode = mode; }

        int stringLength(void) const { return m_stringLength; }
        void stringLength(int stringLength) { m_stringLength = stringLength; }

        template <typename T>
        void minMax(T min, T max) {
            m_minInputRange = QVariant(min);
            m_maxInputRange = QVariant(max);
        }

        const QPair<QVariant, QVariant> minMax(void) const {
            return qMakePair(m_minInputRange, m_maxInputRange);
        }

        const QString defaultValue(void) const { return m_defaultValue; }
        void defaultValue(QString value) { m_defaultValue = value; }

        const QString referencedMenu(void) const { return m_menuReferenceId; }
        void referencedMenu(QString referencedMenu) { m_menuReferenceId = referencedMenu; }

        const QString event(enum XanteItem::Event event) const {
            QMap<enum XanteItem::Event, QString>::const_iterator it;

            for (it = m_events.constBegin(); it != m_events.constEnd(); it++)
                if (it.key() == event)
                    return it.value();

            return QString("");
        }

        void event(QString event, enum XanteItem::Event eventId) {
            m_events[eventId] = event;
        }

        int totalOptions(void) const { return m_options.size(); }
        const QString option(void) const { return m_fixedOption; }
        const QString option(int index) const { return m_options.at(index); }
        void option(QString option) {
            if ((m_type == XanteItem::Type::RadioChecklist) ||
                (m_type == XanteItem::Type::Checklist))
            {
                if (m_options.contains(option) == false)
                    m_options.append(option);
            } else
                m_fixedOption = option;
        }

        int totalHelpOptions(void) const { return m_helpOptions.size(); }
        const QString helpOption(int index) const { return m_helpOptions.at(index); }
        void helpOption(QString help) {
            if (m_helpOptions.contains(help) == false)
                m_helpOptions.append(help);
        }

    private:
        QString m_applicationName, m_menuName, m_name, m_objectId, m_briefHelp,
                m_descriptiveHelp, m_configBlock, m_configItem, m_fixedOption,
                m_defaultValue, m_menuReferenceId;

        QVariant m_minInputRange, m_maxInputRange;
        QList<QString> m_options, m_helpOptions;
        QMap<enum XanteItem::Type, QString> m_typeDescription;
        QMap<enum XanteItem::Event, QString> m_events;
        enum XanteAccessMode m_mode;
        enum XanteItem::Type m_type;
        int m_stringLength = 0;

        void preLoad(void);
        void parse(QJsonObject item);
        void parseCommonData(QJsonObject item);
        void parseEventsData(QJsonObject item);
        void parseConfigData(QJsonObject item);
        void parseRangesData(QJsonObject item);
        void parseHelpData(QJsonObject item);

        void writeOptions(QJsonObject &root) const;
        QJsonObject writeInputRanges(void) const;
        QJsonObject writeConfig(void) const;
        QJsonObject writeEvents(void) const;
        QJsonObject writeHelp(void) const;

        enum XanteItem::Type toXanteItem(const QString &type);
};

#endif

