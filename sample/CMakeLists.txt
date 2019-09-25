# CxxSwizzle
# Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>

find_package(SDL2 REQUIRED)
find_package(OpenMP)
# find_package(Vc CONFIG PATHS "${VC_INSTALL}" REQUIRED)

set (USE_VCPKG True CACHE FALSE "Use vcpkg")

if (OPENMP_FOUND)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -DOMP_ENABLED=1")
else()
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -DOMP_ENABLED=0")
endif()

# get all the shaders
file(GLOB shaders RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag")

source_group("" FILES main.cpp use_scalar.h use_simd.h use_simd_masked.h )
# source_group("shaders" FILES ${shaders})

macro(create_sample_internal prefix defines header)
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
		target_link_libraries(${name} PRIVATE SDL2::Main)
		set_target_properties(${name} PROPERTIES CXX_STANDARD 17)
		set_target_properties(${name} PROPERTIES CXX_STANDARD_REQUIRED ON)
		set_target_properties(${name} PROPERTIES COMPILE_FLAGS "${defines}")

	endforeach()
endmacro()


macro(create_sample name defines)
	if(MSVC)
        create_sample_internal("${name}_sse2" "${defines} /fp:fast /Gv /arch:SSE2" "use_scalar.h")
		# create_sample_internal("${name}_avx2" "${defines} /fp:fast /Gv /arch:AVX2" "use_scalar.h")
    else()
		create_sample_internal("${name}" "${defines}" "use_scalar.h")
    endif()
endmacro()

macro(create_simd_sample name defines)
	if(MSVC)
		create_sample_internal("${name}_sse2" "${defines} /fp:fast /Gv /arch:SSE2" "use_simd.h" ${useVc})
		target_link_libraries("${name}_sse2" PRIVATE ${Vc_LIBRARIES})
		target_include_directories("${name}_sse2" PRIVATE ${Vc_INCLUDE_DIR})
		add_dependencies("${name}_sse2" Vc)

		create_sample_internal("${name}_avx2" "${defines} /fp:fast /Gv /arch:AVX2" "use_simd.h" ${useVc})
		target_link_libraries("${name}_avx2" PRIVATE ${Vc_LIBRARIES})
		target_include_directories("${name}_avx2" PRIVATE ${Vc_INCLUDE_DIR})
		add_dependencies("${name}_avx2" Vc)

    else()
		create_sample_internal("${name}" "${defines}" "use_simd.h" ${useVc})

		target_link_libraries("${name}" PRIVATE ${Vc_LIBRARIES})
		target_include_directories("${name}" PRIVATE ${Vc_INCLUDE_DIR})
		add_dependencies("${name}" Vc)

    endif()
endmacro()

create_sample(sample_scalar "-DUSE_SCALAR")

set (BUILD_SIMD_SAMPLE True CACHE BOOL "BUILD_SIMD_SAMPLE")
if (BUILD_SIMD_SAMPLE)

    set (VC_INSTALL ${CMAKE_CURRENT_BINARY_DIR}/Vc-install)

    ExternalProject_Add(Vc
      GIT_REPOSITORY    https://github.com/VcDevel/Vc
      GIT_TAG           1.4
      CMAKE_ARGS        -DCMAKE_INSTALL_PREFIX:PATH=${VC_INSTALL} -DBUILD_TESTING=OFF
    )

    set (Vc_LIBRARIES ${VC_INSTALL}/lib/Vc.lib)
    set (Vc_INCLUDE_DIR ${VC_INSTALL}/include)

	# create_simd_sample(sample_simd "-DSAMPLE_USE_SIMD_VC")
    # create_simd_sample(sample_masked_simd  "-DSAMPLE_USE_SIMD_VC -DSAMPLE_USE_SIMD_MASKING")

endif()
