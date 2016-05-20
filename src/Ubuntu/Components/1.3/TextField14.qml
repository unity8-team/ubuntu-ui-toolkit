import QtQuick 2.4
import Ubuntu.Components 1.3 as Ubuntu

Ubuntu.ActionItem {
    id: main
    implicitWidth: units.gu(25)
    implicitHeight: units.gu(4)

    property alias primaryItem: primary.data
    Item {
        id: primary
        anchors {
            left: parent.left
            leftMargin: flickable.spacing
        }
        height: parent.height
        width: childrenRect.width
        z: 1
        onChildrenChanged: adjustChildren(primary)
        function adjustChildren(newParent) {
            for (var i = 0; i < children.length; i++) {
                var child = children[i];
                child.parent = newParent;
                child.anchors.verticalCenter = verticalCenter;
            }
        }
    }
    property alias secondaryItem: secondary.data
    Item {
        id: secondary
        anchors {
            right: parent.right
            rightMargin: flickable.spacing
        }
        height: parent.height
        width: childrenRect.width
        z: 1
        onChildrenChanged: primary.adjustChildren(secondary)
    }
    property alias echoMode: input.echoMode
    property alias text: input.text
    property string placeholderText //FIXME: placeHolder is a placeHolder for now

    styleName: "TextFieldStyle"
    Keys.forwardTo: [input]
    // FIXME: property alias activeFocusOnPress: input.activeFocusOnPress
    activeFocusOnPress: true

    Flickable {
        id: flickable
        anchors {
            left: primary.right
            right: secondary.left
            margins: spacing
            verticalCenter: parent.verticalCenter
        }
        property real spacing: main.__styleInstance.frameSpacing
        topMargin: spacing
        leftMargin: spacing
        rightMargin: spacing
        bottomMargin: spacing
        boundsBehavior: Flickable.StopAtBounds
        clip: true
        contentWidth: input.contentWidth
        contentHeight: input.contentHeight
        height: input.contentHeight

        TextInput {
            id: input
            width: parent.width
            height: parent.height

            color: main.__styleInstance.color
            selectedTextColor: main.__styleInstance.selectedTextColor
            selectionColor: main.__styleInstance.selectionColor
            font.pixelSize: FontUtils.sizeToPixels("medium")
            passwordCharacter: "\u2022"

            // FIXME: WIP
            Ubuntu.Mouse.forwardTo: [main]
            // activeFocusOnPress: false
            onFocusChanged: if (focus) main.forceActiveFocus()

            Keys.onPressed: cursorVisible = true
            selectByMouse: true
            mouseSelectionMode: TextInput.SelectWords
            cursorDelegate: Component {
                Rectangle {
                    color: theme.palette.normal.activity
                    width: units.dp(2)
                    height: input.cursorRectangle.height
                    visible: main.activeFocus
                }
            }
            /* cursorDelegate: TextCursor {
                handler: handler
            }

            InputHandler {
                id: handler
                anchors.fill: parent
                main: main
                input: input
                flickable: flickable
            } */
        }
    }
}
