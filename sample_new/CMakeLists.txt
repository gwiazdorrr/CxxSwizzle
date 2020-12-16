# CxxSwizzle
# Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
cmake_minimum_required(VERSION 3.3)
cmake_policy(SET CMP0057 NEW)

include(JSONParser.cmake)

macro(sanitize_name result name)
    string(TOLOWER ${name} ${result})
    string(REPLACE " "  "_" ${result} ${${result}})
    string(REPLACE "&"  "and" ${result} ${${result}})
    string(REPLACE "["  "_" ${result} ${${result}})
    string(REPLACE "]"  "_" ${result} ${${result}})
    string(REPLACE "("  "_" ${result} ${${result}})
    string(REPLACE ")"  "_" ${result} ${${result}})
    string(REPLACE "!"  "_" ${result} ${${result}})
    string(REPLACE "'"  "_" ${result} ${${result}})
    string(REPLACE "\\" "_" ${result} ${${result}})
    string(REPLACE "/"  "_" ${result} ${${result}})
    string(REPLACE ":"  "_" ${result} ${${result}})
    string(REPLACE "*"  "_" ${result} ${${result}})
    string(REPLACE "?"  "_" ${result} ${${result}})
    string(REPLACE "\"" "_" ${result} ${${result}})
    string(REPLACE "<"  "_" ${result} ${${result}})
    string(REPLACE ">"  "_" ${result} ${${result}})
    string(REPLACE "|"  "_" ${result} ${${result}})
endmacro()

macro(create_file path contents msg)
    message(STATUS "Creating ${path} ${msg}")
    file(WRITE ${path} "${contents}")
endmacro()

macro(download_file url path msg)
    message(STATUS "Downloading ${url} to ${path} ${msg}")
    file(DOWNLOAD ${url} ${path})
endmacro()

