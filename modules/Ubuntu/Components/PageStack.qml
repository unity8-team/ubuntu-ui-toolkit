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

    The PageStack supports a two-pane layout that can be used in wide screen devices to optimize
    the viewport usage. This results that there can be two pages visible at the same time, however
    only one of them is active. In order to use this feature the \l automaticWideAspect property
    must be set, and \l pushOrReplace() instead of \l push() should be used when pushing pages.

    When switched from one-pane to two-pane modes, the current page will be reparented to the
    right pane and the previous page will be placed into the left pane. When pages are pushed
    from the current page (right pane) the current page will be moved into the left pane and the
    new one will be placed into the right one. This results in a push operation. When a page is
    pushed from a left pane page, the current page (from the right pane) will be replaced with the
    page that is just pushed.

    \image two-pane-layout.png

    Popping pages from the stack results in moving pages from left pane into the right pane till
    there will be only one page left in the stack. This page will stay in the left pane and will
    be activated there.

    A page stack supporting two-pane layout would look as follows:
    \qml
        import QtQuick 2.0
        import Ubuntu.Components 0.1
        import Ubuntu.Components.ListItems 0.1 as ListItem

        MainView {
            width: units.gu(48)
            height: units.gu(60)

            PageStack {
                id: pageStack
                automaticWideAspect: true
                // the first push can be a simple push
                Component.onCompleted: push(page0)

                Page {
                    id: page0
                    title: i18n.tr("Root page")
                    visible: false

                    Column {
                        anchors.fill: parent
                        ListItem.Standard {
                            text: i18n.tr("Page one")
                            onClicked: pageStack.pushOrReplace(page0, page1, {color: "red"})
                            progression: true
                        }
                        ListItem.Standard {
                            text: i18n.tr("External page")
                            onClicked: pageStack.pushOrReplace(page0, Qt.resolvedUrl("MyCustomPage.qml"))
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

    When the PageStack is used in single-pane mode, the same page can be pushed several times in the stack.
    In two-pane mode this may cause the page to be hidden as PageStack cannot decide whether to show it or
    not. If such a functionality is required, it is recommended to create the pages each time the page is
    pushed onto the stack.
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
      The property drives the use of automatic wide aspect mode of the viewport. When set
      to true, the PageStack will automatically switch to two-pane mode when the width and
      height of the PageStack exceets 40x71 GU. When false, the PageStack will act as a
      single-pane stack.

      The default value is false.
      */
    property bool automaticWideAspect: false

    /*!
      The property defines the condition to turn the wide aspect mode on. By default the condition
      is set to turn wide aspect mode on when the PageStack size exceeds 40x71 GUs.
      */
    property bool wideAspectCondition: ((pageStack.width > units.gu(40)) && (pageStack.height > units.gu(71)))

    /*!
      \preliminary
      Push a page to the stack, and apply the given (optional) properties to the page.
      The pushed page may be an Item, Component or URL.
     */
    function push(page, properties) {
        // make sure the topmost page is parented to "body"
        if (internal.stack.size() > 0) {
            internal.stack.top().parent = body;
        }
        internal.pushPage(page, properties);
    }

    /*!
      The function pushes a page or replaces the topmost page in a pagestack depending
      whether the page stack is in two-pane or single pane mode. In two pane mode
      if the sourcePage is the currentPage, the operation will result as a push, moving
      the current page to the left pane and placing the new one into the right pane. If
      the sourcePage is the page from the left pane, the operation will result in replacing
      the currentPage from the stack with the new page given.
      */
    function pushOrReplace(sourcePage, page, properties) {
        if (internal.wideAspect) {
            if (internal.stack.size() < 2) {
                // we have only one page, so we push and add the next page to the righ pane
                internal.pushPage(page, properties);
            } else if (sourcePage === pageStack.currentPage){
                // normal push
                push(page, properties);
            } else {
                // we replace the topmost page
                pop();
                internal.pushPage(page, properties);
            }
        } else {
            push(page, properties);
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
        else internal.stack.top().object.visible = false;
        internal.stack.pop();
        internal.stackUpdated();

        if (internal.stack.size() > 0) {
            internal.stack.top().active = true;
            if (internal.wideAspect && (pageStack.depth >= 2)) {
                // move the topmost element to contentPane
                internal.stack.top().parent = internal.contentPane;
            }
        }
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
        property Item contentPane: body

        function createWrapper(page, properties) {
            var wrapperComponent = Qt.createComponent("PageWrapper.qml");
            var wrapperObject = wrapperComponent.createObject(body);
            wrapperObject.parent = contentPane;
            wrapperObject.depth = pageStack.depth + 1;
            wrapperObject.visible = Qt.binding(function () {
                if (!wideAspect) {
                    // not in wide aspect, only active pages are visible
                    return this.active;
                } else if (pageStack.depth <= internal.stack.size()) {
                    // the last two pages in the stack are visible
                    if (pageStack.depth >=2) {
                        return (this.depth >= (pageStack.depth - 1));
                    } else if (pageStack.depth > 0) {
                        return true;
                    }
                }

                return false;
            });
            wrapperObject.reference = page;
            wrapperObject.pageStack = pageStack;
            wrapperObject.properties = properties;
            return wrapperObject;
        }

        function pushPage(page, properties) {
            if (internal.stack.size() > 0) internal.stack.top().active = false;
            internal.stack.push(internal.createWrapper(page, properties));
            internal.stack.top().active = true;
            internal.stackUpdated();
        }

        function stackUpdated() {
            pageStack.depth = stack.size();
            if (pageStack.depth > 0) currentPage = stack.top().object;
            else currentPage = null;
        }
    }

    /*!
      \internal
      Pages redirected into default layout.
      */
    default property alias pages: body.data
    Layouts {
        id: layout
        anchors.fill: parent
        layouts: [
            ConditionalLayout {
                name: "wideAspect"
                when: pageStack.wideAspectCondition && pageStack.automaticWideAspect
                Item {
                    anchors.fill: parent
                    ItemLayout {
                        id: leftPane
                        item: "stack"
                        anchors {
                            left: parent.left
                            top: parent.top
                            bottom: parent.bottom
                        }
                        width: units.gu(40)
                    }
                    // pane-separator
                    Rectangle {
                        id: separator
                        width: units.dp(1)
                        anchors {
                            left: leftPane.right
                            top: parent.top
                            bottom: parent.bottom
                            margins: units.gu(0.5)
                        }

                        color: "darkgray"
                    }
                    // right pane
                    Item {
                        id: rightPane
                        anchors {
                            left: separator.right
                            leftMargin: units.gu(0.5)
                            top: parent.top
                            bottom: parent.bottom
                            right: parent.right
                        }
                    }
                    Component.onCompleted: {
                        internal.contentPane = rightPane;
                        if (internal.stack.size() >= 2 && pageStack.currentPage != null) {
                            // mowhichve the last page (current page) into the content page
                            internal.stack.top().parent = rightPane;
                        }
                    }
                    Component.onDestruction: {
                        internal.contentPane = body;
                        // move the current page back to the body
                        if (pageStack.currentPage != null) {
                            internal.stack.top().parent = body;
                        }
                    }
                }
            }

        ]

        // default layout
        Item {
            id: body
            anchors.fill: parent
            Layouts.item: "stack"
        }
    }
}
