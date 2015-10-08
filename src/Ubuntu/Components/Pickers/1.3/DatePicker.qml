/*
 * Copyright 2015 Canonical Ltd.
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
 */

import QtQuick 2.4
import Ubuntu.Components 1.3

/*!
    \qmltype DatePicker
    \inqmlmodule Ubuntu.Components.Pickers 1.0
    \ingroup ubuntu-pickers
    \brief DatePicker component provides date and time value picking functionality.

    DatePicker combines up to three Picker elements providing different date or time
    value selection possibilities. It can be used to select full date (year, month,
    day), full time (hours, minutes, seconds) as well as to select a combination of
    year and month, month and day, hours and minutes, minutes and seconds or individual
    time units (i.e. year, month or day as well as hours, minutes or seconds). The
    selected date as well as the initial one is provided by the \l date property.
    For convenience the component provides also the \a year, \a month, \a day,
    \a week, \a hours, \a minutes and \a seconds values as separate properties,
    however these properties are not writable, and their initialization can happen
    only through the \l date property.

    \qml
    import QtQuick 2.4
    import Ubuntu.Components 1.3
    import Ubuntu.Components.Pickers 1.0

    Column {
        Label {
            text: "Selected date: W" + datePicker.week + " - " +
                    Qt.formatDate(datePicker.date, "dddd, dd-MMMM-yyyy")
        }
        DatePicker {
            id: datePicker
        }
    }
    \endqml

    The \l mode property specifies what time units should be shown by the picker.
    The property holds a string, combining \b Years, \b Months, \b Days, \b Hours,
    \b Minutes and \b Seconds strings sepatared with '|' character. A DatePicker
    which shows only year and month date units would look as follows:
    \qml
    import QtQuick 2.4
    import Ubuntu.Components 1.3
    import Ubuntu.Components.Pickers 1.0

    Column {
        Label {
            text: "Selected month: " + Qt.formatDate(datePicker.date, "MMMM-yyyy")
        }
        DatePicker {
            id: datePicker
            mode: "Years|Months"
        }
    }
    \endqml

    The \b mode of the DatePicker is set to date picking. In case time picking
    is needed, the model should be set to contain the time specific mode flags.
    The following example demonstrates how to use DatePicker for time picking.
    \qml
    import QtQuick 2.4
    import Ubuntu.Components 1.3
    import Ubuntu.Components.Pickers 1.0

    Column {
        Label {
            text: "Selected time: " + Qt.formatTime(datePicker.date, "hh:mm:ss")
        }
        DatePicker {
            id: datePicker
            mode: "Hours|Minutes|Seconds"
        }
    }
    \endqml
    Note that the order in which the mode flags are specified does not influence
    the order the pickers are arranged. That is driven by the date format of the
    \l locale used in the picker. Also not all combinations of mode flags are
    supported. See \l mode for the supported combinations.

    The default interval the date values are chosen is a window starting at
    the current date ending 50 years later. This window is defined by the
    \a minimum and \a maximum properties. The interval can be altered considering
    the following rules:
    \list
        \li - \a minimum must be less or equal than the \l date; if the \a date
                value is less than the given \a minimum, the date will be set to
                the minimum's value
        \li - \a maximum value must be greater than the \a minimum, or invalid.
                When the maximum is smaller than the \l date, the \l date property
                will be updated to get the maximum value.
                When set to invalid date (see Date.getInvalidDate()), the upper
                limit of the date interval becomes infinite, meaning the year
                picker will extend infinitely. This leads to increased memory
                use and should be avoided if possible. Invalid date will make
                hours picker presenting 24 hours.
    \endlist
    \qml
    import QtQuick 2.4
    import Ubuntu.Components 1.3
    import Ubuntu.Components.Pickers 1.0

    Column {
        Label {
            text: "Selected date: " + Qt.formatDate(datePicker.date, "dddd, dd-MMMM-yyyy")
        }
        DatePicker {
            id: datePicker
            minimum: {
                var d = new Date();
                d.setFullYear(d.getFullYear() - 1);
                return d;
            }
            maximum: Date.prototype.getInvalidDate.call()
        }
    }
    \endqml
    \b Note: do not use the \l date property when initializing minimum and maximum
    as it will cause binding loops.

    \section2 Layout
    As mentioned earlier, DatePicker combines up to three Picker tumblers depending
    on the mode requested. These tumblers are laid out in a row in the order the
    default date format of the \l locale is.

    \section3 Date picker layout rules
    The date picker consist of three pickers: year, month, and date. The exact
    contents of the month and date pickers depends on the available width:
    \list
        \li * full name for month, number and full day for date (“August” “28 Wednesday”)
        \li * otherwise full name for month, number and abbreviated day
            for date (“August” “28 Wed”);
        \li * otherwise full name for month, number for date (“August” “28”);
        \li * otherwise abbreviated name for month, number for date (“Aug” “28”).
        \li * otherwise number for month, number for date (“08” “28”).
    \endlist

    \a{If the currently selected date becomes impossible due to year change (from a
    leap to a non-leap year when the date is set to February 29) or month change
    (e.g. from a month that has 31 days to one that has fewer when the date is
    set to 31), the date reduces automatically to the last day of the month (i.e
    February 28 or 30th day of the month).}

    \section3 Time picker layout rules
    Time units are shown in fixed width picker tumblers, numbers padded with
    leading zeroes. There is no other special rule on the formatting of the time
    unit numbers.

    \section3 How minimum/maximum affects the tumblers

    If minimum and maximum are within the same year, the year picker will be
    insensitive. If minimum and maximum are within the same month, the month picker
    will also be insensitive.
  */
