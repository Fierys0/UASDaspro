cmake_minimum_required(VERSION 3.15)
project(ather_project C)

set(CMAKE_C_STANDARD 99)

if (WIN32)
    message(STATUS "Building with PDCurses (wide mode)")

    set(PDCURSES_DIR "${CMAKE_SOURCE_DIR}/pdcurses")
    set(PDC_WINCON "${PDCURSES_DIR}/wincon")

    # Expected output library from MinGW build
    set(PDC_LIB "${PDC_WINCON}/pdcurses.a")

    # Build only if missing
    if (NOT EXISTS "${PDC_LIB}")
        message(STATUS "PDCurses library not found — building...")

        execute_process(
            COMMAND mingw32-make -f Makefile WIDE=Y UTF8=Y
            WORKING_DIRECTORY ${PDC_WINCON}
        )
    endif()

    include_directories(${PDCURSES_DIR})
    include_directories(${PDC_WINCON})
    link_directories(${PDC_WINCON})

    set(CURSES_LIBRARIES ${PDC_LIB})
    message(STATUS "Using PDCurses: ${CURSES_LIBRARIES}")

else()
    find_package(Curses REQUIRED)
    include_directories(${CURSES_INCLUDE_DIR})
    set(CURSES_LIBRARIES ${CURSES_LIBRARIES})
endif()

file(GLOB SRC_FILES "*.c")
add_executable(ather ${SRC_FILES})
target_link_libraries(ather ${CURSES_LIBRARIES})