/*
 * Copyright 2012-2015 Canonical Ltd.
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
 */

import QtQuick 2.4
import "../1.2/stack.js" as Stack
import Ubuntu.Components 1.3
import Ubuntu.Components.Private 1.3

/*!
    \qmltype PageStack
    \inqmlmodule Ubuntu.Components 1.1
    \ingroup ubuntu
    \brief A stack of \l Page items that is used for inter-Page navigation.
        Pages on the stack can be popped, and new Pages can be pushed.
        The page on top of the stack is the visible one.

    PageStack should be used inside a \l MainView in order to automatically add
    a header and toolbar to control the stack. The PageStack will automatically
    set the header title to the title of the \l Page that is currently on top
    of the stack, and the tools of the toolbar to the tools of the \l Page on top
    of the stack. When more than one Pages are on the stack, the toolbar will
    automatically feature a back-button that pop the stack when triggered.

    The anchors of the PageStack are set to fill its parent by default. To use
    left/right/top/bottom anchors, explicitly set anchors.fill of the PageStack to
    undefined:
    \qml
        import QtQuick 2.4
        import Ubuntu.Components 1.3

        MainView {
            width: units.gu(40)
            height: units.gu(71)

            PageStack {
                id: mainStack
                anchors {
                    fill: undefined // unset the default to make the other anchors work
                    left: parent.left
                    right: parent.right
                    top: parent.top
                    bottom: rect.top
                }
            }

            Rectangle {
                id: rect
                color: UbuntuColors.red
                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                }
                height: units.gu(10)
            }

            Component.onCompleted: mainStack.push(Qt.resolvedUrl("MyPage.qml"))
        }
    \endqml

    Pages that are defined inside the PageStack must initially set their visibility
    to false to avoid the pages occluding the PageStack before they are pushed.
    When pushing a \l Page, its visibility is automatically updated.

    Example:
    \qml
        import QtQuick 2.4
        import Ubuntu.Components 1.3
        import Ubuntu.Components.ListItems 1.3 as ListItem

        MainView {
            width: units.gu(48)
            height: units.gu(60)

            PageStack {
                id: pageStack
                Component.onCompleted: push(page0)

                Page {
                    id: page0
                    title: i18n.tr("Root page")
                    visible: false

                    Column {
                        anchors.fill: parent
                        ListItem.Standard {
                            text: i18n.tr("Page one")
                            onClicked: pageStack.push(page1, {color: UbuntuColors.orange})
                            progression: true
                        }
                        ListItem.Standard {
                            text: i18n.tr("External page")
                            onClicked: pageStack.push(Qt.resolvedUrl("MyCustomPage.qml"))
                            progression: true
                        }
                    }
                }

                Page {
                    title: "Rectangle"
                    id: page1
                    visible: false
                    property alias color: rectangle.color
                    Rectangle {
                        id: rectangle
                        anchors {
                            fill: parent
                            margins: units.gu(5)
                        }
                    }
                }
            }
        }
    \endqml
    As shown in the example above, the push() function can take an Item, Component or URL as input.
*/

PageTreeNode {
    id: pageStack
    anchors.fill: parent

    AdaptivePageLayout {
        id: apl
        anchors.fill: parent
        function getTop() {
            if (apl.__d.tree.top()) {
                return apl.__d.tree.top();
            } else {
                return null;
            }
        }
        function getDepth() {
            var topWrapper = getTop();
            var i = apl.__d.tree.index(topWrapper);
            print("topWrapper = "+topWrapper+", index = "+i);
            return (i+1);
        }

        asynchronous: false

        onPagesChanged: {
            var topWrapper = apl.__d.tree.top();
            print("Pages changed! top = "+topWrapper)
            if (topWrapper) {
                pageStack.currentPage = topWrapper.object;
                pageStack.depth = apl.__d.tree.index(topWrapper) + 1;
            } else {
                pageStack.currentPage = null;
                pageStack.depth = 0;
            }
        }
    }

    /*!
      The current size of the stack
     */
    //FIXME: would prefer this be readonly, but readonly properties are only bound at
    //initialisation. Trying to update it in push or pop fails. Not sure how to fix.
    property int depth: 0
    onDepthChanged: print("new depth = "+depth)

    /*!
      The currently active page
     */
    property Item currentPage: null

    /*!
      Push a page to the stack, and apply the given (optional) properties to the page.
      The pushed page may be an Item, Component or URL.
      The function returns the Item that was pushed, or the Item that was created from
      the Component or URL. Depending on the animation of the header, the returned
      Page may or may not be active and on top of the PageStack yet.
     */
    function push(page, properties) {
        if (!pageStack.currentPage) {
            // this is the first page we are pushing
            if (page.createObject || typeof page == "string") {
                // page is a component or url
                apl.primaryPageSource = page;
            } else {
                // page is a Page.
                // Note that the old PageStack supported Items.
                apl.primaryPage = page;
            }
            print("pushed first page. currentPage = "+pageStack.currentPage)
        }    else {
            apl.addPageToCurrentColumn(pageStack.currentPage, page, properties);
            print("pushed "+page+", new currentPage = "+pageStack.currentPage)
        }
        // TODO TIM: check for header!
//        pageStack.currentPage.header.navigationActions = [ backAction ];
        return pageStack.currentPage;
    }

    /*!
      Pop the top item from the stack if the stack size is at least 1.
      Do not do anything if 0 items are on the stack.
     */
    function pop() {
        print("pop, depth = "+depth+" popping "+pageStack.currentPage)
        if (pageStack.depth > 0) {
            apl.removePages(pageStack.currentPage);
            print("new depth = "+pageStack.depth+", new top = "+pageStack.currentPage)
        } else if (pageStack.depth < 1) {
            print("WARNING: Trying to pop an empty PageStack. Ignoring.");
        }
    }

    /*!
      Deactivate the active page and clear the stack.
     */
    function clear() {
        apl.primaryPage = null;
        apl.primaryPageSource = null;
//        pageStack.depth = 0;
//        currentPage = null;
    }

//    property alias bAction: backAction
//    Action {
//        // used when the Page has a Page.header property set.
//        id: backAction
//        visible: pageStack.depth > 1
//        iconName: "back"
//        text: "Back"
//        onTriggered: pageStack.pop()
//        objectName: "pagestack_back_action"
//    }

    /*!
      \qmlproperty list<Object> data
      Children of PageStack are placed in a separate item such that they are
      not active by default until they are pushed on the PageStack.
     */
    default property alias data: apl.data
//    default property alias data: inactiveNode.data
//    PageTreeNode {
//        anchors.fill: parent
//        id: inactiveNode
//        active: false
//    }
}
