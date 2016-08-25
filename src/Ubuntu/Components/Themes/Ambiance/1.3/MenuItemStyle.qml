import QtQuick 2.4
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.3

Item {
    id: stlyeItem
    implicitHeight: units.gu(5)
    implicitWidth: requiredWidth

    property url chevron: Qt.resolvedUrl("../artwork/chevron.png")

    property real requiredWidth: {
        var val = 0;
        val += units.gu(1) + flagGutter.width;
        if (styledItem.iconSource != "") {
            val += units.gu(1) + icon.width
        }
        val += units.gu(1) + title.contentWidth;
        if (styledItem.shortcut != undefined) {
            val += units.gu(3) + shortcutLabel.contentWidth;
        }
        if (styledItem.hasSubmenu) {
            val += units.gu(1) + chevronIcon.width;
        }
        return val + units.gu(1);
    }

    property bool _checked : styledItem.action && styledItem.action.parameterType === Action.Bool && styledItem.action.state == true

//    Rectangle {
//        anchors.fill: parent
//        color: theme.palette.highlighted.background
//        visible: !!__styleData["opened"]
//    }

    RowLayout {
        id: row
        spacing: units.gu(1)
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: units.gu(1)
        anchors.rightMargin: units.gu(1)
        anchors.verticalCenter: parent.verticalCenter

        Icon {
            id: flagGutter
            width: units.gu(1.5)
            height: units.gu(1.5)

            name: _checked ? "tick" : ""
        }

        Icon {
            id: icon
            width: units.gu(2)
            height: units.gu(2)

            visible: styledItem.iconSource != "" || false
            source: styledItem.iconSource || ""
        }

        RowLayout {
            spacing: units.gu(3)

            Label {
                id: title
                elide: Text.ElideNone
                wrapMode: Text.NoWrap
                clip: true
                Layout.fillWidth: true

                text: styledItem.text ? styledItem.text : ""
            }

            Label {
                id: shortcutLabel
                elide: Text.ElideNone
                wrapMode: Text.NoWrap
                clip: true

                visible: styledItem.shortcut != undefined
                text: styledItem.shortcut ? styledItem.shortcut : ""
            }
        }

        Icon {
            id: chevronIcon
            width: units.gu(2)
            height: units.gu(2)

            visible: styledItem.hasSubmenu
            source: chevron
        }
    }
}
