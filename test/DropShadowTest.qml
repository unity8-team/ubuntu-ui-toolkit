import QtQuick 2.4
import QuickPlus 0.1

Item {
    id: root
    width: 600
    height: 600
    focus: true

    property variant qualityTable: [ "Low", "High" ]

    property string textOverlayString:
        "size     (s/S): " + dropShadow.size.toFixed(1) + "\n" +
        "angle    (a/A): " + dropShadow.angle.toFixed(0) + "°\n" +
        "distance (d/D): " + dropShadow.distance.toFixed(1) + "\n" +
        "color      (c): " + dropShadow.color + "\n" +
        "quality    (q): " + qualityTable[dropShadow.quality] + "\n" +
        "opacity  (o/O): " + dropShadow.opacity.toFixed(2) + "\n"

    Item {
        id: scene
        anchors.fill: parent

        Rectangle {
            id: rectangle
            anchors.fill: parent
            anchors.margins: 200.0
            color: "blue"
        }
        DropShadow {
            id: dropShadow
            anchors.fill: parent
            anchors.margins: 200.0
        }
    }

    Text {
        id: textOverlay
        width: 200
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 10
        font.family: "Ubuntu Mono"
        font.pixelSize: 14
        font.weight: Font.Bold
        color: "black"
        text: textOverlayString
    }

    Keys.onPressed: {
        var shift = event.modifiers & Qt.ShiftModifier;
        if (event.key == Qt.Key_S) {
            dropShadow.size = dropShadow.size + (shift ? 1.0 : -1.0);
        } else if (event.key == Qt.Key_A) {
            dropShadow.angle = dropShadow.angle + (shift ? 2.0 : -2.0);
        } else if (event.key == Qt.Key_D) {
            dropShadow.distance = dropShadow.distance + (shift ? 1.0 : -1.0);
        } else if (event.key == Qt.Key_C) {
            dropShadow.color = Qt.rgba(Math.random(), Math.random(), Math.random(), 1.0);
        } else if (event.key == Qt.Key_Q) {
            dropShadow.quality = dropShadow.quality = (dropShadow.quality + 1) % 2;
        } else if (event.key == Qt.Key_O) {
            dropShadow.opacity = dropShadow.opacity + (shift ? 0.02 : -0.02);
        }
    }
}
