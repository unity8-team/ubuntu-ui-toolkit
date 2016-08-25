import QtQuick 2.4
import Ubuntu.Components 1.3

ActionItem {
    id: root
    styleName: "MenuItemStyle"

    property bool hasSubmenu: false

    width: {
        if (!parent) return implicitWidth;
        if (parent.width > implicitWidth) return parent.width;
        return implicitWidth;
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            var check = action && action.parameterType === Action.Bool ? !action.state : undefined;
            root.trigger(check);
        }
    }
}

//Private.MenuItemBase {
//    id: root

//    /*! internal *///
//    property alias __menu: root

//    property string styleName: "MenuItemStyle"

//    property Component style: null

//    delegate: Component {
//        ActionItem {
//            id: itemDelegate
//            objectName: "MenuItem-"+__menu.text

//            styleName: __menu.styleName
//            style: __menu.style
//            action: __menu

//            mnemonic {
//                modifier: 0
//                visible: menuItemContext.active
//            }
//            ActionContext {
//                id: menuItemContext
//                objectName: "menuItemContext"
//                active: {
//                    // if it has a parent menu.
//                    if (__menu.Menus.parentMenu) {
//                        // only active when the parent menu is visible,.
//                        return __menu.Menus.parentMenu.popupVisible;
//                    }
//                    return true;
//                }
//            }

//            property QtObject styleData: QtObject {
//                readonly property bool selected: itemDelegate.focus
//                readonly property string text: itemDelegate.text
//                readonly property var iconSource: __menu.iconSource || undefined
//                readonly property bool enabled: __menu.enabled
//                readonly property bool checkable: __menu.checkable
//                readonly property bool checked: __menu.checked
//                readonly property var shortcut: __menu.shortcut
//                readonly property bool hasSubmenu: false
//            }

//            MouseArea {
//                anchors.fill: parent
//                onClicked: __menu.trigger()
//            }

//            Keys.onReturnPressed: __menu.trigger()
//        }
//    }
//}

