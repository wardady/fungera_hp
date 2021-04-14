cmake_minimum_required(VERSION 3.13)
project(fungera)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost 1.75 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(fungera main.cpp Fungera.cpp Fungera.h Fungera.h Queue.cpp Queue.h Organism.cpp Organism.h Memory.cpp Memory.h Cell.h Config.h common.h CommandHeatMap.cpp CommandHeatMap.h)
target_link_libraries(fungera ${Boost_LIBRARIES})
