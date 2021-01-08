# CxxSwizzle
# Copyright (c) 2013-2021, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
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
find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenMP)

# other compiler required options
set(TARGET_ADDITIONAL_FLAGS "")
set(TARGET_ADDITIONAL_LIBS "")

if (OPENMP_FOUND)
    list(APPEND TARGET_ADDITIONAL_LIBS OpenMP::OpenMP_CXX)
endif()


set(SAMPLE_TRACY_PROFILER_ROOT "" CACHE PATH "Path to the Tracy Profiler root. Leave empty if you don't want to use it.")

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
elseif(MSVC)
    # fast math, _vectorcall
    # the resons permissive can't be disabled in MSVC, because there's no way to disable mnemonics then
    list(APPEND TARGET_ADDITIONAL_FLAGS "/fp:fast")
    list(APPEND TARGET_ADDITIONAL_FLAGS "/Gv")
    list(APPEND TARGET_ADDITIONAL_FLAGS "-D_ENABLE_EXTENDED_ALIGNED_STORAGE")
    list(APPEND TARGET_ADDITIONAL_FLAGS "-D_CRT_SECURE_NO_WARNINGS")
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


# global shaders
set(SAMPLE_SHADERTOYS_ROOT "${PROJECT_SOURCE_DIR}/shadertoys" CACHE STRING "Shadertoys root")
get_subdir_list(SAMPLES_DIRS ${SAMPLE_SHADERTOYS_ROOT})

