cmake_minimum_required(VERSION 3.15)
project(ather_project C)

set(CMAKE_C_STANDARD 99)

if(WIN32)
    message(STATUS "Windows detected — using PDCurses wide version")

    # Path to the pdcurses directory INSIDE your repo
    set(PDCURSES_DIR "${CMAKE_SOURCE_DIR}/pdcurses")

    # Check if library already compiled
    if (NOT EXISTS "${PDCURSES_DIR}/wincon/pdcurses.a")
        message(STATUS "PDCurses not built — building automatically")

        # Build PDCurses (WIDE VERSION)
        # We use CMake's built-in build invocation
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E make_directory "${PDCURSES_DIR}/build"
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} -G "MinGW Makefiles" -DWIDE=Y ..
            WORKING_DIRECTORY "${PDCURSES_DIR}/build"
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} --build . --config Release
            WORKING_DIRECTORY "${PDCURSES_DIR}/build"
        )
    endif()

    # Tell CMake to link with the newly built library
    include_directories(${PDCURSES_DIR})
    link_directories(${PDCURSES_DIR}/wincon)

    set(CURSES_LIBRARIES pdcurses)
    message(STATUS "Using PDCurses library: ${CURSES_LIBRARIES}")

else()
    message(STATUS "UNIX system detected — using ncurses")

    find_package(Curses REQUIRED)
    include_directories(${CURSES_INCLUDE_DIR})
    set(CURSES_LIBRARIES ${CURSES_LIBRARIES})
endif()

file(GLOB SRC_FILES
    "*.c"
)

add_executable(ather ${SRC_FILES})

target_link_libraries(ather ${CURSES_LIBRARIES})

