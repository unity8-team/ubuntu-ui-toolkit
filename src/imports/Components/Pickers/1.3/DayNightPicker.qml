import Ubuntu.Components 1.3
import QtQuick 2.4
import 'dateutils.js' as DU

ListView {
    id: ampmListView
    clip: true
    model: ['AM', 'PM']
    cacheBuffer: 0
    snapMode: ListView.SnapToItem
    orientation: ListView.Vertical
    property real itemHeight

    highlightFollowsCurrentItem: true
    highlightMoveDuration: UbuntuAnimation.FastDuration
    highlightRangeMode: ListView.StrictlyEnforceRange

    preferredHighlightBegin: height / 2 - itemHeight / 2
    preferredHighlightEnd: height / 2 + itemHeight / 2

    onCurrentIndexChanged: {
        var hour = date.getHours()

        if (hour < 12 && currentIndex === 1) {
            hour += 12
        } else if (hour > 12 && currentIndex === 0) {
            hour -= 12
        }

        datePicker.date = DU.clone(date, 'setHours', hour);
    }

    delegate: MouseArea {
        property bool active: ListView.isCurrentItem
        width: parent.width
        height: itemHeight
        onReleased: ampmListView.currentIndex = index
        Label {
            id: item
            color: parent.active? UbuntuColors.darkGrey : UbuntuColors.lightGrey // FIXME:
            width: parent.width - units.gu(1)
            height: parent.height
            x: units.gu(1)
            text: modelData
            textSize: Label.Medium
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }
    }
}
