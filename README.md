# CxxSwizzle

This project provides dependency-free headers that replicate GLSL language syntax and built-in functions in C++17, as completely as possible. Basically, you can do this in C++ now:
```glsl
vec4 foo(0);                        // 0,0,0,0
foo.yx = vec2(2, 1);                // 1,2,0,0
foo.zw = foo.xy * 2;                // 1,2,2,4
vec2 bar = max(foo.xw, foo.yz).yx;  // 4,2
bar = clamp(foo.xw, 0, 2);          // 1,2  
mat2 m(foo.xyz, 1);                 // 1,2,2,1
bar *= m;                           // 5,2
```

Sample project provides Shadertoy integration and is able to download and compile a complete Shadertoy (shaders & textures), provided its visibility is set to `public+api`. Out of 100 most popular shaders:

- XX% compiled without any alterations
- XX% needed minor tweaks
- XX% were impossible to port

## GLSL support status

Parts of [GLSL Lang Spec 4.60](https://www.khronos.org/registry/OpenGL/specs/gl/GLSLangSpec.4.60.pdf) CxxSwizzle attempted at replicating:

- [ ] Baisc Types [4.1]
  - [x] scalar types
  - [x] `vec2..4`
  - [x] `ivec2..4`
  - [x] `uvec2..4`
  - [x] `bvec2..4`
  - [ ] `dvec2..4`
  - [x] `mat2..4`
  - [x] `mat2..4x2..4`
  - [ ] `dmat2..4`
  - [ ] `dmat2..4x2..4`
  - [x] `sampler1D` *(as sampler_generic)*
  - [x] `sampler2D` *(as sampler_generic)*
  - [x] `samplerCube` *(as sampler_generic)*
  - [ ] `sampler3D`
  - [ ] other sampler types 
- [ ] Implicit Conversions [4.1.10]
- [x] Parameter Qualifiers [4.6]
  - [x] `const`
  - [x] `in`
  - [x] `out`
  - [x] `inout`
- [x] Precision Qualifiers [4.7]
  - [x] `highp` *(no effect)*
  - [x] `mediump` *(no effect)*
  - [x] `lowp` *(no effect)*
- [ ] Operators [5.1]
- [x] Constructors [5.4]
  - [x] Conversion and Scalar Constructors [5.4.1]
  - [ ] Vector and Matrix Constructors [5.4.2] *needs tests*
  - [x] Structure Constructors [5.4.3] *(see Workarounds)*
  - [ ] Array Constructors [5.4.4] 
    - [x] One-dimensional  *(see Workarounds)*
    - [ ] Multi-dimensional
- [x] Vector and Scalar Components and Length [5.5]
  - [x] Swizzling
  - [x] Vector `length` method
  - [ ] Scalar `x` compoment
- [x] Matrix Components [5.6]
- [x] Structure and Array Operations [5.7]
  - [ ] Array length method
  - [ ] Equality operator
  - [x] Other operators
- [x] Vector and Matrix Operations [5.10] *needs more tests*
- [x] Function Definitions [6.1]
  - [x] Prototypes *(see Workarounds)*
  - [x] Definitions
- [x] Jumps [6.4]
  - [x] `discard`
- [ ] Built-in Variables [7]
  - [x] `gl_FragCoord`
  - [x] `gl_FragColor`
  - [ ] Other variables
- [ ] Built-in Functions
  - [x] Angle and Trigonometry Functions [8.1]
  - [x] Exponential Functions [8.2]
  - [ ] Common Functions [8.3]
    - [ ] `floatBitsToInt`, `floatBitsToUInt`, 
    - [ ] `intBitsToFloat`, `uintBitsToFloat`
    - [ ] `fma`
    - [ ] `frexp`, `ldexp`
    - [x] Other functions
  - [ ] Floating-Point Pack and Unpack Functions [8.4]
  - [ ] Geometric Functions [8.5]
    - [ ] `ftransform`
    - [x] Other functions
  - [x] Matrix Functions [8.6]
    - [ ] `matrixCompMult`
    - [ ] `outerProduct`
    - [x] `transpose`
    - [ ] `determinant`
    - [ ] `inverse`
  - [x] Vector Relational Functions [8.7]
  - [ ] Integer Functions [8.8]
  - [ ] Texture Functions [8.9] *(sampler ignores lod and partial derivatives)*
    - [x] `textureSize`
    - [x] `texture`
    - [x] `texelFetch` 
    - [x] `textureLod`
    - [x] `textureGrad`
    - [ ] Other  functions
  - [ ] Fragment Processing Functions [8.13] *(SIMD only)*
    - [x] `dfDx`
    - [x] `dfDy`
    - [x] `fwidth`
    - [ ] Coarse and Fine derivatives
    - [ ] Interpolation Functions

## Shadertoy integration status

- [ ] Downloading Shaders with "public+api" visibility
  - [x] By id
    - [x] Single
    - [x] Batch
  - [x] Query
    - [x] Name
    - [x] Sort (Popular, Newest, Love, Hot)
    - [x] From-To
    - [ ] Filters
- [ ] Shader Inputs
  - [x] iResolution
  - [x] iTime
  - [x] iTimeDelta
  - [x] iFrame
  - [ ] iChannelTime *Note: always 0*
  - [x] iChannelResolution
  - [x] iMouse
  - [x] iChannel0...3
  - [x] iDate
  - [ ] iSampleRate *Note: always 0*
  - [x] iFrameRate
- [ ] Sources
  - [x] Common
  - [x] Buffer A
  - [x] Buffer B
  - [x] Buffer C
  - [x] Buffer D
  - [ ] Sound
  - [ ] Cubemap A
- [ ] Channels
  - [ ] Misc
    - [x] Keyboard
    - [ ] Webcam
    - [ ] Microphone
    - [ ] Soundcloud
    - [x] Buffer A
    - [x] Buffer B
    - [x] Buffer C
    - [x] Buffer D
    - [ ] Cubemap A
  - [x] Textures
  - [x] Cubemaps
  - [ ] Volumes
  - [ ] Videos
  - [ ] Music
- [ ] Sampler options
  - [ ] Filter
    - [ ] mipmap *Note: there's generally no support for mipmaps at the moment*
    - [x] linear
    - [x] nearest
  - [x] Wrap
  - [x] VFlip


## GLSL bits that do not work out of the box

### Struct initialization
```glsl
struct Foo { float x; float y; }; 
...
Foo f(0.0, 1.0);`
```
```
error C2440: 'initializing': cannot convert from 'initializer list' to '`anonymous-namespace'::_cxxswizzle_fragment_shader_image::Foo'
```
*Solution*: Add `Foo` to `CUSTOM_STRUCTS` CMake list.

### `out/inout` argument modifiers for structs:
```glsl
void foo(out Foo f) {}
```
```
error C2039: 'Foo': is not a member of '`anonymous-namespace'::_cxxswizzle_fragment_shader_image::inout_proxy'
``` 
*Solution*: Add `Foo` to `CUSTOM_STRUCTS` CMake list.

### Variable being named like a function
```glsl
float cos = cos(x);
```
```
error C2064: term does not evaluate to a function taking 1 arguments
```
*Solution*:  Rename the variable.
  ```glsl
  float cs = cos(x);
  ```

### Array intialization
```glsl
int a[2] = int[](0, 1);
```
```
error C2059: syntax error: ']'
```
*Solution*: a macro needs to be used in place of the initialization.
  ```glsl
  CXXSWIZZLE_ARRAY(float, a, 0, 1);
  ```
  If you intend to move back and forth between CxxSwizzle and Shadertoy, add this to your shader too:
  ```glsl
  #ifndef CXXSWIZZLE_ARRAY
  #define CXXSWIZZLE_ARRAY(type, name, ...) type name[] = type[] ( __VA_ARGS__ )
  #endif
  ```

### Function prototypes
```glsl
void foo();
...
foo();
```
```
error C2535: 'void `anonymous-namespace'::_cxxswizzle_fragment_shader_image::foo(void)': member function already defined or declared
```
*Solution*: remove prototypes or wrap with `#ifndef CXXSWIZZLE` if you intend to copy shader back to Shadertoy.
  ```glsl
  #ifndef CXXSWIZZLE
  void foo();
  #endif
  ```

### const int used for array size
```glsl
const int ArraySize=10; 
vec3 Array[ArraySize];
```
```
error C2327: '`anonymous-namespace'::_cxxswizzle_fragment_shader_image::ArraySize': is not a type name, static, or enumerator
```
*Solution*: use a macro instead.
```glsl
#define ArraySize 10
```
If you can't use a macro then use this:
```glsl
CXXSWIZZLE_STATIC const int ArraySize = 10;
```
If you intend to copy the shader back to Shadertoy you will need to add this as well:
```glsl
#ifndef CXXSWIZZLE_STATIC
#define CXXSWIZZLE_STATIC
#endif
```

### Ternary operator and swizzles

```glsl
p.xz = (p.z>p.x) ? p.zx : p.xz;
```
```
error: conditional expression is ambiguous; 'indexed_proxy<[3 * ...], 2UL aka 2, 0UL aka 0>' can be converted to 'indexed_proxy<[3 * ...], 0UL aka 0, 2UL aka 2>' and vice versa
error: operands to ?: have different types ‘swizzle::detail::vector_base_type_helper<float, 3>::proxy_generator<2, 0>::type’ {aka ‘swizzle::detail::indexed_proxy<swizzle::vector_<float, 0, 1>, std::array<float, 3>, float, 2, 0>’} and ‘swizzle::detail::vector_base_type_helper<float, 3>::proxy_generator<0, 2>::type’ {aka ‘swizzle::detail::indexed_proxy<swizzle::vector_<float, 0, 1>, std::array<float, 3>, float, 0, 2>’}
```
*Solution*: MSVC seems to resolve the ambiguity somehow, but g++ and clang need a hint:
```glsl
p.xz = (p.z>p.x) ? (vec2)p.zx : p.xz;
```


## Problematic Shadertoys examples

#### E1M1 - Hangar (https://www.shadertoy.com/view/lsSXzD)

Will not work out of the box because of function declarations. C++ obviously has function declarations, but because of how CxxSwizzle samples are set up, they are not going to work and need to be wrapped with an `#ifdef`:
```glsl
#ifndef CXXSWIZZLE
vec3 SampleTexture( const in float fTexture, const in vec2 vUV );
void MapIntersect( out float fClosestT, out vec4 vHitInfo );
float hash(float p);
#endif
```


#### [NV15] 2001 Space Station (https://www.shadertoy.com/view/4lBGRh)

GLSL allows you to have variables and functions of the same name. That is not a case with C++, so each `Rotate` function needs to be fixed and not use `sin` and `cos` as variables:
```glsl
vec3 RotateX(vec3 v, float rad)
{
  float cs = cos(rad);
  float sn = sin(rad);
  //if (RIGHT_HANDED_COORD)
  return vec3(v.x, cs * v.y + sn * v.z, -sn * v.y + cs * v.z);
  //else return new float3(x, cos * y - sin * z, sin * y + cos * z);
}
vec3 RotateY(vec3 v, float rad)
{
  float cs = cos(rad);
  float sn = sin(rad);
  //if (RIGHT_HANDED_COORD)
  return vec3(cs * v.x - sn * v.z, v.y, sn * v.x + cs * v.z);
  //else return new float3(cos * x + sin * z, y, -sin * x + cos * z);
}
vec3 RotateZ(vec3 v, float rad)
{
  float cs = cos(rad);
  float sn = sin(rad);
  //if (RIGHT_HANDED_COORD)
  return vec3(cs * v.x + sn * v.y, -sn * v.x + cs * v.y, v.z);
}
```


#### [SH17C] Physically Based Shading (https://www.shadertoy.com/view/4sSfzK)

This is a fun one. To start with GLSL's syntax for arrays initialization is simply not compatible with C++'s one, so it is macro time:

```glsl
CXXSWIZZLE_ARRAY(vec3, BASE_COLORS,
    vec3(0.74),
    vec3(0.51, 0.72, 0.81),
    vec3(0.66, .85, .42),
    vec3(0.87, 0.53, 0.66),
    vec3(0.51, 0.46, 0.74),
    vec3(0.78, 0.71, 0.45)
);
```

Another thing is struct initialization syntax is also incompatible with C++. But this time a sneaky macro can be generated to work around it, provied you pass add the type name (`SHCoefficients` in this case) to `CUSTOM_STRUCTS` CMake list option.

The last issue is regarding `AppState` struct - it is being passed with `out`. In such case a tiny piece of code needs to be generated - use `CUSTOM_STRUCTS` CMake list.


#### [SH17B] Pixel Shader Dungeon (https://www.shadertoy.com/view/Xs2fWD)

Will not compile because of global `const int` being used as an array size. Since in CxxSwizzle global symbols are actually fields of a struct, an equivalent syntax would be `static const int`. So either use a `CXXSWIZZLE_STATIC`:

```glsl
CXXSWIZZLE_STATIC const int ENEMY_NUM           = 3;
CXXSWIZZLE_STATIC const int LOG_NUM             = 4;
```

Or use plain old defines:
```glsl
#define ENEMY_NUM 3
#define LOG_NUM   4
```

Another reason the shader will not compile is because `GameState` is passed with `inout` - add `GameState` to `CUSTOM_STRUCTS` CMake list.

