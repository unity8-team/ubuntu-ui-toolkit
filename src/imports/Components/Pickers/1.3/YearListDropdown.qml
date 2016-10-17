import QtQuick 2.4
import Ubuntu.Components 1.3
import 'dateutils.js' as DU

Rectangle {
    id: root

    property date minimum: DU.clone(null, 'setDate', 1)
    property date maximum: DU.clone(minimum, 'setFullYear', minimum.getFullYear() + 50)
    property real itemHeight: height / 5
    property real itemWidth: width / 2

    property alias monthsPathView: monthsPathView
    property alias yearsListView: yearsListView

    signal requestDateChange(date newDate)

    width: parent.width
    height: parent.height
    y: -height
    color: 'white'

    PathView {
        id: monthsPathView
        property var date: new Date(1970, 0, 1)
        width: root.itemWidth
        height: parent.height
        pathItemCount: 6
        model: 12
        snapMode: PathView.SnapToItem
        clip: true

        preferredHighlightBegin: 0.5
        preferredHighlightEnd: 0.5

        onCurrentIndexChanged: {
            var newDate = DU.clone(datePicker.date, 'setMonth', currentIndex)
            var newDisplayedDate = DU.clone(newDate)
            root.requestDateChange(newDate)
        }

        delegate: Item {
            property bool active: PathView.isCurrentItem
            width: root.itemWidth
            height: root.itemHeight
            Rectangle {
                anchors.fill: parent
                color: 'transparent'
                Label {
                    width: parent.width - units.gu(1)
                    height: parent.height
                    color: parent.parent.active? UbuntuColors.orange : UbuntuColors.darkGrey
                    text: DU.format(DU.clone(monthsPathView.date, 'setMonth', index), 'MMMM')
                    fontSize: 'x-large'
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        path: Path {
            startX: root.itemWidth / 2
            startY: - root.itemHeight / 2
            PathLine {
                x: root.itemWidth / 2
                y: root.height + root.itemHeight / 2
            }
        }

        MouseArea {
            anchors.fill: parent
        }
    }

    ListView {
        id: yearsListView

        clip: true
        width: root.itemWidth
        height: parent.height
        x: width

        model: root.maximum.getFullYear() - root.minimum.getFullYear() + 1
        cacheBuffer: 10
        snapMode: ListView.SnapToItem
        orientation: ListView.Vertical

        highlightFollowsCurrentItem: true
        highlightMoveDuration: UbuntuAnimation.FastDuration
        highlightRangeMode: ListView.StrictlyEnforceRange

        preferredHighlightBegin: height / 2 - root.itemHeight / 2
        preferredHighlightEnd: height / 2 + root.itemHeight / 2

        property bool isCurrentIndexInit: false
        onCurrentIndexChanged: {
            if (!isCurrentIndexInit) {
                isCurrentIndexInit = true
                return
            }
            var newDate = DU.clone(
                minimum, 'setFullYear',
                minimum.getFullYear() + currentIndex
            )
            root.requestDateChange(newDate)
        }

        delegate: MouseArea {
            width: parent.width
            height: root.itemHeight
            property bool active: ListView.isCurrentItem
            onReleased: yearsListView.currentIndex = index
            Label {
                id: item
                color: parent.active? UbuntuColors.orange : UbuntuColors.darkGrey
                width: parent.width - units.gu(1)
                height: parent.height
                x: units.gu(1)
                text: root.minimum.getFullYear() + index
                fontSize: 'x-large'
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }
        }

        Component.onCompleted: {
            // yearsListView.currentIndex = yearsListView.currentIndex
            yearsListView.positionViewAtIndex(yearsListView.currentIndex, ListView.SnapPosition)
        }
    }
}
