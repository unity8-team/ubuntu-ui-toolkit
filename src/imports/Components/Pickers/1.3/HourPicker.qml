import Ubuntu.Components 1.3
import QtQuick 2.4
import 'dateutils.js' as DU

PathView {
    id: pathview
    property date date: datePicker.date // FIXME:
    property real itemAlign: Text.AlignHCenter
    property string type
    property bool ampm: true
    clip: true
    pathItemCount: 6
    model: {
        if (type === 'hours') {
            return ampm? 12 : 24
        }
        if (type === 'minutes' || type === 'seconds') {
            return 60
        }
        return 0
    }
    snapMode: PathView.SnapToItem
    preferredHighlightBegin: 0.5
    preferredHighlightEnd: 0.5

    property bool indexInit: false
    Timer {
        id: afterCompleted
        interval: 0
        onTriggered: indexInit = true
    }
    onCurrentIndexChanged: {
        if (!indexInit) return

        var method = null
        var value = null
        var am = date.getHours() > 0 && date.getHours() < 13

        if (type === 'hours') {
            method = 'setHours'
            if (ampm) {
                value = am? currentIndex + 1 : ((currentIndex + 13) % 24)
            } else {
                value = (currentIndex + 1) % 24
            }
        } else if (type === 'minutes') {
            method = 'setMinutes'
            value = currentIndex

        } else if (type === 'seconds') {
            method = 'setSeconds'
            value = currentIndex
        }

        if (method === null || value === null) {
            return
        }

        datePicker.date = DU.clone(date, method, value) // FIXME:
    }
    delegate: Item {
        property bool active: PathView.isCurrentItem
        width: pathview.width
        height: datePicker.itemHeight // FIXME:
        Label {
            width: parent.width
            height: parent.height
            // FIXME: What color?
            color: parent.active? UbuntuColors.darkGrey : UbuntuColors.lightGrey
            text: {
                if (pathview.type === 'hours') {
                    return DU.format(DU.clone(date, 'setHours', index + 1), 'HH')
                }
                if (pathview.type === 'minutes') {
                    return DU.format(DU.clone(date, 'setMinutes', index), 'mm')
                }
                if (pathview.type === 'seconds') {
                    return DU.format(DU.clone(date, 'setSeconds', index), 'ss')
                }
                return ''
            }
            // fontSize: 'x-large'
            textSize: Label.Medium
            horizontalAlignment: pathview.itemAlign
            verticalAlignment: Text.AlignVCenter
        }
    }
    path: Path {
        startX: pathview.width / 2
        startY: - datePicker.itemHeight / 2 // FIXME:
        PathLine {
            x: pathview.width / 2
            y: pathview.height + datePicker.itemHeight / 2 // FIXME:
        }
    }
    Component.onCompleted: {
        afterCompleted.start()
        switch (type) {
            case 'hours':
                if (ampm) {
                    currentIndex = date.getHours() - 1
                } else {
                    currentIndex = date.getHours() % 24 - 1
                }
                break
            case 'minutes':
                currentIndex = date.getMinutes()
                break
            case 'seconds':
                currentIndex = date.getSeconds()
                break
        }
    }
}
