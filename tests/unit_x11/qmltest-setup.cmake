list(APPEND TEST_COMPILE_DEFINITIONS
     "QT_NO_KEYWORDS"
)

list(APPEND TEST_QT_MODULES Test Qml Quick QuickTest)

add_custom_target(TEST_${TARGET}_uncompiled_files ALL SOURCES ${TESTS})

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

if(TEST_LD_LIBRARY_PATH)
    foreach(PATH ${TEST_LD_LIBRARY_PATH})
        if(LD_PATH_STRING)
            set(LD_PATH_STRING "${LD_PATH_STRING}:${PATH}")
        else()
            set(LD_PATH_STRING ${PATH})
        endif()
    endforeach()
endif()

foreach(TEST ${TESTS})
    get_filename_component(TEST_NAME ${TEST} NAME_WE )
    get_filename_component(SUFFIX ${TEST} EXT)

    string(REPLACE ".qml" "" SUFFIX "${SUFFIX}")
    string(REPLACE "tst_" "" TEST_NAME "${TEST_NAME}")

    if(SUFFIX)
        set(TEST_NAME "${TEST_NAME}${SUFFIX}")
    endif()

    get_filename_component(TEST_FILE ${TEST} NAME )
    add_test(
        NAME ${TARGET}_${TEST_NAME}
        COMMAND ${CMAKE_SOURCE_DIR}/tests/unit/runtest.sh ${CMAKE_CURRENT_BINARY_DIR}/${TARGET} ${TEST} "" \"${CMAKE_BINARY_DIR}/modules\"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    set_property(TEST ${TARGET}_${TEST_NAME} PROPERTY DEPENDS test_install )
    set_property(TEST ${TARGET}_${TEST_NAME} PROPERTY LABELS  unit_x11_${TARGET}_${TEST_NAME})
    #check.commands += sh tests/qmlapicheck.sh || exit 1;
endforeach()

