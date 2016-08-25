/*
 * Copyright 2016 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Nick Dedekind <nick.dedekind@canonical.com>
 */

import QtQuick 2.4
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import Ubuntu.Components 1.3
import Ubuntu.Components.Private 1.3 as Private

/*!
    \qmltype ContextMenu
    \inqmlmodule Ubuntu.Components.Popups 1.0
    \ingroup ubuntu-popups
    \brief A menu popup allows an application to present a menu.
        A menu has a automatically determined width & height, bound by styled mininum & maximum heights and widths.
        It can be closed by clicking anywhere outside of the menu area.

    Example:
    \qml
        import QtQuick 2.4
        import Ubuntu.Components 1.3
        import Ubuntu.Components.Popups 1.3

        Rectangle {
            width: units.gu(80)
            height: units.gu(80)

            Menu {
                id: contextMenu

                MenuItem { text: "Item 1" }
                MenuItem { text: "Item 2" }
                MenuItem { text: "Item 3" }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    var point = mapToItem(null, mouse.x, mouse.y);
                    contextMenu.show(point)
                }
            }
        }
    \endqml
*/

StyledItem {
    id: root
    styleName: "MenuStyle"

    /*!
       \qmlproperty list<Item> items
    */
    property alias items: itemModel.children

    /*!
       \qmlproperty ItemInstanceModel model
    */
    property alias model: itemModel

    /*!
       \qmlproperty int currentIndex
    */
    property alias currentIndex: listView.currentIndex

    readonly property real __defaultMaximumWidth: Screen.width * 0.7
    readonly property real __defaultMaximumHeight: Screen.height * 0.7

    /*!
       \qmlsignal This handler is called when the menu shows
    */
    signal show()

    readonly property real __ajustedMinimumHeight: {
        if (listView.contentHeight > __styleInstance.minimumHeight) {
            return __styleInstance.minimumHeight;
        }
        return Math.max(listView.contentHeight, units.gu(2));
    }

    implicitWidth: container.width
    implicitHeight: MathUtils.clamp(listView.contentHeight, __ajustedMinimumHeight, __styleInstance.maximumHeight)

    Keys.onUpPressed: navigator.selectPrevious(listView.currentIndex)

    Keys.onDownPressed: navigator.selectNext(listView.currentIndex)

//    Keys.onEscapePressed: {
//        if (__menu.Menus.parentMenu === null && __menu.Menus.menuBar === null) {
//            __menu.dismiss();
//        } else {
//            event.accepted = false;
//        }
//    }

    Private.ItemInstanceModel {
        id: itemModel
    }

    MenuNavigator {
        id: navigator

        onSelect: {
            listView.currentIndex = index
        }
    }

    ColumnLayout {
        id: container
        objectName: "container"
        parent: root.__styleInstance.contentItem

        width: listView.contentWidth
        height: parent.height
        spacing: 0

        // FIXME use ListView.header - tried but was flaky with positionViewAtIndex.
        Loader {
            id: headerLoader
            Layout.fillWidth: true;
            Layout.maximumHeight: item ? item.height : 0
            Layout.minimumHeight: item ? item.height : 0
            visible: listView.contentHeight > root.height
            enabled: !listView.atYBeginning

            sourceComponent: root.__styleInstance.headerDelegate

            MouseArea {
                anchors.fill: parent
                onPressed: {
                    var index = listView.indexAt(0, listView.contentY);
                    listView.positionViewAtIndex(index-1, ListView.Beginning);
                }
            }
        } // Loader

        ListView {
            id: listView
            objectName: "listView"
            Layout.fillHeight: true;
            Layout.fillWidth: true;
            contentWidth: MathUtils.clamp(contentItem.childrenRect.width,
                                          root.__styleInstance.minimumWidth,
                                          root.__styleInstance.maximumWidth)

            orientation: Qt.Vertical
            interactive: contentHeight > height
            clip: interactive
            currentIndex: -1
            highlightFollowsCurrentItem: false

            model: itemModel

            onCurrentItemChanged: {
                if (currentItem) {
                    currentItem.forceActiveFocus();
                } else {
                    menuMouseArea.hoveredItem = null;
                }
            }

            highlight: Loader {
                sourceComponent: root.__styleInstance.highlightDelegate

                width: listView.width
                height:  listView.currentItem ? listView.currentItem.height : 0
                y:  listView.currentItem ? listView.currentItem.y : 0
                visible: listView.currentItem
                z: 1
            }

            MouseArea {
                id: menuMouseArea
                anchors.fill: parent
                hoverEnabled: true
                z: -1

                onPositionChanged: updateCurrentItemFromPosition(mouse)

                property Item hoveredItem: null

                function updateCurrentItemFromPosition(point) {
                    var pos = mapToItem(listView.contentItem, point.x, point.y);

                    if (!hoveredItem || !listView.currentItem ||
                            !hoveredItem.contains(Qt.point(pos.x - listView.currentItem.x, pos.y - listView.currentItem.y))) {
                        hoveredItem = listView.itemAt(pos.x, pos.y);
                        if (!hoveredItem || !hoveredItem.enabled)
                            return false;
                        listView.currentIndex = listView.indexAt(pos.x, pos.y);
                    }
                    return true;
                }
            }
        } // ListView

        // FIXME use ListView.footer - tried but was flaky with positionViewAtIndex.
        Loader {
            id: footerLoader
            Layout.fillWidth: true;
            Layout.maximumHeight: item ? item.height : 0
            Layout.minimumHeight: item ? item.height : 0
            visible: listView.contentHeight > root.height
            enabled: !listView.atYEnd

            sourceComponent: root.__styleInstance.footerDelegate

            MouseArea {
                anchors.fill: parent
                onPressed: {
                    var index = listView.indexAt(0, listView.contentY);
                    listView.positionViewAtIndex(index+1, ListView.Beginning);
                }
            }
        } // Loader
    } // Column
}