macro(create_sample_from_template template_dir target_name shadertoy_dir custom_structs)

    file(GLOB shader_file_list RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${shadertoy_dir}/*.frag")
    
    set(gen_include_dir "${CMAKE_CURRENT_BINARY_DIR}/${target_name}_gen")

    # codegen for custom strcuts
    set(CONFIG_FRAG_CUSTOM_STRUCTS_SUPPORT "")
    foreach(STRUCT ${custom_structs})
        set(CONFIG_FRAG_CUSTOM_STRUCTS_SUPPORT "${CONFIG_FRAG_CUSTOM_STRUCTS_SUPPORT}        struct ${STRUCT};\n")
    endforeach()
    set(CONFIG_FRAG_CUSTOM_STRUCTS_SUPPORT     "${CONFIG_FRAG_CUSTOM_STRUCTS_SUPPORT}        struct inout_proxy : builtin_inout_proxy\n")
    set(CONFIG_FRAG_CUSTOM_STRUCTS_SUPPORT     "${CONFIG_FRAG_CUSTOM_STRUCTS_SUPPORT}        {\n")
    foreach(STRUCT ${custom_structs})
        set(CONFIG_FRAG_CUSTOM_STRUCTS_SUPPORT "${CONFIG_FRAG_CUSTOM_STRUCTS_SUPPORT}            using ${STRUCT} = _cxxswizzle_fragment_shader::${STRUCT}&;\n")
    endforeach()
    set(CONFIG_FRAG_CUSTOM_STRUCTS_SUPPORT     "${CONFIG_FRAG_CUSTOM_STRUCTS_SUPPORT}        };\n")
    foreach(STRUCT ${custom_structs})
        set(CONFIG_FRAG_CUSTOM_STRUCTS_SUPPORT "${CONFIG_FRAG_CUSTOM_STRUCTS_SUPPORT}        #define ${STRUCT}(...) ${STRUCT} { __VA_ARGS__ } \n")
    endforeach()

    # find common
    unset(CONFIG_FRAG_COMMON_INCLUDE)
    foreach(shader_file ${shader_file_list})
        get_filename_component(shader_name ${shader_file} NAME_WE)
        string(TOLOWER ${shader_name} shader_name)
        if (shader_name STREQUAL "common")
            message(STATUS "* found common include: ${shader_file}")
            set(CONFIG_FRAG_COMMON_INCLUDE "${SAMPLE_SHADERTOYS_ROOT}/${shader_file}")
            break()
        endif()
    endforeach()

    # handle shaders
    set(codegen_file_list)

    set(valid_shader_names "common" "image" "buffer_a" "buffer_b" "buffer_c" "buffer_d" )
    foreach(shader_name ${valid_shader_names})
        string(TOUPPER ${shader_name} shader_name)
        unset(CONFIG_SAMPLE_HAS_${shader_name})
    endforeach()    

    foreach(shader_file ${shader_file_list})
        get_filename_component(shader_name ${shader_file} NAME_WE)
        string(TOLOWER ${shader_name} shader_name)
        if (shader_name STREQUAL "common")
            # ignore
        elseif(shader_name IN_LIST valid_shader_names)
            message(STATUS "* found pass: ${shader_file}") 
            set(gen_sandbox_path "${gen_include_dir}/shadertoy_sandbox_${shader_name}.cpp")
            set(CONFIG_FRAG_PATH "${SAMPLE_SHADERTOYS_ROOT}/${shader_file}")
            set(CONFIG_FRAG_NAME "${shader_name}")
            configure_file("shadertoy_sandbox.cpp.in" "${gen_sandbox_path}")
            list(APPEND codegen_file_list ${gen_sandbox_path})
            string(TOUPPER ${shader_name} shader_name)
            set(CONFIG_SAMPLE_HAS_${shader_name} True)    
        else()
            message(WARNING "Pass name not recognised: ${shader_name}, not going to get included.")
        endif()
    endforeach()

    set(config_json_path "${shadertoy_dir}/config.json")
    if (EXISTS ${config_json_path})
        set(CONFIG_SAMPLE_CONFIG_PATH ${config_json_path})
    else()
        unset(CONFIG_SAMPLE_CONFIG_PATH)
        set(config_json_path)
    endif()

    set(codegen_config_path "${gen_include_dir}/config.hpp")
    configure_file("${template_dir}/config.hpp.in" "${codegen_config_path}")

    list(APPEND codegen_file_list ${codegen_config_path})

    set(shared_file_list "${template_dir}/main.cpp" 
                         "${template_dir}/shadertoy_sandbox.hpp"
                         "${template_dir}/shadertoy_sandbox.cpp.in" 
                         "${template_dir}/config.hpp.in"
                         "${template_dir}/cxxswizzle.natvis")


    source_group("" FILES ${shader_file_list} ${config_json_path})
    source_group("codegen" FILES ${codegen_file_list} )
    source_group("shared" FILES ${shared_file_list} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt" REGULAR_EXPRESSION "\\.natvis")

    add_executable(${target_name} ${shared_file_list} ${codegen_file_list}  ${shader_file_list} ${config_json_path})
    
    target_include_directories(${target_name} PRIVATE ${CxxSwizzle_SOURCE_DIR}/include ${gen_include_dir} ${template_dir})
    target_link_libraries(${target_name} PRIVATE SDL2::SDL2 SDL2::SDL2main SDL2::SDL2_image nlohmann_json::nlohmann_json ${TARGET_ADDITIONAL_LIBS})
    set_target_properties(${target_name} PROPERTIES CXX_STANDARD 17)
    set_target_properties(${target_name} PROPERTIES CXX_STANDARD_REQUIRED ON)
    target_compile_options(${target_name} PRIVATE ${TARGET_ADDITIONAL_FLAGS})

    if (NOT SAMPLE_TRACY_PROFILER_ROOT STREQUAL "")
        source_group("shared" FILES   "${SAMPLE_TRACY_PROFILER_ROOT}/TracyClient.cpp")
        target_sources(${target_name} PRIVATE "${SAMPLE_TRACY_PROFILER_ROOT}/TracyClient.cpp")
        target_include_directories(${target_name} PRIVATE ${SAMPLE_TRACY_PROFILER_ROOT})
        target_compile_options(${target_name} PRIVATE "-DTRACY_ENABLE")
    endif()

endmacro()

foreach(SAMPLE_DIR ${SAMPLES_DIRS})
    message(STATUS "Sample ${SAMPLE_DIR}")

    get_filename_component(shadertoy_dir ${SAMPLE_DIR} NAME_WE)
    set(target_name "shadertoy_${shadertoy_dir}")
    create_sample_from_template("${CMAKE_CURRENT_SOURCE_DIR}" ${target_name} "${SAMPLE_SHADERTOYS_ROOT}/${SAMPLE_DIR}" "${SAMPLE_CUSTOM_STRUCTS}")

endforeach()