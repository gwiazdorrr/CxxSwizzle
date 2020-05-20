# CxxSwizzle
# Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>

macro(get_subdir_list result curdir)
    file(GLOB children RELATIVE ${curdir} ${curdir}/*)
    set(dirlist "")
    foreach(child ${children})
        if(IS_DIRECTORY ${curdir}/${child})
            list(APPEND dirlist ${child})
        endif()
    endforeach()
    set(${result} ${dirlist})
endmacro()

set (USE_VCPKG False CACHE BOOL "Use vcpkg")
set (USE_OPENMP True CACHE BOOL "Use OpenMP")

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2-image CONFIG REQUIRED)
set(SAMPLE_SDL2IMAGE_FOUND True)

if (USE_OPENMP)
    find_package(OpenMP)
endif()

# enable OpenMP
if (OPENMP_FOUND AND USE_OPENMP)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -DSAMPLE_OMP_ENABLED")
endif()

set(COMPILER_SPECIFIC_FILES "")
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # GLSL has "not" function, so better get rid of C++ operator mnemonics; this is one of
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-operator-names")
elseif(MSVC)
    # fast math, _vectorcall
    # the resons permissive can't be disabled in MSVC, because there's no way to disable mnemonics then
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:fast /Gv -D_ENABLE_EXTENDED_ALIGNED_STORAGE")
    # Add debug visualisers (work great!)	
    set(COMPILER_SPECIFIC_FILES "cxxswizzle.natvis")

    # let user force SIMD architecture
    set(SIMD_ARCHITECTURE_VALUES Default SSE2 AVX2)
    set(SIMD_ARCHITECTURE Default CACHE STRING "SIMD architecture (VC++ only)")
    set_property(CACHE SIMD_ARCHITECTURE PROPERTY STRINGS ${SIMD_ARCHITECTURE_VALUES})

    if(NOT ${SIMD_ARCHITECTURE} STREQUAL "Default")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:${SIMD_ARCHITECTURE}")
    endif()
endif()

set (BUILD_SIMD_SAMPLE False CACHE BOOL "BUILD_SIMD_SAMPLE")
if (BUILD_SIMD_SAMPLE)
    set (VC_INSTALL ${CMAKE_CURRENT_BINARY_DIR}/Vc-install)

    # TODO: add link
    if (CMAKE_CONFIGURATION_TYPES)
        set(PROPAGATE_BUILD_TYPE --config $<CONFIG>)
    else()
        set(PROPAGATE_BUILD_TYPE -DCMAKE_BUILD_TYPE=$<CONFIG>)
    endif()

    ExternalProject_Add(Vc
        GIT_REPOSITORY    https://github.com/VcDevel/Vc
        GIT_TAG           1.4
        CMAKE_ARGS        -DCMAKE_INSTALL_PREFIX:PATH=${VC_INSTALL} -DBUILD_TESTING=OFF ${PROPAGATE_BUILD_TYPE}
        BUILD_BYPRODUCTS  ${VC_INSTALL}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}Vc${CMAKE_STATIC_LIBRARY_SUFFIX}
    )

    set (Vc_LIBRARIES ${VC_INSTALL}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}Vc${CMAKE_STATIC_LIBRARY_SUFFIX})
    set (Vc_INCLUDE_DIR ${VC_INSTALL}/include)
endif()


# TODO iChannelX
# TODO BufC

get_subdir_list(SAMPLES_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

foreach(SAMPLE_DIR ${SAMPLES_DIRS})
    file(GLOB SHADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/${SAMPLE_DIR}/*.frag")
    file(GLOB TEXTURES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/${SAMPLE_DIR}/*.png" "${CMAKE_CURRENT_SOURCE_DIR}/${SAMPLE_DIR}/*.jpg")
    file(GLOB BUFFERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/${SAMPLE_DIR}/*.buffer")

    get_filename_component(SAMPLE_NAME_SANITIZED ${SAMPLE_DIR} NAME_WE)
    string(REGEX REPLACE "[ \(\)]" "_" SAMPLE_NAME_SANITIZED "${SAMPLE_NAME_SANITIZED}")
    set(SAMPLE_NAME ${SAMPLE_NAME_SANITIZED})

    set(GEN_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/${SAMPLE_NAME}_gen")
    set(GEN_CONFIG_PATH "${GEN_INCLUDE_DIR}/config.hpp")

    # set the config default values
    foreach(PREFIX SAMPLE SAMPLE_BUFFER_A SAMPLE_BUFFER_B SAMPLE_BUFFER_C SAMPLE_BUFFER_D)
        foreach(INDEX 0 1 2 3)
            set(${PREFIX}_CHANNEL${INDEX}_PATH "\\0")
        endforeach()
    endforeach()

    # defines will be needed
    foreach(SHADER ${SHADERS})
        get_filename_component(SHADER_NAME_SANITIZED ${SHADER} NAME_WE)
        string(TOLOWER ${SHADER_NAME_SANITIZED} SHADER_NAME_SANITIZED_LOWER)

        set(CMAKE_CONFIGURABLE_FILE_CONTENT "#include \"${CMAKE_CURRENT_SOURCE_DIR}/${SHADER}\"")
        configure_file("${CMAKE_ROOT}/Modules/CMakeConfigurableFile.in" "${GEN_INCLUDE_DIR}/${SHADER_NAME_SANITIZED_LOWER}_include.hpp" @ONLY)
        unset(CMAKE_CONFIGURABLE_FILE_CONTENT)

        string(TOUPPER ${SHADER_NAME_SANITIZED} SHADER_NAME_SANITIZED)
        set(SAMPLE_HAS_${SHADER_NAME_SANITIZED} True)
    endforeach()

    foreach (TEXTURE ${TEXTURES})
        get_filename_component(CHANNEL_NAME ${TEXTURE} NAME_WE)
        string(TOUPPER ${CHANNEL_NAME} CHANNEL_NAME)
        set(SAMPLE_${CHANNEL_NAME}_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${TEXTURE}")
    endforeach()

    foreach (BUFFER ${BUFFERS})
        file(READ "${CMAKE_CURRENT_SOURCE_DIR}/${BUFFER}" BUFFER_ID)
        get_filename_component(CHANNEL_NAME ${BUFFER} NAME_WE)
        string(TOUPPER ${CHANNEL_NAME} CHANNEL_NAME)
        set(SAMPLE_${CHANNEL_NAME}_PATH "${BUFFER_ID}")
    endforeach()

    configure_file("config.hpp.in" "${GEN_CONFIG_PATH}")

    # no stupid grouping
    set(FILES main.cpp shadertoy_sandbox.cpp shadertoy_sandbox.hpp ${SHADERS} ${GEN_CONFIG_PATH} ${COMPILER_SPECIFIC_FILES})
    source_group("" FILES ${FILES} )

    set(SAMPLE_NAME "scalar_${SAMPLE_NAME_SANITIZED}")
    add_executable(${SAMPLE_NAME} ${FILES})
    target_include_directories(${SAMPLE_NAME} PRIVATE ${CxxSwizzle_SOURCE_DIR}/include ${GEN_INCLUDE_DIR})
    target_link_libraries(${SAMPLE_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main SDL2::SDL2_image)
    set_target_properties(${SAMPLE_NAME} PROPERTIES CXX_STANDARD 17)
    set_target_properties(${SAMPLE_NAME} PROPERTIES CXX_STANDARD_REQUIRED ON)

    if (BUILD_SIMD_SAMPLE)
        set(SAMPLE_NAME "simd_${SAMPLE_NAME_SANITIZED}")
        add_executable(${SAMPLE_NAME} ${FILES})
        target_include_directories(${SAMPLE_NAME} PRIVATE ${CxxSwizzle_SOURCE_DIR}/include ${GEN_INCLUDE_DIR} ${Vc_INCLUDE_DIR})
        target_link_libraries(${SAMPLE_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main SDL2::SDL2_image ${Vc_LIBRARIES})
        set_target_properties(${SAMPLE_NAME} PROPERTIES CXX_STANDARD 17)
        set_target_properties(${SAMPLE_NAME} PROPERTIES CXX_STANDARD_REQUIRED ON)
        add_dependencies(${SAMPLE_NAME} Vc)
        set_target_properties(${SAMPLE_NAME} PROPERTIES COMPILE_FLAGS "-DSAMPLE_USE_SIMD_VC")
    endif()

    #undo variables
    foreach(SHADER ${SHADERS})
        get_filename_component(SHADER_NAME_SANITIZED ${SHADER} NAME_WE)
        string(TOUPPER ${SHADER_NAME_SANITIZED} SHADER_NAME_SANITIZED)
        unset(SAMPLE_HAS_${SHADER_NAME_SANITIZED})
    endforeach()		



endforeach()

