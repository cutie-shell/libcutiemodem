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
    src/backend/backend.h
    src/backend/ofono/ofono_p.h
    src/backend/ofono/ofono.cpp
    src/backend/ofono/ofonocall_p.h
    src/backend/ofono/ofonocall.cpp
    src/backend/ofono/ofonomodem_p.h
    src/backend/ofono/ofonomodem.cpp
    src/backend/mm/mm_p.h
    src/backend/mm/mm.cpp
    src/backend/mm/mmcall_p.h
    src/backend/mm/mmcall.cpp
    src/backend/mm/mmmodem_p.h
    src/backend/mm/mmmodem.cpp)

set(plugin_SOURCES
    src/plugin.cpp src/plugin.h)

qt6_add_library(cutiemodem
    SHARED ${lib_SOURCES}
)

qt6_add_qml_module(qmlcutiemodemplugin
    URI Cutie.Modem
    VERSION ${PROJECT_VERSION}
    PLUGIN_TARGET qmlcutiemodemplugin
    NO_GENERATE_PLUGIN_SOURCE
    SOURCES
        ${plugin_SOURCES}
)

set_target_properties(cutiemodem PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(cutiemodem PROPERTIES SOVERSION 0)
set_target_properties(cutiemodem PROPERTIES PUBLIC_HEADER src/cutiemodem.h)

target_link_libraries(cutiemodem PUBLIC
    Qt::Core
    Qt::Quick
    Qt::DBus
)

target_link_libraries(qmlcutiemodemplugin PUBLIC
    Qt::Core
    Qt::Quick
    cutiemodem
)

configure_file(src/cutiemodem.pc.in cutiemodem.pc @ONLY)

install(TARGETS cutiemodem
    RUNTIME DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    BUNDLE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(FILES ${CMAKE_BINARY_DIR}/cutiemodem.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

install(TARGETS qmlcutiemodemplugin
    RUNTIME DESTINATION "${CMAKE_INSTALL_LIBDIR}/qt6/qml/Cutie/Modem"
    BUNDLE DESTINATION "${CMAKE_INSTALL_LIBDIR}/qt6/qml/Cutie/Modem"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}/qt6/qml/Cutie/Modem"
)

install(DIRECTORY src/qml/Cutie/Modem DESTINATION "${CMAKE_INSTALL_LIBDIR}/qt6/qml/Cutie")
