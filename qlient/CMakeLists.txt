# Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2024/October/21
cmake_minimum_required(VERSION 3.16)
project(qclient LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

find_package(Qt6
    REQUIRED COMPONENTS
      Core
      Gui
      Widgets
      WebSockets
)
find_package(fmt REQUIRED)

qt_add_executable(qclient
    common.h
    client.cpp client.h
    club.cpp club.h
    newtbl.cpp newtbl.h
    ui.cpp ui.h
    main.cpp
)

target_link_libraries(qclient PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::WebSockets
    fmt::fmt
)

if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    message(STATUS "Building for WebAssembly")
endif()
