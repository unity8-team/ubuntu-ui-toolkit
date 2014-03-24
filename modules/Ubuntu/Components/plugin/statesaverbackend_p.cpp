/*
 * Copyright 2013 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#include "statesaverbackend_p.h"
#include "ucapplication.h"
#include <QtQml/QQmlContext>
#include <QtQml/QQmlProperty>
#include <QtQml/qqmlinfo.h>
#include <QtQml/qqml.h>
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QStringList>
#include "i18n.h"
#include "quickutils.h"
#include <QtCore/QStandardPaths>

#include "unixsignalhandler_p.h"

StateSaverBackend::StateSaverBackend(QObject *parent)
    : QObject(parent)
    , m_archive(0)
    , m_globalEnabled(true)
    , m_permanent(true)
{
    // disable permanent state saving by default
    setPermanent(false);
    // save state when application is deactivated
    QObject::connect(&QuickUtils::instance(), &QuickUtils::deactivated,
                     this, &StateSaverBackend::initiateStateSaving);
    if (isBackendReady()) {
        initialize();
    } else {
        QObject::connect(&UCApplication::instance(), &UCApplication::applicationNameChanged,
                         this, &StateSaverBackend::initialize);
    }

    UnixSignalHandler::instance().connectSignal(UnixSignalHandler::Terminate);
    UnixSignalHandler::instance().connectSignal(UnixSignalHandler::Interrupt);
    QObject::connect(&UnixSignalHandler::instance(), SIGNAL(signalTriggered(int)),
                     this, SLOT(signalHandler(int)));
}

StateSaverBackend::~StateSaverBackend()
{
    if (m_archive) {
        delete m_archive;
    }
}

bool StateSaverBackend::isBackendReady()
{
    return !qgetenv("APP_ID").isEmpty() || !UCApplication::instance().applicationName().isEmpty();
}

bool StateSaverBackend::moveStateFile(bool permanentLocation)
{
    if (!isBackendReady()) {
        return false;
    }

    QString location = QStandardPaths::writableLocation(
                (permanentLocation) ? QStandardPaths::DataLocation : QStandardPaths::TempLocation);
    if (location.isEmpty()) {
        qFatal("StateSaver cannot save property states!");
        return false;
    }

    QString applicationName(qgetenv("APP_ID"));
    if (applicationName.isEmpty()) {
        applicationName = UCApplication::instance().applicationName();
    }
    QString fileName = QString("%1/%2.state")
            .arg(location)
            .arg(applicationName);
    if (m_archive) {
        // move file
        QString oldFile = m_archive->fileName();
        if (oldFile == fileName) {
            // no change occurred, leave
            return true;
        }

        // transfer all data from the old file to new
        QSettings newSettings(fileName, QSettings::NativeFormat);
        m_archive->sync();
        QStringList keys = m_archive->allKeys();
        Q_FOREACH(const QString &key, keys) {
            newSettings.setValue(key, m_archive->value(key));
        }
        newSettings.sync();
        // clear previous state data
        delete m_archive;
        m_archive = 0;
        QFile::remove(oldFile);
    }
    // (re)create the settings object
    m_archive = new QSettings(fileName, QSettings::NativeFormat);
    m_archive->setFallbacksEnabled(false);
    return true;
}


void StateSaverBackend::initialize()
{
    moveStateFile(m_permanent);
}

void StateSaverBackend::cleanup()
{
    reset();
    m_archive.clear();
}

void StateSaverBackend::signalHandler(int type)
{
    if (type == UnixSignalHandler::Interrupt) {
        Q_EMIT initiateStateSaving();
        // disconnect aboutToQuit() so the state file doesn't get wiped upon quit
        QObject::disconnect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
                         this, &StateSaverBackend::cleanup);
    }
    QCoreApplication::quit();
}

bool StateSaverBackend::enabled() const
{
    return m_globalEnabled;
}
void StateSaverBackend::setEnabled(bool enabled)
{
    if (m_globalEnabled != enabled) {
        m_globalEnabled = enabled;
        Q_EMIT enabledChanged(m_globalEnabled);
        if (!m_globalEnabled) {
            reset();
        }
    }
}

bool StateSaverBackend::registerId(const QString &id)
{
    if (m_register.contains(id)) {
        return false;
    }
    m_register.insert(id);
    return true;
}

void StateSaverBackend::removeId(const QString &id)
{
    m_register.remove(id);
}

bool StateSaverBackend::permanent() const
{
    return m_permanent;
}
/*
 * Return true if the setting got changed, false otherwise
 */
