import QtQuick 2.0
import QtQuick.Window 2.0

Window {
    id: win1
    width: 600
    height: 400
    visible: true
    color: "yellow"
    title: "First Window"

    Text {
        id: statusText
        anchors.centerIn: parent
        text: "Click to " + (win2.visible ? "hide" : "show") + " second window."
    }

    MouseArea {
        anchors.fill: parent
        onClicked: { win2.visible = !win2.visible }
    }

    Window {
        id: win2
        width: win1.width
        height: win1.height
        x: win1.x + win1.width
        y: win1.y
        color: "green"
        title: "Second Window"
    }
}
