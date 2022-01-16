# CxxSwizzle (c) 2013-2021 Piotr Gwiazdowski
cmake_minimum_required(VERSION 3.3)
project (CxxSwizzle)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules")

include(ShadertoyAPI)
include(CxxSwizzleSandbox)

set_property(GLOBAL PROPERTY USE_FOLDERS On)

set(TRACY_PROFILER_ROOT "" CACHE PATH "Path to the Tracy Profiler root. Leave empty if you don't want to use it.")
option(ENABLE_PARALLELISM "Enables C++ Parallelism. Support for this is patchy to say the least." ON)

option(BUILD_SAMPLES "Build Samples (Scalar)" ON)
option(BUILD_SAMPLES_SIMD "Build Samples (SIMD)" OFF)
option(BUILD_TESTS "Build Tests" OFF)
option(BUILD_SHADERTOYS "Build Shadertoys" ON)

if (BUILD_SAMPLES OR BUILD_SAMPLES_SIMD)
	add_subdirectory(samples)
endif()

if (BUILD_TESTS)
	add_subdirectory(test)
endif()

if (BUILD_SHADERTOYS)

	set(SHADERTOY_TEXTURES_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/textures" CACHE PATH "Directory where textures are downloaded to/read from.")
	set(SHADERTOY_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/shadertoys" CACHE PATH "Directory where shadertoys are downloaded to/read from.")

	set(SHADERTOY_SETUP "scalar" CACHE STRING "SIMD type")
	mark_as_advanced(SHADERTOY_SETUP)
	set_property(CACHE SHADERTOY_SETUP PROPERTY STRINGS ${CXXSWIZZLE_SETUPS})

	# handle downloads

	set(SHADERTOY_API_KEY "BtntM7" CACHE STRING "Shadertoy API Key (go to https://www.shadertoy.com/howto#q2 for details)")
	mark_as_advanced(SHADERTOY_API_KEY)

	set(SHADERTOY_DOWNLOAD "none" CACHE STRING "Download Shader")
	set_property(CACHE SHADERTOY_DOWNLOAD PROPERTY STRINGS none id top_love top_popular top_newest top_hot)

	set(SHADERTOY_DOWNLOAD_PARAM "" CACHE STRING "List of Shadertoys ids (if SHADERTOY_DOWNLOAD is set to id) or a search term")
	set(SHADERTOY_DOWNLOAD_MAX_COUNT "10" CACHE STRING "Max Shadertoys to download")

	option(SHADERTOY_APPLY_TRIVIAL_FIXES "" ON)

	if (SHADERTOY_DOWNLOAD STREQUAL "id")
	    if (NOT SHADERTOY_DOWNLOAD_PARAM)
	        message(FATAL_ERROR "SHADERTOY_DOWNLOAD_PARAM needs to be set")
	    else()
	        foreach (id ${SHADERTOY_DOWNLOAD_PARAM})
	            shadertoy_download(${SHADERTOY_API_KEY} ${id} "${SHADERTOY_ROOT}" "${SHADERTOY_TEXTURES_ROOT}" True ${SHADERTOY_APPLY_TRIVIAL_FIXES})
	        endforeach()
	        message(STATUS "Download done, setting SHADERTOY_DOWNLOAD to none")
	        set(SHADERTOY_DOWNLOAD "none" CACHE STRING "Download Shader" FORCE)
	    endif()
	elseif(SHADERTOY_DOWNLOAD MATCHES "^top_")
	    string (REPLACE "top_" "" SHADERTOY_DOWNLOAD_LIST_TYPE ${SHADERTOY_DOWNLOAD})
	    shadertoy_query(${SHADERTOY_API_KEY} ${SHADERTOY_DOWNLOAD_LIST_TYPE} "${SHADERTOY_DOWNLOAD_PARAM}" ${SHADERTOY_DOWNLOAD_MAX_COUNT} SHADERTOY_SHADER_LIST)
	    foreach (id ${SHADERTOY_SHADER_LIST})
	        shadertoy_download(${SHADERTOY_API_KEY} ${id} "${SHADERTOY_ROOT}" "${SHADERTOY_TEXTURES_ROOT}" False ${SHADERTOY_APPLY_TRIVIAL_FIXES})
	    endforeach()
	    message(STATUS "Download done, setting SHADERTOY_DOWNLOAD to none")
	    set(SHADERTOY_DOWNLOAD "none" CACHE STRING "Download Shader" FORCE)
	endif()

	if (BUILD_SHADERTOYS)
	    cxxswizzle_prepare_setup("${SHADERTOY_SETUP}")
	    file(GLOB shadertoys RELATIVE ${SHADERTOY_ROOT} "${SHADERTOY_ROOT}/*/image.frag")
	    foreach(shadertoy ${shadertoys})
	       get_filename_component(name ${shadertoy} DIRECTORY)
	       cxxswizzle_create_runner("shadertoy_${name}" "${SHADERTOY_ROOT}/${name}" "${SHADERTOY_SETUP}" "${SHADERTOY_TEXTURES_ROOT}" )
	    endforeach()
	endif()

endif()

# get all the shaders
file(GLOB root RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include/swizzle/*.*")
file(GLOB detail RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include/swizzle/detail/*.*")

source_group("swizzle" FILES ${root})
source_group("swizzle\\detail" FILES ${detail})

file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp" CONTENT "// a dummy source file for build systems complaining about no .cpp in a project\n")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${Vc_INCLUDE_DIR})
add_library(headers STATIC ${root} ${detail} ${glsl} ${glsl_detail} README.md "${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp")
set_target_properties(headers PROPERTIES LINKER_LANGUAGE CXX)