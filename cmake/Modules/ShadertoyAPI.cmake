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
    string(REPLACE "."  "_" ${result} ${${result}})
endmacro()

macro(_shadertoyCreateFile path contents msg)
    message(STATUS "Creating ${path} ${msg}")
    file(WRITE ${path} "${contents}")
endmacro()

macro(_shadertoyDownloadFile url path msg)
    message(STATUS "Downloading ${url} to ${path} ${msg}")
    file(DOWNLOAD ${url} ${path})
endmacro()

macro(_shadertoyUnescapeStr str result)
	string(REPLACE "@BRACKET_OPEN@"  "["  tmp "${str}")
	string(REPLACE "@BRACKET_CLOSE@" "]"  tmp "${tmp}")
	string(REPLACE "@SEMICOLON@"     ";"  tmp "${tmp}")
	string(REPLACE "@SLASH@"         "\\" tmp "${tmp}")
	set(${result} "${tmp}")
endmacro()

macro(_shadertoyEscapeStr str result)
	string(REPLACE "\\" "@SLASH@"         tmp "${str}")
	string(REPLACE ";"  "@SEMICOLON@"     tmp "${tmp}")
	string(REPLACE "]"  "@BRACKET_CLOSE@" tmp "${tmp}")
	string(REPLACE "["  "@BRACKET_OPEN@"  tmp "${tmp}")
	set(${result} "${tmp}")
endmacro()