bool StateSaverBackend::setPermanent(bool permanent)
{
    if (m_permanent != permanent) {
        m_permanent = permanent;
        if (m_permanent) {
            // don't clean anymore
            QObject::disconnect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
                             this, &StateSaverBackend::cleanup);
            QObject::disconnect(&QuickUtils::instance(), &QuickUtils::activated,
                             this, &StateSaverBackend::reset);
            // connect quit to save state
            QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
                             this, &StateSaverBackend::initiateStateSaving);
        } else {
            // disconnect quit from state saving
            QObject::disconnect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
                                this, &StateSaverBackend::initiateStateSaving);
            // do cleanup on exit
            QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
                             this, &StateSaverBackend::cleanup);
            QObject::connect(&QuickUtils::instance(), &QuickUtils::activated,
                             this, &StateSaverBackend::reset);
        }
        moveStateFile(m_permanent);
        return true;
    }
    return false;
}

int StateSaverBackend::load(const QString &id, QObject *item, const QStringList &properties)
{
    if (m_archive.isNull()) {
        return 0;
    }

    int result = 0;
    // save the previous group
    bool restorePreviousGroup = !m_archive->group().isEmpty();
    if (restorePreviousGroup) {
        m_groupStack.push(m_archive->group());
        // leave the group so we can read the next one
        m_archive->endGroup();
    }
    m_archive.data()->beginGroup(id);
    QStringList propertyNames = m_archive.data()->childKeys();
    Q_FOREACH(const QString &propertyName, propertyNames) {
        QVariant value = m_archive.data()->value(propertyName);
        if (!properties.contains(propertyName)) {
            // skip the property
            continue;
        }
        QQmlProperty qmlProperty(item, propertyName.toLocal8Bit().constData(), qmlContext(item));
        if (qmlProperty.isValid() && qmlProperty.isWritable()) {
            qmlProperty.write(value);
            result++;
        } else {
            qmlInfo(item) << UbuntuI18n::instance().tr("property \"%1\" does not exist or is not writable for object %2")
                             .arg(propertyName).arg(qmlContext(item)->nameForObject(item));
        }
    }
    m_archive.data()->endGroup();
    // restore leaved group if needed
    if (restorePreviousGroup) {
        m_archive->beginGroup(m_groupStack.pop());
    }
    return result;
}

int StateSaverBackend::save(const QString &id, QObject *item, const QStringList &properties)
{
    if (m_archive.isNull()) {
        return 0;
    }
    m_archive.data()->beginGroup(id);
    int result = 0;
    Q_FOREACH(const QString &propertyName, properties) {
        QQmlProperty qmlProperty(item, propertyName.toLocal8Bit().constData());
        if (qmlProperty.isValid()) {
            QVariant value = qmlProperty.read();
            if (static_cast<QMetaType::Type>(value.type()) != QMetaType::QObjectStar) {
                m_archive.data()->setValue(propertyName, value);
                result++;
            }
        }
    }
    m_archive.data()->endGroup();
    m_archive.data()->sync();
    return result;
}

/*
 * The method resets the register and the state archive for the application.
 */
bool StateSaverBackend::reset()
{
    m_register.clear();
    if (m_archive) {
        QFile archiveFile(m_archive.data()->fileName());
        return archiveFile.remove();
    }
    return true;
}


