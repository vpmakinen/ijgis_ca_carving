project(flowrouter)

cmake_minimum_required(VERSION 2.8.2)
include(ExternalProject)
if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" VERSION_GREATER 2.6)
  if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}" VERSION_GREATER 2.8.3)
    cmake_policy(VERSION 2.8.4)
  else()
    cmake_policy(VERSION 2.6)
  endif()
endif()

set( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true )

if ( COMMAND cmake_policy )

  cmake_policy( SET CMP0003 NEW )

endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

enable_testing()

if (NOT DEFINED CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release")
endif()

add_definitions(-std=c++1y)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_definitions(-Wall -Wextra -Wpedantic -pedantic-errors -Werror
    -Wdouble-promotion -Wswitch-default -Wswitch-enum -Wconversion
    -Wsign-compare -Wsign-conversion)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_definitions(-Weverything -Werror -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-covered-switch-default -Wno-documentation-unknown-command)
endif()

# Required packages

# Boost and its components
if (DEFINED CUSTOM_BOOST_INCLUDE_DIR AND DEFINED CUSTOM_BOOST_LIBRARIES)
    set(Boost_INCLUDE_DIRS ${CUSTOM_BOOST_INCLUDE_DIR})
    set(Boost_LIBRARIES ${CUSTOM_BOOST_LIBRARIES})
elseif (DEFINED CUSTOM_BOOST_INCLUDE_DIR OR DEFINED CUSTOM_BOOST_LIBRARIES)
    message(FATAL_ERROR
"  You must define both CUSTOM_BOOST_INCLUDE_DIR and CUSTOM_BOOST_LIBRARIES.
  The required components are: filesystem, program_options and system.
  An example for osx/clang:
  -DCUSTOM_BOOST_INCLUDE_DIR=/opt/local/include -DCUSTOM_BOOST_LIBRARIES=\"/opt/local/lib/libboost_filesystem-mt.dylib;/opt/local/lib/libboost_program_options-mt.dylib;/opt/local/lib/libboost_system-mt.dylib\"")
else()
    find_package(Boost REQUIRED COMPONENTS filesystem program_options regex system)
    if (NOT Boost_FOUND)
        message(FATAL_ERROR "This project requires the Boost library, and will not be compiled.")
    endif()
endif()
add_library(ext_boost INTERFACE)
target_link_libraries(ext_boost INTERFACE ${Boost_LIBRARIES})
target_include_directories(ext_boost SYSTEM INTERFACE ${Boost_INCLUDE_DIRS})

# GDAL
if (DEFINED CUSTOM_GDAL_INCLUDE_DIR AND DEFINED CUSTOM_GDAL_LIBRARY)
    message(STATUS "Using custom gdal library")
    set(GDAL_INCLUDE_DIRS ${CUSTOM_GDAL_INCLUDE_DIR})
    set(GDAL_LIBRARY ${CUSTOM_GDAL_LIBRARY})
elseif (DEFINED CUSTOM_GDAL_INCLUDE_DIR OR DEFINED CUSTOM_GDAL_LIBRARY)
    message(FATAL_ERROR "You must define both CUSTOM_GDAL_INCLUDE_DIR and CUSTOM_GDAL_LIBRARY")
else()
    find_package(GDAL 2.0 REQUIRED)
endif()
if (NOT EXISTS ${GDAL_INCLUDE_DIRS}/gdal.h)
    message(FATAL_ERROR "Could not locate file ${GDAL_INCLUDE_DIRS}/gdal.h")
endif()
add_library(ext_gdal INTERFACE)
target_link_libraries(ext_gdal INTERFACE ${GDAL_LIBRARY})
target_include_directories(ext_gdal SYSTEM INTERFACE ${GDAL_INCLUDE_DIRS})

add_subdirectory(cmake_extras)
include_directories("$(PROJECT_SOURCE_DIR)/src")
add_subdirectory(src)
