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
if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -I.")
else()
    message(STATUS "Compiling for Desktop")
endif()
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -DFMT_HEADER_ONLY=1")

# Add fmt as a header-only library
include(FetchContent)
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 8.1.1
)
FetchContent_MakeAvailable(fmt)

qt_add_executable(qclient
    common.h
    client.cpp client.h
    club.cpp club.h
    table.cpp table.h
    newtbl.cpp newtbl.h
    ui.cpp ui.h
    player.cpp player.h
    game.cpp game.h
    golden.cpp golden.h
    main.cpp
)

target_link_libraries(qclient PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::WebSockets
    fmt::fmt-header-only
)
