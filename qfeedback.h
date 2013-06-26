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

#ifndef QFEEDBACK_MIR_H
#define QFEEDBACK_MIR_H

#include <QtCore/QList>
#include <QtCore/QVector>
#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <QtCore/QTimer>

#include <qfeedbackplugininterfaces.h>

QT_BEGIN_HEADER
QT_USE_NAMESPACE

class QFeedbackMir : public QObject, public QFeedbackHapticsInterface, public QFeedbackFileInterface, public QFeedbackThemeInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtFeedbackPlugin" FILE "mir.json")

    Q_INTERFACES(QFeedbackHapticsInterface)
    Q_INTERFACES(QFeedbackFileInterface)
    Q_INTERFACES(QFeedbackThemeInterface)

public:
    QFeedbackMir();
    virtual ~QFeedbackMir();

    virtual PluginPriority pluginPriority();
    virtual QList<QFeedbackActuator*> actuators();

    //for actuator handling
    virtual void setActuatorProperty(const QFeedbackActuator &, ActuatorProperty, const QVariant &);
    virtual QVariant actuatorProperty(const QFeedbackActuator &, ActuatorProperty);
    virtual bool isActuatorCapabilitySupported(const QFeedbackActuator &, QFeedbackActuator::Capability);

    virtual void updateEffectProperty(const QFeedbackHapticsEffect *, EffectProperty);
    virtual void setEffectState(const QFeedbackHapticsEffect *, QFeedbackEffect::State);
    virtual QFeedbackEffect::State effectState(const QFeedbackHapticsEffect *);

    //for loading files
    virtual void setLoaded(QFeedbackFileEffect*, bool);
    virtual void setEffectState(QFeedbackFileEffect *, QFeedbackEffect::State);
    virtual QFeedbackEffect::State effectState(const QFeedbackFileEffect *);
    virtual int effectDuration(const QFeedbackFileEffect *);
    virtual QStringList supportedMimeTypes();

    // theming
    virtual bool play(QFeedbackEffect::Effect);
private:
    QList<QFeedbackActuator*> actuatorList;

    void vibrateOnce(const QFeedbackEffect* effect);
};

QT_END_HEADER

#endif // QFEEDBACK_MIR_H
