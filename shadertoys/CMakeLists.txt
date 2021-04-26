include(ShadertoyAPI)
include(CxxSwizzleSandbox)

# handle downloads

set(SHADERTOY_API_KEY "BtntM7" CACHE STRING "Shadertoy API Key (go to https://www.shadertoy.com/howto#q2 for details)")
mark_as_advanced(SHADERTOY_API_KEY)

set(SHADERTOY_DOWNLOAD "none" CACHE STRING "Download Shader")
set_property(CACHE SHADERTOY_DOWNLOAD PROPERTY STRINGS none id top_love top_popular top_newest top_hot)

set(SHADERTOY_DOWNLOAD_PARAM "" CACHE STRING "List of Shadertoys ids (if SHADERTOY_DOWNLOAD is set to id) or a search term")
set(SHADERTOY_DOWNLOAD_MAX_COUNT "10" CACHE STRING "Max Shadertoys to download")

option(SHADERTOY_FIX_COMMON_PROBLEMS "" ON)

if (SHADERTOY_DOWNLOAD STREQUAL "id")
    if (NOT SHADERTOY_DOWNLOAD_PARAM)
        message(FATAL_ERROR "SHADERTOY_DOWNLOAD_PARAM needs to be set")
    else()
        foreach (id ${SHADERTOY_DOWNLOAD_PARAM})
            shadertoyDownload(${SHADERTOY_API_KEY} ${id} "${CMAKE_CURRENT_SOURCE_DIR}" "${CXXSWIZZLE_SANDBOX_TEXTURES_ROOT}" True ${SHADERTOY_FIX_COMMON_PROBLEMS})
        endforeach()
        message(STATUS "Download done, setting SHADERTOY_DOWNLOAD to none")
        set(SHADERTOY_DOWNLOAD "none" CACHE STRING "Download Shader" FORCE)
    endif()
elseif(SHADERTOY_DOWNLOAD MATCHES "^top_")
    string (REPLACE "top_" "" SHADERTOY_DOWNLOAD_LIST_TYPE ${SHADERTOY_DOWNLOAD})
    shadertoyQuery(${SHADERTOY_API_KEY} ${SHADERTOY_DOWNLOAD_LIST_TYPE} "${SHADERTOY_DOWNLOAD_PARAM}" ${SHADERTOY_DOWNLOAD_MAX_COUNT} SHADERTOY_SHADER_LIST)
    foreach (id ${SHADERTOY_SHADER_LIST})
        shadertoyDownload(${SHADERTOY_API_KEY} ${id} "${CMAKE_CURRENT_SOURCE_DIR}" "${CXXSWIZZLE_SANDBOX_TEXTURES_ROOT}" False ${SHADERTOY_FIX_COMMON_PROBLEMS})
    endforeach()
    message(STATUS "Download done, setting SHADERTOY_DOWNLOAD to none")
    set(SHADERTOY_DOWNLOAD "none" CACHE STRING "Download Shader" FORCE)
endif()


file(GLOB shadertoys RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/*/image.frag")
foreach(shadertoy ${shadertoys})
   get_filename_component(name ${shadertoy} DIRECTORY)
   cxxswizzle_create_sandbox("${CMAKE_CURRENT_SOURCE_DIR}/../sandbox_template" "shadertoy_${name}" "${CMAKE_CURRENT_SOURCE_DIR}/${name}" "" "" "" )
endforeach()