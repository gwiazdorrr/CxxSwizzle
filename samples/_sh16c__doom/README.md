# [SH16C] Doom
* Author: P_Malin
* Link: https://www.shadertoy.com/view/lldGDr

## Changes

This shadertoy comes with automated trivial fixes applied only - it uses lots of arrays and syntax GLSL for these is fundamentally incompatible with C++. Sadly it will not compile in SIMD mode - non-scalar indexing operators for arrays are not supported yet.

## Configurations
* [x] scalar
* [ ] simd_naive (compile errors)
* [ ] simd_vc (compile errors)
* [ ] simd_vc_masked (compile errors)
