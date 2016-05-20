import QtQuick 2.4
import Ubuntu.Components 1.3 as Ubuntu

Ubuntu.StyledItem {
    id: main
    implicitWidth: units.gu(30)
    implicitHeight: units.gu(4) * 5

    property alias text: input.text
    property string placeholderText //FIXME: placeHolder is a placeHolder for now

    styleName: "TextAreaStyle"
    Keys.forwardTo: [input]
    // FIXME: property alias activeFocusOnPress: input.activeFocusOnPress
    activeFocusOnPress: true

    ScrollView {
        anchors.fill: parent
        Flickable {
            id: flickable
            anchors {
                fill: parent
                //margins: spacing
                //verticalCenter: parent.verticalCenter
            }
            property real spacing: main.__styleInstance.frameSpacing
            topMargin: spacing
            leftMargin: spacing
            rightMargin: spacing
            bottomMargin: spacing
            boundsBehavior: Flickable.StopAtBounds
            clip: true
            contentWidth: input.paintedWidth
            contentHeight: input.paintedHeight
            // height: input.contentHeight

            TextEdit {
                id: input
                width: main.width - 2 * flickable.spacing
                height: Math.max(main.height - 2 * flickable.spacing, contentHeight)
                wrapMode: TextEdit.WrapAtWordBoundaryOrAnywhere

                color: main.__styleInstance.color
                selectedTextColor: main.__styleInstance.selectedTextColor
                selectionColor: main.__styleInstance.selectionColor
                font.pixelSize: FontUtils.sizeToPixels("medium")

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
}
