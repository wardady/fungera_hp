cmake_minimum_required(VERSION 3.13)
project(fungera LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost 1.75 REQUIRED serialization program_options)
include_directories(${Boost_INCLUDE_DIRS})

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        main.cpp
        Fungera.cpp
        Fungera.h
        Fungera.h
        Queue.cpp
        Queue.h
        Organism.cpp
        Organism.h
        Memory.cpp
        Memory.h
        Cell.h
        Config.h
        common.h
        CommandHeatMap.cpp
        CommandHeatMap.h
        Config.cpp)
if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(fungera
            ${PROJECT_SOURCES}
            )
else ()
    if (ANDROID)
        add_library(fungera SHARED
                ${PROJECT_SOURCES}
                )
    else ()
        add_executable(fungera
                ${PROJECT_SOURCES}
                )
    endif ()
endif ()

target_link_libraries(fungera PRIVATE ${Boost_LIBRARIES} Qt${QT_VERSION_MAJOR}::Widgets)
