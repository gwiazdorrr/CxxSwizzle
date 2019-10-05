namespace glsl_sandbox
{
    #include <swizzle/define_vector_functions.hpp>

    

    sampler2D samplers[4];

    const struct
    {
        // TODO: this will fail if int is a batch, need to add something like vector indexer
        const vec3& operator[](int i) const
        {
            return samplers[i].resolution;
        }

    } iChannelResolution;

    const sampler2D& iChannel0 = samplers[0];
    const sampler2D& iChannel1 = samplers[1];
    const sampler2D& iChannel2 = samplers[2];
    const sampler2D& iChannel3 = samplers[3];

    // change meaning of glsl keywords to match sandbox
    #define uniform extern
    #define in
    #define out ref_proxy::
    #define inout ref_proxy::
    #define main operator()
    #define mainImage operator()
    #define float batch_float_t   
    #define int batch_int_t
    #define uint batch_uint_t
    #define bool batch_bool_t
    #define char definitely_not_a_char
    #define lowp
    #define highp

#undef M_PI

#ifdef SIMD_IF
    #define if(x) SIMD_IF(x)
#endif
#ifdef SIMD_ELSE
    #define else SIMD_ELSE
#endif
#ifdef SIMD_WHILE
    #define while(x) SIMD_WHILE(x)
#endif
#ifdef SIMD_CONDITION
    #define condition(x) SIMD_CONDITION(x)
#endif
    
    #pragma warning(push)
    #pragma warning(disable: 4244) // disable return implicit conversion warning
    #pragma warning(disable: 4305) // disable truncation warning
    


    struct fragment_shader : fragment_shader_uniforms
    {
        struct Ray;
        struct ray;

        using sampler2D = const sampler2D&;

        template <template <typename> class mod>
        struct modifier
        {
            using vec2 = typename mod<vec2>::type;
            typedef vec3& vec3;
            typedef vec4& vec4;
        };

        // a nested namespace used when redefining 'inout' and 'out' keywords
        struct ref_proxy
        {
#ifdef CXXSWIZZLE_inout_wrapper_ENABLED
            typedef swizzle::inout_wrapper<vec2> vec2;
            typedef swizzle::inout_wrapper<vec3> vec3;
            typedef swizzle::inout_wrapper<vec4> vec4;
#else
            typedef vec2& vec2;
            typedef vec3& vec3;
            typedef vec4& vec4;
#endif
            typedef ::batch_float_t& batch_float_t;
            typedef int& int;

            typedef Ray& Ray;
            typedef ray& ray;
        };

        struct in_proxy
        {
            typedef const ::vec2& vec2;
            typedef const ::vec3& vec3;
            typedef const ::vec4& vec4;
            typedef const ::batch_float_t& batch_float_t;
            typedef int& int;
            typedef const Ray& Ray;
            typedef const ray& ray;
        };

        vec2 gl_FragCoord;
        vec4 gl_FragColor;


        #define COMBINE1(X,Y) X##Y  // helper macro
        #define COMBINE(X,Y) COMBINE1(X,Y)
        #define CXXSWIZZLE_LOAD_TEXTURE(i, name) static inline auto COMBINE(__cxxswizzle_sampler_init, i) = [](){ return load_texture(samplers[i], name); }();
        #include "shader_include.hpp"
    };

    // be a dear a clean up
    #pragma warning(pop)
}

// need to undef any stuff that might possibly have been defined in the shader...
#ifdef vec4
#undef vec4
#endif

#ifdef shade
#undef shade
#endif

#ifdef const
#undef const
#endif

#ifdef fragment_shader_uniforms
#undef fragment_shader_uniforms
#endif

#ifdef uniforms
#undef uniforms
#endif

#ifdef vec2
#undef vec2
#endif

#ifdef fragCoord
#undef fragCoord
#endif

#ifdef static
#undef static
#endif

#ifdef vec4
#undef vec4
#endif

#ifdef c_zero
#undef c_zero
#endif

#ifdef c_one
#undef c_one
#endif

#ifdef namespace
#undef namespace
#endif

namespace
{
    static vec4 c_zero = vec4(0);
    static vec4 c_one = vec4(1, 1, 1, 1);
}

vec4 shade(const fragment_shader_uniforms& uniforms, vec2 fragCoord)
{
    glsl_sandbox::fragment_shader shader;
    static_cast<fragment_shader_uniforms&>(shader) = uniforms;

    shader.gl_FragCoord = fragCoord;

    // vvvvvvvvvvvvvvvvvvvvvvvvvv
    // THE SHADER IS INVOKED HERE
    shader(shader.gl_FragColor, shader.gl_FragCoord);

    // convert to [0;255]
    return glsl_sandbox::clamp(shader.gl_FragColor, c_zero, c_one);
}