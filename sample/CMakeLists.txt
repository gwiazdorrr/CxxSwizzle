find_package(SDL)
if(SDL_FOUND)

set_property(GLOBAL PROPERTY USE_FOLDERS On) 

	# get all the shaders
	file(GLOB shaders RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag")

	source_group("" FILES main.cpp)
	source_group("shaders" FILES ${shaders})
	
	include_directories(${SDL_INCLUDE_DIR} ${CxxSwizzle_SOURCE_DIR}/include)
	add_executable (sample main.cpp ${shaders})
	
	target_link_libraries (sample ${SDL_LIBRARY})
endif(SDL_FOUND)