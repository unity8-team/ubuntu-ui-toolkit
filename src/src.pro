TEMPLATE = subdirs

SUBDIRS += plus

!equals(DISABLE_LTTNG, "1") {
    SUBDIRS += lttng
    lttng.depends = plus
}

SUBDIRS += imports
imports.depends = plus
