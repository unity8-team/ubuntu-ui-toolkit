requires(linux:!android)
requires(qtHaveModule(quick))

defineTest(minQtVersion) {
    major = $${1}
    minor = $${2}
    patch = $${3}
    isEqual(QT_MAJOR_VERSION, $${major}) {
        isEqual(QT_MINOR_VERSION, $${minor}) {
            isEqual(QT_PATCH_VERSION, $${patch}) {
                return(true)
            }
            greaterThan(QT_PATCH_VERSION, $${patch}) {
                return(true)
            }
        }
        greaterThan(QT_MINOR_VERSION, $${minor}) {
            return(true)
        }
    }
    greaterThan(QT_MAJOR_VERSION, $${major}) {
        return(true)
    }
    return(false)
}

!minQtVersion(5, 1, 0) {
    message("Cannot build Quick+ without Qt version >= $${QT_VERSION}.")
    error("Qt >= 5.1.0 required!")
}

!equals(DISABLE_LTTNG, "1") {
    !packagesExist(lttng-ust) {
        message("Cannot build Quick+ without lttng-ust.")
        error("liblttng-ust-dev required!")
    }
}

TEMPLATE = subdirs
SUBDIRS += \
    src\
    tools

tools.depends = src

include(version.pri)

BUILD_DIR = "BUILD_DIR=\"$${OUT_PWD}\""
write_file($${PWD}/.build_dir.inc, BUILD_DIR)
