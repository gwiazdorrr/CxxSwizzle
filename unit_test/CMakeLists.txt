# CxxSwizzle
# Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>

find_package(GTest CONFIG REQUIRED)

set(COMPILER_SPECIFIC_FLAGS "")

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # GLSL has "not" function, so better get rid of C++ operator mnemonics
    set(COMPILER_SPECIFIC_FLAGS "-fno-operator-names")
endif()

file(GLOB headers RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB source RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

source_group("" FILES ${source} ${headers})

add_executable (unit_test ${source} ${headers})
include_directories(${CxxSwizzle_SOURCE_DIR}/include)

set_target_properties(unit_test PROPERTIES CXX_STANDARD 17)
set_target_properties(unit_test PROPERTIES CXX_STANDARD_REQUIRED ON)
target_link_libraries(unit_test PRIVATE GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main)
target_compile_options(unit_test PRIVATE ${COMPILER_SPECIFIC_FLAGS})