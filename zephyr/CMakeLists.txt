cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

# Platformio demands a CMake project declaration in this folder
# Zephyr demands a Zephyr specialized CMake declaration in this folder
# Both can be found below.

file(GLOB_RECURSE SPINE_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/*.hpp
)

# Check if Zephyr is already included (e.g., when using West)
if (EXISTS "${CMAKE_SOURCE_DIR}/../.pio")
    message(STATUS "Spine building modality: platformio")
    project(Spine)
    target_sources(app PRIVATE ${SPINE_SOURCES})
    target_include_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
    target_compile_definitions(app PRIVATE ZEPHYR)
else ()
    message(STATUS "Spine building modality: zephyr")
    zephyr_library_named(spine)
    zephyr_library_sources(${SPINE_SOURCES})
    target_include_directories(spine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
    target_compile_definitions(spine PRIVATE ZEPHYR)
endif ()