//Private.MenuBase {
//    id: root

//    /*! internal *///
//    property alias __menu: root

//    property string styleName: "MenuItemStyle"

//    property Component style: null

//    property Component popupForegroundStyle: null

//    property Component popupBackgroundStyle: null

//    property var popupItem: null

//    delegate: Component {
//        ActionItem {
//            id: visualItem
//            objectName: "menuItem"

//            property QtObject styleData: QtObject {
//                readonly property bool selected: visualItem.focus
//                readonly property bool opened: __menu.popupVisible
//                readonly property string text: visualItem.text
//                readonly property var iconSource: __menu.iconSource || undefined
//                readonly property bool enabled: __menu.enabled
//                readonly property bool checkable: false
//                readonly property bool checked: false
//                readonly property var shortcut: undefined
//                readonly property bool hasSubmenu: true // This is a Menu
//            }

//            styleName: __menu.styleName
//            style: __menu.style

//            action: __menu.action
//            mnemonic {
//                modifier: 0
//                visible: menuContext.active
//            }
//            ActionContext {
//                id: menuContext
//                objectName: "menuContext"
//                active: {
//                    // if it has a parent menu.
//                    if (__menu.Menus.parentMenu) {
//                        // only active when the parent menu is visible, and this one is not open.
//                        return !__menu.popupVisible && __menu.Menus.parentMenu.popupVisible;
//                    }
//                    return !__menu.popupVisible;
//                }
//            }

//            Connections {
//                target: visualItem.action
//                onTriggered: visualItem.show(0)
//            }

//            MouseArea {
//                anchors.fill: parent
//                onClicked: show()
//            }

//            onFocusChanged: {
//                if (!focus) hide()
//            }

//            Keys.onLeftPressed: {
//                if (styleData.opened) {
//                    hide();
//                    forceActiveFocus();
//                } else {
//                    event.accepted = false;
//                }
//            }

//            Keys.onReturnPressed: show(0)
//            Keys.onRightPressed: show(0)

//            function show(index) {
//                forceActiveFocus();
//                if (!styleData.opened) {
//                    var point = mapToItem(null, 0, 0);
//                    __menu.show(point);
//                    if (index !== undefined) {
//                        if (popupItem) popupItem.select(index)
//                    }
//                }
//            }
//            function hide() {
//                if (styleData.opened) {
//                    __menu.hide();
//                }
//            }
//        }
//    }

//    /*!
//        \internal
//        The popup component
//        Defaults to a QWindow which poistions itsself on the screen
//    */
//    popupComponent: Component {
//        Private.MenuWindow {
//            menu: __menu

//            popupContent: Loader {
//                id: loader

//                width: item ? item.implicitWidth : 0
//                height: item ? item.implicitHeight : 0
//                sourceComponent: __menuContent
//                focus: true
//            }

//        }
//    }

//    /*!
//        \internal
//        The item which is used for visual menu placement.
//        Should be read only, but qtcreator complains.
//    */
//    property Component __menuContent: MenuNavigator {
//        id: menuPopup
//        objectName: "menu"

//        focus: true
//        Keys.forwardTo: [ __menu.visualDelegate ]
//        model: listView.model

//        implicitWidth: foreground.implicitWidth
//        implicitHeight: foreground.implicitHeight

//        StyledItem {
//            id: background
//            anchors.fill: foreground
//            styleName: "MenuBackgroundStyle"
//            style: popupBackgroundStyle
//        }

//        StyledItem {
//            id: foreground
//            styleName: "MenuForegroundStyle"
//            style: popupForegroundStyle

//            //styling properties
//            readonly property real __defaultMaximumWidth: Screen.width * 0.7
//            readonly property real __defaultMaximumHeight: Screen.height * 0.7

