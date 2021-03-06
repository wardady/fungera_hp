cmake_minimum_required(VERSION 3.13)
project(fungera)

set(CMAKE_CXX_STANDARD 20)

if(WIN32 AND NOT MINGW )
find_path(CURSES_INCLUDE_DIR curses.h)
find_library(CURSES_LIBRARIES pdcurses REQUIRED)
else()
find_package(Curses REQUIRED)
endif()

include_directories(fungera ${CURSES_INCLUDE_DIR})
add_executable(fungera main.cpp Fungera.cpp Fungera.h Fungera.h Queue.cpp Queue.h Organism.cpp Organism.h Memory.cpp Memory.h Cell.h Config.h)
target_link_libraries(fungera ${CURSES_LIBRARIES})
