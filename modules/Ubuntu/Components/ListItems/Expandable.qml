import QtQuick 2.0
import Ubuntu.Components 0.1

Empty {
    id: root
    implicitHeight: expanded ? priv.maxExpandedHeight : collapsedHeight
    clip: true

    property bool expanded: false
    property real collapsedHeight: __height
    property real expandedHeight: collapsedHeight

    onExpandedChanged: {
        if (!expanded) {
            contentFlickable.contentY = 0;
        }

        if (priv.isInExpandableList) {
            if (expanded) {
                __view.expandItem(root);
            } else {
                __view.collapse();
            }
        }
    }

    Behavior on height {
        UbuntuNumberAnimation {}
    }
    Behavior on opacity {
        UbuntuNumberAnimation {}
    }

    states: [
        State {
            name: "otherExpanded"; when: priv.otherExpanded
            PropertyChanges { target: root; opacity: .6 }
        },
        State {
            name: "expanded"; when: expanded
            PropertyChanges { target: root; z: 3 }
        }
    ]

    property Item __view: ListView.view
    QtObject {
        id: priv
        property bool isInExpandableList: __view && __view !== undefined && __view.hasOwnProperty("expandItem") && __view.hasOwnProperty("collapse")
        property bool otherExpanded: isInExpandableList && __view.expandedItem !== null && __view.expandedItem !== undefined && __view.expandedItem !== root
        property real maxExpandedHeight: isInExpandableList ? Math.min(__view.height - collapsedHeight, expandedHeight) : expandedHeight
    }

    property alias children: flickableContent.data
    Flickable {
        id: contentFlickable
        anchors { fill: parent; leftMargin: root.__contentsMargins; rightMargin: __contentsMargins }
        interactive: root.expanded && flickableContent.height > height
        contentHeight: root.expandedHeight
        flickableDirection: Flickable.VerticalFlick

        Behavior on contentY {
            UbuntuNumberAnimation {}
        }

        Item {
            id: flickableContent
            anchors {
                left: parent.left
                right: parent.right
            }
            height: childrenRect.height
        }
    }
}