macro(download_shadertoy shader_id api_key)
    set(SHADERTOY_JSON_PATH "${CMAKE_CURRENT_BINARY_DIR}/${shader_id}.json")
    set(SHADERTOY_QUERY "https://www.shadertoy.com/api/v1/shaders/${shader_id}?key=${api_key}")
        
    download_file(${SHADERTOY_QUERY} ${SHADERTOY_JSON_PATH} "(Shadertoy JSON shader description)")

    message(STATUS "Parsing ${SHADERTOY_JSON_PATH}")
    file(READ "${SHADERTOY_JSON_PATH}" SHADERTOY_JSON)
    sbeParseJson(SHADER SHADERTOY_JSON)

    if (SHADER.Error)
        if (SHADER.Error STREQUAL "Shader not found")
            message(FATAL_ERROR "Shartoy API returned an error: ${SHADER.Error}.\nThis happens if shader is not published "
            "as public+api. Anyway, this shader needs to be downloaded manually (see README.md for instructions)")
        else()
            message(FATAL_ERROR "Shartoy API returned an error: ${SHADER.Error}.")
        endif()
    endif()

    # debug
    # foreach(var ${SHADER})
    #     message("${var} = ${${var}}") 
    # endforeach()

    set(SHADER_NAME ${SHADER.Shader.info.name})

    # create directory
    sanitize_name(SHADER_NAME_SANITIZED ${SHADER_NAME})
    
    set(SHADER_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_NAME_SANITIZED}")

    file(MAKE_DIRECTORY ${SHADER_DIRECTORY})
    create_file("${SHADER_DIRECTORY}/README.md" "https://www.shadertoy.com/view/${shader_id}" "" )

    foreach(r ${SHADER.Shader.renderpass})
        set(RENDERPASS SHADER.Shader.renderpass_${r})
            
        if (${RENDERPASS}.name)
            sanitize_name(RENDER_PASS_NAME ${${RENDERPASS}.name})
        else()
            # seems this fallback is needed sometimes
            sanitize_name(RENDER_PASS_NAME ${${RENDERPASS}.type})
        endif()

        string(REPLACE "buf_" "buffer_" RENDER_PASS_NAME ${RENDER_PASS_NAME})

        message(STATUS "Creating ${SHADER_DIRECTORY}/${RENDER_PASS_NAME}.frag (Shader pass)")
        file(WRITE "${SHADER_DIRECTORY}/${RENDER_PASS_NAME}.frag" "${${RENDERPASS}.code}")
            
        set(INPUT_PREFIX "${RENDER_PASS_NAME}_")
        if(RENDER_PASS_NAME STREQUAL "image")
            set(INPUT_PREFIX "")
        endif()


        foreach(i ${${RENDERPASS}.inputs})
            set(INPUT ${RENDERPASS}.inputs_${i})
            set(INPUT_INDEX ${${INPUT}.channel})
            if (${INPUT}.ctype STREQUAL "texture")
                get_filename_component(INPUT_EXTENSION ${${INPUT}.src} EXT)
                download_file("https://www.shadertoy.com${${INPUT}.src}" "${SHADER_DIRECTORY}/${INPUT_PREFIX}ichannel_${${INPUT}.channel}${INPUT_EXTENSION}" "")
            elseif (${INPUT}.ctype STREQUAL "cubemap")
                get_filename_component(INPUT_EXTENSION ${${INPUT}.src} EXT)
                get_filename_component(INPUT_NAME      ${${INPUT}.src} NAME_WE)
                get_filename_component(INPUT_DIR       ${${INPUT}.src} DIRECTORY)
                download_file("https://www.shadertoy.com${${INPUT}.src}" "${SHADER_DIRECTORY}/${INPUT_PREFIX}ichannel_${${INPUT}.channel}${INPUT_EXTENSION}" "")
                download_file("https://www.shadertoy.com${INPUT_DIR}/${INPUT_NAME}_1${INPUT_EXTENSION}" "${SHADER_DIRECTORY}/${INPUT_PREFIX}ichannel_${${INPUT}.channel}_1${INPUT_EXTENSION}" "")
                download_file("https://www.shadertoy.com${INPUT_DIR}/${INPUT_NAME}_2${INPUT_EXTENSION}" "${SHADER_DIRECTORY}/${INPUT_PREFIX}ichannel_${${INPUT}.channel}_2${INPUT_EXTENSION}" "")
                download_file("https://www.shadertoy.com${INPUT_DIR}/${INPUT_NAME}_3${INPUT_EXTENSION}" "${SHADER_DIRECTORY}/${INPUT_PREFIX}ichannel_${${INPUT}.channel}_3${INPUT_EXTENSION}" "")
                download_file("https://www.shadertoy.com${INPUT_DIR}/${INPUT_NAME}_4${INPUT_EXTENSION}" "${SHADER_DIRECTORY}/${INPUT_PREFIX}ichannel_${${INPUT}.channel}_4${INPUT_EXTENSION}" "")
                download_file("https://www.shadertoy.com${INPUT_DIR}/${INPUT_NAME}_5${INPUT_EXTENSION}" "${SHADER_DIRECTORY}/${INPUT_PREFIX}ichannel_${${INPUT}.channel}_5${INPUT_EXTENSION}" "")
            elseif(${INPUT}.ctype STREQUAL "buffer")
                set(TARGET_FILE "${SHADER_DIRECTORY}/${INPUT_PREFIX}ichannel_${${INPUT}.channel}.txt")
                if(${INPUT}.src STREQUAL "/media/previz/buffer00.png")
                    create_file("${TARGET_FILE}" "buffer_a" "(Redirecting to buffer_a)")
                elseif(${INPUT}.src STREQUAL "/media/previz/buffer01.png")
                    create_file("${TARGET_FILE}" "buffer_b" "(Redirecting to buffer_b)")
                elseif(${INPUT}.src STREQUAL "/media/previz/buffer02.png")
                    create_file("${TARGET_FILE}" "buffer_c" "(Redirecting to buffer_c)")
                elseif(${INPUT}.src STREQUAL "/media/previz/buffer03.png")
                    create_file("${TARGET_FILE}" "buffer_d" "(Redirecting to buffer_d)")
                else()
                    message(WARNING "Unable to guess buffer index: ${${INPUT}.src} (channel ${INPUT_INDEX})")
                endif()
            elseif(${INPUT}.ctype STREQUAL "keyboard")
                set(TARGET_FILE "${SHADER_DIRECTORY}/${INPUT_PREFIX}ichannel_${${INPUT}.channel}.txt")
                create_file("${TARGET_FILE}" "keyboard" "(Redirecting to keyboard)")
            elseif(${INPUT}.ctype)
                message(WARNING "Channel type ${${INPUT}.ctype} not supported (channel ${INPUT_INDEX})")
            endif()

        endforeach()

    endforeach()

    sbeClearJson(SHADER)
endmacro()


set(SHADERTOY_API_KEY "BtntM7" CACHE STRING "Shadertoy API Key (go to https://www.shadertoy.com/howto#q2 for details)")
mark_as_advanced(SHADERTOY_API_KEY)

set(SHADERTOY_SHADER_ID "" CACHE STRING "Download Shader")


option(SHADERTOY_DOWNLOAD_SHADER "Download shader defined with SHADERTOY_SHADER_ID. Shader needs to be public and not unlisted.")
if (SHADERTOY_DOWNLOAD_SHADER)
    if (NOT SHADERTOY_SHADER_ID)
        message(FATAL_ERROR "SHADERTOY_SHADER_ID needs to be set")
    else()
        foreach (id ${SHADERTOY_SHADER_ID})
            download_shadertoy(${id} ${SHADERTOY_API_KEY})
        endforeach()
    endif()
endif()

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

