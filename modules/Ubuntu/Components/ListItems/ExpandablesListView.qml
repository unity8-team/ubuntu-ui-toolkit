import QtQuick 2.0
import Ubuntu.Components 0.1

ListView {
    id: root

    readonly property alias expandedItem: priv.expandedItem

    function expandItem(item) {
        if (!item.hasOwnProperty("expandedHeight") || !item.hasOwnProperty("collapsedHeight")) {
            return;
        }

        if (priv.expandedItem != null) {
            priv.expandedItem.expanded = false;
        }
        priv.expandedItem = item;

        var itemY = contentItem.mapFromItem(item).y;
        var itemHeight = item.expandedHeight;
        var itemIndex = indexAt(item.x, item.y);

        var bottomSpacing = itemIndex == model.count - 1 ? 0 : item.collapsedHeight;
        if (itemY + itemHeight > height + contentY - bottomSpacing) {
            contentY = Math.min(itemY, itemY + itemHeight - height + bottomSpacing);
        }
    }

    function collapse() {
        priv.expandedItem.expanded = false;
        priv.expandedItem = null;
    }

    Behavior on contentY {
        UbuntuNumberAnimation {}
    }

    QtObject {
        id: priv
        property Item expandedItem: null
    }

    MouseArea {
        parent: contentItem
        anchors.fill: parent
        z: 2
        enabled: priv.expandedItem != null
        onClicked: root.collapse();
    }
}
