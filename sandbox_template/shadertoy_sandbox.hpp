#include "config.hpp"

#include <swizzle/vector.hpp>
#include <swizzle/matrix.hpp>
#include <swizzle/array.hpp>
#include <swizzle/sampler_generic.hpp>
#include <swizzle/detail/simd_mask.h>
#include <swizzle/inout_wrapper.hpp>
#include <swizzle/detail/batch_write_mask.hpp>
#include <type_traits>

namespace shadertoy
{
    enum class sampler_type : int
    {
        none,
        texture,
        buffer,
        keyboard,
        cubemap,
    };

    enum class pass_type : int
    {
        buffer_a,
        buffer_b,
        buffer_c,
        buffer_d,
        image,
        sound,
        cube_a
    };

    const int num_samplers = 4;
    const int num_buffers = 4;

    struct shader_inputs
    {
        swizzle::vec3 iResolution;               // viewport resolution (in pixels)
        swizzle::float_type iTime;               // shader playback time (in seconds)
        swizzle::float_type iTimeDelta;          // render time (in seconds)
        swizzle::int_type iFrame;                // shader playback frame
        swizzle::float_type iChannelTime[4];     // channel playback time (in seconds) NOTE: not supported yet
        swizzle::vec3 iChannelResolution[4];     // channel resolution (in pixels)
        swizzle::vec4 iMouse;                    // mouse pixel coords. xy: current (if MLB down), zw: click
        swizzle::sampler2D iChannel0;            // input channel. XX = 2D/Cube
        swizzle::sampler2D iChannel1;
        swizzle::sampler2D iChannel2;
        swizzle::sampler2D iChannel3;
        swizzle::vec4 iDate;                     // (year, month, day, time in seconds)
        swizzle::float_type iSampleRate;         // sound sample rate (i.e., 44100)
        swizzle::int_type iFrameRate;            // NOTE: undocummented
    };

    using shader_fun = swizzle::vec4 (CONFIG_CALLING_CONVENTION *)(const shader_inputs& uniforms, swizzle::vec2 fragCoord, swizzle::vec4 prevFragColor, swizzle::bool_type* discarded);


    //inline shader_fun image;
    //inline shader_fun buffer_a;
    //inline shader_fun buffer_b;
    //inline shader_fun buffer_c;
    //inline shader_fun buffer_d;


    swizzle::vec4 CONFIG_CALLING_CONVENTION image(const shader_inputs& uniforms, swizzle::vec2 fragCoord, swizzle::vec4 prevFragColor, swizzle::bool_type* discarded);
    swizzle::vec4 CONFIG_CALLING_CONVENTION buffer_a(const shader_inputs& input, swizzle::vec2 fragCoord, swizzle::vec4 prevFragColor, swizzle::bool_type* discarded);
    swizzle::vec4 CONFIG_CALLING_CONVENTION buffer_b(const shader_inputs& input, swizzle::vec2 fragCoord, swizzle::vec4 prevFragColor, swizzle::bool_type* discarded);
    swizzle::vec4 CONFIG_CALLING_CONVENTION buffer_c(const shader_inputs& input, swizzle::vec2 fragCoord, swizzle::vec4 prevFragColor, swizzle::bool_type* discarded);
    swizzle::vec4 CONFIG_CALLING_CONVENTION buffer_d(const shader_inputs& input, swizzle::vec2 fragCoord, swizzle::vec4 prevFragColor, swizzle::bool_type* discarded);


    inline const pass_type passes [] = 
    {
        pass_type::buffer_a,
        pass_type::buffer_b,
        pass_type::buffer_c,
        pass_type::buffer_d,
        pass_type::image
    };


    constexpr bool has_pass(pass_type pass)
    {
        if (pass == pass_type::image)    return true;
#ifdef CONFIG_SAMPLE_HAS_BUFFER_A
        if (pass == pass_type::buffer_a) return true;
#endif
#ifdef CONFIG_SAMPLE_HAS_BUFFER_B
        if (pass == pass_type::buffer_b) return true;
#endif
#ifdef CONFIG_SAMPLE_HAS_BUFFER_C
        if (pass == pass_type::buffer_c) return true;
#endif
#ifdef CONFIG_SAMPLE_HAS_BUFFER_D
        if (pass == pass_type::buffer_d) return true;
#endif
        return false;
    }

    constexpr shader_fun get_pass(pass_type pass)
    {
        if (pass == pass_type::image)    return image;
#ifdef CONFIG_SAMPLE_HAS_BUFFER_A
        if (pass == pass_type::buffer_a) return buffer_a;
#endif
#ifdef CONFIG_SAMPLE_HAS_BUFFER_B
        if (pass == pass_type::buffer_b) return buffer_b;
#endif
#ifdef CONFIG_SAMPLE_HAS_BUFFER_C
        if (pass == pass_type::buffer_c) return buffer_c;
#endif
#ifdef CONFIG_SAMPLE_HAS_BUFFER_D
        if (pass == pass_type::buffer_d) return buffer_d;
#endif
        return nullptr;
    }
}