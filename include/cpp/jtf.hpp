
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

#ifndef _LIBXANTE_JTF_HPP
#define _LIBXANTE_JTF_HPP

#include <QList>
#include <QJsonObject>

class XanteJTF
{
    public:
        class Builder;
        XanteJTF() {}
        XanteJTF(const QString applicationName, const QString description,
                 const QString company, const QString plugin,
                 const QString cfgPathname, const QString logPathname,
                 const QString version, const int revision, const int build,
                 const bool beta)
            :
                m_applicationName(applicationName),
                m_description(description),
                m_company(company),
                m_plugin(plugin),
                m_cfgPathname(cfgPathname),
                m_logPathname(logPathname),
                m_version(version),
                m_revision(revision),
                m_build(build),
                m_beta(beta)
        {
            buildDefaultMenu();

            /*
             * Sets the main menu of a JTF as the first one inside our list.
             */
            mainMenu(m_menus.at(0).objectId());

            /* We're not empty anymore */
            m_empty = false;
        }

        static QString objectIdCalc(QString applicationName,
                                    QString menuName,
                                    QString itemName = nullptr);

        /* Operations */
        bool isEmpty(void) const { return m_empty; }
        bool save(QString filename);
        bool load(const QString &filename);
        void clear(void);

        /* JTF information */
        const QString mainMenu(void) const { return m_mainMenu; }
        void mainMenu(QString mainMenu) { m_mainMenu = mainMenu; }

        const QString applicationName(void) const { return m_applicationName; }
        void applicationName(QString applicationName) { m_applicationName = applicationName; }

        const QString description(void) const { return m_description; }
        void description(QString description) { m_description = description; }

        const QString company(void) const { return m_company; }
        void company(QString company) { m_company = company; }

        const QString plugin(void) const { return m_plugin; }
        void plugin(QString plugin) { m_plugin = plugin; }

        const QString cfgPathname(void) const { return m_cfgPathname; }
        void cfgPathname(QString cfgPathname) { m_cfgPathname = cfgPathname; }

        const QString logPathname(void) const { return m_logPathname; }
        void logPathname(QString logPathname) { m_logPathname = logPathname; }

        const QString version(void) const { return m_version; }
        void version(QString version) { m_version = version; }

        int revision(void) const { return m_revision; }
        void revision(int revision) { m_revision = revision; }

        int build(void) const { return m_build; }
        void build(int build) { m_build = build; }

        bool beta(void) const { return m_beta; }
        void beta(bool beta) { m_beta = beta; }

        XanteMenu &menuAt(int index);
        XanteMenu &getMenu(QString objectId);
        int totalMenus(void) const { return m_menus.size(); }
        void addMenu(XanteMenu menu) { m_menus.append(menu); }
        void removeMenu(int index) { m_menus.removeAt(index); }

    private:
        QString m_applicationName, m_description, m_company, m_plugin,
                m_cfgPathname, m_logPathname, m_version, m_mainMenu;

        int m_revision, m_build, m_fileRevision = 1;
        bool m_beta, m_empty = true;
        QList<XanteMenu> m_menus;
        QJsonObject m_jtfRoot;

        void writeJtfData(QJsonObject &root);
        void writeJtfUi(QJsonObject &root);
        void writeJtfGeneral(QJsonObject &root);
        void writeJtfInternal(QJsonObject &root);
        bool loadJtfFromFile(void);
        bool loadJtfInternal(void);
        bool loadJtfGeneral(void);
        bool loadJtfUi(void);
        void buildDefaultMenu(void);
};

class XanteJTF::Builder
{
    public:
        Builder()
            :
                m_version(QString("0.1")),
                m_revision(1),
                m_build(1),
                m_beta(false){}

        Builder &setApplicationName(const QString applicationName) {
            m_applicationName = applicationName;
            return *this;
        }

        Builder &setDescription(const QString description) {
            m_description = description;
            return *this;
        }

        Builder &setCompany(const QString company) {
            m_company= company;
            return *this;
        }

        Builder &setPlugin(const QString plugin) {
            m_plugin = plugin;
            return *this;
        }

        Builder &setCfgPathname(const QString cfgPathname) {
            m_cfgPathname = cfgPathname;
            return *this;
        }

        Builder &setLogPathname(const QString logPathname) {
            m_logPathname = logPathname;
            return *this;
        }

        Builder &setVersion(const QString version) {
            m_version = version;
            return *this;
        }

        Builder &setRevision(const int revision) {
            m_revision = revision;
            return *this;
        }

        Builder &setBuild(const int build_) {
            m_build = build_;
            return *this;
        }

        Builder &setBeta(const bool beta) {
            m_beta = beta;
            return *this;
        }

        XanteJTF build() {
            return XanteJTF(m_applicationName, m_description,
                            m_company, m_plugin, m_cfgPathname,
                            m_logPathname, m_version,
                            m_revision, m_build, m_beta);
        }

    private:
        QString m_applicationName, m_description, m_company, m_plugin,
                m_cfgPathname, m_logPathname, m_version;

        int m_revision, m_build;
        bool m_beta;
};

#endif

