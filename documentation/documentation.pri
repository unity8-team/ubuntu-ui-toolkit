install_docs.files = $$PWD/online
install_docs.path = /usr/share/ubuntu-ui-toolkit/doc
install_docs.depends = docs

install_qch.files = $$PWD/offline/ubuntuuserinterfacetoolkit.qch
install_qch.path = /usr/share/qt5/doc/qch
install_qch.depends = docs

INSTALLS += install_docs install_qch


