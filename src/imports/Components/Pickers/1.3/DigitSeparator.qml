import QtQuick 2.4
import Ubuntu.Components 1.3

Label {
    anchors.verticalCenter: parent.verticalCenter
    color: theme.palette.normal.backgroundText
    text: ':'
    textSize: Label.Medium
    Component.onCompleted: x -= width / 2
}
