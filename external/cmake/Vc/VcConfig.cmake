set(Vc_VERSION_MAJOR 0)
set(Vc_VERSION_MINOR 7)
set(Vc_VERSION_PATCH 4)
set(Vc_VERSION 0.7.4)
set(Vc_VERSION_STRING "0.7.4")

set(Vc_INSTALL_DIR 			"${CMAKE_SOURCE_DIR}/external")
set(Vc_LIB_DIR              "${Vc_INSTALL_DIR}/lib")
set(Vc_CMAKE_MODULES_DIR    "${Vc_INSTALL_DIR}/cmake/Vc")

find_path(Vc_INCLUDE_DIR Vc/Vc PATHS "${Vc_INSTALL_DIR}" NO_DEFAULT_PATH PATH_SUFFIXES include)
find_library(Vc_RELASE_LIBRARIES Vc PATHS "${Vc_INSTALL_DIR}" NO_DEFAULT_PATH PATH_SUFFIXES lib)
find_library(Vc_DEBUG_LIBRARIES Vc-d PATHS "${Vc_INSTALL_DIR}" NO_DEFAULT_PATH PATH_SUFFIXES lib)

set(Vc_LIBRARIES optimized ${Vc_RELASE_LIBRARIES} debug ${Vc_DEBUG_LIBRARIES})

include("${Vc_CMAKE_MODULES_DIR}/VcMacros.cmake")

set(Vc_DEFINITIONS)
vc_set_preferred_compiler_flags()
