# CxxSwizzle
# Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>

find_package(SDL)
find_package(SDL_image)
find_package(OpenMP)

if(SDL_FOUND AND SDL_IMAGE_FOUND)

	if (OPENMP_FOUND)
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -DOMP_ENABLED=1")
	else()
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -DOMP_ENABLED=0")
	endif()

	# get all the shaders
	file(GLOB shaders RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag")

	source_group("" FILES main.cpp)
	source_group("shaders" FILES ${shaders})
	
	include_directories(${SDL_INCLUDE_DIR} ${SDL_IMAGE_INCLUDE_DIR} ${CxxSwizzle_SOURCE_DIR}/include)
	add_executable (sample main.cpp ${shaders})
	
	target_link_libraries (sample ${SDL_LIBRARY} ${SDL_IMAGE_LIBRARY})
endif()