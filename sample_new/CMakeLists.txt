# CxxSwizzle
# Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
cmake_minimum_required(VERSION 3.3)

include(CheckCXXCompilerFlag)

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

# basic requirements
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2-image CONFIG REQUIRED)

# other compiler required options
set(TARGET_ADDITIONAL_FILES "")
set(TARGET_ADDITIONAL_FLAGS "")
set(TARGET_ADDITIONAL_LIBS "")

# handle OpenMP
option(SAMPLE_USE_OPENMP "Use OpenMP" ON)
if (SAMPLE_USE_OPENMP)
    find_package(OpenMP)
    if (OPENMP_FOUND)
        list(APPEND TARGET_ADDITIONAL_FLAGS ${OpenMP_CXX_FLAGS})
        list(APPEND TARGET_ADDITIONAL_FLAGS "-DSAMPLE_OMP_ENABLED")
    endif()
endif()

# handle -march flag (otherwise g++ fails its GitHub Action)
option(SAMPLE_OPTIMIZE_FOR_NATIVE "Build with -march=native (if compiler supports it)" ON)
if (SAMPLE_OPTIMIZE_FOR_NATIVE)
    CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
    if(COMPILER_SUPPORTS_MARCH_NATIVE)
        list(APPEND TARGET_ADDITIONAL_FLAGS "-march=native")
    endif()
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # GLSL has "not" function, so better get rid of C++ operator mnemonics
    list(APPEND TARGET_ADDITIONAL_FLAGS "-fno-operator-names")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        list(APPEND TARGET_ADDITIONAL_LIBS "stdc++fs")
    endif()
elseif(MSVC)
    # fast math, _vectorcall
    # the resons permissive can't be disabled in MSVC, because there's no way to disable mnemonics then
    list(APPEND TARGET_ADDITIONAL_FLAGS "/fp:fast")
    list(APPEND TARGET_ADDITIONAL_FLAGS "/Gv")
    list(APPEND TARGET_ADDITIONAL_FLAGS "-D_ENABLE_EXTENDED_ALIGNED_STORAGE")
    # Add debug visualisers (work great!)	
    list(APPEND TARGET_ADDITIONAL_FILES "cxxswizzle.natvis")
endif()


# handle SIMD type
set(SAMPLE_SIMD_TYPE "none" CACHE STRING "SIMD type")
set_property(CACHE SAMPLE_SIMD_TYPE PROPERTY STRINGS none naive VcSIMD)
if (SAMPLE_SIMD_TYPE STREQUAL "none")
    # nothing to do, default is scalar
elseif (SAMPLE_SIMD_TYPE STREQUAL "VcSIMD")
    find_package(Vc CONFIG REQUIRED)
    # find_package(Vc CONFIG REQUIRED) seems to disable this policy somehow, hence it is enabled here
    cmake_policy(SET CMP0057 NEW)
    list(APPEND TARGET_ADDITIONAL_LIBS Vc::Vc)
    list(APPEND TARGET_ADDITIONAL_FLAGS "-DSAMPLE_USE_SIMD_VC")
endif()


# handle custom structs
set(SAMPLE_CUSTOM_STRUCTS Ray;ray CACHE STRING "Custom structs that need function-style init or out/inout modifier")
set(FRAG_CUSTOM_STRUCTS_SUPPORT "")
foreach(STRUCT ${SAMPLE_CUSTOM_STRUCTS})
    set(FRAG_CUSTOM_STRUCTS_SUPPORT "${FRAG_CUSTOM_STRUCTS_SUPPORT}        struct ${STRUCT};\n")
endforeach()
set(FRAG_CUSTOM_STRUCTS_SUPPORT     "${FRAG_CUSTOM_STRUCTS_SUPPORT}        struct inout_proxy : builtin_inout_proxy\n")
set(FRAG_CUSTOM_STRUCTS_SUPPORT     "${FRAG_CUSTOM_STRUCTS_SUPPORT}        {\n")
foreach(STRUCT ${SAMPLE_CUSTOM_STRUCTS})
    set(FRAG_CUSTOM_STRUCTS_SUPPORT "${FRAG_CUSTOM_STRUCTS_SUPPORT}            using ${STRUCT} = _cxxswizzle_fragment_shader::${STRUCT}&;\n")
