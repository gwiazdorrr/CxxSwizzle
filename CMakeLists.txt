# CxxSwizzle
# Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>

cmake_minimum_required (VERSION 2.6)
project (CxxSwizzle)

set_property(GLOBAL PROPERTY USE_FOLDERS On)

add_subdirectory(sample)
add_subdirectory(unit_test)

# get all the shaders
file(GLOB detail RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include/swizzle/detail/*.h")
file(GLOB glsl RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include/swizzle/glsl/*.h")
file(GLOB glsl_detail RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include/swizzle/detail/glsl/*.h")

source_group("swizzle\\detail" FILES ${detail})
source_group("swizzle\\glsl" FILES ${glsl})
source_group("swizzle\\detail\\glsl" FILES ${glsl_detail})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${Vc_INCLUDE_DIR})
add_library(headers STATIC ${detail} ${glsl} ${glsl_detail} Readme.md)
set_target_properties(headers PROPERTIES LINKER_LANGUAGE CXX)
