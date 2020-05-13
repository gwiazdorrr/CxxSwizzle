# CxxSwizzle
# Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>

find_package(SDL2 REQUIRED)
find_package(OpenMP)
# find_package(Vc CONFIG PATHS "${VC_INSTALL}" REQUIRED)

if (OPENMP_FOUND)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -DOMP_ENABLED=1")
else()
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -DOMP_ENABLED=0")
endif()

# get all the shaders
file(GLOB shaders RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag")

source_group("" FILES main.cpp use_scalar.h use_simd.h use_simd_masked.h )
source_group("shaders" FILES ${shaders})




macro(create_sample name defines header)

	add_executable (${name} main.cpp ${header} ${shaders})
	target_include_directories(${name} PRIVATE ${CxxSwizzle_SOURCE_DIR}/include)
	target_link_libraries(${name} PRIVATE SDL2::Main)

	set_target_properties(${name} PROPERTIES CXX_STANDARD 17)
	set_target_properties(${name} PROPERTIES CXX_STANDARD_REQUIRED ON)

	if(MSVC)
        set_target_properties(${name} PROPERTIES COMPILE_FLAGS "${defines} /Gv")
    else()
        set_target_properties(${name} PROPERTIES COMPILE_FLAGS "${defines}")
    endif()


endmacro()

macro(create_simd_project name defines)

	create_sample(${name} ${defines} "use_simd.h")
    
    target_link_libraries(${name} PRIVATE ${Vc_LIBRARIES})
    target_include_directories(${name} PRIVATE ${Vc_INCLUDE_DIR})
    add_dependencies(${name} Vc)

endmacro()

create_sample(sample_scalar "-DUSE_SCALAR" "use_scalar.h")

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

	create_simd_project(sample_simd "-DSAMPLE_USE_SIMD_VC")
    create_simd_project(sample_simd_masked "-DSAMPLE_USE_SIMD_VC -DSAMPLE_USE_SIMD_MASKING")

endif()