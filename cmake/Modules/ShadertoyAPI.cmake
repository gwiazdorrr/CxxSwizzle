cmake_minimum_required(VERSION 3.3)

if (DEFINED ShadertoyAPI)
    return()
endif()

set(ShadertoyAPI yes)

include(JSONParser)

macro(_shadertoy_sanitize_name result name)
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

macro(_shadertoy_create_file path contents msg)
    message(STATUS "Creating ${path} ${msg}")
    file(WRITE ${path} "${contents}")
endmacro()

macro(_shadertoy_download_file url path msg)
    message(STATUS "Downloading ${url} to ${path} ${msg}")
    file(DOWNLOAD ${url} ${path})
endmacro()

macro(_shadertoy_unescape_str str result)
	string(REPLACE "@BRACKET_OPEN@"  "["  tmp "${str}")
	string(REPLACE "@BRACKET_CLOSE@" "]"  tmp "${tmp}")
	string(REPLACE "@SEMICOLON@"     ";"  tmp "${tmp}")
	string(REPLACE "@SLASH@"         "\\" tmp "${tmp}")
	set(${result} "${tmp}")
endmacro()

macro(_shadertoy_escape_str str result)
	string(REPLACE "\\" "@SLASH@"         tmp "${str}")
	string(REPLACE ";"  "@SEMICOLON@"     tmp "${tmp}")
	string(REPLACE "]"  "@BRACKET_CLOSE@" tmp "${tmp}")
	string(REPLACE "["  "@BRACKET_OPEN@"  tmp "${tmp}")
	set(${result} "${tmp}")
endmacro()

