# Creation by Silexars

* Author: Danilo Guanabara
* Link: https://www.shadertoy.com/view/XsXXDn

## Changes

This shadertoy run perfectly without any modifications, but since SIMD-compatible version was such a low-hanging fruit, there is one change here - the loop index variable is explicitly declared as scalar to allow it to be used as argument for array index operator.

## Configurations
* [x] scalar
* [x] simd_naive
* [x] simd_vc
* [x] simd_vc_masked
