include(ShadertoyAPI)
include(CxxSwizzleSandbox)

set(SHADERTOY_SETUP "scalar" CACHE STRING "SIMD type")
set_property(CACHE SHADERTOY_SETUP PROPERTY STRINGS ${CXXSWIZZLE_SETUPS})

# handle downloads

set(SHADERTOY_API_KEY "BtntM7" CACHE STRING "Shadertoy API Key (go to https://www.shadertoy.com/howto#q2 for details)")
mark_as_advanced(SHADERTOY_API_KEY)

set(SHADERTOY_DOWNLOAD "none" CACHE STRING "Download Shader")
set_property(CACHE SHADERTOY_DOWNLOAD PROPERTY STRINGS none id top_love top_popular top_newest top_hot)

set(SHADERTOY_DOWNLOAD_PARAM "" CACHE STRING "List of Shadertoys ids (if SHADERTOY_DOWNLOAD is set to id) or a search term")
set(SHADERTOY_DOWNLOAD_MAX_COUNT "10" CACHE STRING "Max Shadertoys to download")

option(SHADERTOY_APPLY_TRIVIAL_FIXES "" ON)

if (SHADERTOY_DOWNLOAD STREQUAL "id")
    if (NOT SHADERTOY_DOWNLOAD_PARAM)
        message(FATAL_ERROR "SHADERTOY_DOWNLOAD_PARAM needs to be set")
    else()
        foreach (id ${SHADERTOY_DOWNLOAD_PARAM})
            shadertoy_download(${SHADERTOY_API_KEY} ${id} "${CMAKE_CURRENT_SOURCE_DIR}" "${CXXSWIZZLE_SANDBOX_TEXTURES_ROOT}" True ${SHADERTOY_APPLY_TRIVIAL_FIXES})
        endforeach()
        message(STATUS "Download done, setting SHADERTOY_DOWNLOAD to none")
        set(SHADERTOY_DOWNLOAD "none" CACHE STRING "Download Shader" FORCE)
    endif()
elseif(SHADERTOY_DOWNLOAD MATCHES "^top_")
    string (REPLACE "top_" "" SHADERTOY_DOWNLOAD_LIST_TYPE ${SHADERTOY_DOWNLOAD})
    shadertoy_query(${SHADERTOY_API_KEY} ${SHADERTOY_DOWNLOAD_LIST_TYPE} "${SHADERTOY_DOWNLOAD_PARAM}" ${SHADERTOY_DOWNLOAD_MAX_COUNT} SHADERTOY_SHADER_LIST)
    foreach (id ${SHADERTOY_SHADER_LIST})
        shadertoy_download(${SHADERTOY_API_KEY} ${id} "${CMAKE_CURRENT_SOURCE_DIR}" "${CXXSWIZZLE_SANDBOX_TEXTURES_ROOT}" False ${SHADERTOY_APPLY_TRIVIAL_FIXES})
    endforeach()
    message(STATUS "Download done, setting SHADERTOY_DOWNLOAD to none")
    set(SHADERTOY_DOWNLOAD "none" CACHE STRING "Download Shader" FORCE)
endif()

if (BUILD_SHADERTOYS)
    cxxswizzle_prepare_setup("${SHADERTOY_SETUP}")
    file(GLOB shadertoys RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/*/image.frag")
    foreach(shadertoy ${shadertoys})
       get_filename_component(name ${shadertoy} DIRECTORY)
       cxxswizzle_create_runner("shadertoy_${name}" "${CMAKE_CURRENT_SOURCE_DIR}/${name}" "${SHADERTOY_SETUP}" "" )
    endforeach()
endif()