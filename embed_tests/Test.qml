import QtQuick 2.0
import Ubuntu.Components 1.1

MainView {
    id: root
    width: 500
    height: 500

    Page {
        title: "Simple page"
        Button {
            anchors.centerIn: parent
            text: "Push me"
            width: units.gu(15)
        }
        Switch {
            anchors.centerIn: parent
        }
        Scrollbar {
            anchors.centerIn: parent
        }
    }
}
