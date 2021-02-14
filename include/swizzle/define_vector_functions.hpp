// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
//
// This header contains functions that are proxies to vector static functions. Vector
// type common for all the arguments is chosen based on ::swizzle::detail::get_vector_type,
// which relies on ::swizzle::detail::get_vector_type_impl specialisations. This is a solution
// to a few problems:
// - if the vector type of size 1 is implicitly constructible from a scalar these functions
//   will work, even if called with scalars only
// - gets rid of ambiguity, provided vector's static functions are not ambiguous 
// - can decay result of the static functions ( vec1 -> scalar ); in here the assumption
//   is that the result is convertible to the decay type
//
// You should include this file *in the namespace* you are go ing to use CxxSwizzle. 
// The reason is standard headers sneakily include <math.h>, which in turn pollutes
// global namespace with functions such as sin and cos. Due to the way ADL works,
// all calls with scalars only be matched against these global functions first.
// That is usually not a problem, but they might give different results then
// shader language's you are trying to emulate.
// Only defining functions of same name in a namespace overrides this behavior
// (importing with using statement doesn't), giving you full control of how scalars
// are handled.
//
// Another "funny" thing: the bit in the decltype is different to the one in return statement;
// Both are meant to call static functions. In C++ it is legal to call a static function
// like a member function, i.e. using an instance. This form is used in the decltype as
// it yields better error messages in MSVC if something goes wrong (can't specialize vs.
// internal complier linkage).

#ifdef FUNCNAME

#else
//#define FUNCNAME aaaa
//#include "self"
//#undef FUNCNAME
#endif


#ifdef CXXSWIZZLE_MASK_FUNCTION_NAMES

