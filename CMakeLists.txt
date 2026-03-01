cmake_minimum_required(VERSION 3.14)
project(qpsk LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type: Debug or Release" FORCE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -Wextra -Werror")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -march=native")
else()
    message(FATAL_ERROR "Unsupported build type: ${CMAKE_BUILD_TYPE}. Use Debug or Release.")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -Wno-sign-compare")

file(GLOB_RECURSE LIB_SOURCES "lib/*.cpp")
add_library(qpsk_core STATIC ${LIB_SOURCES})
target_include_directories(qpsk_core PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/third_party
)

file(GLOB_RECURSE SRC_SOURCES "src/*.cpp")
add_executable(qpsk ${SRC_SOURCES})
target_link_libraries(qpsk qpsk_core)

configure_file(
    ${CMAKE_SOURCE_DIR}/scripts/plot_bler_curves.py
    ${CMAKE_BINARY_DIR}/plot_bler_curves.py
    COPYONLY
)

add_custom_target(run_plot
    COMMAND python3 ${CMAKE_BINARY_DIR}/plot_bler_curves.py
    DEPENDS ${TARGET_NAME} ${CMAKE_BINARY_DIR}/plot_bler_curves.py
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

enable_testing()
if(EXISTS "${PROJECT_SOURCE_DIR}/tests/CMakeLists.txt")
    add_subdirectory(tests)
endif()
