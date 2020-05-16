# CxxSwizzle
# Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>

set (USE_VCPKG False CACHE BOOL "Use vcpkg")
set (USE_OPENMP True CACHE BOOL "Use OpenMP")

find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-image CONFIG REQUIRED)

if (USE_OPENMP)
	find_package(OpenMP)
endif()

if (OPENMP_FOUND)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -DOMP_ENABLED=1")
else()
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOMP_ENABLED=0")
endif()


set(SIMD_ARCHITECTURE_VALUES Default SSE2 AVX2)
set(SIMD_ARCHITECTURE Default CACHE STRING "SIMD architecture (VC++ only)")
set_property(CACHE SIMD_ARCHITECTURE PROPERTY STRINGS ${SIMD_ARCHITECTURE_VALUES})

set(COMMON_DEFINES "")
if(NOT ${SIMD_ARCHITECTURE} STREQUAL "Default")
	set(COMMON_DEFINES "${COMMON_DEFINES} /arch:${SIMD_ARCHITECTURE}")
endif()

# get all the shaders
file(GLOB shaders RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag")

source_group("" FILES main.cpp shadertoy_sandbox.cpp shadertoy_sandbox.hpp use_scalar.h use_simd.h use_simd_masked.h shader_include.hpp config.hpp )
# source_group("shaders" FILES ${shaders})

set(NATVIS_FILE "")
if(MSVC)
	set(NATVIS_FILE "cxxswizzle.natvis")
endif()


macro(create_sample_internal prefix defines header useVcLib)
	foreach (shader ${shaders})
		get_filename_component(shader_sanitized ${shader} NAME_WE)
		string(REGEX REPLACE "[ \(\)]" "_" shader_sanitized "${shader_sanitized}")
		set(name ${prefix}_${shader_sanitized})

		set(generated_include_dir "${CMAKE_CURRENT_BINARY_DIR}/${name}_gen")
		set(generated_shader_path "${generated_include_dir}/shader_include.hpp")
		set(generated_config_path "${generated_include_dir}/config.hpp")


		configure_file("config.hpp.in" "${generated_config_path}")
		set(CMAKE_CONFIGURABLE_FILE_CONTENT "#include \"${CMAKE_CURRENT_SOURCE_DIR}/${shader}\"")
		configure_file("${CMAKE_ROOT}/Modules/CMakeConfigurableFile.in" "${generated_shader_path}" @ONLY)
		unset(CMAKE_CONFIGURABLE_FILE_CONTENT)

	  	add_executable (${name} main.cpp shadertoy_sandbox.cpp shadertoy_sandbox.hpp ${header} ${shader} ${generated_shader_path} ${generated_config_path} ${NATVIS_FILE})
		target_include_directories(${name} PRIVATE ${CxxSwizzle_SOURCE_DIR}/include ${generated_include_dir})

		target_link_libraries(${name} PRIVATE SDL2::SDL2 SDL2::SDL2main SDL2::SDL2_image)

		set_target_properties(${name} PROPERTIES CXX_STANDARD 17)
		set_target_properties(${name} PROPERTIES CXX_STANDARD_REQUIRED ON)

		if (${useVcLib})
			target_link_libraries("${name}" PRIVATE ${Vc_LIBRARIES})
			target_include_directories("${name}" PRIVATE ${Vc_INCLUDE_DIR})
			add_dependencies("${name}" Vc)
		endif()

		set (SDL_DEFINES "")
		#if(SDLIMAGE_FOUND)
		#	include_directories("${name}" PRIVATE ${SDL2_IMAGE_INCLUDE_DIRS})
		#	target_link_libraries("${name}" PRIVATE ${SDL2_IMAGE_LIBRARIES})
		#	set(SDL_DEFINES "-DSDLIMAGE_FOUND")
		#else()
	#		message(WARNING "SDL_image not found, loading textures not going to be available.")
		#endif()

		set_target_properties(${name} PROPERTIES COMPILE_FLAGS "${COMMON_DEFINES} ${defines} ${SDL_DEFINES}")

	endforeach()
endmacro()


macro(create_sample name defines)
	if(MSVC)
        create_sample_internal("${name}" "${defines} /fp:fast /Gv" "use_scalar.h" 0)
    else()
		create_sample_internal("${name}" "${defines}" "use_scalar.h" 0)
    endif()
endmacro()

macro(create_simd_sample name defines)
	if(MSVC)
		create_sample_internal("${name}" "${defines} /fp:fast /Gv /D_ENABLE_EXTENDED_ALIGNED_STORAGE" "use_simd.h" 1)
    else()
		create_sample_internal("${name}" "${defines}" "use_simd.h" 1)
    endif()
endmacro()

create_sample(sample_scalar "-DUSE_SCALAR")

set (BUILD_SIMD_SAMPLE True CACHE BOOL "BUILD_SIMD_SAMPLE")
if (BUILD_SIMD_SAMPLE)

    set (VC_INSTALL ${CMAKE_CURRENT_BINARY_DIR}/Vc-install)

	# TODO: add link
	if (CMAKE_CONFIGURATION_TYPES)
		set(propagate_build_type --config $<CONFIG>)
	else()
		set(propagate_build_type -DCMAKE_BUILD_TYPE=$<CONFIG>)
	endif()

    ExternalProject_Add(Vc
      GIT_REPOSITORY    https://github.com/VcDevel/Vc
      GIT_TAG           1.4
      CMAKE_ARGS        -DCMAKE_INSTALL_PREFIX:PATH=${VC_INSTALL} -DBUILD_TESTING=OFF ${propagate_build_type}
	  BUILD_BYPRODUCTS  ${VC_INSTALL}/lib/Vc.lib
    )

    set (Vc_LIBRARIES ${VC_INSTALL}/lib/Vc.lib)
    set (Vc_INCLUDE_DIR ${VC_INSTALL}/include)


	create_simd_sample(sample_simd "-DSAMPLE_USE_SIMD_VC")
    # create_simd_sample(sample_masked_simd  "-DSAMPLE_USE_SIMD_VC -DSAMPLE_USE_SIMD_MASKING")

endif()
