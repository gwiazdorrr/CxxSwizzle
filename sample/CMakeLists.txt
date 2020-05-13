# CxxSwizzle
# Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>

set (USE_VCPKG False CACHE BOOL "Use vcpkg")
set (USE_OPENMP True CACHE BOOL "Use OpenMP")

if (USE_VCPKG)
	find_package(SDL2 CONFIG REQUIRED)
else()
	message("${CMAKE_SOURCE_DIR}/cmake/")

	

	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
	# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

	find_package(SDL2 REQUIRED)
	# find_package(SDL2_image REQUIRED)

endif()

find_package(OpenMP)
# find_package(Vc CONFIG PATHS "${VC_INSTALL}" REQUIRED)


if (OPENMP_FOUND)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -DOMP_ENABLED=1")
else()
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -DOMP_ENABLED=0")
endif()

# I can't believe this needs to be set to be able to pass booleans to macros... what the hell CMake...
# see for details: 
#if(POLICY CMP0012)
#  cmake_policy(SET CMP0012 NEW)
#endif()

# get all the shaders
file(GLOB shaders RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag")

source_group("" FILES main.cpp use_scalar.h use_simd.h use_simd_masked.h )
# source_group("shaders" FILES ${shaders})

macro(create_sample_internal prefix defines header use_vc)
	foreach (shader ${shaders})
		get_filename_component(shader_sanitized ${shader} NAME_WE)
		string(REGEX REPLACE "[ \(\)]" "_" shader_sanitized "${shader_sanitized}")
		set(name ${prefix}_${shader_sanitized})

		set(generated_include_dir "${CMAKE_CURRENT_BINARY_DIR}/${name}_gen")
		set(generated_path "${generated_include_dir}/shader.frag")

		set(CMAKE_CONFIGURABLE_FILE_CONTENT "#include \"${shader}\"")
		configure_file("${CMAKE_ROOT}/Modules/CMakeConfigurableFile.in" "${generated_path}" @ONLY)
		unset(CMAKE_CONFIGURABLE_FILE_CONTENT)

	  	add_executable (${name} main.cpp ${header} ${shader} ${generated_path})
		target_include_directories(${name} PRIVATE ${CxxSwizzle_SOURCE_DIR}/include ${generated_include_dir})

		if (USE_VCPKG)
			target_link_libraries(${name} PRIVATE SDL2::SDL2 SDL2::SDL2main)
		else()
			target_link_libraries(${name} PRIVATE SDL2::Main)
		endif()

		set_target_properties(${name} PROPERTIES CXX_STANDARD 17)
		set_target_properties(${name} PROPERTIES CXX_STANDARD_REQUIRED ON)
		set_target_properties(${name} PROPERTIES COMPILE_FLAGS "${defines}")

		if (${use_vc})
			target_link_libraries("${name}" PRIVATE ${Vc_LIBRARIES})
			target_include_directories("${name}" PRIVATE ${Vc_INCLUDE_DIR})
			add_dependencies("${name}" Vc)
		endif()

	endforeach()
endmacro()


macro(create_sample name defines)
	if(MSVC)
        create_sample_internal("${name}" "${defines} /fp:fast /Gv" "use_scalar.h" 0)
		# create_sample_internal("${name}_avx2" "${defines} /fp:fast /Gv /arch:AVX2" "use_scalar.h")
    else()
		create_sample_internal("${name}" "${defines}" "use_scalar.h" 0)
    endif()
endmacro()

macro(create_simd_sample name defines)
	if(MSVC)
		create_sample_internal("${name}_simd" "${defines} /fp:fast /Gv /D_ENABLE_EXTENDED_ALIGNED_STORAGE" "use_simd.h" 1)
    else()
		create_sample_internal("${name}" "${defines}" "use_simd.h" 1)
    endif()
endmacro()

create_sample(sample_scalar "-DUSE_SCALAR")

set (BUILD_SIMD_SAMPLE True CACHE BOOL "BUILD_SIMD_SAMPLE")
if (BUILD_SIMD_SAMPLE)

    set (VC_INSTALL ${CMAKE_CURRENT_BINARY_DIR}/Vc-install)

	if (CMAKE_CONFIGURATION_TYPES)
		set(propagate_build_type --config $<CONFIG>)
	else()
		set(propagate_build_type -DCMAKE_BUILD_TYPE=$<CONFIG>)
	endif()


    ExternalProject_Add(Vc
      GIT_REPOSITORY    https://github.com/VcDevel/Vc
      GIT_TAG           1.4
      CMAKE_ARGS        -DCMAKE_INSTALL_PREFIX:PATH=${VC_INSTALL} -DBUILD_TESTING=OFF ${propagate_build_type}
    )

    set (Vc_LIBRARIES ${VC_INSTALL}/lib/Vc.lib)
    set (Vc_INCLUDE_DIR ${VC_INSTALL}/include)

	create_simd_sample(sample_simd "-DSAMPLE_USE_SIMD_VC")
    # create_simd_sample(sample_masked_simd  "-DSAMPLE_USE_SIMD_VC -DSAMPLE_USE_SIMD_MASKING")

endif()
