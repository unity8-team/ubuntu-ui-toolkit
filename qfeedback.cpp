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
 * Author: Christian Dywan <christian.dywan@canonical.com>
 */

#include <qfeedbackactuator.h>
#include "qfeedback.h"
#include <QtCore/QtPlugin>
#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QVariant>
#include <QtCore/QTimer>
#include <QDebug>
#include <QtCore/QProcess>

QFeedbackMir::QFeedbackMir() : QObject(qApp),
    QFeedbackThemeInterface()
{
    if (false) {
        // one-time initialization; currently not required
        qWarning() << "initialization failed";
    } else {
        const int nbDev = 1;
        for (int i = 0; i < nbDev; ++i) {
            actuatorList << createFeedbackActuator(this, i);
        }
    }
}

QFeedbackMir::~QFeedbackMir()
{
    // Close device handles
    const int nbDev = 1;
    for (int i = 0 ; i < nbDev; ++i)
        /* close device */;

    // Done
}

QFeedbackInterface::PluginPriority QFeedbackMir::pluginPriority()
{
    return PluginNormalPriority;
}

QList<QFeedbackActuator*> QFeedbackMir::actuators()
{
    return actuatorList;
}

void QFeedbackMir::setActuatorProperty(const QFeedbackActuator &actuator, ActuatorProperty prop, const QVariant &value)
{
    switch (prop)
    {
    case Enabled:
        qWarning() << "mir fp enabled";
        break;
    default:
        break;
    }
}

QVariant QFeedbackMir::actuatorProperty(const QFeedbackActuator &actuator, ActuatorProperty prop)
{
    switch (prop)
    {
    case Name:
        {
            if (false /* actuator.id()*/)
                return QString();

            return QString::fromLocal8Bit("Command line vibrator");
        }

    case State:
        {
            QFeedbackActuator::State ret = QFeedbackActuator::Unknown;
            if (actuator.isValid() && true /* actuator.id() */) {
                ret = QFeedbackActuator::Ready;
                // ret = QFeedbackActuator:: Busy;
            }

            return ret;
        }
    case Enabled:
        {
            // no global vibration policy, always enabled
            return true;
        }
    default:
        return QVariant();
    }
}

bool QFeedbackMir::isActuatorCapabilitySupported(const QFeedbackActuator &, QFeedbackActuator::Capability cap)
{
    switch(cap)
    {
    case QFeedbackActuator::Envelope:
    case QFeedbackActuator::Period:
        return true;
    default:
        return false;
    }
}

void QFeedbackMir::updateEffectProperty(const QFeedbackHapticsEffect *effect, EffectProperty)
{
    if (effect->period() > 0) {
        /*
        periodic effect
        effect->duration(), // QFeedbackEffect::Infinite means infinite
        effect->intensity(),
        effect->period(),
        effect->attackTime(),
        effect->attackIntensity(),
        effect->fadeTime(),
        effect->fadeIntensity()
        */
    } else {
        /* one-off */
    }

    if (false)
        reportError(effect, QFeedbackEffect::UnknownError);

}

void QFeedbackMir::vibrateOnce(const QFeedbackEffect* effect)
{
    int effectiveDuration;
    switch (effect->duration())
    {
        case QFeedbackEffect::Infinite:
        case 0:
            effectiveDuration = 150;
    }
    qWarning() << QString("vibrateOnce effect for %1ms").arg(effectiveDuration);

    QProcess gzip;
    gzip.setStandardOutputFile("/sys/class/timed_output/vibrator/enable");
    gzip.start("echo", QStringList() << QString("%1").arg(effectiveDuration));
    if (!gzip.waitForStarted())
        qWarning("!started");

    if (!gzip.waitForFinished())
        qWarning("!finished");
}

void QFeedbackMir::setEffectState(const QFeedbackHapticsEffect *effect, QFeedbackEffect::State state)
{
    switch (state)
    {
    case QFeedbackEffect::Stopped:
        break;
    case QFeedbackEffect::Paused:
        break;
    case QFeedbackEffect::Running:
        vibrateOnce(effect);
        break;
    default:
        break;
    }
}

QFeedbackEffect::State QFeedbackMir::effectState(const QFeedbackHapticsEffect *effect)
{
    return QFeedbackEffect::Stopped;
}

void QFeedbackMir::setLoaded(QFeedbackFileEffect *effect, bool load)
{
    const QUrl url = effect->source();
    // This doesn't handle qrc urls..
    const QString fileName = url.toLocalFile();
    if (fileName.isEmpty())
        return;

    if (!load /* && !fileData.contains(filename) */)
        return;

    if (false)
        reportLoadFinished(effect, true);
}

void QFeedbackMir::setEffectState(QFeedbackFileEffect *effect, QFeedbackEffect::State state)
{
    switch (state)
    {
    case QFeedbackEffect::Stopped:
        break;
    case QFeedbackEffect::Paused:
        break;
    case QFeedbackEffect::Running:
        break;
    default:
        break;
    }

    if (false)
        reportError(effect, QFeedbackEffect::UnknownError);
}

QFeedbackEffect::State QFeedbackMir::effectState(const QFeedbackFileEffect *effect)
{
    //return QFeedbackEffect::Paused;
    //return QFeedbackEffect::Running;
    return QFeedbackEffect::Stopped;
}

int QFeedbackMir::effectDuration(const QFeedbackFileEffect *effect)
{
    QString fileName = effect->source().toLocalFile();
    return 0;
}

QStringList QFeedbackMir::supportedMimeTypes()
{
    return QStringList() << QLatin1String("vibra/ivt");
}

bool QFeedbackMir::play(QFeedbackEffect::Effect effect)
{
    QFeedbackHapticsEffect themeEffect;
    vibrateOnce(&themeEffect);
    return true;
}