get_subdir_list(SAMPLES_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

set(CUSTOM_STRUCTS Ray;ray CACHE STRING "Download Shader")
set(FRAG_CUSTOM_STRUCTS_SUPPORT "")
foreach(STRUCT ${CUSTOM_STRUCTS})
    set(FRAG_CUSTOM_STRUCTS_SUPPORT "${FRAG_CUSTOM_STRUCTS_SUPPORT}        struct ${STRUCT};\n")
endforeach()
set(FRAG_CUSTOM_STRUCTS_SUPPORT     "${FRAG_CUSTOM_STRUCTS_SUPPORT}        struct inout_proxy : builtin_inout_proxy\n")
set(FRAG_CUSTOM_STRUCTS_SUPPORT     "${FRAG_CUSTOM_STRUCTS_SUPPORT}        {\n")
foreach(STRUCT ${CUSTOM_STRUCTS})
    set(FRAG_CUSTOM_STRUCTS_SUPPORT "${FRAG_CUSTOM_STRUCTS_SUPPORT}            using ${STRUCT} = _cxxswizzle_fragment_shader::${STRUCT}&;\n")
endforeach()
set(FRAG_CUSTOM_STRUCTS_SUPPORT     "${FRAG_CUSTOM_STRUCTS_SUPPORT}        };\n")
foreach(STRUCT ${CUSTOM_STRUCTS})
    set(FRAG_CUSTOM_STRUCTS_SUPPORT "${FRAG_CUSTOM_STRUCTS_SUPPORT}        #define ${STRUCT}(...) ${STRUCT}\{ __VA_ARGS__ \}\n")
endforeach()


foreach(SAMPLE_DIR ${SAMPLES_DIRS})
    message(STATUS "Sample ${SAMPLE_DIR}")

    file(GLOB SHADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/${SAMPLE_DIR}/*.frag")
    file(GLOB TEXTURES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/${SAMPLE_DIR}/*.png" "${CMAKE_CURRENT_SOURCE_DIR}/${SAMPLE_DIR}/*.jpg")
    file(GLOB BUFFERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/${SAMPLE_DIR}/*.txt")

    get_filename_component(SAMPLE_NAME ${SAMPLE_DIR} NAME_WE)
    sanitize_name(SAMPLE_NAME ${SAMPLE_NAME})

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
            set(FRAG_COMMON_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER}")
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
            set(FRAG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER}")
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
        set(SAMPLE_${CHANNEL_NAME}_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${TEXTURE}")
    endforeach()

    foreach (BUFFER ${BUFFERS})
        file(READ "${CMAKE_CURRENT_SOURCE_DIR}/${BUFFER}" BUFFER_ID)
        message(STATUS "* found special texture: ${BUFFER} (points to ${BUFFER_ID})") 
        get_filename_component(CHANNEL_NAME ${BUFFER} NAME_WE)
        string(TOUPPER ${CHANNEL_NAME} CHANNEL_NAME)
        set(SAMPLE_${CHANNEL_NAME}_PATH "${BUFFER_ID}")
    endforeach()

    configure_file("config.hpp.in" "${GEN_CONFIG_PATH}")

    set(CODEGEN_TEMPLATE_FILES "shadertoy_sandbox.cpp.in" "config.hpp.in")
    set(README_PATH "${SAMPLE_DIR}/README.md")

    set(TARGET_FILES main.cpp ${GEN_SHADERS} shadertoy_sandbox.hpp ${SHADERS} ${GEN_CONFIG_PATH} ${COMPILER_SPECIFIC_FILES} ${CODEGEN_TEMPLATE_FILES})
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${README_PATH}")
        list(APPEND TARGET_FILES ${README_PATH})
    endif()

    source_group("shaders" FILES ${SHADERS} ${README_PATH})
    source_group("codegen" FILES ${GEN_SHADERS} ${GEN_CONFIG_PATH} )
    source_group("codegen-templates" FILES ${CODEGEN_TEMPLATE_FILES} )
    source_group("" FILES main.cpp shadertoy_sandbox.hpp ${COMPILER_SPECIFIC_FILES})

    set(TARGET_NAME "scalar_${SAMPLE_NAME}")
    add_executable(${TARGET_NAME} ${TARGET_FILES})
    target_include_directories(${TARGET_NAME} PRIVATE ${CxxSwizzle_SOURCE_DIR}/include ${GEN_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(${TARGET_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main SDL2::SDL2_image)
    set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17)
    set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD_REQUIRED ON)

    if (BUILD_SIMD_SAMPLE)
        set(TARGET_NAME "simd_${SAMPLE_NAME}")
        add_executable(${TARGET_NAME} ${TARGET_FILES})
        target_include_directories(${TARGET_NAME} PRIVATE ${CxxSwizzle_SOURCE_DIR}/include ${GEN_INCLUDE_DIR} ${Vc_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
        target_link_libraries(${TARGET_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main SDL2::SDL2_image ${Vc_LIBRARIES})
        set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17)
        set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD_REQUIRED ON)
        add_dependencies(${TARGET_NAME} Vc)
        set_target_properties(${TARGET_NAME} PROPERTIES COMPILE_FLAGS "-DSAMPLE_USE_SIMD_VC")
    endif()

    #undo variables
    foreach(SHADER ${SHADERS})
        get_filename_component(SHADER_NAME_SANITIZED ${SHADER} NAME_WE)
        string(TOUPPER ${SHADER_NAME_SANITIZED} SHADER_NAME_SANITIZED)
        unset(SAMPLE_HAS_${SHADER_NAME_SANITIZED})
    endforeach()		



endforeach()

