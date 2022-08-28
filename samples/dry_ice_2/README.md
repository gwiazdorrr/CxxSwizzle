# `Dry ice 2` by `xjorma`: https://www.shadertoy.com/view/WlVyRV

No changes were needed to run this shadertoy. However, running it in SIMD mode will yield results full of blocky artifacts - that's likely due to many points of return in many of the functions. This can be fixed by some careful restructuring, but is yet to be done.