endforeach()
set(FRAG_CUSTOM_STRUCTS_SUPPORT     "${FRAG_CUSTOM_STRUCTS_SUPPORT}        };\n")
foreach(STRUCT ${SAMPLE_CUSTOM_STRUCTS})
    set(FRAG_CUSTOM_STRUCTS_SUPPORT "${FRAG_CUSTOM_STRUCTS_SUPPORT}        #define ${STRUCT}(...) ${STRUCT}\\{ __VA_ARGS__ \\}\n")
endforeach()


# global shaders
set(SAMPLE_SHADERTOYS_ROOT "${PROJECT_SOURCE_DIR}/shadertoys" CACHE STRING "Shadertoys root")
get_subdir_list(SAMPLES_DIRS ${SAMPLE_SHADERTOYS_ROOT})
foreach(SAMPLE_DIR ${SAMPLES_DIRS})
    message(STATUS "Sample ${SAMPLE_DIR}")

    file(GLOB SHADERS  RELATIVE "${SAMPLE_SHADERTOYS_ROOT}" "${SAMPLE_SHADERTOYS_ROOT}/${SAMPLE_DIR}/*.frag")
    file(GLOB TEXTURES RELATIVE "${SAMPLE_SHADERTOYS_ROOT}" "${SAMPLE_SHADERTOYS_ROOT}/${SAMPLE_DIR}/*.png" "${SAMPLE_SHADERTOYS_ROOT}/${SAMPLE_DIR}/*.jpg")
    file(GLOB BUFFERS  RELATIVE "${SAMPLE_SHADERTOYS_ROOT}" "${SAMPLE_SHADERTOYS_ROOT}/${SAMPLE_DIR}/*.txt")

    get_filename_component(SAMPLE_NAME ${SAMPLE_DIR} NAME_WE)

    set(GEN_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/${SAMPLE_NAME}_gen")
    set(GEN_CONFIG_PATH "${GEN_INCLUDE_DIR}/config.hpp")

    # set the config default values
    foreach(PREFIX SAMPLE SAMPLE_BUFFER_A SAMPLE_BUFFER_B SAMPLE_BUFFER_C SAMPLE_BUFFER_D)
        foreach(INDEX 0 1 2 3)
            set(${PREFIX}_ICHANNEL_${INDEX}_PATH "\\0")
        endforeach()
    endforeach()

    unset(FRAG_COMMON_INCLUDE)
    unset(FRAG_HAS_COMMON_INCLUDE)
    foreach(SHADER ${SHADERS})
        get_filename_component(SHADER_NAME_SANITIZED ${SHADER} NAME_WE)
        string(TOLOWER ${SHADER_NAME_SANITIZED} SHADER_NAME_SANITIZED_LOWER)
        if (SHADER_NAME_SANITIZED STREQUAL "common")
            message(STATUS "* found common include: ${SHADER}")
            set(FRAG_COMMON_INCLUDE "${SAMPLE_SHADERTOYS_ROOT}/${SHADER}")
            set(FRAG_HAS_COMMON_INCLUDE True)
            break()
        endif()
    endforeach()

    # defines will be needed
    set(GEN_SHADERS)

    set(VALID_PASS_NAMES "image" "buffer_a" "buffer_b" "buffer_c" "buffer_d" "common")
    foreach(SHADER ${SHADERS})
        get_filename_component(SHADER_NAME_SANITIZED ${SHADER} NAME_WE)
        string(TOLOWER ${SHADER_NAME_SANITIZED} SHADER_NAME_SANITIZED_LOWER)
        if (SHADER_NAME_SANITIZED STREQUAL "common")
            # ignore
        elseif(SHADER_NAME_SANITIZED IN_LIST VALID_PASS_NAMES)
            message(STATUS "* found pass: ${SHADER}") 
            set(GEN_SANDBOX_PATH "${GEN_INCLUDE_DIR}/shadertoy_sandbox_${SHADER_NAME_SANITIZED}.cpp")
            set(FRAG_PATH "${SAMPLE_SHADERTOYS_ROOT}/${SHADER}")
            set(FRAG_NAME "${SHADER_NAME_SANITIZED}")
            configure_file("shadertoy_sandbox.cpp.in" "${GEN_SANDBOX_PATH}")
            list(APPEND GEN_SHADERS ${GEN_SANDBOX_PATH})
            string(TOUPPER ${SHADER_NAME_SANITIZED} SHADER_NAME_SANITIZED)
            set(SAMPLE_HAS_${SHADER_NAME_SANITIZED} True)    
        else()
            message(WARNING "Pass name not recognised: ${SHADER_NAME_SANITIZED}, not going to get included.")
        endif()
    endforeach()

    foreach (TEXTURE ${TEXTURES})
        get_filename_component(CHANNEL_NAME ${TEXTURE} NAME_WE)
        message(STATUS "* found texture: ${TEXTURE}") 
        string(TOUPPER ${CHANNEL_NAME} CHANNEL_NAME)
        set(SAMPLE_${CHANNEL_NAME}_PATH "${SAMPLE_SHADERTOYS_ROOT}/${TEXTURE}")
    endforeach()

    foreach (BUFFER ${BUFFERS})
        file(READ "${SAMPLE_SHADERTOYS_ROOT}/${BUFFER}" BUFFER_ID)
        message(STATUS "* found special texture: ${BUFFER} (points to ${BUFFER_ID})") 
        get_filename_component(CHANNEL_NAME ${BUFFER} NAME_WE)
        string(TOUPPER ${CHANNEL_NAME} CHANNEL_NAME)
        set(SAMPLE_${CHANNEL_NAME}_PATH "${BUFFER_ID}")
    endforeach()

    configure_file("config.hpp.in" "${GEN_CONFIG_PATH}")

    set(CODEGEN_TEMPLATE_FILES "shadertoy_sandbox.cpp.in" "config.hpp.in")
    set(README_PATH "${SAMPLE_DIR}/README.md")

    set(TARGET_FILES main.cpp ${GEN_SHADERS} shadertoy_sandbox.hpp ${SHADERS} ${GEN_CONFIG_PATH} ${TARGET_ADDITIONAL_FILES} ${CODEGEN_TEMPLATE_FILES})
    if (EXISTS "${SAMPLE_SHADERTOYS_ROOT}/${README_PATH}")
        list(APPEND TARGET_FILES ${README_PATH})
    endif()

    source_group("" FILES ${SHADERS} ${README_PATH})
    source_group("codegen" FILES ${GEN_SHADERS} ${GEN_CONFIG_PATH} )
    source_group("shared" FILES CMakeList.txt main.cpp shadertoy_sandbox.hpp ${TARGET_ADDITIONAL_FILES} ${CODEGEN_TEMPLATE_FILES})

    set(TARGET_NAME "shadertoy_${SAMPLE_NAME}")
    add_executable(${TARGET_NAME} ${TARGET_FILES})
    target_include_directories(${TARGET_NAME} PRIVATE ${CxxSwizzle_SOURCE_DIR}/include ${GEN_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(${TARGET_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main SDL2::SDL2_image ${TARGET_ADDITIONAL_LIBS})
    set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17)
    set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD_REQUIRED ON)
    target_compile_options(${TARGET_NAME} PRIVATE ${TARGET_ADDITIONAL_FLAGS})

    # undo variables
    foreach(SHADER ${SHADERS})
        get_filename_component(SHADER_NAME_SANITIZED ${SHADER} NAME_WE)
        string(TOUPPER ${SHADER_NAME_SANITIZED} SHADER_NAME_SANITIZED)
        unset(SAMPLE_HAS_${SHADER_NAME_SANITIZED})
    endforeach()		
endforeach()