cmake_minimum_required(VERSION 3.0)
project(phone_numbers C)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif ()

set_property(GLOBAL PROPERTY RULE_MESSAGES OFF)


set(CMAKE_VERBOSE_MAKEFILE ON)


set(CMAKE_C_FLAGS "-std=c17 -Wall -Wextra -Wno-implicit-fallthrough")

set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_C_FLAGS_DEBUG "-g")


set(SOURCE_FILES
    src/phone_forward.h
    src/phone_forward.c
    src/phone_forward_example.c)

add_executable(phone_forward ${SOURCE_FILES})

find_package(Doxygen)
if (DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
    )
endif (DOXYGEN_FOUND)
