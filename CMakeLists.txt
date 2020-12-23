# CxxSwizzle
# Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>

cmake_minimum_required (VERSION 2.6)
project (CxxSwizzle)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules")

set_property(GLOBAL PROPERTY USE_FOLDERS On)

add_subdirectory(shadertoys)
add_subdirectory(sample)
add_subdirectory(unit_test)

# get all the shaders
file(GLOB root RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include/swizzle/*.*")
file(GLOB detail RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include/swizzle/detail/*.*")
file(GLOB glsl RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include/swizzle/glsl/*.*")
file(GLOB glsl_detail RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include/swizzle/detail/glsl/*.*")

source_group("swizzle" FILES ${root})
source_group("swizzle\\detail" FILES ${detail})
source_group("swizzle\\glsl" FILES ${glsl})
source_group("swizzle\\detail\\glsl" FILES ${glsl_detail})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${Vc_INCLUDE_DIR})
add_library(headers STATIC ${root} ${detail} ${glsl} ${glsl_detail} dummy.cpp README.md)
set_target_properties(headers PROPERTIES LINKER_LANGUAGE CXX)