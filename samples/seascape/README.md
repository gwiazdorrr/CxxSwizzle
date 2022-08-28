# Seascape

* Author: TDM
* Link: https://www.shadertoy.com/view/Ms2SD1

## Changes

No changes were needed to run this shadertoy in scalar mode. To make it SIMD-compatible, a few steps needed to be taken:
- loops index variables are explicitly marked as scalar for arrays index operators to work
- `heightMapTracing` has a single return now

With these changes the shader runs without artifacts in `simd_vc_masked` mode. Other SIMD mode suffer from artifacts dues to conditionals.

## Configurations

* [x] scalar
* [ ] simd_naive (artifacts)
* [ ] simd_vc (artifacts)
* [x] simd_vc_masked