//            readonly property real ajustedMinimumHeight: {
//                if (listView.contentHeight > __styleInstance.minimumHeight) {
//                    return __styleInstance.minimumHeight;
//                }
//                return Math.max(listView.contentHeight, units.gu(2));
//            }

//            implicitWidth: container.width
//            implicitHeight: MathUtils.clamp(listView.contentHeight, ajustedMinimumHeight, __styleInstance.maximumHeight)

//            Binding {
//                target: __menu
//                property: "contentItem"
//                value: listView.contentItem
//            }

//            Binding {
//                target: __menu
//                property: "popupItem"
//                value: menuPopup
//            }

//            ColumnLayout {
//                id: container
//                objectName: "container"
//                parent: background.__styleInstance.contentItem

//                width: listView.contentWidth
//                height: parent.height
//                spacing: 0

//                // FIXME use ListView.header - tried but was flaky.
//                Loader {
//                    id: headerLoader
//                    Layout.fillWidth: true;
//                    Layout.maximumHeight: item.height
//                    Layout.minimumHeight: item.height
//                    visible: listView.contentHeight > menuPopup.height
//                    enabled: !listView.atYBeginning

//                    sourceComponent: foreground.__styleInstance.headerDelegate

//                    MouseArea {
//                        anchors.fill: parent
//                        onPressed: {
//                            var index = listView.indexAt(0, listView.contentY);
//                            listView.positionViewAtIndex(index-1, ListView.Beginning);
//                        }
//                    }
//                } // Loader

//                ListView {
//                    id: listView
//                    objectName: "listView"
//                    Layout.fillHeight: true;
//                    Layout.fillWidth: true;
//                    contentWidth: MathUtils.clamp(contentItem.childrenRect.width,
//                                                  foreground.__styleInstance.minimumWidth,
//                                                  foreground.__styleInstance.maximumWidth)

//                    model: __menu.visualItemModel
//                    orientation: Qt.Vertical
//                    interactive: contentHeight > height
//                    clip: interactive
//                    currentIndex: -1
//                    highlightFollowsCurrentItem: false

//                    onCurrentItemChanged: {
//                        if (currentItem) {
//                            currentItem.forceActiveFocus();
//                        } else {
//                            menuMouseArea.hoveredItem = null;
//                        }
//                    }

//                    highlight: Loader {
//                        sourceComponent: foreground.__styleInstance.highlight

//                        width: listView.width
//                        height:  listView.currentItem ? listView.currentItem.height : 0
//                        y:  listView.currentItem ? listView.currentItem.y : 0
//                        visible: listView.currentItem
//                        z: 1
//                    }

//                    MouseArea {
//                        id: menuMouseArea
//                        anchors.fill: parent
//                        hoverEnabled: true
//                        z: -1

//                        onPositionChanged: updateCurrentItemFromPosition(mouse)

//                        property Item hoveredItem: null

//                        function updateCurrentItemFromPosition(point) {
//                            var pos = mapToItem(listView.contentItem, point.x, point.y);

//                            if (!hoveredItem || !listView.currentItem ||
//                                    !hoveredItem.contains(Qt.point(pos.x - listView.currentItem.x, pos.y - listView.currentItem.y))) {
//                                hoveredItem = listView.itemAt(pos.x, pos.y);
//                                if (!hoveredItem || !hoveredItem.enabled)
//                                    return false;
//                                listView.currentIndex = listView.indexAt(pos.x, pos.y);
//                            }
//                            return true;
//                        }
//                    }
//                } // ListView

//                // FIXME use ListView.header - tried but was flaky with positionViewAtIndex.
//                Loader {
//                    id: footerLoader
//                    Layout.fillWidth: true;
//                    Layout.maximumHeight: item.height
//                    Layout.minimumHeight: item.height
//                    visible: listView.contentHeight > menuPopup.height
//                    enabled: !listView.atYEnd

//                    sourceComponent: foreground.__styleInstance.footerDelegate

//                    MouseArea {
//                        anchors.fill: parent
//                        onPressed: {
//                            var index = listView.indexAt(0, listView.contentY);
//                            listView.positionViewAtIndex(index+1, ListView.Beginning);
//                        }
//                    }
//                } // Loader
//            } // Column

//            Connections {
//                target: __menu
//                onPopupVisibleChanged: {
//                    if (!popupVisible) {
//                        if (listView.currentItem) listView.currentItem.focus = false;
//                        listView.currentIndex = -1;
//                    }
//                }
//            }
//        }

//        Keys.onUpPressed: selectPrevious(listView.currentIndex)

//        Keys.onDownPressed: selectNext(listView.currentIndex)

//        Keys.onEscapePressed: {
//            if (__menu.Menus.parentMenu === null && __menu.Menus.menuBar === null) {
//                __menu.dismiss();
//            } else {
//                event.accepted = false;
//            }
//        }

//        onSelect: {
//            listView.currentIndex = index
//        }
//    } // MenuNavigator
//}
