import QtQuick 2.0
import Ubuntu.Components 0.1

ListView {

    property Item expandedItem: null

    function expandItem(item) {
        if (!item.hasOwnProperty("expandedHeight")) {
            return;
        }
        expandedItem = item;

        var itemY = contentItem.mapFromItem(item).y;
        var itemHeight = item.expandedHeight;
        if (itemY + itemHeight > height + contentY) {
            contentY = Math.min(itemY, itemY + itemHeight - height);
        }
    }

    function collapse() {
        expandedItem = null;
    }

    Behavior on contentY {
        UbuntuNumberAnimation {}
    }
}
