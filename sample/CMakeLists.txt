# CxxSwizzle
# Copyright (c) 2013-2021, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
cmake_minimum_required(VERSION 3.3)

include(CheckCXXCompilerFlag)

set(SAMPLE_TRACY_PROFILER_ROOT "" CACHE PATH "Path to the Tracy Profiler root. Leave empty if you don't want to use it.")

# handle SIMD type
set(SAMPLE_SETUP "scalar" CACHE STRING "SIMD type")
set_property(CACHE SAMPLE_SETUP PROPERTY STRINGS scalar simd_naive simd_vc simd_vc_with_masking)

# handle custom structs
set(SAMPLE_CUSTOM_STRUCTS Ray;ray CACHE STRING "Custom structs that need function-style init or out/inout modifier")

# global shaders
set(SAMPLE_SHADERTOYS_ROOT "${PROJECT_SOURCE_DIR}/shadertoys" CACHE STRING "Shadertoys root")

macro(create_sample_from_template template_dir target_name shadertoy_dir custom_structs setup_type tracy_profiler_root)

    # basic requirements
    find_package(SDL2 CONFIG REQUIRED)
    find_package(SDL2-image CONFIG REQUIRED)
    find_package(nlohmann_json CONFIG REQUIRED)
    find_package(OpenMP)

    file(GLOB shader_file_list "${shadertoy_dir}/*.frag")
    
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
            set(CONFIG_FRAG_COMMON_INCLUDE "${shader_file}")
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
            set(CONFIG_FRAG_PATH "${shader_file}")
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

    set(CONFIG_SAMPLE_SETUP_INCLUDE "swizzle/setup_${setup_type}.hpp")
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

    add_executable(${target_name} ${shared_file_list} ${codegen_file_list} ${shader_file_list} ${config_json_path})
    
    target_include_directories(${target_name} PRIVATE ${CxxSwizzle_SOURCE_DIR}/include ${gen_include_dir} ${template_dir} ${shadertoy_dir})
    target_link_libraries(${target_name} PRIVATE SDL2::SDL2 SDL2::SDL2main SDL2::SDL2_image nlohmann_json::nlohmann_json)
    set_target_properties(${target_name} PROPERTIES CXX_STANDARD 17)
    set_target_properties(${target_name} PROPERTIES CXX_STANDARD_REQUIRED ON)

    if (${setup_type} STREQUAL "simd_vc" OR ${setup_type} STREQUAL "simd_vc_with_masking")
        find_package(Vc CONFIG REQUIRED)
        # find_package(Vc CONFIG REQUIRED) seems to disable this policy somehow, hence it is enabled here
        cmake_policy(SET CMP0057 NEW)
        target_link_libraries(${target_name} PRIVATE Vc::Vc)
    endif()

    if (OPENMP_FOUND)
        target_link_libraries(${target_name} PRIVATE OpenMP::OpenMP_CXX)
    endif()


    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        # GLSL has "not" function, so better get rid of C++ operator mnemonics
        target_compile_options(${target_name} PRIVATE "-fno-operator-names")
    elseif(MSVC)
        # fast math, _vectorcall
        # the resons permissive can't be disabled in MSVC, because there's no way to disable mnemonics then
        target_compile_options(${target_name} PRIVATE "/fp:fast"
                                                      "/Gv"
                                                      "-D_ENABLE_EXTENDED_ALIGNED_STORAGE"
                                                      "-D_CRT_SECURE_NO_WARNINGS")
    endif()

    if (NOT ${tracy_profiler_root} STREQUAL "")
        source_group("shared" FILES "${tracy_profiler_root}/TracyClient.cpp")
        target_sources(${target_name} PRIVATE "${tracy_profiler_root}/TracyClient.cpp")
        target_include_directories(${target_name} PRIVATE ${tracy_profiler_root})
        target_compile_options(${target_name} PRIVATE "-DTRACY_ENABLE")
    endif()

endmacro()


file(GLOB shadertoy_dirs ${SAMPLE_SHADERTOYS_ROOT}/*)
foreach(shadertoy_dir ${shadertoy_dirs})
    if(IS_DIRECTORY ${shadertoy_dir})

        get_filename_component(shadertoy_name ${shadertoy_dir} NAME_WE)

        message(STATUS "Sample ${shadertoy_name} ${shadertoy_dir}")

        set(target_name "shadertoy_${shadertoy_name}")
        create_sample_from_template("${CMAKE_CURRENT_SOURCE_DIR}" ${target_name} "${shadertoy_dir}" "${SAMPLE_CUSTOM_STRUCTS}" "${SAMPLE_SETUP}" "${SAMPLE_TRACY_PROFILER_ROOT}")
    endif()
endforeach()