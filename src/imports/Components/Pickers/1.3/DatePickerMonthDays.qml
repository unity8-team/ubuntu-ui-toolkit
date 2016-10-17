import QtQuick 2.4
import Ubuntu.Components 1.3
import './calendar.js' as Calendar
import './dateutils.js' as DU

MouseArea {
    id: root

    height: cellHeight * 7
    onPressed: lastPressedPosition = getPosition(mouse)

    onReleased: {
        lastPressedPosition = null
        var pressedPosition = getPosition(mouse)
        var index = pressedPosition[1] * 7 + pressedPosition[0]
        var item = monthDaysRepeater.itemAt(index)

        var currDay = root.monthDays[index]
        var currDate = currDay.date
        var dispDate = root.displayedMonth
        if (currDay.surrounding) {
            if (DU.compareDates(currDate, dispDate) < 0) {
                requestPreviousMonth()
            } else {
                requestNextMonth()
            }
        } else if (root.date.getTime() !== currDate.getTime()) {
            requestDateChange(currDate)
        }
    }

    property date date: new Date()
    property date displayedMonth: new Date(date.getTime())
    property int firstDayOfWeek: 1
    property var monthDays: []
    property Component delegate

    readonly property real cellWidth: width / 7
    readonly property real cellHeight: cellWidth * 0.62

    property var lastPressedPosition: null

    property Item pressedItem: {
        if (!(root.containsPress && root.lastPressedPosition)) {
            return null
        }
        var index = lastPressedPosition[1] * 7 + lastPressedPosition[0]
        return monthDaysRepeater.itemAt(index)
    }

    function getPosition(mouse) {
        return [
            Math.floor(mouse.x / cellWidth),
            Math.floor((mouse.y - cellHeight) / cellHeight),
        ]
    }

    WorkerScript {
        source: 'DatePickerMonthDaysWorker.js'
        onMessage: {
            if (messageObject.name === 'monthDays') {
                root.monthDays = messageObject.monthDays
                monthDaysRepeater.model = messageObject.monthDays.length
            }
        }
        Component.onCompleted: {
            this.sendMessage({
                name: 'requestMonthDays',
                firstDayOfWeek: firstDayOfWeek,
                displayedMonth: displayedMonth,
            })
        }
    }

    signal requestPreviousMonth()
    signal requestNextMonth()
    signal requestDateChange(date date)

    Repeater {
        model: 7
        Label {
            id: label
            width: cellWidth
            height: cellHeight
            x: index * cellWidth
            y: 0
            text: DU.shortDayName(index).toUpperCase()
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            textSize: Label.XSmall
            color: theme.palette.normal.backgroundTertiaryText
        }
    }

    Repeater {
        id: monthDaysRepeater
        model: 0
        Item {
            id: monthDayItem
            width: cellWidth
            height: cellHeight
            x: (index % 7) * cellWidth
            y: ((index - index % 7) / 7) * cellHeight + cellHeight
            Loader {
                property var modelData: root.monthDays[index]
                property date date: modelData.date
                property string text: modelData.text
                property bool currentMonth: !modelData.surrounding
                property bool pressed: monthDayItem === pressedItem
                anchors.fill: parent
                sourceComponent: delegate
            }
        }
    }
}