macro(_shadertoy_patch_code path)

	message(VERBOSE "Patching ${path}")
	set(lines)
	file(STRINGS ${path} lines)

	set(out)
	list(APPEND out "#ifndef CXX_CONST" )
	list(APPEND out "#define CXX_CONST const")
	list(APPEND out "#define CXX_IGNORE(x) x")
    list(APPEND out "#define CXX_ARRAY_FIELD(type, name) type[] name")
	list(APPEND out "#define CXX_ARRAY(type) type[]")
	list(APPEND out "#define CXX_ARRAY_N(type, size) type[size]")
	list(APPEND out "#define CXX_MAKE_ARRAY(type) type[]")
	list(APPEND out "#endif")
    list(APPEND out "")

	# set(func_decl_regex "[A-Za-z0-9_]+[ \t]+[A-Za-z0-9_]+[ \t]*\\(.*\\);")

    set(glsl_style_array "([A-Za-z0-9_]+)[ \t]*\\[([A-Za-z0-9_\t\\+\\-\\*/]*)\\][ \t]*([A-Za-z0-9_]+)")
    set(c_style_array    "([A-Za-z0-9_]+)[ \t]+([A-Za-z0-9_]+)[ \t]*\\[([A-Za-z0-9_ \t\\+\\-\\*/]*)\\]")
    set(glsl_array_initializer "([A-Za-z0-9_]+)[ \t]*\\[[A-Za-z0-9_ \t\\+\\-\\*/]*\\][ \t]*(\\(|$)")

	unset(pending_array_line)

    foreach(line IN LISTS lines)

		_shadertoy_unescape_str("${line}" line)

        set(orignalLine "${line}")

		if (pending_array_line)
			if (line MATCHES "^[ \t]*\\(")
                message(VERBOSE "Array initializer found ${pending_array_line}")
				string(REGEX REPLACE "${glsl_array_initializer}" "CXX_MAKE_ARRAY(\\1)" pending_array_line "${pending_array_line}")
				#string(REGEX REPLACE "^([ \t]*)\\(" "\\1," line "${line}")
			endif()

			_shadertoy_escape_str("${pending_array_line}" pending_array_line)
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
			string(REGEX REPLACE "^const " "CXX_CONST " line "${line}")
		endif()

		# matches xxxxx xxxxxx(xxxxx);
		if (line MATCHES "^([A-Za-z0-9_]+)[ \t]+[A-Za-z0-9_]+[ \t]*\\([^\\)]*\\)[ \t]*;[ \t]*$")
            if (NOT "${CMAKE_MATCH_1}" STREQUAL "return")
    			message(VERBOSE "Hiding function declaration: ${line}")
    			set(line "CXX_IGNORE(${line})")
            endif()
		endif()

        if (line MATCHES         "^${glsl_style_array}[ \t]*=[ \t]*${glsl_array_initializer}" AND "${CMAKE_MATCH_2}" STREQUAL "")
            message(VERBOSE "Global auto-sized array: ${line}")
            string(REGEX REPLACE "^${glsl_style_array}[ \t]*=[ \t]*${glsl_array_initializer}" "CXX_ARRAY_FIELD(${CMAKE_MATCH_1}, ${CMAKE_MATCH_3})${CMAKE_MATCH_5}" line "${line}")

        elseif (line MATCHES     "^${c_style_array}[ \t]*=[ \t]*${glsl_array_initializer}" AND "${CMAKE_MATCH_3}" STREQUAL "")
            message(VERBOSE "Global auto-sized array: ${line}")
            string(REGEX REPLACE "^${c_style_array}[ \t]*=[ \t]*${glsl_array_initializer}" "CXX_ARRAY_FIELD(${CMAKE_MATCH_1}, ${CMAKE_MATCH_2})${CMAKE_MATCH_5}" line "${line}")
            
        elseif (NOT line MATCHES "^#define.*")

			while (line MATCHES "${glsl_style_array}[ \t]*([;=,\)]|$)")
                message(VERBOSE "Array found ${line}")
                if (NOT "${CMAKE_MATCH_2}" STREQUAL "")
                    set(replacement "CXX_ARRAY_N(\\1, \\2) \\3") 
                else (line MATCHES "^[ \t]")
                    set(replacement "CXX_ARRAY(\\1) \\3")
                endif()

                if ("${CMAKE_MATCH_4}" STREQUAL "," AND NOT line MATCHES "\\)[ \t]*({|$)")
                    # is this a fuction argument or multiple declarations per line?
                    string(REGEX REPLACE "${glsl_style_array}[ \t]*," "${replacement}; \\1 " line "${line}")
                else()
                    string(REGEX REPLACE "${glsl_style_array}" "${replacement}" line "${line}")
                endif()
            endwhile()

            while (line MATCHES "${c_style_array}[ \t]*([;=,\)]|$)")
                if ("${CMAKE_MATCH_1}" STREQUAL "return" OR "${CMAKE_MATCH_1}" STREQUAL "in" OR "${CMAKE_MATCH_1}" STREQUAL "inout")
                    break()
                endif()

                message(VERBOSE "Array found ${line}")
                if (NOT "${CMAKE_MATCH_3}" STREQUAL "")
                    set(replacement "CXX_ARRAY_N(\\1, \\3) \\2")
                else()
                    set(replacement "CXX_ARRAY(\\1) \\2")
                endif()

                if ("${CMAKE_MATCH_4}" STREQUAL "," AND NOT line MATCHES "\\)[ \t]*({|$)")
                    string(REGEX REPLACE "${c_style_array}[ \t]*," "${replacement}; \\1 " line "${line}")
                else()
                    string(REGEX REPLACE "${c_style_array}" "${replacement}" line "${line}")
                endif()
            endwhile()

            if (line MATCHES "${glsl_array_initializer}")
                if ("${CMAKE_MATCH_2}" STREQUAL "(")
                    message(VERBOSE "Array initializer found ${line}")
                    string(REGEX REPLACE "${glsl_array_initializer}" "CXX_MAKE_ARRAY(\\1)( " line "${line}")
                else()
                    set(pending_array_line ${line})
                endif()
            endif()

        endif()

		if (NOT pending_array_line)
			#string(REPLACE "\\" "@SLASH@" line "${line}")
			#message(STATUS "${line}")
            if (NOT "${line}" STREQUAL "${orignalLine}")
                _shadertoy_escape_str("${orignalLine}" orignalLine)
                # list(APPEND out "// ${orignalLine}")
            endif()

			_shadertoy_escape_str("${line}" line)
			#message(STATUS "${line}")
			list(APPEND out "${line}")
		endif()

	endforeach()

	list(JOIN out "\n" out)
	_shadertoy_unescape_str("${out}" out)
	file(WRITE ${path} "${out}")
    
endmacro()

