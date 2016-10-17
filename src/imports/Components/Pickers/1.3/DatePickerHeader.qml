import QtQuick 2.4
import Ubuntu.Components 1.3
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
            spacing: 0
            anchors.horizontalCenter: parent.horizontalCenter
            height: parent.height
            Label {
                id: month
                textSize: root.fontSize === 'large' ? Label.Medium : Label.Large
                text: DU.format(root.date, 'MMMM')
                horizontalAlignment: Text.AlignHCenter
                color: yearDropDown.containsPress? root.activeColor : root.color
            }
            Item {
                width: units.gu(0.5)
                height: 1
            }
            Label {
                id: year
                textSize: root.fontSize === 'large' ? Label.Medium : Label.Large
                text: DU.format(root.date, 'yyyy')
                horizontalAlignment: Text.AlignHCenter
                color: yearDropDown.containsPress? root.activeColor : root.color
            }
            Item {
                width: units.gu(0.5)
                height: 1
            }
            Icon {
                name: yearsListOpened? 'up' : 'down'
                height: year.height / 1.5
                y: parent.height / 2 - height / 2 + height * 0.1
                color: yearDropDown.containsPress? root.activeColor : root.color
            }
        }
    }

    MouseArea {
        visible: !DU.sameDay(root.date, minimum)
        anchors.left: parent.left
        anchors.leftMargin: units.gu(1.7)
        width: parent.height
        height: width
        onReleased: if (containsMouse) requestPreviousMonth()
        Icon {
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width
            height: width
            color: parent.containsPress? root.activeColor : root.color
            name: 'previous'
        }
    }

    MouseArea {
        visible: !DU.sameDay(root.date, maximum)
        anchors.right: parent.right
        anchors.rightMargin: units.gu(1.7)
        width: parent.height
        height: width
        onReleased: if (containsMouse) requestNextMonth()
        Icon {
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width
            height: width
            color: parent.containsPress? root.activeColor : root.color
            name: 'next'
        }
    }
}
