
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

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QCryptographicHash>
#include <QByteArray>
#include <QMap>

#include "libxante.h"

/*
 *
 * XanteJTF
 *
 */

void XanteJTF::writeJtfInternal(QJsonObject &root)
{
    QJsonObject japplication;

    japplication[XANTE_JTF_VERSION] = m_version;
    japplication[XANTE_JTF_REVISION] = m_revision;
    japplication[XANTE_JTF_BUILD] = m_build;
    japplication[XANTE_JTF_BETA] = m_beta;

    root[XANTE_JTF_FILE_REVISION] = m_fileRevision;
    root[XANTE_JTF_LANGUAGE] = "en-us";
    root[XANTE_JTF_APPLICATION] = japplication;
}

void XanteJTF::writeJtfGeneral(QJsonObject &root)
{
    root[XANTE_JTF_NAME] = m_applicationName;
    root[XANTE_JTF_DESCRIPTION] = m_description;
    root[XANTE_JTF_MODULE_NAME] = m_plugin;
    root[XANTE_JTF_CONFIG_PATHNAME] = m_cfgPathname;
    root[XANTE_JTF_LOG_PATHNAME] = m_logPathname;
    root[XANTE_JTF_LOG_LEVEL] = "info";
    root[XANTE_JTF_COMPANY] = m_company;
}

void XanteJTF::writeJtfUi(QJsonObject &root)
{
    QJsonArray jmenus;
    XanteMenu menu;

    foreach (const XanteMenu menu, m_menus) {
        QJsonObject m;
        menu.write(m);
        jmenus.append(m);
    }

    root[XANTE_JTF_MAIN_MENU] = m_mainMenu;
    root[XANTE_JTF_MENUS] = jmenus;
}

void XanteJTF::writeJtfData(QJsonObject &root)
{
    QJsonObject jinternal, jui, jgeneral;

    writeJtfInternal(jinternal);
    writeJtfUi(jui);
    writeJtfGeneral(jgeneral);

    root[XANTE_JTF_INTERNAL] = jinternal;
    root[XANTE_JTF_GENERAL] = jgeneral;
    root[XANTE_JTF_UI] = jui;
}

bool XanteJTF::save(QString filename)
{
    QFile file(filename);

    if (file.open(QIODevice::WriteOnly) == false)
        return false;

    QJsonObject jtfObject;
    writeJtfData(jtfObject);
    QJsonDocument doc(jtfObject);
    file.write(doc.toJson(QJsonDocument::Compact));

    return true;
}

bool XanteJTF::loadJtfInternal(void)
{
    QJsonValue value = m_jtfRoot.value(QString(XANTE_JTF_INTERNAL));

    if (value.isObject() == false)
        return false;

    QJsonObject internal = value.toObject();

    m_fileRevision = internal[XANTE_JTF_FILE_REVISION].toInt();
    value = internal[XANTE_JTF_APPLICATION];

    if (value.isObject() == false)
        return false;

    QJsonObject application = value.toObject();

    m_version = application[XANTE_JTF_VERSION].toString();
    m_revision = application[XANTE_JTF_REVISION].toInt();
    m_build = application[XANTE_JTF_BUILD].toInt(-1);
    m_beta = application[XANTE_JTF_BETA].toBool();

    return true;
}

bool XanteJTF::loadJtfGeneral(void)
{
    QJsonValue value = m_jtfRoot.value(QString(XANTE_JTF_GENERAL));

    if (value.isObject() == false)
        return false;

    QJsonObject general = value.toObject();

    m_applicationName = general[XANTE_JTF_NAME].toString();
    m_description = general[XANTE_JTF_DESCRIPTION].toString();
    m_company = general[XANTE_JTF_COMPANY].toString();
    m_plugin = general[XANTE_JTF_MODULE_NAME].toString();
    m_cfgPathname = general[XANTE_JTF_CONFIG_PATHNAME].toString();
    m_logPathname = general[XANTE_JTF_LOG_PATHNAME].toString();

    return true;
}

bool XanteJTF::loadJtfUi(void)
{
    QJsonValue value = m_jtfRoot.value(QString(XANTE_JTF_UI));
    QJsonObject ui = value.toObject();
    QJsonArray jmenus = ui[XANTE_JTF_MENUS].toArray();
    m_mainMenu = ui[XANTE_JTF_MAIN_MENU].toString();

    foreach(const QJsonValue &v, jmenus) {
        XanteMenu m(m_applicationName, v.toObject());
        m_menus.append(m);
    }

    return true;
}

bool XanteJTF::loadJtfFromFile(void)
{
    if ((loadJtfInternal() == false) ||
        (loadJtfGeneral() == false) ||
        (loadJtfUi() == false))
    {
        return false;
    }

    return true;
}

bool XanteJTF::load(const QString &filename)
{
    QFile file;
    QString data;

    file.setFileName(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    data = file.readAll();
    file.close();

    QJsonDocument d = QJsonDocument::fromJson(data.toUtf8());
    m_jtfRoot = d.object();

    if (loadJtfFromFile() == false)
        return false;

    /* We're not empty anymore */
    m_empty = false;

    return true;
}

/*
 * Here we calculate the object objectId property. It is composed of the
 * application name and the menu name, if we're talking about a menu. If it
 * is an item, the item name is also used.
 */
QString XanteJTF::objectIdCalc(QString applicationName, QString menuName,
    QString itemName)
{
    QCryptographicHash hash(QCryptographicHash::Sha1);

    hash.addData(applicationName.toStdString().c_str(), menuName.size());
    hash.addData(menuName.toStdString().c_str(), menuName.size());

    if (itemName != nullptr)
        hash.addData(itemName.toStdString().c_str(), itemName.size());

    QByteArray res = hash.result().toHex();

    return QString(res);
}

void XanteJTF::buildDefaultMenu(void)
{
    XanteMenu m(m_applicationName, QString("Main"));
    m_menus.append(m);
}

XanteMenu &XanteJTF::getMenu(QString objectId)
{
    int index = m_menus.indexOf(objectId);

    return menuAt(index);
}

XanteMenu &XanteJTF::menuAt(int index)
{
    if ((index < 0) || (index > m_menus.size()))
        throw std::out_of_range(QObject::tr("Menu not found.").toLocal8Bit().data());

    return m_menus[index];
}

void XanteJTF::clear(void)
{
    m_empty = true;
}

