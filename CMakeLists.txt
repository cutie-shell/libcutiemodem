cmake_minimum_required(VERSION 3.16)
project(cutiemodem VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 REQUIRED COMPONENTS Core Quick DBus)
qt_standard_project_setup()

set(lib_SOURCES
    src/cutiemodem.h
    src/cutiemodem.cpp
    src/cutiecall.cpp
    src/modemsettings.cpp
    src/ofonocall.cpp
    src/ofonomodem.cpp)

qt6_add_library(cutiemodem
    SHARED ${lib_SOURCES}
)

set_target_properties(cutiemodem PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(cutiemodem PROPERTIES SOVERSION 0)
set_target_properties(cutiemodem PROPERTIES PUBLIC_HEADER src/cutiemodem.h)

target_link_libraries(cutiemodem PUBLIC
    Qt::Core
    Qt::Quick
    Qt::DBus
)

configure_file(src/cutiemodem.pc.in cutiemodem.pc @ONLY)

install(TARGETS cutiemodem
    RUNTIME DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    BUNDLE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(FILES ${CMAKE_BINARY_DIR}/cutiemodem.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
