# CxxSwizzle
# Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>

find_package(Boost 1.73 COMPONENTS unit_test_framework)

if(Boost_FOUND)

	file(GLOB headers RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
	file(GLOB source RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

	source_group("" FILES ${source} ${headers})
	
	include_directories(${Boost_INCLUDE_DIR} ${CxxSwizzle_SOURCE_DIR}/include)
	
	add_executable (unit_test ${source} ${headers})

	set_target_properties(unit_test PROPERTIES CXX_STANDARD 17)
	set_target_properties(unit_test PROPERTIES CXX_STANDARD_REQUIRED ON)
	target_link_libraries(unit_test ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
endif(Boost_FOUND)