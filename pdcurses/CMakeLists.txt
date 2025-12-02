cmake_minimum_required(VERSION 3.10)

project(PDCurses C)

set(CMAKE_C_STANDARD 11)

# PDCurses sources
file(GLOB PDCURSES_CORE
    pdcurses/*.c
    wincon/*.c
)

add_library(pdcurses STATIC ${PDCURSES_CORE})

target_include_directories(pdcurses PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/pdcurses
)

