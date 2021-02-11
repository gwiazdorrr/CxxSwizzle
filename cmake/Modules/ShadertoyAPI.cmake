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
    string(REPLACE "#"  "_" ${result} ${${result}})
    string(REPLACE "^"  "_" ${result} ${${result}})
endmacro()

macro(_shadertoyCreateFile path contents msg)
    message(STATUS "Creating ${path} ${msg}")
    file(WRITE ${path} "${contents}")
endmacro()

macro(_shadertoyDownloadFile url path msg)
    message(STATUS "Downloading ${url} to ${path} ${msg}")
    file(DOWNLOAD ${url} ${path})
endmacro()

macro(shadertoyDownload api_key shader_id root_dir textures_root download_error_is_fatal)
    set(shadertoy_json_path "${CMAKE_CURRENT_BINARY_DIR}/${shader_id}.json")
    set(shadertoy_query "https://www.shadertoy.com/api/v1/shaders/${shader_id}?key=${api_key}")
        
    _shadertoyDownloadFile(${shadertoy_query} ${shadertoy_json_path} "(Shadertoy JSON shader description)")

    message(STATUS "Parsing ${shadertoy_json_path}")
    file(READ "${shadertoy_json_path}" shadertoy_json)
    sbeParseJson(SHADER shadertoy_json)

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

        set(shader_name ${SHADER.Shader.info.name})

        # create directory
        _shadertoySanitizeName(shader_name_sanitized ${shader_name})
    
        set(shader_directory "${root_dir}/${shader_name_sanitized}")

        if ("${textures_root}" STREQUAL "")
            message(STATUS "No textures_root provided, going to download textures to the shadertoy's directory")
            set (textures_root ${shader_directory})
        endif()


        file(MAKE_DIRECTORY ${shader_directory})

        set(shader_info "")

        list(APPEND shader_info "  \"url\": \"https://www.shadertoy.com/view/${shader_id}\"")

        foreach(r ${SHADER.Shader.renderpass})
            set(renderpass SHADER.Shader.renderpass_${r})
            
            if (${renderpass}.name)
                _shadertoySanitizeName(renderpass_name ${${renderpass}.name})
            else()
                # seems this fallback is needed sometimes
                _shadertoySanitizeName(renderpass_name ${${renderpass}.type})
            endif()

            string(REPLACE "buf_" "buffer_" renderpass_name ${renderpass_name})

            message(STATUS "Creating ${shader_directory}/${renderpass_name}.frag (Shader pass)")
            file(WRITE "${shader_directory}/${renderpass_name}.frag" "${${renderpass}.code}")
            
            set(input_prefix "${renderpass_name}_")
            if(renderpass_name STREQUAL "image")
                set(input_prefix "")
            endif()

            set(renderpass_info "")
            set(spacing "    ")

            foreach(i ${${renderpass}.inputs})
                set(entry "")
                set(input ${renderpass}.inputs_${i})
                set(input_index ${${input}.channel})
                if (${input}.ctype STREQUAL "texture")
                    get_filename_component(input_name ${${input}.src} NAME)
                    set(target_file "${textures_root}/${input_name}")
                    set(source_url "https://www.shadertoy.com${${input}.src}")

                    if (EXISTS ${target_file})
                        message(STATUS "Skipping texture ${source_url}, already in cache.")
                    else()
                        _shadertoyDownloadFile(${source_url} ${target_file}  "")
                    endif()
                    string(CONCAT entry "${spacing}  \"type\": \"texture\",\n"
                                        "${spacing}  \"src\":  \"${input_name}\",\n")
                elseif (${input}.ctype STREQUAL "cubemap")
                    get_filename_component(input_ext    ${${input}.src} EXT)
                    get_filename_component(input_name   ${${input}.src} NAME_WE)
                    get_filename_component(input_dir    ${${input}.src} DIRECTORY)

                    set(faces "")

                    if ("${input_name}" STREQUAL "cubemap00")
                        message(WARNING "Cubemap buffers are not supported (channel ${input_index})")
                    else()
                        foreach (index RANGE 5)
                            set(target_file_name "${input_name}_${index}${input_ext}")
                            set(target_file "${textures_root}/${target_file_name}")

                            if (index EQUAL 0)
                                set(source_url "https://www.shadertoy.com${${input}.src}")
                            else()
                                set(source_url "https://www.shadertoy.com${input_dir}/${input_name}_${index}${input_ext}")
                            endif()

                            if (EXISTS ${target_file})
                                message(STATUS "Skipping texture ${source_url}, already in cache.")
                            else()
                                _shadertoyDownloadFile(${source_url} ${target_file}  "")
                            endif()

                            list(APPEND faces "${spacing}    \"${target_file_name}\"")
                        endforeach(index)
                    endif()

                    list(JOIN faces ",\n" faces)
                    string(CONCAT entry "${spacing}  \"type\": \"cubemap\",\n"
                                        "${spacing}  \"src\":  [\n${faces}\n${spacing}  ],\n")
                    
                elseif(${input}.ctype STREQUAL "buffer")
                    set(buffer_name "")
                    if(${input}.src STREQUAL "/media/previz/buffer00.png")
                        set(buffer_name "buffer_a")
                    elseif(${input}.src STREQUAL "/media/previz/buffer01.png")
                        set(buffer_name "buffer_b")
                    elseif(${input}.src STREQUAL "/media/previz/buffer02.png")
                        set(buffer_name "buffer_c")
                    elseif(${input}.src STREQUAL "/media/previz/buffer03.png")
                        set(buffer_name "buffer_d")
                    else()
                        message(WARNING "Unable to guess buffer index: ${${input}.src} (channel ${input_index})")
                    endif()

                    string(CONCAT entry "${spacing}  \"type\": \"buffer\",\n"
                                        "${spacing}  \"src\":  \"${buffer_name}\",\n")

                elseif(${input}.ctype STREQUAL "keyboard")
                    string(CONCAT entry "${spacing}  \"type\": \"keyboard\",\n")

                elseif(${input}.ctype)
                    message(WARNING "Channel type ${${input}.ctype} not supported (channel ${input_index})")
                endif()

                string(CONCAT entry "${entry}"
                                    "${spacing}  \"vflip\": \"${${input}.sampler.vflip}\",\n"
                                    "${spacing}  \"filter\": \"${${input}.sampler.filter}\",\n"
                                    "${spacing}  \"wrap\": \"${${input}.sampler.wrap}\"")

                list(APPEND renderpass_info "${spacing}\"iChannel${${input}.channel}\": {\n${entry}\n${spacing}}")

            endforeach()

            list(JOIN renderpass_info ",\n" renderpass_info)
            list(APPEND shader_info "  \"${renderpass_name}\": {\n${renderpass_info}\n  }")
            

        endforeach()

        list(JOIN shader_info ",\n" shader_info)
        _shadertoyCreateFile("${shader_directory}/config.json" "{\n${shader_info}\n}" "")

    endif()


    sbeClearJson(SHADER)
endmacro()

macro(shadertoyQuery api_key sort_type query max_count out_list)
    set(shadertoy_json_path "${CMAKE_CURRENT_BINARY_DIR}/sort_${sort_type}.json")
    set(shadertoy_query "https://www.shadertoy.com/api/v1/shaders/query/${query}?sort=${sort_type}&key=${api_key}&from=0&num=${max_count}")

    _shadertoyDownloadFile(${shadertoy_query} ${shadertoy_json_path} "(Shadertoy JSON shader list)")

    message(STATUS "Parsing ${shadertoy_json_path}")
    file(READ "${shadertoy_json_path}" shadertoy_json)
    sbeParseJson(SHADER_LIST shadertoy_json)

    if (SHADER_LIST.Error)
        message(FATAL_ERROR "Shartoy API returned an error: ${SHADER_LIST.Error}.")
    endif()

    message(STATUS "Found ${SHADER_LIST.Shaders} shaders")

    foreach(shader_index ${SHADER_LIST.Results})
        set(SHADER_ID SHADER_LIST.Results_${shader_index})
        list(APPEND ${out_list} "${${SHADER_ID}}")
    endforeach()

endmacro()

