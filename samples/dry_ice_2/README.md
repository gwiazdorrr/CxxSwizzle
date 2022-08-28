# Dry ice 2

* Author: xjorma
* Link: https://www.shadertoy.com/view/WlVyRV

## Changes

No changes were needed to run this shadertoy. However, running it in SIMD mode will yield results full of blocky artifacts - that's likely due to many points of return in many of the functions. This can be fixed by some careful restructuring, but is yet to be done.

## Configurations

* [x] scalar
* [ ] simd_naive (artifacts)
* [ ] simd_vc (artifacts)
* [ ] simd_vc_masked (artifacts)
