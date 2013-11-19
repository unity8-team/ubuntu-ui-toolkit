import QtQuick 2.0
import Ubuntu.Components 0.1

Flickable {
    id: root
    contentHeight: column.height

    readonly property alias expandedItem: priv.expandedItem

    function expandItem(item) {
        if (!item.hasOwnProperty("expandedHeight") || !item.hasOwnProperty("collapsedHeight")) {
            return;
        }

        if (priv.expandedItem != null) {
            priv.expandedItem.expanded = false;
        }
        priv.expandedItem = item;

        var maxExpandedHeight = root.height - item.collapsedHeight;
        var expandedItemHeight = Math.min(item.expandedHeight, maxExpandedHeight);
        var bottomIntersect = root.mapFromItem(item).y + expandedItemHeight - maxExpandedHeight;
        if (bottomIntersect > 0) {
            contentY += bottomIntersect;
        }
    }

    function setViewToItems() {
        for (var i = 0; i < column.data.length; ++i) {
            if (column.data[i].hasOwnProperty("__view")) {
                column.data[i].__view = root;
            }
        }
    }

    function collapse() {
        priv.expandedItem.expanded = false;
        priv.expandedItem = null;
    }

    Behavior on contentY {
        UbuntuNumberAnimation { }
    }

    QtObject {
        id: priv
        property var expandedItem: null
    }

    default property alias children: column.data

    onChildrenChanged: {
        setViewToItems();
    }

    Component.onCompleted: {
        setViewToItems();
    }

    Column {
        id: column
        anchors { left: parent.left; right: parent.right }

    }

    MouseArea {
        anchors {left: parent.left; right: parent.right; top: parent.top }
        height: root.mapFromItem(priv.expandedItem).y + root.contentY
        enabled: priv.expandedItem != null
        onClicked: root.collapse();
    }

    MouseArea {
        anchors {left: parent.left; right: parent.right; bottom: parent.bottom }
        height: priv.expandedItem != null ? root.contentHeight - (root.mapFromItem(priv.expandedItem).y + root.contentY + priv.expandedItem.height) : 0
        enabled: priv.expandedItem != null
        onClicked: root.collapse();
    }
}
