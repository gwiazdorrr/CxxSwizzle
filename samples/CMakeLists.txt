# CxxSwizzle (c) 2013-2021 Piotr Gwiazdowski
cmake_minimum_required(VERSION 3.3)

include(CxxSwizzleSandbox)

macro(create_sample name simd_type)
	cxxswizzle_create_runner("sample_${simd_type}_${name}" "${CMAKE_CURRENT_SOURCE_DIR}/${name}" "${simd_type}" "" )
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
	create_sample("${name}" "simd_vc_masked")
endmacro()

if (BUILD_SAMPLES)
	cxxswizzle_prepare_setup("scalar")
	create_sample_scalar("_sh16c__doom")
	create_sample_scalar("creation_by_silexars")
	create_sample_scalar("dry_ice_2")
	create_sample_scalar("seascape")
endif()

if (BUILD_SAMPLES_SIMD)
	cxxswizzle_prepare_setup("simd_vc")
	create_sample_simd_vc("creation_by_silexars_patched")

	# has artifacts due to one branch being followed at a time
	create_sample_simd_vc("seascape_patched")

	cxxswizzle_prepare_setup("simd_vc_masked")
	# no artifacts
	create_sample_simd_vc_masked("seascape_patched")
endif()