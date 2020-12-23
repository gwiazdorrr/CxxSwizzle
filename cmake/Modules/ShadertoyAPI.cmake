cmake_minimum_required(VERSION 3.3)

if (DEFINED ShadertoyAPI)
    return()
endif()

set(ShadertoyAPI yes)

include(JSONParser)

macro(_shadertoySanitizeName result name)
    string(TOLOWER ${name} ${result})
    string(REPLACE " "  "_" ${result} ${${result}})
    string(REPLACE ","  "" ${result} ${${result}})
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

macro(_shadertoyCreateFile path contents msg)
    message(STATUS "Creating ${path} ${msg}")
    file(WRITE ${path} "${contents}")
endmacro()

macro(_shadertoyDownloadFile url path msg)
    message(STATUS "Downloading ${url} to ${path} ${msg}")
    file(DOWNLOAD ${url} ${path})
endmacro()

macro(shadertoyDownload api_key shader_id root_dir download_error_is_fatal)
    set(SHADERTOY_JSON_PATH "${CMAKE_CURRENT_BINARY_DIR}/${shader_id}.json")
    set(SHADERTOY_QUERY "https://www.shadertoy.com/api/v1/shaders/${shader_id}?key=${api_key}")
        
    _shadertoyDownloadFile(${SHADERTOY_QUERY} ${SHADERTOY_JSON_PATH} "(Shadertoy JSON shader description)")

    message(STATUS "Parsing ${SHADERTOY_JSON_PATH}")
    file(READ "${SHADERTOY_JSON_PATH}" SHADERTOY_JSON)
    sbeParseJson(SHADER SHADERTOY_JSON)

    if (SHADER.Error)
        if (${download_error_is_fatal})
            if (SHADER.Error STREQUAL "Shader not found")
                message(FATAL_ERROR "Shartoy API returned an error: ${SHADER.Error}.\nThis happens if shader is not published "
                "as public+api. Anyway, this shader needs to be downloaded manually (see README.md for instructions)")
            else()
                message(FATAL_ERROR "Shartoy API returned an error: ${SHADER.Error}.")
            endif()
        else()
            if (SHADER.Error STREQUAL "Shader not found")
                message(WARNING "Shartoy API returned an error: ${SHADER.Error}.\nThis happens if shader is not published "
                "as public+api. Anyway, this shader needs to be downloaded manually (see README.md for instructions)")
            else()
                message(WARNING "Shartoy API returned an error: ${SHADER.Error}.")
            endif()
        endif()
    else()

        # debug
        # foreach(var ${SHADER})
        #     message("${var} = ${${var}}") 
        # endforeach()

        set(SHADER_NAME ${SHADER.Shader.info.name})

        # create directory
        _shadertoySanitizeName(SHADER_NAME_SANITIZED ${SHADER_NAME})
    
        set(SHADER_DIRECTORY "${root_dir}/${SHADER_NAME_SANITIZED}")

        file(MAKE_DIRECTORY ${SHADER_DIRECTORY})
        _shadertoyCreateFile("${SHADER_DIRECTORY}/README.md" "https://www.shadertoy.com/view/${shader_id}" "" )

        foreach(r ${SHADER.Shader.renderpass})
            set(RENDERPASS SHADER.Shader.renderpass_${r})
            
            if (${RENDERPASS}.name)
                _shadertoySanitizeName(RENDER_PASS_NAME ${${RENDERPASS}.name})
            else()
                # seems this fallback is needed sometimes
                _shadertoySanitizeName(RENDER_PASS_NAME ${${RENDERPASS}.type})
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
                    _shadertoyDownloadFile("https://www.shadertoy.com${${INPUT}.src}" "${SHADER_DIRECTORY}/${INPUT_PREFIX}ichannel_${${INPUT}.channel}${INPUT_EXTENSION}" "")
                elseif (${INPUT}.ctype STREQUAL "cubemap")
                    get_filename_component(INPUT_EXTENSION ${${INPUT}.src} EXT)
                    get_filename_component(INPUT_NAME      ${${INPUT}.src} NAME_WE)
                    get_filename_component(INPUT_DIR       ${${INPUT}.src} DIRECTORY)
                    _shadertoyDownloadFile("https://www.shadertoy.com${${INPUT}.src}" "${SHADER_DIRECTORY}/${INPUT_PREFIX}ichannel_${${INPUT}.channel}${INPUT_EXTENSION}" "")
                    _shadertoyDownloadFile("https://www.shadertoy.com${INPUT_DIR}/${INPUT_NAME}_1${INPUT_EXTENSION}" "${SHADER_DIRECTORY}/${INPUT_PREFIX}ichannel_${${INPUT}.channel}_1${INPUT_EXTENSION}" "")
                    _shadertoyDownloadFile("https://www.shadertoy.com${INPUT_DIR}/${INPUT_NAME}_2${INPUT_EXTENSION}" "${SHADER_DIRECTORY}/${INPUT_PREFIX}ichannel_${${INPUT}.channel}_2${INPUT_EXTENSION}" "")
                    _shadertoyDownloadFile("https://www.shadertoy.com${INPUT_DIR}/${INPUT_NAME}_3${INPUT_EXTENSION}" "${SHADER_DIRECTORY}/${INPUT_PREFIX}ichannel_${${INPUT}.channel}_3${INPUT_EXTENSION}" "")
                    _shadertoyDownloadFile("https://www.shadertoy.com${INPUT_DIR}/${INPUT_NAME}_4${INPUT_EXTENSION}" "${SHADER_DIRECTORY}/${INPUT_PREFIX}ichannel_${${INPUT}.channel}_4${INPUT_EXTENSION}" "")
                    _shadertoyDownloadFile("https://www.shadertoy.com${INPUT_DIR}/${INPUT_NAME}_5${INPUT_EXTENSION}" "${SHADER_DIRECTORY}/${INPUT_PREFIX}ichannel_${${INPUT}.channel}_5${INPUT_EXTENSION}" "")
                elseif(${INPUT}.ctype STREQUAL "buffer")
                    set(TARGET_FILE "${SHADER_DIRECTORY}/${INPUT_PREFIX}ichannel_${${INPUT}.channel}.txt")
                    if(${INPUT}.src STREQUAL "/media/previz/buffer00.png")
                        _shadertoyCreateFile("${TARGET_FILE}" "buffer_a" "(Redirecting to buffer_a)")
                    elseif(${INPUT}.src STREQUAL "/media/previz/buffer01.png")
                        _shadertoyCreateFile("${TARGET_FILE}" "buffer_b" "(Redirecting to buffer_b)")
                    elseif(${INPUT}.src STREQUAL "/media/previz/buffer02.png")
                        _shadertoyCreateFile("${TARGET_FILE}" "buffer_c" "(Redirecting to buffer_c)")
                    elseif(${INPUT}.src STREQUAL "/media/previz/buffer03.png")
                        _shadertoyCreateFile("${TARGET_FILE}" "buffer_d" "(Redirecting to buffer_d)")
                    else()
                        message(WARNING "Unable to guess buffer index: ${${INPUT}.src} (channel ${INPUT_INDEX})")
                    endif()
                elseif(${INPUT}.ctype STREQUAL "keyboard")
                    set(TARGET_FILE "${SHADER_DIRECTORY}/${INPUT_PREFIX}ichannel_${${INPUT}.channel}.txt")
                    _shadertoyCreateFile("${TARGET_FILE}" "keyboard" "(Redirecting to keyboard)")
                elseif(${INPUT}.ctype)
                    message(WARNING "Channel type ${${INPUT}.ctype} not supported (channel ${INPUT_INDEX})")
                endif()

            endforeach()

        endforeach()
    endif()
    sbeClearJson(SHADER)
endmacro()

macro(shadertoyQuery api_key sort_type query max_count out_list)
    set(SHADERTOY_JSON_PATH "${CMAKE_CURRENT_BINARY_DIR}/sort_${sort_type}.json")
    set(SHADERTOY_QUERY "https://www.shadertoy.com/api/v1/shaders/query/${query}?sort=${sort_type}&key=${api_key}&from=0&num=${max_count}")

    _shadertoyDownloadFile(${SHADERTOY_QUERY} ${SHADERTOY_JSON_PATH} "(Shadertoy JSON shader list)")

    message(STATUS "Parsing ${SHADERTOY_JSON_PATH}")
    file(READ "${SHADERTOY_JSON_PATH}" SHADERTOY_JSON)
    sbeParseJson(SHADER_LIST SHADERTOY_JSON)

    if (SHADER_LIST.Error)
        message(FATAL_ERROR "Shartoy API returned an error: ${SHADER_LIST.Error}.")
    endif()

    message(STATUS "Found ${SHADER_LIST.Shaders} shaders")

    foreach(shader_index ${SHADER_LIST.Results})
        set(SHADER_ID SHADER_LIST.Results_${shader_index})
        list(APPEND ${out_list} "${${SHADER_ID}}")
    endforeach()

endmacro()

