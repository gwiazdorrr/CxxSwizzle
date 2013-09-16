cmake_minimum_required (VERSION 2.6)
project (CxxSwizzle)

set_property(GLOBAL PROPERTY USE_FOLDERS On)

add_subdirectory(sample)
add_subdirectory(unit_test)

# get all the shaders
file(GLOB detail RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include/swizzle/detail/*.h")
file(GLOB naive RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include/swizzle/naive/*.h")
file(GLOB glsl RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include/swizzle/glsl/*.h")

source_group("swizzle\\detail" FILES ${detail})
source_group("swizzle\\naive" FILES ${naive})
source_group("swizzle\\glsl" FILES ${glsl})

add_library(headers STATIC ${naive} ${detail} ${glsl} Readme.md)
set_target_properties(headers PROPERTIES LINKER_LANGUAGE CXX)
