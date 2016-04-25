import QtQuick 2.0

Rectangle {
    id: scene
    width: 500
    height: 500
    color: "black"

    Image {
        id: quad
        anchors.centerIn: parent
        width: Math.min(parent.width, parent.height) * 0.75
        height: Math.min(parent.width, parent.height) * 0.75
        source: "ubuntu.svg"
        sourceSize: Qt.size(width, height)
        transform: Rotation {
            id: rotation
            axis: Qt.vector3d(0.0, 1.0, 0.0)
            origin: Qt.vector3d(quad.width * 0.5, quad.height * 0.5, 0.0)
        }

        PropertyAnimation {
            id: animation
            target: rotation
            property: "angle"
            from: 0.0
            to: 360.0
            duration: 5000
            loops: Animation.Infinite
            running: true
        }
    }
}
