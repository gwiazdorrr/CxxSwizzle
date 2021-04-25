# CxxSwizzle
# Copyright (c) 2013-2021, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
cmake_minimum_required(VERSION 3.3)

macro(_cxxswizzles_find_structs path result_var)
    file(READ ${path} contents)
    string(REGEX MATCHALL "struct[ \t\r\r\n]+[a-zA-Z0-9_]*" structs "${contents}")

    set(result "")
    foreach (struct ${structs})
        string(REGEX REPLACE "^struct[ \t\r\n]+" "" struct ${struct})
        list(APPEND result ${struct})
    endforeach()

    list(REMOVE_DUPLICATES result)
    set(${result_var} ${result})
endmacro()

macro(_cxxswizzle_emit_structs structs result_var)
    set(result "")
    foreach(struct ${structs})
        set(result "${result}        struct ${struct};\n")
    endforeach()
    set(result     "${result}        struct inout_proxy : builtin_inout_proxy\n")
    set(result     "${result}        {\n")
    foreach(struct ${structs})
        set(result "${result}            using ${struct} = _cxxswizzle_fragment_shader::${struct}&;\n")
    endforeach()
    set(result     "${result}        };\n")
    foreach(struct ${structs})
        set(result "${result}        #define ${struct}(...) _cxxswizzle_make<${struct}>( __VA_ARGS__ ) \n")
    endforeach()
    set(${result_var} ${result})
endmacro()