StyledItem {
    id: datePicker

    /*!
      Specifies what kind of date value selectors should be shown by the picker.
      This is a string of 'flags' separated by '|' separator, where flags are:
      \table
        \header
        \li {2, 1} Date picker modes
            \header
                \li Value
                \li Description
            \row
                \li Years
                \li Specifies to show the year picker
            \row
                \li Months
                \li Specifies to show the month picker
            \row
                \li Days
                \li Specifies to show the day picker
        \header
        \li {2, 1} Time picker modes
            \header
                \li Value
                \li Description
            \row
                \li Hours
                \li Specifies to show the hours picker
            \row
                \li Minutes
                \li Specifies to show the minutes picker
            \row
                \li Seconds
                \li Specifies to show the seconds picker
      \endtable
      With some exceptions, any combination of these flags is allowed within the
      same group. Date and time picker modes cannot be combined.

      The supported combinations are: \a{Years|Months|Days}, \a{Years|Months},
      \a{Months|Days}, \a{Hours|Minutes|Seconds}, \a{Hours|Minutes} and \a{Minutes|Seconds},
      as well as each mode flag individually.

      The default value is "\a{Years|Months|Days}".
      */
    property string mode: "Years|Months|Days"

    /*!
      The date chosen by the DatePicker. The default value is the date at the
      component creation time. The property automatically updates year, month
      and day properties.
      */
    property date date: Date.prototype.midnight.call(new Date())

    /*!
      \qmlproperty int minimum
      The minimum date (inclusive) to be shown in the picker.
      Both year and month values will be considered from the properties.

      The year and month picker values are filled based on these values. The
      year picker will be infinite (extending infinitely) if the maximum is
      an invalid date. If the distance between maximum and minimum is less than
      a year, the year picker will be shown disabled.

      The month picker will be circular if the distance between maximum and minimum
      is at least one year, or if the maximum date is invalid.

      The default values are the current date for the minimum, and 50 year distance
      value for maximum.
      */
    property date minimum: Date.prototype.midnight.call(new Date())
    /*!
      \qmlproperty int maximum

      The maximum date (inclusive) to be shown in the picker.
      Both year and month values will be considered from the properties.

      See \l minimum for more details.
     */
    property date maximum: {
        var d = Date.prototype.midnight.call(new Date());
        d.setFullYear(d.getFullYear() + 50);
        return d;
    }

    /*!
      For convenience, the \b year value of the \l date property.
      */
    readonly property int year: datePicker.date.getFullYear()
    /*!
      For convenience, the \b month value of the \l date property.
     */
    readonly property int month: datePicker.date.getMonth()
    /*!
      For convenience, the \b day value of the \l date property.
     */
    readonly property int day: datePicker.date.getDate()
    /*!
      For convenience, the \b week value of the \l date property.
     */
    readonly property int week: datePicker.date.getWeek()
    /*!
      For convenience, the \b hours value of the \l date property.
     */
    readonly property int hours: datePicker.date.getHours()
    /*!
      For convenience, the \b minutes value of the \l date property.
     */
    readonly property int minutes: datePicker.date.getMinutes()
    /*!
      For convenience, the \b seconds value of the \l date property.
     */
    readonly property int seconds: datePicker.date.getSeconds()

    /*!
      The property defines the locale used in the picker. The default value is
      the system locale.
      \qml
      DatePicker {
           locale: Qt.locale("hu_HU")
      }
      \endqml
      */
    property var locale: Qt.locale()

    /*!
      \qmlproperty bool moving
      \readonly
      The property holds whether the component's pickers are moving.
      \sa Picker::moving
      */
    readonly property alias moving: positioner.moving

    implicitWidth: units.gu(36)
    implicitHeight: units.gu(20)
    activeFocusOnPress: true

    /*! \internal */
    onMinimumChanged: {
        if (minimum.isValid()) {
            // set the minimum to the date
            minimum = date;
        }

        // adjust date
        if (date !== undefined && Date.prototype.isValid.call(minimum) && date < minimum) {
            date = minimum;
        }
    }
    /*! \internal */
    onMaximumChanged: {
        // adjust date
        if (date !== undefined && Date.prototype.isValid.call(maximum) && date > maximum && maximum > minimum) {
            date = maximum;
        }
    }
    /*! \internal */
    onWidthChanged: {
        // use dayPicker narrowFormatLimit even if the dayPicker is hidden
        // and clamp the width so it cannot have less width that the sum of
        // the three tumblers' narrowFormatLimit
        var minWidth = positioner.tumblerModel.length * positioner.narrowFormatLimit;
        width = Math.max(width, minWidth);
    }

    // tumbler positioner
    Row {
        id: positioner
        anchors {
            top: parent.top
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
        }
        parent: (datePicker.__styleInstance && datePicker.__styleInstance.hasOwnProperty("tumblerHolder")) ?
                 datePicker.__styleInstance.tumblerHolder : datePicker
        property bool moving
        // FIXME: The js split(/\W/g) terminates the process on armhf with Qt 5.3 (v4 js) (https://bugreports.qt-project.org/browse/QTBUG-39255)
        property var modes: mode.match(/\w+/g)
        // use short format to exclude any extra characters
        property var dateFormats: locale.dateFormat(Locale.ShortFormat).match(/\w+/g)
        property var timeFormats: 'h:m:s'.match(/\w+/g)
        property var map: {
            'y': 'Years',
            'M': 'Months',
            'd': 'Days',
            'h': 'Hours',
            'm': 'Minutes',
            's': 'Seconds',
        }
        property real margin: 2 * units.gu(1.5)
        property real narrowFormatLimit
        property real shortYearLimit
        property real longMonthLimit
        property real shortMonthLimit
        property real longDayLimit
        property real shortDayLimit
        Component.onCompleted: {
            textSizer.text = '99';
            narrowFormatLimit = textSizer.paintedWidth + margin;
            textSizer.text = '9999';
            shortYearLimit = textSizer.paintedWidth + margin;
            var width = narrowFormatLimit;
            for (var i = 0; i < 12; i++) {
                textSizer.text = locale.monthName(i, Locale.LongFormat);
                width = Math.max(width, textSizer.paintedWidth + margin);
            }
            longMonthLimit = width;
            width = narrowFormatLimit;
            for (var i = 0; i < 12; i++) {
                textSizer.text = locale.monthName(i, Locale.ShortFormat);
                width = Math.max(width, textSizer.paintedWidth + margin);
            }
            shortMonthLimit = width;
            width = narrowFormatLimit;
            for (var i = 0; i < 7; i++) {
                textSizer.text = locale.dayName(i, Locale.LongFormat);
                width = Math.max(width, textSizer.paintedWidth + margin);
            }
            longDayLimit = width;
            width = narrowFormatLimit;
            for (var i = 0; i < 7; i++) {
                textSizer.text = locale.dayName(i, Locale.ShortFormat);
                width = Math.max(width, textSizer.paintedWidth + margin);
            }
            shortDayLimit = width;
        }
        property real dayPickerRatio: {
            var maxWidth = 0.0;
            if (modes.indexOf('Years') > -1)
                maxWidth += shortYearLimit;
            if (modes.indexOf('Months') > -1)
                maxWidth += longMonthLimit;
            if (modes.indexOf('Days') > -1) {
                maxWidth += longDayLimit;
                return (longDayLimit / maxWidth).toPrecision(3);
            }
            return 0.1
        }
        property real daysWidth: {
            var w = Math.max(datePicker.width * dayPickerRatio, narrowFormatLimit);
            if (w < longDayLimit && w >= shortDayLimit)
                return shortDayLimit;
            return narrowFormatLimit;
        }
        property var pickerSizes: {
            var sizes = [];
            for (var i in tumblerModel) {
                var mode = tumblerModel[i];
                if (mode == 'Months')
                    sizes.push(MathUtils.clamp(datePicker.width - narrowFormatLimit - daysWidth, narrowFormatLimit, longMonthLimit));
                else if (mode == 'Days')
                    sizes.push(daysWidth);
                else if (mode == 'Years')
                    sizes.push(shortYearLimit);
                else
                    sizes.push(narrowFormatLimit);
            }
            return sizes;
        }
        property var tumblerModel: {
            var ordered = [];
            for (var i in dateFormats) {
                var format = dateFormats[i];
                var mode = map[format[0]];
                if (modes.indexOf(mode) > -1)
                    ordered.push(mode);
            }
            if (ordered.length > 0) {
                return ordered;
            }
            for (var i in timeFormats) {
                var format = timeFormats[i];
                var mode = map[format[0].toLowerCase()];
                if (modes.indexOf(mode) > -1)
                    ordered.push(mode);
            }
            return ordered;
        }
        // loop through the format to decide the position of the tumbler
        Repeater {
            model: positioner.tumblerModel

            Picker {
                id: pickerUnit
                model: {
                    switch (modelData) {
                    case "Years":
                        var from = (minimum.getFullYear() <= 0) ? date.getFullYear() : minimum.getFullYear();
                        var to = (maximum < minimum) ? -1 : maximum.getFullYear();
                        var items = to - from, baseYear = from;
                        if (items < 0)
                            items = 50;
                        var years = [];
                        for (var i = baseYear; i <= baseYear + items; i++) {
                            var year = new Date(date);
                            year.setYear(i)
                            years.push({'format': 'yyyy', 'date': year});
                            if (year.getYear() == date.getYear())
                                selectedIndex = i - baseYear;
                        }
                        return years;
                    case "Months":
                        var distance, from, to;
                        distance = to = maximum.isValid() ? minimum.monthsTo(maximum) : 11;
                        if (to < 0 || to > 11)
                            to = 11;
                        from = (to < 11) ? minimum.getMonth() : 0;
                        var format = datePicker.width >= positioner.longMonthLimit ? 'MMMM' : 'MMM';
                        var format = 'MM';
                        if (datePicker.width >= positioner.longMonthLimit)
                           format = 'MMMM';
                        else if (datePicker.width >= positioner.shortMonthLimit)
                           format = 'MMM';
                        var months = [];
                        for (var i = from; i <= from + to; i++) {
                            var month = new Date(date);
                            month.setMonth(i)
                            months.push({'format': format, 'date': month});
                            if (month.getMonth() == date.getMonth())
                                selectedIndex = i - from;
                        }
                        return months;
                    case "Days":
                        var format = 'dd';
                        if (datePicker.width >= positioner.longDayLimit)
                           format += ' dddd';
                        else if (datePicker.width >= positioner.shortDayLimit)
                           format += ' ddd';
                        var days = [];
                        for (var i = 1; i <= date.daysInMonth(year, month); i++) {
                            var day = new Date(date);
                            day.setDate(i);
                            days.push({'format': format, 'date': day});
                            if (day.getDate() == date.getDate())
                                selectedIndex = i - 1;
                        }
                        return days;
                    case "Hours":
                        var from = minimum.getHours();
                        var distance = (!Date.prototype.isValid.call(maximum) || (minimum.daysTo(maximum) > 1)) ? 24 : minimum.hoursTo(maximum);
                        var hours = []
                        for (var i = 0; i < distance; i++) {
                            var hour = new Date(date);
                            hour.setHours((from + i) % 24);
                            hours.push({'format': 'hh', 'date': hour});
                            if (hour.getHours() == date.getHours())
                                selectedIndex = i;
                        }
                        return hours;
                    case "Minutes":
                        var from = minimum.getMinutes();
                        var distance = (!maximum.isValid() || (minimum.daysTo(maximum) > 1) || (minimum.minutesTo(maximum) >= 60)) ? 60 : minimum.minutesTo(maximum);
                        var minutes = [];
                        for (var i = 0; i < distance; i++) {
                            var minute = new Date(date);
                            minute.setMinutes((from + i) % 60);
                            minutes.push({'format': 'mm', 'date': minute});
                            if (minute.getMinutes() == date.getMinutes())
                                selectedIndex = i;
                        }
                        return minutes;
                    case "Seconds":
                        var from = minimum.getSeconds();
                        var distance = (!maximum.isValid() || (minimum.daysTo(maximum) > 1) || (minimum.secondsTo(maximum) >= 60)) ? 59 : minimum.secondsTo(maximum);
                        var seconds = [];
                        for (var i = 0; i <= distance; i++) {
                            var second = new Date(date);
                            second.setSeconds((from + i) % 60);
                            seconds.push({'format': 'ss', 'date': second});
                            if (second.getSeconds() == date.getSeconds())
                                selectedIndex = i;
                        }
                        return seconds;
                    default:
                        console.warn("Unhandled mode flag: " + modelData + ". Mode will not be set!");
                    }
                }
                live: false
                circular: modelData != 'Years'
                width: positioner.pickerSizes[index]
                height: datePicker.height
                delegate: PickerDelegate {
                    Label {
                        text: {
                            if (!modelData.hasOwnProperty('date'))
                                return modelData;
                            var format = modelData.format;
                            if (format.match('[dMy]'))
                                return Qt.formatDate(modelData.date, format);
                            return Qt.formatTime(modelData.date, format)
                        }
                        anchors.fill: parent
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
                style: Rectangle {
                    anchors.fill: parent
                    color: (pickerUnit.Positioner.index % 2) ? Qt.rgba(0, 0, 0, 0.03) : Qt.rgba(0, 0, 0, 0.07)
                }
            }
        }
    }

    // component to calculate text fitting
    Label {
        id: textSizer
        visible: false
    }

    theme.version: Ubuntu.toolkitVersion
    styleName: "DatePickerStyle"
    Binding {
        target: __styleInstance
        property: "view"
        value: positioner
    }
    Binding {
        target: __styleInstance
        property: "pickerModels"
        value: positioner.tumblerModel
    }
    Binding {
        target: __styleInstance
        property: "pickerSizes"
        value: positioner.pickerSizes
    }
    Binding {
        target: __styleInstance
        property: "unitSeparator"
        value: mode.indexOf('Minutes') > -1 ? ":" : ""
    }
}
