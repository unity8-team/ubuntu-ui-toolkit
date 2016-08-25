import QtQuick 2.4
import Ubuntu.Components 1.3

MenuBarBase {
    id: menuBar
    objectName: "menuBar"

    property Item __contentItem: Loader {
        active: !menuBar.__isNative
        sourceComponent: menuBar.__isNative ? null : __menuBarComponent

        focus: true
        Keys.forwardTo: [item]
    }

    // should be read only, but qtcreator complains.
    property Component __menuBarComponent: Component {
        MenuNavigator {
            id: menuBarItem
            objectName: "barContainer"
            styleName: "MenuBarItemStyle"
            menuItems: menuBar.menus

            QtObject {
                id: d
                property bool altPressed: false
            }

            implicitHeight: units.gu(3)
            implicitWidth: row.width

            Keys.onPressed: {
                console.log("BAR pressed", event)
                if (event.key === Qt.Key_Alt) {
                    d.altPressed = true;
                }
            }
            Keys.onReleased: {
                if (event.key === Qt.Key_Alt) {
                    d.altPressed = false;
                }
            }

            Keys.onLeftPressed: {
                console.log("BAR Left")
                selectPrevious();
            }
            Keys.onRightPressed: {
                console.log("BAR RIGHT")
                selectNext();
            }
            Keys.onEscapePressed: {
                console.log("BAR ESC")
            }
            onSelect: {
                var selectedItem = repeater.itemAt(index);
                if (!selectedItem) return;
                updateCurrentItem(selectedItem, true);
            }

            ActionContext {
                id: actionContext
                active: d.altPressed
            }

            function formatMnemonic(text, underline) {
                if (underline) return text;
                return text.replace(/<(\/)?u>/g, "")
            }

            Row {
                id: row
                height: parent.height
                spacing: 0

                Repeater {
                    id: repeater
                    model: menuBar.menus

                    Loader {
                        id: itemLoader
                        objectName: "barMenuItemLoader"+index

                        Keys.forwardTo: [menuBarItem]

                        property QtObject __menuItem: modelData
                        property int __index: index

                        property var styleData: QtObject {
                            id: opts
                            readonly property string text: StringUtils.styleMnemonic(__menuItem.text, d.altPressed)
                            readonly property bool enabled: __menuItem.enabled
                            readonly property url iconSource: __menuItem.iconSource
                            readonly property bool selected: menuBarItem.openItem === itemLoader
                        }

                        enabled: __menuItem.enabled
                        height: row.height
                        width: item.width

                        sourceComponent: menuBarItem.__styleInstance.itemDelegate

                        Binding {
                            target: __menuItem
                            property: "__visualItem"
                            value: itemLoader
                        }

                        function __showSubMenu() {
                            if (!__menuItem.enabled)
                                return;
                            __menuItem.popup(Qt.rect(itemLoader.width, 0, 0, 0), -1)
                        }

                        function __closeSubMenu() {
                            __menuItem.closePopup();
                        }

                        Connections {
                            target: __menuItem.action
                            onTriggered: {
                                __showSubMenu();
                            }
                        }

    //                    Connections {
    //                        target: menuBarItem
    //                        onOpenItemChanged: {
    //                            if (menuBarItem.openItem !== itemLoader) {
    //                                if (__menuItem.__popupVisible) {
    //                                    __menuItem.closePopup();
    //                                }
    //                            } else if (__menuItem.enabled) {
    //                                if (!__menuItem.__popupVisible) {
    //                                    __menuItem.popup(Qt.rect(itemLoader.width, 0, 0, 0), -1)
    //                                }
    //                            }
    //                        }
    //                    }

                        Connections {
                            target: __menuItem
                            onPopupVisibleChanged: {
                                if (__menuItem.__popupVisible) {
                                    menuBarItem.openItem = itemLoader;
                                } else if (menuBarItem.openItem === itemLoader) {
                                    menuBarItem.openItem = null;
                                }
                            }
                        }
                    }
                }
            }

            MouseArea {
                id: menuMouseArea
                anchors.fill: parent
                hoverEnabled: menuBarItem.openItem !== null

                onPositionChanged: updateCurrentItemFromPosition(mouse)
                onPressed: {
                    if (updateCurrentItemFromPosition(mouse)) {
                        menuBarItem.currentItem.__showSubMenu();
                    }
                }
                onExited: menuBarItem.hoveredItem = null

                function updateCurrentItemFromPosition(point) {
                    var pos = mapToItem(row, point.x, point.y);

                    if (!hoveredItem || !hoveredItem.contains(Qt.point(pos.x - currentItem.x, pos.y - currentItem.y))) {
                        hoveredItem = row.childAt(pos.x, pos.y);
                        if (!hoveredItem)
                            return false;
                        menuBarItem.updateCurrentItem(hoveredItem, false);
                    }
                    return true;
                }
            }
        }
    }
}