macro(cxxswizzle_create_sandbox template_dir target_name shadertoy_dir sandbox_type_override custom_structs_override textures_root_override)

    # basic requirements
    find_package(SDL2 CONFIG REQUIRED)
    find_package(SDL2-image CONFIG REQUIRED)
    find_package(nlohmann_json CONFIG REQUIRED)
    find_package(OpenMP)

    file(GLOB shader_file_list "${shadertoy_dir}/*.frag")
    
    set(gen_include_dir "${CMAKE_CURRENT_BINARY_DIR}/${target_name}_gen")

    set(custom_structs ${CXXSWIZZLE_SANDBOX_STRUCTS})
    if (NOT "${custom_structs_override}" STREQUAL "")
        set(custom_structs ${custom_structs_override})
    endif()


    set(sandbox_type ${CXXSWIZZLE_SANDBOX_TYPE})
    if (NOT "${sandbox_type_override}" STREQUAL "")
        set(sandbox_type ${sandbox_type_override})
    endif()

    set(textures_root ${CXXSWIZZLE_SANDBOX_TEXTURES_ROOT})
    if (NOT "${textures_root_override}" STREQUAL "")
        set(textures_root ${textures_root_override})
    endif()


    # codegen for custom strcuts
    # set(CONFIG_FRAG_CUSTOM_STRUCTS_SUPPORT "")
    # foreach(STRUCT ${custom_structs})
    #     set(CONFIG_FRAG_CUSTOM_STRUCTS_SUPPORT "${CONFIG_FRAG_CUSTOM_STRUCTS_SUPPORT}        struct ${STRUCT};\n")
    # endforeach()
    # set(CONFIG_FRAG_CUSTOM_STRUCTS_SUPPORT     "${CONFIG_FRAG_CUSTOM_STRUCTS_SUPPORT}        struct inout_proxy : builtin_inout_proxy\n")
    # set(CONFIG_FRAG_CUSTOM_STRUCTS_SUPPORT     "${CONFIG_FRAG_CUSTOM_STRUCTS_SUPPORT}        {\n")
    # foreach(STRUCT ${custom_structs})
    #     set(CONFIG_FRAG_CUSTOM_STRUCTS_SUPPORT "${CONFIG_FRAG_CUSTOM_STRUCTS_SUPPORT}            using ${STRUCT} = _cxxswizzle_fragment_shader::${STRUCT}&;\n")
    # endforeach()
    # set(CONFIG_FRAG_CUSTOM_STRUCTS_SUPPORT     "${CONFIG_FRAG_CUSTOM_STRUCTS_SUPPORT}        };\n")
    # foreach(STRUCT ${custom_structs})
    #     set(CONFIG_FRAG_CUSTOM_STRUCTS_SUPPORT "${CONFIG_FRAG_CUSTOM_STRUCTS_SUPPORT}        #define ${STRUCT}(...) ${STRUCT} { __VA_ARGS__ } \n")
    # endforeach()

    # find common
    unset(CONFIG_FRAG_COMMON_INCLUDE)
    foreach(shader_file ${shader_file_list})
        get_filename_component(shader_name ${shader_file} NAME_WE)
        string(TOLOWER ${shader_name} shader_name)
        if (shader_name STREQUAL "common")
            message(VERBOSE "* found common include: ${shader_file}")
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

    set(common_structs)
    foreach(shader_file ${shader_file_list})
        get_filename_component(shader_name ${shader_file} NAME_WE)
        string(TOLOWER ${shader_name} shader_name)
        if (shader_name STREQUAL "common")
            _cxxswizzles_find_structs(${shader_file} common_structs)
        endif()
    endforeach()

    foreach(shader_file ${shader_file_list})
        get_filename_component(shader_name ${shader_file} NAME_WE)
        string(TOLOWER ${shader_name} shader_name)
        if (shader_name STREQUAL "common")
            
        elseif(shader_name IN_LIST valid_shader_names)

            _cxxswizzles_find_structs(${shader_file} structs)
            list(APPEND structs ${common_structs})

            _cxxswizzle_emit_structs("${structs}" CONFIG_FRAG_CUSTOM_STRUCTS_SUPPORT)

            message(VERBOSE "* found pass: ${shader_file}") 
            set(gen_sandbox_path "${gen_include_dir}/shadertoy_sandbox_${shader_name}.cpp")
            set(CONFIG_FRAG_PATH "${shader_file}")
            set(CONFIG_FRAG_NAME "${shader_name}")
            configure_file("${template_dir}/shadertoy_sandbox.cpp.in" "${gen_sandbox_path}")
            list(APPEND codegen_file_list ${gen_sandbox_path})
            string(TOUPPER ${shader_name} shader_name)
            set(CONFIG_SAMPLE_HAS_${shader_name} True)    
        else()
            message(VERBOSE "Pass name not supported: ${shader_name}, not going to get included.")
        endif()
    endforeach()

    set(config_json_path "${shadertoy_dir}/config.json")
    if (EXISTS ${config_json_path})
        file(RELATIVE_PATH CONFIG_SAMPLE_CONFIG_PATH ${CMAKE_CURRENT_BINARY_DIR} ${config_json_path})
    else()
        set(CONFIG_SAMPLE_CONFIG_PATH "${config_json_path}")
    endif()

    if (EXISTS ${textures_root})
        file(RELATIVE_PATH CONFIG_SAMPLE_TEXTURES_ROOT ${CMAKE_CURRENT_BINARY_DIR} ${textures_root})
    else()
        set(CONFIG_SAMPLE_TEXTURES_ROOT "${textures_root}")
    endif()

    set(CONFIG_SAMPLE_SETUP_INCLUDE "swizzle/setup_${sandbox_type}.hpp")
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

    if (${sandbox_type} STREQUAL "simd_vc" OR ${sandbox_type} STREQUAL "simd_vc_with_masking")
        find_package(Vc CONFIG REQUIRED)
        # find_package(Vc CONFIG REQUIRED) seems to disable this policy somehow, hence it is enabled here
        cmake_policy(SET CMP0057 NEW)
        target_link_libraries(${target_name} PRIVATE Vc::Vc)
    endif()

    if (OPENMP_FOUND)
        target_link_libraries(${target_name} PRIVATE OpenMP::OpenMP_CXX)
    endif()

    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # filesystem support
        target_link_libraries(${target_name} PRIVATE stdc++fs)
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

    if (NOT ${CXXSWIZZLE_SANDBOX_TRACY_PROFILER_ROOT} STREQUAL "")
        source_group("shared" FILES "${tracy_profiler_root}/TracyClient.cpp")
        target_sources(${target_name} PRIVATE "${tracy_profiler_root}/TracyClient.cpp")
        target_include_directories(${target_name} PRIVATE ${tracy_profiler_root})
        target_compile_options(${target_name} PRIVATE "-DTRACY_ENABLE")
    endif()

endmacro()