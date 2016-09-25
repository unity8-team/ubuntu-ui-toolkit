/*
 * Copyright 2016 Canonical Ltd.
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

#include "ucalarm_p_p.h"
#include "alarmmanager_p_p.h"

UT_NAMESPACE_BEGIN

class DummyAlarmData : public UCAlarmPrivate
{
public:
    DummyAlarmData(UCAlarm *qq)
        : UCAlarmPrivate(qq)
    {}

    bool enabled() const override
    {
        return _enabled;
    }
    bool setEnabled(bool enabled) override
    {
        _enabled = enabled;
        return true;
    }
    QDateTime date() const override
    {
        return _date;
    }
    bool setDate(const QDateTime &date) override
    {
        _date = date;
        return true;
    }
    QString message() const override
    {
        return _message;
    }
    bool setMessage(const QString &message) override
    {
        _message = message;
        return true;
    }
    UCAlarm::AlarmType type() const override
    {
        return _type;
    }
    bool setType(UCAlarm::AlarmType type) override
    {
        _type = type;
        return true;
    }
    UCAlarm::DaysOfWeek daysOfWeek() const override
    {
        return _dow;
    }
    bool setDaysOfWeek(UCAlarm::DaysOfWeek days) override
    {
        _dow = days;
        return true;
    }
    QUrl sound() const override
    {
        return _sound;
    }
    bool setSound(const QUrl &sound) override
    {
        _sound = sound;
        return true;
    }
    QVariant cookie() const override
    {
        return _cookie;
    }
    UCAlarm::Error checkAlarm() override
    {
        return UCAlarm::AdaptationError;
    }

    void save() override
    {
        // noop
    }
    void cancel() override
    {
        // noop
    }
    void reset() override
    {
        // noop
    }
    void completeSave() override
    {
        // noop
    }
    void completeCancel() override
    {
        // noop
    }
    void copyAlarmData(const UCAlarm &other) override
    {
        _message = other.message();
        _sound = other.sound();
        _date = other.date();
        _cookie = other.cookie();
        _type = other.type();
        _dow = other.daysOfWeek();
        _enabled = other.enabled();
    }

private:
    QString _message;
    QUrl _sound;
    QDateTime _date;
    QVariant _cookie;
    UCAlarm::AlarmType _type;
    UCAlarm::DaysOfWeek _dow;
    bool _enabled{true};
};

class DummyManager : public AlarmManagerPrivate
{
public:
    DummyManager(AlarmManager *manager)
        : AlarmManagerPrivate(manager)
    {}
    void init() override
    {
        completed = true;
    }
    bool fetchAlarms() override
    {
        return false;
    }
    int alarmCount() override
    {
        return 0;
    }
    UCAlarm *getAlarmAt(int index) const override
    {
        Q_UNUSED(index);
        return Q_NULLPTR;
    }
    bool findAlarm(const UCAlarm &alarm, const QVariant &cookie) const override
    {
        Q_UNUSED(alarm);
        Q_UNUSED(cookie);
        return false;
    }

    // function to verify whether the given alarm property has a given value set
    // used for testing purposes
    bool verifyChange(UCAlarm *alarm, AlarmManager::Change change, const QVariant &value) override
    {
        Q_UNUSED(alarm);
        Q_UNUSED(change);
        Q_UNUSED(value);
        return false;
    }

    // creates an alarm data adaptation object
    UCAlarmPrivate *createAlarmData(UCAlarm *alarm) override
    {
        return new DummyAlarmData(alarm);
    }
};

AlarmManagerPrivate * createAlarmsAdapter(AlarmManager *alarms)
{
    return new DummyManager(alarms);
}

UT_NAMESPACE_END
