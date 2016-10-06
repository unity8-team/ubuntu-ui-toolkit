import QtQuick 2.2
import Ubuntu.Components 1.1
import './calendar.js' as Calendar
import './dateutils.js' as DU

Item {
    id: root

    property string fontSize: 'large'
    property color color: 'black'
    property color activeColor: 'grey'

    property date date: new Date()
    property date minimum: DU.clone(null, 'setDate', 1)
    property date maximum: DU.clone(minimum, 'setFullYear', minimum.getFullYear() + 50)

    property bool yearsListOpened: false

    signal requestPreviousMonth()
    signal requestNextMonth()
    signal requestToggleYearList()

    width: parent.width
    height: month.height

    MouseArea {
        id: yearDropDown
        onReleased: if (containsMouse) requestToggleYearList()
        anchors.fill: parent
        Row {
            spacing: FontUtils.sizeToPixels(root.fontSize) / 2
            anchors.horizontalCenter: parent.horizontalCenter
            height: parent.height
            Label {
                id: month
                fontSize: root.fontSize
                text: DU.format(root.date, 'MMMM')
                horizontalAlignment: Text.AlignHCenter
                color: yearDropDown.containsPress? root.activeColor : root.color
            }
            Label {
                id: year
                fontSize: root.fontSize
                text: DU.format(root.date, 'yyyy')
                horizontalAlignment: Text.AlignHCenter
                color: yearDropDown.containsPress? root.activeColor : root.color
            }
            Icon {
                name: yearsListOpened? 'up' : 'down'
                height: year.height / 2
                y: height / 2
                color: yearDropDown.containsPress? root.activeColor : root.color
            }
        }
    }

    MouseArea {
        visible: !DU.sameDay(root.date, minimum)
        anchors.left: parent.left
        width: parent.height
        height: width
        onReleased: if (containsMouse) requestPreviousMonth()
        Icon {
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width * 0.85
            height: width
            color: parent.containsPress? root.activeColor : root.color
            name: 'previous'
        }
    }

    MouseArea {
        visible: !DU.sameDay(root.date, maximum)
        anchors.right: parent.right
        width: parent.height
        height: width
        onReleased: if (containsMouse) requestNextMonth()
        Icon {
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width * 0.85
            height: width
            color: parent.containsPress? root.activeColor : root.color
            name: 'next'
        }
    }
}
