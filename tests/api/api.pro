TEMPLATE = subdirs

SUBDIRS = Extinct

check.commands += QML2_IMPORT_PATH=$$ROOT_BUILD_DIR/qml $$ROOT_BUILD_DIR/apicheck/ubuntu-ui-toolkit-apicheck Extinct.Animals $$ROOT_BUILD_DIR/tests/api/components.api > $$ROOT_BUILD_DIR/tests/api/Extinct.Animals.api.new || exit 1;

api.path = $$[QT_INSTALL_PREFIX]/share/click/api/qml
api.files = components.api
INSTALLS += api
check.commands += ALARM_BACKEND=memory LD_LIBRARY_PATH=$$ROOT_BUILD_DIR/lib QML2_IMPORT_PATH=$$ROOT_BUILD_DIR/qml $$ROOT_BUILD_DIR/apicheck/ubuntu-ui-toolkit-apicheck Ubuntu.Components Ubuntu.Components.ListItems Ubuntu.Components.Popups Ubuntu.Components.Pickers Ubuntu.Components.Styles Ubuntu.Components.Themes Ubuntu.Layouts Ubuntu.PerformanceMetrics Ubuntu.Test $$ROOT_BUILD_DIR/components.api > $$ROOT_BUILD_DIR/components.api.new || exit 1;
