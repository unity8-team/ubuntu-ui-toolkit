list(APPEND TEST_COMPILE_DEFINITIONS
     "QT_NO_KEYWORDS"
      UBUNTU_COMPONENT_PATH=\"${CMAKE_BINARY_DIR}/modules/Ubuntu/Components\"
      UBUNTU_QML_IMPORT_PATH=\"${CMAKE_BINARY_DIR}/modules\"
      UBUNTU_SOURCE_ROOT=\"${CMAKE_SOURCE_DIR}\"
      UBUNTU_BUILD_ROOT=\"${CMAKE_BINARY_DIR}\"
      SRCDIR=\"${CMAKE_SOURCE_DIR}\"
)

list(APPEND TEST_QT_MODULES Test Qml Quick)

target_compile_options(${TARGET} PUBLIC -std=c++11)

if(TEST_INCLUDE_DIRS)
    set_property(TARGET ${TARGET} PROPERTY
        INCLUDE_DIRECTORIES ${TEST_INCLUDE_DIRS}
    )
endif()

if(TEST_QT_MODULES)
    qt5_use_modules(${TARGET} ${TEST_QT_MODULES})
endif()

if(TEST_LINK_FLAGS)
    set_property(TARGET ${TARGET} PROPERTY LINK_FLAGS
        ${TEST_LINK_FLAGS}
    )
endif()

if(TEST_COMPILE_DEFINITIONS)
    set_property(TARGET ${TARGET} PROPERTY COMPILE_DEFINITIONS
        ${TEST_COMPILE_DEFINITIONS}
    )
endif()

if(TEST_TARGET_LINK_LIBRARIES)
    target_link_libraries(${TARGET} ${TEST_TARGET_LINK_LIBRARIES})
endif()

if(QML_SOURCE)
    add_custom_target(${TARGET}_uncompiled_files ALL SOURCES ${QML_SOURCE})
endif()

if(TEST_LD_LIBRARY_PATH)
    foreach(PATH ${TEST_LD_LIBRARY_PATH})
        if(LD_PATH_STRING)
            set(LD_PATH_STRING "${LD_PATH_STRING}:${PATH}")
        else()
            set(LD_PATH_STRING ${PATH})
        endif()
    endforeach()
endif()

add_test(NAME ${TARGET}
    COMMAND ${CMAKE_SOURCE_DIR}/tests/unit/runtest.sh ${CMAKE_CURRENT_BINARY_DIR}/${TARGET} ${CMAKE_CURRENT_BINARY_DIR}/${TARGET} "" \"${CMAKE_BINARY_DIR}/modules\"
)
set_property(TEST ${TARGET} PROPERTY LABELS unit_x11_${TARGET} )
set_property(TEST ${TARGET} PROPERTY ENVIRONMENT
    QML_IMPORT_PATH=${CMAKE_BINARY_DIR}/modules
    QML2_IMPORT_PATH=${CMAKE_BINARY_DIR}/modules
    UBUNTU_UI_TOOLKIT_THEMES_PATH=${CMAKE_BINARY_DIR}/modules
    LD_LIBRARY_PATH=${LD_PATH_STRING}
)


