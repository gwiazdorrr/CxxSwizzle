# CxxSwizzle
# Copyright (c) 2013-2021, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>

cmake_minimum_required(VERSION 3.3)
project (CxxSwizzle)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules")

set_property(GLOBAL PROPERTY USE_FOLDERS On)

set(CXXSWIZZLE_SANDBOX_TYPE "scalar" CACHE STRING "SIMD type")
set_property(CACHE CXXSWIZZLE_SANDBOX_TYPE PROPERTY STRINGS scalar simd_naive simd_vc simd_vc_with_masking)
set(CXXSWIZZLE_SANDBOX_STRUCTS Ray;ray CACHE STRING "Custom structs that need function-style init or out/inout modifier")
set(CXXSWIZZLE_SANDBOX_TRACY_PROFILER_ROOT "" CACHE PATH "Path to the Tracy Profiler root. Leave empty if you don't want to use it.")
set(CXXSWIZZLE_SANDBOX_TEXTURES_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/textures" CACHE PATH "Path to the Tracy Profiler root. Leave empty if you don't want to use it.")

option(BUILD_SAMPLES "Build Samples" ON)
option(BUILD_TESTS "Build Tests" OFF)
option(BUILD_SHADERTOYS "Build Shadertoys" ON)

if (BUILD_SAMPLES)
	add_subdirectory(samples)
endif()

if (BUILD_TESTS)
	add_subdirectory(test)
endif()

if (BUILD_SHADERTOYS)
	add_subdirectory(shadertoys)
endif()

# get all the shaders
file(GLOB root RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include/swizzle/*.*")
file(GLOB detail RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include/swizzle/detail/*.*")

source_group("swizzle" FILES ${root})
source_group("swizzle\\detail" FILES ${detail})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${Vc_INCLUDE_DIR})
add_library(headers STATIC ${root} ${detail} ${glsl} ${glsl_detail} dummy.cpp README.md)
set_target_properties(headers PROPERTIES LINKER_LANGUAGE CXX)