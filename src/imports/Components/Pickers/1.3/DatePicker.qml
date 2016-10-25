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
import Ubuntu.Components.Private 1.3
import 'dateutils.js' as DU

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
        \li \a minimum must be less or equal than the \l date; if the \a date
                value is less than the given \a minimum, the date will be set to
                the minimum's value
        \li \a maximum value must be greater than the \a minimum, or invalid.
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
        \li full name for month, number and full day for date (“August” “28 Wednesday”)
        \li otherwise full name for month, number and abbreviated day
            for date (“August” “28 Wed”);
        \li otherwise full name for month, number for date (“August” “28”);
        \li otherwise abbreviated name for month, number for date (“Aug” “28”).
        \li otherwise number for month, number for date (“08” “28”).
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
Item {
// StyledItem {
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
    //property date minimum: Date.prototype.midnight.call(new Date())
    property date minimum: DU.clone(null, 'setDate', 1)
    /*!
      \qmlproperty int maximum

      The maximum date (inclusive) to be shown in the picker.
      Both year and month values will be considered from the properties.

      See \l minimum for more details.
     */
    /*property date maximum: {
        var d = Date.prototype.midnight.call(new Date());
        d.setFullYear(d.getFullYear() + 50);
        return d;
    }*/
    property date maximum: DU.clone(minimum, 'setFullYear', minimum.getFullYear() + 50)

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
    readonly property alias moving: listView.moving

    implicitWidth: parent.width
    implicitHeight: content.visible ? content.height : timeContent.height
 // activeFocusOnPress: true

    /*!
      \qmlproperty Component delegate
      Individual days are rendered by the delegate, in the case of \l mode
      containing "Months". This is useful to customize the colors, frame, or
      add visuals for calendar events.
      Roles:
        date: the date of the rendered day
        text: the day as a string
        currentMonth: the date lies in the currently selected month
        pressed: whether this day is being pressed
     */
    property Component delegate: Label {
        id: label
        anchors.centerIn: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        property color dayColor: {
            if (DU.sameDay(date, datePicker.date))
                return theme.palette.normal.activity;
            if (currentMonth)
                return theme.palette.normal.backgroundText;
            // FIXME: Original color is too light for Qt.lighter
            return Qt.darker(theme.palette.normal.base);
        }
        color: pressed ? Qt.lighter(dayColor) : dayColor
        textSize: Label.Medium
        text: modelData.text

        Frame {
            anchors.fill: parent
            thickness: units.dp(2)
            radius: units.gu(1.7)
            color: theme.palette.normal.backgroundText
            property date today: new Date()
            visible: date.getFullYear() == today.getFullYear() && date.getMonth() == today.getMonth() && DU.sameDay(date, today)
        }
    }

    /*! \internal */
    onMinimumChanged: {
        if (internals.completed && !minimum.isValid()) {
            // set the minimum to the date
            minimum = date;
        }

        // adjust date
        if (date !== undefined && Date.prototype.isValid.call(minimum) && date < minimum && internals.completed) {
            date = minimum;
        }
    }
    /*! \internal */
    onMaximumChanged: {
        // adjust date
        if (date !== undefined && Date.prototype.isValid.call(maximum) && date > maximum && maximum > minimum  && internals.completed) {
            date = maximum;
        }
    }

    onDateChanged: {
        if (!moving)
            listView.positionViewAtIndex(DU.getMonthsRange(minimum, date), ListView.Visible)
    }

    property int monthsRange: DU.getMonthsRange(minimum, maximum)
    // FIXME: Move to Style
    property int scrollDirection: Qt.Horizontal
    property int snapMode: ListView.SnapToItem
    state: ''

    function cancelFlick() {
        listView.cancelFlick()
    }

    function toggleYearList() {
        datePicker.state = datePicker.state === 'years-months'? '' : 'years-months'
    }

    /*! \internal */
    onModeChanged: internals.updatePickers()
    /*! \internal */
 // onLocaleChanged: internals.updatePickers()

    Component.onCompleted: {
        if (minimum === undefined) {
            minimum = date;
        }
        internals.completed = true;
        internals.updatePickers();
    }

    width: parent.width
    height: content.height

 /*
    styleName: "DatePickerStyle"
    Binding {
        target: __styleInstance
        property: "view"
        value: positioner
    }
    Binding {
        target: __styleInstance
        property: "pickerModels"
        value: tumblerModel
    }
    Binding {
        target: __styleInstance
        property: "unitSeparator"
        value: (internals.showHoursPicker || internals.showMinutesPicker || internals.showSecondsPicker) ?
                   ":" : ""
    }
  */

    QtObject {
        id: internals
        property bool completed: false
        property real margin: units.gu(2)
        property real dayPickerRatio: 0.1

        property bool showYearPicker: true
        property bool showMonthPicker: true
        property bool showDayPicker: true

        property bool showHoursPicker: false
        property bool showMinutesPicker: false
        property bool showSecondsPicker: false
        property bool showDayNightPicker: showHoursPicker || showMinutesPicker || showSecondsPicker

        /*
          Update pickers.
          */
        function updatePickers() {
            if (completed) {
                // check mode flags first
                // FIXME: The js split(/\W/g) terminates the process on armhf with Qt 5.3 (v4 js) (https://bugreports.qt-project.org/browse/QTBUG-39255)
                var modes = datePicker.mode.match(/\w+/g);

                showYearPicker = showMonthPicker = showDayPicker =
                showHoursPicker = showMinutesPicker = showSecondsPicker = false;
                while (modes.length > 0) {
                    var modeFlag = modes.pop();
                    switch (modeFlag) {
                    case "Years":
                        showYearPicker = true;
                        break;
                    case "Months":
                        showMonthPicker = true;
                        break;
                    case "Days":
                        showDayPicker = true;
                        break;
                    case "Hours":
                        showHoursPicker = true;
                        break;
                    case "Minutes":
                        showMinutesPicker = true;
                        break;
                    case "Seconds":
                        showSecondsPicker = true;
                        break;
                    default:
                        console.warn("Unhandled mode flag: " + modeFlag + ". Mode will not be set!");
                        return;
                    }
                }

                // filter unaccepted date picking mode
                if (!showMonthPicker && showYearPicker && showDayPicker) {
                    console.warn("Invalid DatePicker mode: " + datePicker.mode);
                    return;
                }

                // filter unaccepted time picking mode
                if (showHoursPicker && showSecondsPicker && !showMinutesPicker) {
                    console.warn("Invalid DatePicker mode: " + datePicker.mode);
                    return;
                }

                // date and time picking not allowed at the same time
                if ((showYearPicker || showMonthPicker || showDayPicker) &&
                        (showHoursPicker || showMinutesPicker || showSecondsPicker)) {
                    console.warn("Date and Time picking not allowed at the same time.");
                    return;
                }
            }
        }
    }

    states: [
        State { name: '' },
        State { name: 'years-months' }
    ]

    property real dropdownAnimDuration: UbuntuAnimation.FastDuration

    transitions: [
        Transition {
            from: ''
            to: 'years-months'
            ParallelAnimation {
                UbuntuNumberAnimation {
                    target: headerShadow
                    property: 'width'
                    to: content.width
                    duration: datePicker.dropdownAnimDuration / 2
                }
                UbuntuNumberAnimation {
                    target: headerShadow
                    property: 'opacity'
                    to: 1
                    duration: datePicker.dropdownAnimDuration / 2
                }
                UbuntuNumberAnimation {
                    target: yearsListDropdown
                    property: 'y'
                    to: 0
                    duration: datePicker.dropdownAnimDuration
                }
                SequentialAnimation {
                    PauseAnimation {
                        duration: datePicker.dropdownAnimDuration
                    }
                    UbuntuNumberAnimation {
                        target: headerShadow
                        property: 'width'
                        to: 0
                        duration: datePicker.dropdownAnimDuration / 1.5
                    }
                }
            }
        },
        Transition {
            from: 'years-months'
            to: ''
            ParallelAnimation {
                property real duration: 300
                UbuntuNumberAnimation {
                    target: headerShadow
                    property: 'width'
                    to: content.width
                    duration: datePicker.dropdownAnimDuration / 2
                }
                UbuntuNumberAnimation {
                    target: headerShadow
                    property: 'opacity'
                    to: 1
                    duration: datePicker.dropdownAnimDuration / 2
                }
                UbuntuNumberAnimation {
                    target: yearsListDropdown
                    property: 'y'
                    to: -yearsListDropdown.height
                    duration: datePicker.dropdownAnimDuration
                }
                SequentialAnimation {
                    PauseAnimation {
                        duration: datePicker.dropdownAnimDuration
                    }
                    UbuntuNumberAnimation {
                        target: headerShadow
                        property: 'width'
                        to: 0
                        duration: datePicker.dropdownAnimDuration / 1.5
                    }
                    UbuntuNumberAnimation {
                        target: headerShadow
                        property: 'opacity'
                        to: 0
                        duration: datePicker.dropdownAnimDuration / 3
                    }
                }
            }
        }
    ]

    Rectangle {
        color: theme.palette.normal.background
        anchors.fill: parent
    }

    // TimePicker

    Item {
        id: timeContent
        height: container.itemHeight * 5
        anchors.leftMargin: root.margin
        anchors.rightMargin: root.margin
        visible: internals.showHoursPicker || internals.showMinutesPicker || internals.showSecondsPicker

        Row {
            id: container
            anchors.fill: parent
            property real itemHeight: units.gu(2)
            property int pickerCount: 3
            property real pickerWidth: (container.width - internals.margin * container.pickerCount - 1) / container.pickerCount

            HourPicker {
                type: 'hours'
                width: container.pickerWidth
                height: container.height
                ampm: internals.showDayPicker
            }

            HourPicker {
                type: 'minutes'
                width: container.pickerWidth
                height: container.height
            }

            HourPicker {
                type: 'seconds'
                width: container.pickerWidth
                height: container.height
            }

            DayNightPicker {
                width: container.pickerWidth
                height: container.height
                itemHeight: container.itemHeight
            }

            DigitSeparator {
                width: container.pickerWidth
                height: container.height
            }
        }
    }

    Item {
        id: selectedShape
        width: parent.width
        height: parent.height / 5
        y: parent.height / 2 - height / 2
        visible: internals.showHoursPicker || internals.showMinutesPicker || internals.showSecondsPicker

        Rectangle {
            width: parent.width
            height: units.dp(1)
            y: 0
            color: UbuntuColors.lightGrey
        }

        Rectangle {
            width: parent.width
            height: units.dp(1)
            y: parent.height - height
            color: UbuntuColors.lightGrey
        }
    }

    // DatePicker

    Column {
        id: content
        width: parent.width
        visible: internals.showMonthPicker || internals.showYearPicker || internals.showDayPicker

        DatePickerHeader {
            id: header
            anchors.left: parent.left
            anchors.right: parent.right
            fontSize: 'large'
            color: theme.palette.normal.backgroundText
            activeColor: UbuntuColors.darkGrey
            date: DU.clone(datePicker.date, 'setDate', 1)
            minimum: datePicker.minimum
            maximum: datePicker.maximum
            onRequestNextMonth: datePicker.date = DU.clone(datePicker.date, 'setMonth', datePicker.date.getMonth() + 1)
            onRequestPreviousMonth: datePicker.date = DU.clone(datePicker.date, 'setMonth', datePicker.date.getMonth() - 1)
            onRequestToggleYearList: datePicker.toggleYearList()
            yearsListOpened: datePicker.state === 'years-months'
            visible: internals.showYearPicker
        }

        Item {
            width: parent.width
            height: header.height * 0.5
            Rectangle {
                id: headerShadow
                y: parent.height
                visible: false
                opacity: 0
                color: theme.palette.normal.baseText
                width: 0
                height: units.gu(1/16)
                anchors.horizontalCenter: parent.horizontalCenter
            }
            visible: internals.showYearPicker || internals.showMonthPicker
        }

        Item {
            width: parent.width
            height: listView.height
            visible: internals.showDayPicker
            ListView {
                id: listView
                clip: true
                width: parent.width
                height: currentItem.height
                model: datePicker.monthsRange
                snapMode: datePicker.snapMode
                orientation: datePicker.scrollDirection === Qt.Horizontal ? ListView.Horizontal : ListView.Vertical
                highlightFollowsCurrentItem: true
                highlightMoveDuration: 0
                highlightRangeMode: ListView.StrictlyEnforceRange
                property bool indexInit: false
                onCurrentIndexChanged: {
                    if (!indexInit && currentIndex === 0) {
                        indexInit = true;
                        return;
                    }
                    if (!listView.moving) {
                        return;
                    }
                    datePicker.date = DU.clone(minimum, 'setMonth', minimum.getMonth() + currentIndex);
                }

                delegate: DatePickerMonthDays {
                    id: panel
                    width: datePicker.width
                    displayedMonth: DU.clone(minimum, 'setMonth', minimum.getMonth() + index)
                    onRequestNextMonth: datePicker.date = DU.clone(displayedMonth, 'setMonth', displayedMonth.getMonth() + 1)
                    onRequestPreviousMonth: datePicker.date = DU.clone(displayedMonth, 'setMonth', displayedMonth.getMonth() - 1)
                    onRequestDateChange: datePicker.date = date
                    delegate: datePicker.delegate
                }
            }

            Item {
                clip: true
                width: parent.width
                height: parent.height
                YearListDropdown {
                    id: yearsListDropdown
                    minimum: datePicker.minimum
                    maximum: datePicker.maximum
                    onRequestDateChange: datePicker.date = newDate
                }
            }
        }
    }
}
