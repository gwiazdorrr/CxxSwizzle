# CxxSwizzle
# Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>

find_package(Boost)

if(Boost_FOUND)

	file(GLOB headers RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
	file(GLOB source RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

	source_group("" FILES ${source} ${headers})
	
	include_directories(${Boost_INCLUDE_DIR} ${CxxSwizzle_SOURCE_DIR}/include)
	
	add_executable (unit_test ${source} ${headers})
endif(Boost_FOUND)