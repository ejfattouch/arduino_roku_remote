cmake_minimum_required(VERSION 3.29)
project(RokuDiscovery)

set(CMAKE_CXX_STANDARD 17)

include_directories(/opt/homebrew/include)

# Find ncurses
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(RokuDiscovery main.cpp)
target_link_libraries(RokuDiscovery ${CURSES_LIBRARIES} pthread)