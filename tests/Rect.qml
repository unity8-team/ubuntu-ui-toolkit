import QtQuick 2.0

Rectangle {
    id: scene
    width: 800
    height: 800
    color: "black"

    Rectangle {
        id: quad
        anchors.centerIn: parent
        width: Math.min(parent.width, parent.height) * 0.8
        height: Math.min(parent.width, parent.height) * 0.8
        color: "green"
        transform: Rotation {
            axis: Qt.vector3d(0.0, 0.0, 1.0)
            origin: Qt.vector3d(quad.width * 0.5, quad.height * 0.5, 0.0)
            angle: 5.0
        }
    }
}
