TEMPLATE = aux

QMAKE_DOCS = $$PWD/config/ubuntu-ui-toolkit-online.qdocconf

# FIXME: Refactor into load(qt_docs) or something similar
# that can be used from all non-module projects that also
# provide modularized docs, for example qmake.
QTDIR = $$[QT_HOST_PREFIX]
