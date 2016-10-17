import Ubuntu.Components 1.3
import QtQuick 2.4
import 'dateutils.js' as DU

PathView {
    id: pathview
    property real itemWidth: width
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
        var am = root.date.getHours() > 0 && root.date.getHours() < 13

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

        root.requestDateChange(DU.clone(date, method, value))
    }
    delegate: Item {
        property bool active: PathView.isCurrentItem
        width: pathview.itemWidth
        height: root.itemHeight
        Label {
            width: parent.width
            height: parent.height
            color: parent.active? UbuntuColors.darkGray : UbuntuColors.gray
            text: {
                if (pathview.type === 'hours') {
                    return DU.format(DU.clone(root.date, 'setHours', index + 1), 'HH')
                }
                if (pathview.type === 'minutes') {
                    return DU.format(DU.clone(root.date, 'setMinutes', index), 'mm')
                }
                if (pathview.type === 'seconds') {
                    return DU.format(DU.clone(root.date, 'setSeconds', index), 'ss')
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
        startX: pathview.itemWidth / 2
        startY: - root.itemHeight / 2
        PathLine {
            x: pathview.itemWidth / 2
            y: pathview.height + root.itemHeight / 2
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
                console.log('BBB', date)
                currentIndex = date.getMinutes()
                break
            case 'seconds':
                currentIndex = date.getSeconds()
                break
        }
        // if (type === 'hours') {
        // } else if (type === '
        // indexInit = true
    }
}
