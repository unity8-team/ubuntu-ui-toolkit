import QtQuick 2.2
import Ubuntu.Components 1.1
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

    property color labelColor: '#aaaaaa'

    property color color: '#cbcbcb'
    property color pressedColor: '#dfdfdf'

    property color activeColor: '#e65e17'
    property color pressedActiveColor: Qt.lighter(activeColor)

    property color surroundingColor: '#dfdfdf'
    property color pressedSurroundingColor: '#e8e8e8'

    readonly property real cellWidth: width / 7
    readonly property real cellHeight: cellWidth * 0.62

    readonly property real fontSizeLimitLarge: units.gu(45)
    readonly property real fontSizeLimitMedium: units.gu(35)

    readonly property string dayFontSize: {
        if (root.width > fontSizeLimitLarge) return 'x-large'
        if (root.width < fontSizeLimitMedium) return 'medium'
        return 'large'
    }

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

    function getDayColor(item, day) {
        var pressed = item === pressedItem
        if (DU.sameDay(day.date, root.date)) {
            return pressed? pressedActiveColor : activeColor
        }
        if (day.surrounding) {
            return pressed? pressedSurroundingColor : surroundingColor
        }
        return pressed? pressedColor : color
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
        Text {
            id: label
            width: cellWidth
            height: cellHeight
            x: index * cellWidth
            y: 0
            text: DU.shortDayName(index).toUpperCase()
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: FontUtils.sizeToPixels('small')
            color: root.labelColor
        }
    }

    Repeater {
        id: monthDaysRepeater
        model: 0
        Text {
            id: label
            width: cellWidth
            height: cellHeight
            x: (index % 7) * cellWidth
            y: ((index - index % 7) / 7) * cellHeight + cellHeight
            text: root.monthDays[index].text
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: FontUtils.sizeToPixels(root.dayFontSize)
            color: getDayColor(label, root.monthDays[index])
        }
    }
}
