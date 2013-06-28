/*
 * Copyright 2012 Canonical Ltd.
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

import QtQuick 2.0
import "stack.js" as Stack
import Ubuntu.Layouts 0.1

/*!
    \qmltype PageStack
    \inqmlmodule Ubuntu.Components 0.1
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

    Pages that are defined inside the PageStack must initially set their visibility
    to false to avoid the pages occluding the PageStack before they are pushed.
    When pushing a \l Page, its visibility is automatically updated.

    Example:
    \qml
        import QtQuick 2.0
        import Ubuntu.Components 0.1
        import Ubuntu.Components.ListItems 0.1 as ListItem

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
                            onClicked: pageStack.push(page1, {color: "red"})
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

    /*!
      \internal
      Please do not use this property any more. \l MainView now has a header
      property that controls when the header is shown/hidden.
     */
    property bool __showHeader: true
    QtObject {
        property alias showHeader: pageStack.__showHeader
        onShowHeaderChanged: print("__showHeader is deprecated. Do not use it.")
    }

    /*!
      \deprecated
      This property is deprecated. Pages will now automatically update the toolbar when activated.
     */
    property ToolbarActions tools: null
    /*!
      \deprecated
      \internal
     */
    onToolsChanged: print("MainView.tools property was deprecated. "+
                          "Pages will automatically update the toolbar when activated. "+
                          "See CHANGES file, and use toolbar.tools instead when needed.");

    /*!
      \preliminary
      The current size of the stack
     */
    //FIXME: would prefer this be readonly, but readonly properties are only bound at
    //initialisation. Trying to update it in push or pop fails. Not sure how to fix.
    property int depth: 0

    /*!
      \preliminary
      The currently active page
     */
    property Item currentPage: null

    /*!
      \preliminary
      Push a page to the stack, and apply the given (optional) properties to the page.
      The pushed page may be an Item, Component or URL.
     */
    function push(page, properties) {
        internal.pushPage(page, properties, body);
    }

    /*!
      The function pushes a page or replaces the topmost page in a pagestack depending
      whether the page stack is in two-column or single column mode. In two column mode
      if the sourcePage is the currentPage, the operation will result as a push, moving
      the current page to the left pane and placing the new one into the right pane. If
      the sourcePage is the page from the left pane, the operation will result in replacing
      the currentPage from the stack with the new page given.
      */
    function pushOrReplace(sourcePage, page, properties) {
        if (internal.wideAspect) {
            internal.pushPage(page, properties, body);
        } else {
            internal.pushPage(page, properties, body);
        }
    }

    /*!
      \preliminary
      Pop the top item from the stack if the stack size is at least 1.
      Do not do anything if 0 or 1 items are on the stack.
     */
    function pop() {
        if (internal.stack.size() < 1) {
            print("WARNING: Trying to pop an empty PageStack. Ignoring.");
            return;
        }
        internal.stack.top().active = false;
        if (internal.stack.top().canDestroy) internal.stack.top().destroyObject();
        internal.stack.pop();
        internal.stackUpdated();

        if (internal.stack.size() > 0) internal.stack.top().active = true;
    }

    /*!
      \preliminary
      Deactivate the active page and clear the stack.
     */
    function clear() {
        while (internal.stack.size() > 0) {
            internal.stack.top().active = false;
            if (internal.stack.top().canDestroy) internal.stack.top().destroyObject();
            internal.stack.pop();
        }
        internal.stackUpdated();
    }

    QtObject {
        id: internal

        /*!
          The instance of the stack from javascript
         */
        property var stack: new Stack.Stack()

        /*!
          Wide aspect
         */
        property bool wideAspect: (layout.currentLayout === "wideAspect")
        property Item contentPane: null

        function createWrapper(page, properties, parent) {
            var wrapperComponent = Qt.createComponent("PageWrapper.qml");
            var wrapperObject = wrapperComponent.createObject(parent);
            wrapperObject.reference = page;
            wrapperObject.pageStack = pageStack;
            wrapperObject.properties = properties;
            return wrapperObject;
        }

        function pushPage(page, properties, parent) {
            if (internal.stack.size() > 0) internal.stack.top().active = false;
            internal.stack.push(internal.createWrapper(page, properties, parent));
            internal.stack.top().active = true;
            internal.stackUpdated();
        }

        function stackUpdated() {
            pageStack.depth =+ stack.size();
            if (pageStack.depth > 0) currentPage = stack.top().object;
            else currentPage = null;
        }
    }

    /*!
      \internal
      Pages redirected into default layout
      */
    default property alias pages: body.data
    Layouts {
        id: layout
        anchors.fill: parent
        layouts: [
            ConditionalLayout {
                name: "wideAspect"
                when: (pageStack.width > units.gu(40) && pageStack.height > units.gu(71)) ||
                      (pageStack.height < pageStack.width)
                Item {
                    anchors.fill: parent
                    ItemLayout {
                        id: leftPane
                        item: "body"
                        anchors {
                            left: parent.left
                            top: parent.top
                            bottom: parent.bottom
                        }
                        width: units.gu(40)
                    }
                    Item {
                        id: rightPane
                        anchors {
                            left: leftPane.right
                            top: parent.top
                            bottom: parent.bottom
                            right: parent.right
                        }
                    }
                    Component.onCompleted: {
                        internal.contentPane = rightPane;
                        if (internal.stack.size() >= 2 && pageStack.currentPage != null) {
                            // move the last page (current page) into the content page
                            pageStack.currentPage.parent = rightPane;
                        }
                    }
                    Component.onDestruction: {
                        internal.contentPane = null;
                        // move the current page back to the body
                        if (pageStack.currentPage != null) {
                            pageStack.currentPage.parent = body;
                        }
                    }
                }
            }

        ]

        // default layout
        Item {
            id: body
            anchors.fill: parent
            Layouts.item: "body"
        }
    }
}