macro(shadertoy_download api_key shader_id root_dir textures_root download_error_is_fatal patch_source_files)
    set(shadertoy_json_path "${CMAKE_CURRENT_BINARY_DIR}/${shader_id}.json")
    set(shadertoy_query "https://www.shadertoy.com/api/v1/shaders/${shader_id}?key=${api_key}")
        
    _shadertoy_download_file(${shadertoy_query} ${shadertoy_json_path} "(Shadertoy JSON shader description)")

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
        _shadertoy_sanitize_name(shader_name_sanitized ${shader_name})
    
        set(shader_directory "${root_dir}/${shader_name_sanitized}")

        if ("${textures_root}" STREQUAL "")
            message(STATUS "No textures_root provided, going to download textures to the shadertoy's directory")
            set (textures_root ${shader_directory})
        endif()


        file(MAKE_DIRECTORY ${shader_directory})

        set(shadertoy_config "")

        list(APPEND shadertoy_config "shadertoy_config shadertoy_config::make_default()")
        list(APPEND shadertoy_config "{")
        list(APPEND shadertoy_config "    // url: https://www.shadertoy.com/view/${shader_id}")
        list(APPEND shadertoy_config "    shadertoy_config config\;")
        list(APPEND shadertoy_config "")
        
        foreach(r ${SHADER.Shader.renderpass})
            set(renderpass SHADER.Shader.renderpass_${r})
            
            if (${renderpass}.name)
                _shadertoy_sanitize_name(renderpass_name ${${renderpass}.name})
            else()
                # seems this fallback is needed sometimes
                _shadertoy_sanitize_name(renderpass_name ${${renderpass}.type})
            endif()

            string(REPLACE "buf_" "buffer_" renderpass_name ${renderpass_name})

            set (shader_path "${shader_directory}/${renderpass_name}.frag")
            message(STATUS "Creating ${shader_path} (Shader pass)")

            if (${patch_source_files})
            	# escape eveything
            	string(REPLACE "\\" "@SLASH@@SLASH@" shader_code "${${renderpass}.code}")
				_shadertoy_escape_str("${shader_code}" shader_code)
				file(WRITE "${shader_path}" "${shader_code}")
                _shadertoy_patch_code("${shader_path}")
            else()
        		set(shader_code "${${renderpass}.code}")
        		file(WRITE "${shader_path}" "${shader_code}")
            endif()
            
            set(pass_accessor "config.get_pass(pass_type::${renderpass_name})")

            foreach(i ${${renderpass}.inputs})
                set(entry "")
                set(input ${renderpass}.inputs_${i})
                set(input_index ${${input}.channel})

                set(vflip "false")
                set(filter "nearest")
                set(wrap "clamp")
                set(spacing "\n        ")

                unset(make)
                if (${input}.ctype STREQUAL "texture")
                    get_filename_component(input_name ${${input}.src} NAME)
                    set(target_file "${textures_root}/${input_name}")
                    set(source_url "https://www.shadertoy.com${${input}.src}")

                    if (EXISTS ${target_file})
                        message(STATUS "Skipping texture ${source_url}, already in cache.")
                    else()
                        _shadertoy_download_file(${source_url} ${target_file}  "")
                    endif()

                    set(make "sampler_config::make_texture(${spacing}\"${input_name}\")")

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
                                _shadertoy_download_file(${source_url} ${target_file}  "")
                            endif()

                            list(APPEND faces "\"${target_file_name}\"")
                        endforeach(index)
                    endif()

                    
                    list(JOIN faces ",${spacing}" faces)
                    set(make "sampler_config::make_cubemap({${spacing}${faces}})")
                    
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

                    set(make "sampler_config::make_buffer(pass_type::${buffer_name})")

                elseif(${input}.ctype STREQUAL "keyboard")
                    set(make "sampler_config::make_keyboard()")

                elseif(${input}.ctype)
                    message(WARNING "Channel type ${${input}.ctype} not supported (channel ${input_index})")
                endif()

                if (make)
                    if (${input}.sampler.vflip)
                        set(vflip ${${input}.sampler.vflip})
                    endif()
                    if (${input}.sampler.filter)
                        set(filter ${${input}.sampler.filter})
                    endif()
                    if (${input}.sampler.wrap)
                        set(wrap ${${input}.sampler.wrap})
                    endif()

                    list(APPEND shadertoy_config "    ${pass_accessor}.get_sampler(${${input}.channel}) = ${make}${spacing}.init(texture_wrap_modes::${wrap}, texture_filter_modes::${filter}, ${vflip})\;")
                    list(APPEND shadertoy_config "")
                endif()

            endforeach()

            list(APPEND shadertoy_config "")

        endforeach()

        list(APPEND shadertoy_config "    return config\;")
        list(APPEND shadertoy_config "}")
        list(JOIN shadertoy_config "\n" shadertoy_config)
        
        _shadertoy_create_file("${shader_directory}/shadertoy_config.hpp" "${shadertoy_config}" "")

    endif()


    sbeClearJson(SHADER)
endmacro()

macro(shadertoy_query api_key sort_type query max_count out_list)
    set(shadertoy_json_path "${CMAKE_CURRENT_BINARY_DIR}/sort_${sort_type}.json")
    set(shadertoy_query "https://www.shadertoy.com/api/v1/shaders/query/${query}?sort=${sort_type}&key=${api_key}&from=0&num=${max_count}")

    _shadertoy_download_file(${shadertoy_query} ${shadertoy_json_path} "(Shadertoy JSON shader list)")

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

