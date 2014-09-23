project(ubuntu-sdk C CXX)
cmake_minimum_required(VERSION 2.8.12)

#find required Qt packages
find_package(Qt5Core REQUIRED COMPONENTS PRIVATE_HEADERS)
find_package(Qt5Qml REQUIRED COMPONENTS PRIVATE_HEADERS)
find_package(Qt5Quick REQUIRED COMPONENTS PRIVATE_HEADERS)
find_package(Qt5Gui REQUIRED COMPONENTS PRIVATE_HEADERS)
find_package(Qt5DBus REQUIRED)
find_package(Qt5Svg REQUIRED)
find_package(Qt5Organizer REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(Qt5QuickTest REQUIRED)

#automatically create moc files
set(CMAKE_AUTOMOC ON)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#no idea how to require the V8 headers in cmake
if (Qt5Core_VERSION VERSION_LESS 5.2.0)
    message(STATUS "Requiring V8 private headers")
endif()

# components PATH
execute_process(
    COMMAND qmake -query QT_INSTALL_QML
    OUTPUT_VARIABLE QT_INSTALL_QML
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

#install directory of qt binaries
execute_process(
    COMMAND qmake -query QT_INSTALL_BINS
    OUTPUT_VARIABLE QT_INSTALL_BINS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

#install directory of qt binaries
execute_process(
    COMMAND qmake -query QT_INSTALL_LIBS
    OUTPUT_VARIABLE QT_INSTALL_LIBS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

enable_testing()
add_subdirectory(documentation)
add_subdirectory(modules)
add_subdirectory(tests)
add_subdirectory(examples)
add_subdirectory(po)
