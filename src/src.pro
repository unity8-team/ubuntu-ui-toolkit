TEMPLATE = subdirs

SUBDIRS += quickplus

!equals(DISABLE_LTTNG, "1") {
    SUBDIRS += quickpluslttng
    quickpluslttng.depends = quickplus
}

SUBDIRS += imports
imports.depends = quickplus