macro(_shadertoyPatchSourceCode path)

	message(STATUS "PATCHING ${path}")
	set(lines)
	file(STRINGS ${path} lines)

	set(out)
	list(APPEND out "#if !defined(CXXSWIZZLE) && !defined(CXXSWIZZLE_CONST)" )
	list(APPEND out "#define CXXSWIZZLE_CONST const")
	list(APPEND out "#define CXXSWIZZLE_IGNORE(x) x")
    list(APPEND out "#define CXXSWIZZLE_ARRAY_FIELD(type) type[]")
	list(APPEND out "#define CXXSWIZZLE_ARRAY(type) type[]")
	list(APPEND out "#define CXXSWIZZLE_ARRAY_N(type, size) type[size]")
	list(APPEND out "#define CXXSWIZZLE_MAKE_ARRAY(type) type[]")
	list(APPEND out "#endif")

	# set(func_decl_regex "[A-Za-z0-9_]+[ \t]+[A-Za-z0-9_]+[ \t]*\\(.*\\);")

    set(glsl_style_array "([A-Za-z0-9_]+)[ \t]*\\[([A-Za-z0-9_ \t]*)\\][ \t]*([A-Za-z0-9_]+)")
    set(c_style_array    "([A-Za-z0-9_]+)[ \t]+([A-Za-z0-9_]+)[ \t]*\\[([A-Za-z0-9_ \t]*)\\]")
    set(glsl_array_initializer "([A-Za-z0-9_]+)[ \t]*\\[[A-Za-z0-9_ \t]*\\][ \t]*(\\(|$)")

	unset(pending_array_line)

	foreach(line ${lines})

		_shadertoyUnescapeStr("${line}" line)

		if (pending_array_line)
			if (line MATCHES "^[ \t]*\\(")
                message(VERBOSE "Array initializer found ${pending_array_line}")
				string(REGEX REPLACE "${glsl_array_initializer}" "CXXSWIZZLE_MAKE_ARRAY(\\1)" pending_array_line "${pending_array_line}")
				#string(REGEX REPLACE "^([ \t]*)\\(" "\\1," line "${line}")
			endif()

			_shadertoyEscapeStr("${pending_array_line}" pending_array_line)
			list(APPEND out "${pending_array_line}")
			unset(pending_array_line)
		endif()
		# message(STATUS "${line}")

		if (line MATCHES "\\^\\^")
			message(VERBOSE "Replacing ^^ in line: ${line}")
			string(REPLACE "^^" "!=" line "${line}")
		endif()

		if (line MATCHES "^const ")
			message(VERBOSE "Replacing global const in line: ${line}")
			string(REGEX REPLACE "^const " "CXXSWIZZLE_CONST " line "${line}")
		endif()

		if (line MATCHES "^precision ")
            message(VERBOSE "Ignoring a line with precision: ${line}")
			set(line "CXXSWIZZLE_IGNORE(${line})")
		endif()

		# matches xxxxx xxxxxx(xxxxx);
		if (line MATCHES "^[A-Za-z0-9_]+[ \t]+[A-Za-z0-9_]+[ \t]*\\([^\\)]*\\)[ \t]*;[ \t]*$")
			message(VERBOSE "Hiding function declaration: ${line}")
			set(line "CXXSWIZZLE_IGNORE(${line})")
		endif()

		if (NOT line MATCHES "^#define.*")
			if (line MATCHES "${glsl_style_array}")
				message(VERBOSE "Array found ${line}")
				if (NOT "${CMAKE_MATCH_2}" STREQUAL "")
                    set(replacement "CXXSWIZZLE_ARRAY_N(\\1, \\2) \\3")
                elseif (line MATCHES ^"[ \t]")
                    set(replacement "CXXSWIZZLE_ARRAY(\\1) \\3")
                else()
                    set(replacement "CXXSWIZZLE_ARRAY_FIELD(\\1) \\3")
                endif()
                string(REGEX REPLACE "${glsl_style_array}" "${replacement}" line "${line}")
			elseif (line MATCHES "${c_style_array}")
				if (NOT "${CMAKE_MATCH_1}" STREQUAL "return")
                    message(VERBOSE "Array found ${line}")
                    if (NOT "${CMAKE_MATCH_3}" STREQUAL "")
                        set(replacement "CXXSWIZZLE_ARRAY_N(\\1, \\3) \\2")
                    elseif (line MATCHES ^"[ \t]")
                        set(replacement "CXXSWIZZLE_ARRAY(\\1) \\2")
                    else()
                        set(replacement "CXXSWIZZLE_ARRAY_FIELD(\\1) \\2")
                    endif()
                    string(REGEX REPLACE "${c_style_array}" "${replacement}" line "${line}")
				endif()
			endif()

			if (line MATCHES "${glsl_array_initializer}")
                if ("${CMAKE_MATCH_2}" STREQUAL "(")
                    message(VERBOSE "Array initializer found ${line}")
				    string(REGEX REPLACE "${glsl_array_initializer}" "CXXSWIZZLE_MAKE_ARRAY(\\1)( " line "${line}")
			    else()
				    set(pending_array_line ${line})
                endif()
			endif()
		endif()

		if (NOT pending_array_line)
			#string(REPLACE "\\" "@SLASH@" line "${line}")
			#message(STATUS "${line}")
			_shadertoyEscapeStr("${line}" line)
			#message(STATUS "${line}")
			list(APPEND out "${line}")
		endif()

	endforeach()

	list(JOIN out "\n" out)
	_shadertoyUnescapeStr("${out}" out)
	file(WRITE ${path} "${out}")

    # logical xor operator
    # string(REPLACE "^^" "!=" result "${code}")
    # global consts
    # string(REGEX REPLACE "\nconst" "\nCXXSWIZZLE_CONST" result "${result}")

    # function declarations
    # string(REGEX REPLACE "\n([A-Za-z0-9_]+[ \t]+[A-Za-z0-9_]+[ \t]*\\([^\n\\(\\)]*\\);)" "\nCXXSWIZZLE_IGNORE(\\1)" result "${result}")

    # arrays
    # string(REGEX REPLACE "([A-Za-z0-9_]+)[ \t]*\\[([A-Za-z0-9_]+)\\][ \t]*\\(" "CXXSWIZZLE_ARRAY_VAL(\\1, \\2" result "${result}")

    # set(${result_var} "${result}")
endmacro()

macro(shadertoyDownload api_key shader_id root_dir textures_root download_error_is_fatal patch_source_files)
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

            set (shader_path "${shader_directory}/${renderpass_name}.frag")
            message(STATUS "Creating ${shader_path} (Shader pass)")

            if (${patch_source_files})
            	# escape eveything
            	string(REPLACE "\\" "@SLASH@@SLASH@" shader_code "${${renderpass}.code}")
				_shadertoyEscapeStr("${shader_code}" shader_code)
				file(WRITE "${shader_path}" "${shader_code}")
                _shadertoyPatchSourceCode("${shader_path}")
            else()
        		set(shader_code "${${renderpass}.code}")
        		file(WRITE "${shader_path}" "${shader_code}")
            endif()
            
            
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

    #_shadertoyDownloadFile(${shadertoy_query} ${shadertoy_json_path} "(Shadertoy JSON shader list)")

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

