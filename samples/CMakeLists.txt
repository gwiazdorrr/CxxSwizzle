# CxxSwizzle
# Copyright (c) 2013-2021, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
cmake_minimum_required(VERSION 3.3)

include(CxxSwizzleSandbox)

macro(create_sample name simd_type)
	cxxswizzle_create_sandbox("${CMAKE_CURRENT_SOURCE_DIR}/../sandbox_template" "sample_${simd_type}_${name}" "${CMAKE_CURRENT_SOURCE_DIR}/${name}" "${simd_type}" "" "" )
endmacro()

macro(create_sample_scalar name)
	create_sample("${name}" "scalar")
endmacro()

macro(create_sample_simd_naive name)
	create_sample("${name}" "simd_naive")
endmacro()

macro(create_sample_simd_vc name)
	create_sample("${name}" "simd_vc")
endmacro()

macro(create_sample_simd_vc_masked name)
	create_sample("${name}" "simd_vc_with_masking")
endmacro()

set(SAMPLE_TRACY_PROFILER_ROOT "" CACHE PATH "Path to the Tracy Profiler root. Leave empty if you don't want to use it.")

create_sample_scalar("creation_by_silexars")
create_sample_scalar("flame")
create_sample_scalar("raymarching_-_primitives")
create_sample_scalar("wolfenstein_3d")
create_sample_scalar("_sh17c__physically_based_shading")
create_sample_scalar("_nv15__2001_space_station")
create_sample_scalar("_sh17b__pixel_shader_dungeon")
create_sample_scalar("iresolution_imouse_idate_etc")
create_sample_scalar("zone_plate_checkerboard")
create_sample_scalar("_sh16c__doom")

create_sample_simd_naive("isoline_triangle")
create_sample_simd_vc("isoline_triangle")
create_sample_simd_vc_masked("zone_plate_checkerboard")