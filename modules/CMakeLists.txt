add_subdirectory(Ubuntu/Components)
add_subdirectory(Ubuntu/Layouts)
add_subdirectory(Ubuntu/PerformanceMetrics)
add_subdirectory(Ubuntu/Test)

#this ugly piece of code makes sure that all required files are copied over to the builddirectory
#so we can safely run our tests there without polluting the source directory
if(NOT "${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    file(GLOB_RECURSE UNCOMPILED_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" *.qml *.png *.js *.sci)
    foreach( file_to_copy ${UNCOMPILED_FILES} )
      add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${file_to_copy}"
        COMMAND cmake -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/${file_to_copy}"
        "${CMAKE_CURRENT_BINARY_DIR}/${file_to_copy}"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${file_to_copy}"
      )
      list( APPEND copy_files_dest "${file_to_copy}" )
    endforeach()
    add_custom_target( CopyUncompiledSources ALL DEPENDS ${copy_files_dest} )
endif()
