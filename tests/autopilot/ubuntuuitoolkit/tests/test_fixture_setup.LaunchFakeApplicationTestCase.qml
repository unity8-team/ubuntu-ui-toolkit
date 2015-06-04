import QtQuick 2.3
import Ubuntu.Components 1.2

MainView {
    width: units.gu(48)
    height: units.gu(60)
    objectName: "mainView"

    Label {
        id: label
        objectName: 'testLabel'
        text: 'Test application.'
    }

    Connections {
        target: UriHandler
        onOpened: {
            for (var i = 0; i < uris.length; ++i) {
                label.text += uris[i];
            }
        }
    }
}
