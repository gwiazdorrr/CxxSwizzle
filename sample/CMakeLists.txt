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

add_executable (sample_scalar main.cpp use_scalar.h ${shaders})
target_include_directories(sample_scalar PRIVATE ${CxxSwizzle_SOURCE_DIR}/include)
target_link_libraries(sample_scalar PRIVATE SDL2::Main)
set_target_properties(sample_scalar PROPERTIES COMPILE_FLAGS "-DUSE_SCALAR")


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

    add_executable(sample_simd main.cpp use_simd.h ${shaders})
    target_include_directories(sample_simd PRIVATE ${CxxSwizzle_SOURCE_DIR}/include)
    target_link_libraries(sample_simd PRIVATE SDL2::Main)
    
    target_link_libraries(sample_simd PRIVATE ${Vc_LIBRARIES})
    target_include_directories(sample_simd PRIVATE ${Vc_INCLUDE_DIR})
    add_dependencies(sample_simd Vc)

    if(MSVC)
        set_target_properties(sample_simd PROPERTIES COMPILE_FLAGS "-DUSE_SIMD /Gv")
    else()
        set_target_properties(sample_simd PROPERTIES COMPILE_FLAGS "-DUSE_SIMD")
    endif()
    
endif()