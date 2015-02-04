# CxxSwizzle
# Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>

find_package(SDL REQUIRED)
find_package(SDL_image)
find_package(OpenMP)

# this will look in the local cmake directory only if Vc hasn't been built/installed locally

if(MSVC)
	# hint to use supplied, patched build
	find_package(Vc CONFIG PATHS "${CMAKE_SOURCE_DIR}/external/cmake")
else()
	# regular search
	find_package(Vc)
endif()

if(SDL_FOUND AND SDL_IMAGE_FOUND)

	if (OPENMP_FOUND)
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -DOMP_ENABLED=1")
	else()
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -DOMP_ENABLED=0")
	endif()
	
	# get all the shaders
	file(GLOB shaders RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag")

	source_group("" FILES main.cpp use_scalar.h use_simd.h use_simd_masked.h )
	source_group("shaders" FILES ${shaders})
	
	add_executable (sample_scalar main.cpp use_scalar.h ${shaders})
	include_directories(${SDL_INCLUDE_DIR} ${CxxSwizzle_SOURCE_DIR}/include)
	target_link_libraries (sample_scalar ${SDL_LIBRARY})

	if(SDLIMAGE_FOUND)
		include_directories(${SDL_IMAGE_INCLUDE_DIR})
		target_link_libraries (sample_scalar ${SDL_IMAGE_LIBRARY})
		set_target_properties(sample_scalar PROPERTIES COMPILE_FLAGS "-DUSE_SCALAR -DSDLIMAGE_FOUND")
	else()
		set_target_properties(sample_scalar PROPERTIES COMPILE_FLAGS "-DUSE_SCALAR")
		message(WARNING "SDL_image not found, loading textures not going to be available.")
	endif()

	
	if(Vc_FOUND)
		add_executable(sample_simd main.cpp use_simd.h ${shaders})
		target_link_libraries(sample_simd ${SDL_LIBRARY} ${Vc_LIBRARIES})
		
		if(SDLIMAGE_FOUND)
			target_link_libraries(sample_simd ${SDL_IMAGE_LIBRARY})
			set_target_properties(sample_simd PROPERTIES COMPILE_FLAGS "${Vc_DEFINITIONS} -DUSE_SIMD -DSDLIMAGE_FOUND")
		else()
			set_target_properties(sample_simd PROPERTIES COMPILE_FLAGS "${Vc_DEFINITIONS} -DUSE_SIMD")
		endif()

		target_include_directories(sample_simd PRIVATE ${Vc_INCLUDE_DIR})
	else()
		message(WARNING "Vc not found, SIMD sample not going to be available.")
	endif()
endif()