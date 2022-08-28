# `Seascape` by `TDM`: https://www.shadertoy.com/view/Ms2SD1

No changes were needed to run this shadertoy in scalar mode. To make it SIMD-compatible, a few steps needed to be taken:
- loops index variables are explicitly marked as scalar for arrays index operators to work
- `heightMapTracing` has a single return now

With these changes the shader runs without artifacts in `simd_vc_masked` mode.