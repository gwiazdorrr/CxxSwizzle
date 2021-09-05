# CxxSwizzle (c) 2013-2021 Piotr Gwiazdowski

enable_testing()

find_package(GTest CONFIG REQUIRED)

set(COMPILER_SPECIFIC_FLAGS "")

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # GLSL has "not" function, so better get rid of C++ operator mnemonics
    set(COMPILER_SPECIFIC_FLAGS "-fno-operator-names")
endif()

set(HEADERS "setup.h")
set(SOURCE "main.cpp" "test_basic.cpp" "test_proxies.cpp" "test_specs.cpp" "test_inout_proxies.cpp")

source_group("" FILES ${SOURCE} ${HEADERS})

add_executable (test ${SOURCE} ${HEADERS})
set_target_properties(test PROPERTIES CXX_STANDARD 17)
set_target_properties(test PROPERTIES CXX_STANDARD_REQUIRED ON)
target_link_libraries(test PRIVATE GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main)
target_compile_options(test PRIVATE ${COMPILER_SPECIFIC_FLAGS})
target_include_directories(test PRIVATE ${CxxSwizzle_SOURCE_DIR}/include)
    
add_test(AllTests test)