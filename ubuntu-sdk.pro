include( common.pri )
#include( documentation/documentation.pri )

TEMPLATE = subdirs
SUBDIRS += demos themes modules/ubuntu-ui-toolkit.pro tests examples documentation

tests/unit.depends = modules/ubuntu-ui-toolkit.pro

# additional 'make test' target required by continuous integration system
test.target = test
test.commands = make check
test.depends = license
QMAKE_EXTRA_TARGETS += test

license.target = license
license.commands = ./tests/license/checklicense.sh
QMAKE_EXTRA_TARGETS += license


#QMAKE_DOCS = $$PWD/doc/ubuntu-ui-toolkit-qtcreator.qdocconf


#DOC_PATH=${PWD}/documentation
#docs.target = docs
#docs.commands += qdoc $$DOC_PATH/ubuntu-ui-toolkit-qtcreator.qdocconf;
#docs.commands += qhelpgenerator "$$DOC_PATH/html/ubuntuuserinterfacetoolkit.qhp"
#QMAKE_EXTRA_TARGETS += docs

