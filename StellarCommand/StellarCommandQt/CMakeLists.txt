cmake_minimum_required(VERSION 3.16)
project(StellarCommandQt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets Svg QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Widgets Svg REQUIRED)
endif()

add_executable(StellarCommandQt
    main.cpp
    mainwindow.cpp
    mainwindow.h
    engine.h
    icons.qrc
)

if(Qt6_FOUND)
    target_link_libraries(StellarCommandQt PRIVATE Qt6::Widgets Qt6::Svg)
else()
    target_link_libraries(StellarCommandQt PRIVATE Qt5::Widgets Qt5::Svg)
endif()