#define SWIZZLE_FORWARD_FUNC(name) \
    template <class T, class... U> inline auto _cxxswizzle_func_##name(T&& t, U&&... u) -> \
    decltype(::swizzle::detail::decay(std::declval<typename ::swizzle::detail::get_vector_type<T, U...>::type>().call_##name(t, u...))) \
    { return ::swizzle::detail::get_vector_type<T, U...>::type::call_##name(std::forward<T>(t), std::forward<U>(u)...); }

#define radians(...)          _cxxswizzle_func_radians(__VA_ARGS__)
#define degrees(...)          _cxxswizzle_func_degrees(__VA_ARGS__)
#define cos(...)              _cxxswizzle_func_cos(__VA_ARGS__)
#define tan(...)              _cxxswizzle_func_tan(__VA_ARGS__)
#define sin(...)              _cxxswizzle_func_sin(__VA_ARGS__)
#define asin(...)             _cxxswizzle_func_asin(__VA_ARGS__)
#define acos(...)             _cxxswizzle_func_acos(__VA_ARGS__)
#define atan(...)             _cxxswizzle_func_atan(__VA_ARGS__)
#define sinh(...)             _cxxswizzle_func_sinh(__VA_ARGS__)
#define cosh(...)             _cxxswizzle_func_cosh(__VA_ARGS__)
#define tanh(...)             _cxxswizzle_func_tanh(__VA_ARGS__)
#define asinh(...)            _cxxswizzle_func_asinh(__VA_ARGS__)
#define acosh(...)            _cxxswizzle_func_acosh(__VA_ARGS__)
#define atanh(...)            _cxxswizzle_func_atanh(__VA_ARGS__)
#define pow(...)              _cxxswizzle_func_pow(__VA_ARGS__)
#define exp(...)              _cxxswizzle_func_exp(__VA_ARGS__)
#define log(...)              _cxxswizzle_func_log(__VA_ARGS__)
#define exp2(...)             _cxxswizzle_func_exp2(__VA_ARGS__)
#define log2(...)             _cxxswizzle_func_log2(__VA_ARGS__)
#define sqrt(...)             _cxxswizzle_func_sqrt(__VA_ARGS__)
#define inversesqrt(...)      _cxxswizzle_func_inversesqrt(__VA_ARGS__)
#define abs(...)              _cxxswizzle_func_abs(__VA_ARGS__)
#define sign(...)             _cxxswizzle_func_sign(__VA_ARGS__)
#define floor(...)            _cxxswizzle_func_floor(__VA_ARGS__)
#define trunc(...)            _cxxswizzle_func_trunc(__VA_ARGS__)
#define round(...)            _cxxswizzle_func_round(__VA_ARGS__)
#define roundEven(...)        _cxxswizzle_func_roundEven(__VA_ARGS__)
#define ceil(...)             _cxxswizzle_func_ceil(__VA_ARGS__)
#define fract(...)            _cxxswizzle_func_fract(__VA_ARGS__)
#define mod(...)              _cxxswizzle_func_mod(__VA_ARGS__)
#define modf(...)             _cxxswizzle_func_modf(__VA_ARGS__)
#define min(...)              _cxxswizzle_func_min(__VA_ARGS__)
#define max(...)              _cxxswizzle_func_max(__VA_ARGS__)
#define clamp(...)            _cxxswizzle_func_clamp(__VA_ARGS__)
#define mix(...)              _cxxswizzle_func_mix(__VA_ARGS__)
#define step(...)             _cxxswizzle_func_step(__VA_ARGS__)
#define smoothstep(...)       _cxxswizzle_func_smoothstep(__VA_ARGS__)
#define isnan(...)            _cxxswizzle_func_isnan(__VA_ARGS__)
#define isinf(...)            _cxxswizzle_func_isinf(__VA_ARGS__)
#define floatBitsToInt(...)   _cxxswizzle_func_floatBitsToInt(__VA_ARGS__)
#define floatBitsToUint(...)  _cxxswizzle_func_floatBitsToUint(__VA_ARGS__)
#define intBitsToFloat(...)   _cxxswizzle_func_intBitsToFloat(__VA_ARGS__)
#define uintBitsToFloat(...)  _cxxswizzle_func_uintBitsToFloat(__VA_ARGS__)
#define length(...)           _cxxswizzle_func_length(__VA_ARGS__)
#define distance(...)         _cxxswizzle_func_distance(__VA_ARGS__)
#define dot(...)              _cxxswizzle_func_dot(__VA_ARGS__)
#define cross(...)            _cxxswizzle_func_cross(__VA_ARGS__)
#define normalize(...)        _cxxswizzle_func_normalize(__VA_ARGS__)
#define faceforward(...)      _cxxswizzle_func_faceforward(__VA_ARGS__)
#define reflect(...)          _cxxswizzle_func_reflect(__VA_ARGS__)
#define refract(...)          _cxxswizzle_func_refract(__VA_ARGS__)
#define lessThan(...)         _cxxswizzle_func_lessThan(__VA_ARGS__)
#define lessThanEqual(...)    _cxxswizzle_func_lessThanEqual(__VA_ARGS__)
#define greaterThan(...)      _cxxswizzle_func_greaterThan(__VA_ARGS__)
#define greaterThanEqual(...) _cxxswizzle_func_greaterThanEqual(__VA_ARGS__)
#define equal(...)            _cxxswizzle_func_equal(__VA_ARGS__)
#define notEqual(...)         _cxxswizzle_func_notEqual(__VA_ARGS__)
#define any(...)              _cxxswizzle_func_any(__VA_ARGS__)
#define all(...)              _cxxswizzle_func_all(__VA_ARGS__)
#define not(...)              _cxxswizzle_func_not(__VA_ARGS__)
#define dFdx(...)             _cxxswizzle_func_dFdx(__VA_ARGS__)
#define dFdy(...)             _cxxswizzle_func_dFdy(__VA_ARGS__)
#define fwidth(...)           _cxxswizzle_func_fwidth(__VA_ARGS__)
#define outerProduct(...)     _cxxswizzle_func_outerProduct(__VA_ARGS__)

#else

#define SWIZZLE_FORWARD_FUNC(name) \
    template <class T, class... U> inline auto name(T&& t, U&&... u) -> \
    decltype(::swizzle::detail::decay(std::declval<typename ::swizzle::detail::get_vector_type<T, U...>::type>().call_##name(t, u...))) \
    { return ::swizzle::detail::get_vector_type<T, U...>::type::call_##name(std::forward<T>(t), std::forward<U>(u)...); }
    
#endif

// 8.1
SWIZZLE_FORWARD_FUNC(radians)
SWIZZLE_FORWARD_FUNC(degrees)
SWIZZLE_FORWARD_FUNC(cos)
SWIZZLE_FORWARD_FUNC(tan)
SWIZZLE_FORWARD_FUNC(sin)
SWIZZLE_FORWARD_FUNC(asin)
SWIZZLE_FORWARD_FUNC(acos)
SWIZZLE_FORWARD_FUNC(atan)
SWIZZLE_FORWARD_FUNC(sinh)
SWIZZLE_FORWARD_FUNC(cosh)
SWIZZLE_FORWARD_FUNC(tanh)
SWIZZLE_FORWARD_FUNC(asinh)
SWIZZLE_FORWARD_FUNC(acosh)
SWIZZLE_FORWARD_FUNC(atanh)

// 8.2
SWIZZLE_FORWARD_FUNC(pow)
SWIZZLE_FORWARD_FUNC(exp)
SWIZZLE_FORWARD_FUNC(log)
SWIZZLE_FORWARD_FUNC(exp2)
SWIZZLE_FORWARD_FUNC(log2)
SWIZZLE_FORWARD_FUNC(sqrt)
SWIZZLE_FORWARD_FUNC(inversesqrt)

// 8.3
SWIZZLE_FORWARD_FUNC(abs)
SWIZZLE_FORWARD_FUNC(sign)
SWIZZLE_FORWARD_FUNC(floor)
SWIZZLE_FORWARD_FUNC(trunc)
SWIZZLE_FORWARD_FUNC(round)
SWIZZLE_FORWARD_FUNC(roundEven)
SWIZZLE_FORWARD_FUNC(ceil)
SWIZZLE_FORWARD_FUNC(fract)
SWIZZLE_FORWARD_FUNC(mod)
SWIZZLE_FORWARD_FUNC(modf)
SWIZZLE_FORWARD_FUNC(min)
SWIZZLE_FORWARD_FUNC(max)
SWIZZLE_FORWARD_FUNC(clamp)
SWIZZLE_FORWARD_FUNC(mix)
SWIZZLE_FORWARD_FUNC(step)
SWIZZLE_FORWARD_FUNC(smoothstep)
SWIZZLE_FORWARD_FUNC(isnan)
SWIZZLE_FORWARD_FUNC(isinf)
SWIZZLE_FORWARD_FUNC(floatBitsToInt)
SWIZZLE_FORWARD_FUNC(floatBitsToUint)
SWIZZLE_FORWARD_FUNC(intBitsToFloat)
SWIZZLE_FORWARD_FUNC(uintBitsToFloat)

// 8.4
SWIZZLE_FORWARD_FUNC(length)
SWIZZLE_FORWARD_FUNC(distance)
SWIZZLE_FORWARD_FUNC(dot)
SWIZZLE_FORWARD_FUNC(cross)
SWIZZLE_FORWARD_FUNC(normalize)
SWIZZLE_FORWARD_FUNC(faceforward)
SWIZZLE_FORWARD_FUNC(reflect)
SWIZZLE_FORWARD_FUNC(refract)


// 8.6
SWIZZLE_FORWARD_FUNC(lessThan)
SWIZZLE_FORWARD_FUNC(lessThanEqual)
SWIZZLE_FORWARD_FUNC(greaterThan)
SWIZZLE_FORWARD_FUNC(greaterThanEqual)
SWIZZLE_FORWARD_FUNC(equal)
SWIZZLE_FORWARD_FUNC(notEqual)
SWIZZLE_FORWARD_FUNC(any)
SWIZZLE_FORWARD_FUNC(all)
SWIZZLE_FORWARD_FUNC(not)


// 8.8
SWIZZLE_FORWARD_FUNC(dFdx)
SWIZZLE_FORWARD_FUNC(dFdy)
SWIZZLE_FORWARD_FUNC(fwidth)

SWIZZLE_FORWARD_FUNC(outerProduct)

// a convenient list of functions
// radians
// degrees
// cos
// tan
// sin
// asin
// acos
// atan
// sinh
// cosh
// tanh
// asinh
// acosh
// atanh
// pow
// exp
// log
// exp2
// log2
// sqrt
// inversesqrt
// abs
// sign
// floor
// trunc
// round
// roundEven
// ceil
// fract
// mod
// modf
// min
// max
// clamp
// mix
// step
// smoothstep
// isnan
// isinf
// floatBitsToInt
// floatBitsToUint
// intBitsToFloat
// uintBitsToFloat
// length
// distance
// dot
// cross
// normalize
// faceforward
// reflect
// refract
// lessThan
// lessThanEqual
// greaterThan
// greaterThanEqual
// equal
// notEqual
// any
// all
// not
// dFdx
// dFdy
// fwidth
// outerProduct


#undef SWIZZLE_FORWARD_FUNC