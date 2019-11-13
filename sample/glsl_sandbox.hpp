namespace
{
    #include <swizzle/define_vector_functions.hpp>

    using sampler2D = swizzle::naive_sampler2D<float_type, int_type, uint_type>;

    sampler2D iChannel0 = sampler2D(&::_cxxswizzle::texture_data[0]);
    sampler2D iChannel1 = sampler2D(&::_cxxswizzle::texture_data[1]);
    sampler2D iChannel2 = sampler2D(&::_cxxswizzle::texture_data[2]);
    sampler2D iChannel3 = sampler2D(&::_cxxswizzle::texture_data[3]);

    const struct
    {
        // TODO: this will fail if int is a batch, need to add something like vector indexer
        vec3 operator[](int i) const
        {
            return vec3(::_cxxswizzle::texture_data[i].widthf, ::_cxxswizzle::texture_data[i].heightf, 0.0f);
        }

    } iChannelResolution;

    struct _cxxswizzle_fragment_shader : fragment_shader_uniforms
    {
        // put commonly used struct names here - support for inout needs to be explicit
        struct Ray;
        struct ray;

        struct inout
        {
            // proxies are needed for vectors, as they need to work with swizzles
            using vec2 = swizzle::inout_wrapper<vec2>;
            using vec3 = swizzle::inout_wrapper<vec3>;
            using vec4 = swizzle::inout_wrapper<vec4>;
            using ivec2 = swizzle::inout_wrapper<ivec2>;
            using ivec3 = swizzle::inout_wrapper<ivec3>;
            using ivec4 = swizzle::inout_wrapper<ivec4>;
            using uvec2 = swizzle::inout_wrapper<uvec2>;
            using uvec3 = swizzle::inout_wrapper<uvec3>;
            using uvec4 = swizzle::inout_wrapper<uvec4>;
            using bvec2 = swizzle::inout_wrapper<bvec2>;
            using bvec3 = swizzle::inout_wrapper<bvec3>;
            using bvec4 = swizzle::inout_wrapper<bvec4>;

            // regular types don't need proxies
            using float_type = float_type & ;
            using int_type = int_type & ;
            using uint_type = uint_type & ;
            using bool_type = bool_type & ;

            // add custom types here
            using ray = ray & ;
            using Ray = Ray & ;
        };

        vec2 gl_FragCoord;
        vec4 gl_FragColor;

        // need a function that's not likely to be defined in the shader itself; operator seems like a good choice
        vec4 operator()(const fragment_shader_uniforms& uniforms, vec2 fragCoord)
        {
            static_cast<fragment_shader_uniforms&>(*this) = uniforms;
            gl_FragCoord = fragCoord;

            // vvvvvvvvvvvvvvvvvvvvvvvvvv
            // THE SHADER IS INVOKED HERE
            mainImage(gl_FragColor, gl_FragCoord);

            return clamp(gl_FragColor, vec4(0), vec4(1));
        }

        #define CXXSWIZZLE_COMBINE1(X,Y) X##Y  // helper macro
        #define CXXSWIZZLE_COMBINE(X,Y) CXXSWIZZLE_COMBINE1(X,Y)
        #define CXXSWIZZLE_TEXTURE_PATH_HINT(i, name) static inline auto CXXSWIZZLE_COMBINE(_cxxswizzle_texture_path, i) = []() { ::_cxxswizzle::texture_data[i].path = name; return true; }();


        // change meaning of glsl keywords to match sandbox
        #define uniform extern
        #define in
        #define out inout
        #define inout inout::
        #define float float_type
        #define int  int_type
        #define uint uint_type
        #define bool bool_type
        #define lowp
        #define highp
        // char is not a type in glsl so can be used freely
        #define char definitely_not_a_char

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

        #include "shader_include.hpp"
    };
}

// need to undef any stuff that might possibly have been defined in the shader...
#undef vec4
#undef shade
#undef const
#undef fragment_shader_uniforms
#undef uniforms
#undef vec2
#undef fragCoord
#undef return
#undef _cxxswizzle_fragment_shader

vec4 shade(const fragment_shader_uniforms& uniforms, vec2 fragCoord)
{
    return _cxxswizzle_fragment_shader()(uniforms, fragCoord);
}