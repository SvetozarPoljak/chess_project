cmake_minimum_required(VERSION 3.16)
project(chess_project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

include_directories(${CMAKE_SOURCE_DIR}/src)

set(SOURCES
    src/main.cpp
    src/dialog.cpp
    src/boardscanner.cpp
)

set(HEADERS
    src/dialog.h
    src/boardscanner.h
    src/chess.hpp
)

add_executable(chess ${SOURCES} ${HEADERS})

# ========================================
# LINUX / RASPBERRY PI CONFIGURATION (Qt5)
# ========================================
if(UNIX AND NOT APPLE)
    message(STATUS "Building for Linux/Raspberry Pi - using Qt5")

    # Finding Qt5
    find_package(Qt5 REQUIRED COMPONENTS Widgets Core Gui Bluetooth)
    
    target_link_libraries(chess
        Qt5::Widgets
        Qt5::Core
        Qt5::Gui
        Qt5::Bluetooth
    )

    # Linking bcm2835 library
    find_library(BCM2835_LIB bcm2835 HINTS /usr/local/lib /usr/lib)
    if(BCM2835_LIB)
        target_link_libraries(chess ${BCM2835_LIB})
        message(STATUS "bcm2835 found: ${BCM2835_LIB}")
    else()
        message(WARNING "bcm2835 not found")
    endif()
endif()
