import QtQuick 2.0
import Ubuntu.Components 0.1

Item {
    id: root
    width: 400
    height: 400

    Rectangle {
        anchors.fill: parent
        color: "#cccccc"
    }

    Shape2 {
        anchors.fill: parent
        anchors.margins: 10
        baseColor: "blue"
        shadowRadiusOut: Qt.vector4d(30.0, 30.0, 30.0, 85.0)
        shadowColorOut: Qt.rgba(0.0, 0.0, 0.0, 0.9)
        cornerRadius: 100
        source: Image { source: "demo_image.jpg" }
    }
}
