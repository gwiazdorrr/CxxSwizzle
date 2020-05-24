#include "config.hpp"

#include <swizzle/vector.hpp>
#include <swizzle/matrix.hpp>
#include <swizzle/sampler2D.hpp>
#include <swizzle/detail/simd_mask.h>
#include <swizzle/inout_wrapper.hpp>
#include <swizzle/detail/batch_write_mask.hpp>

namespace shadertoy
{
    const size_t num_samplers = 4;
    const size_t num_buffers = 4;

    struct shader_inputs
    {
        swizzle::vec3 iResolution;               // viewport resolution (in pixels)
        swizzle::float_type iTime;               // shader playback time (in seconds)
        swizzle::float_type iTimeDelta;          // render time (in seconds)
        swizzle::int_type iFrame;                // shader playback frame
        //float_type     iChannelTime[4];       // channel playback time (in seconds)
        swizzle::vec3 iChannelResolution[4];     // channel resolution (in pixels)
        swizzle::vec4 iMouse;                    // mouse pixel coords. xy: current (if MLB down), zw: click
        swizzle::sampler2D iChannel0;            // input channel. XX = 2D/Cube
        swizzle::sampler2D iChannel1;
        swizzle::sampler2D iChannel2;
        swizzle::sampler2D iChannel3;
        //vec4 iDate;                   // (year, month, day, time in seconds)
        //float_type iSampleRate;       // sound sample rate (i.e., 44100)
    };

    swizzle::vec4 image(const shader_inputs& input, swizzle::vec2 fragCoord);

#ifdef SAMPLE_HAS_BUFFER_A
    swizzle::vec4 buffer_a(const shader_inputs& input, swizzle::vec2 fragCoord);
#endif

#ifdef SAMPLE_HAS_BUFFER_B
    swizzle::vec4 buffer_b(const shader_inputs& input, swizzle::vec2 fragCoord);
#endif

#ifdef SAMPLE_HAS_BUFFER_C
    swizzle::vec4 buffer_c(const shader_inputs& input, swizzle::vec2 fragCoord);
#endif

#ifdef SAMPLE_HAS_BUFFER_D
    swizzle::vec4 buffer_d(const shader_inputs& input, swizzle::vec2 fragCoord);
#endif
}