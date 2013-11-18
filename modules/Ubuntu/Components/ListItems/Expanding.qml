import QtQuick 2.0
import Ubuntu.Components 0.1

Base {
    id: root
    implicitHeight: expanded ? expandedHeight : collapsedHeight
    clip: true
    opacity: priv.otherExpanded ? .6 : 1

    property bool expanded: false
    property int collapsedHeight: __height
    property int expandedHeight: collapsedHeight

    onClicked: {
        expanded = !expanded
    }

    onExpandedChanged: {
        if (priv.isInExpandableList) {
            if (expanded) {
                ListView.view.expandItem(root);
            } else {
                ListView.view.collapse();
            }
        }
    }

    Behavior on height {
        UbuntuNumberAnimation {}
    }
    Behavior on opacity {
        UbuntuNumberAnimation {}
    }

    property Item __view: ListView.view
    QtObject {
        id: priv
        property bool isInExpandableList: __view !== undefined && __view.hasOwnProperty("expandItem") && __view.hasOwnProperty("collapse")
        property bool otherExpanded: __view !== undefined && __view.expandedItem !== null && __view.expandedItem !== root
    }
}
