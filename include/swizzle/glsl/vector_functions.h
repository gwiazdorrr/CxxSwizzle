// CxxSwizzle
// Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
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
// You should include this file *in the namespace* you are going to use CxxSwizzle. 
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


#define SWIZZLE_FORWARD_FUNC(name) \
template <class T, class... U> inline auto name(T&& t, U&&... u) -> \
decltype(::swizzle::detail::decay( ::swizzle::detail::declval<typename ::swizzle::detail::get_vector_type<T, U...>::type>().name(t, u...)) ) \
{ return ::swizzle::detail::get_vector_type<T, U...>::type::name(std::forward<T>(t), std::forward<U>(u)... ); }      


SWIZZLE_FORWARD_FUNC(radians)
SWIZZLE_FORWARD_FUNC(degrees)
SWIZZLE_FORWARD_FUNC(sin)
SWIZZLE_FORWARD_FUNC(cos)
SWIZZLE_FORWARD_FUNC(tan)
SWIZZLE_FORWARD_FUNC(asin)
SWIZZLE_FORWARD_FUNC(acos)
SWIZZLE_FORWARD_FUNC(atan)
SWIZZLE_FORWARD_FUNC(pow)
SWIZZLE_FORWARD_FUNC(exp)
SWIZZLE_FORWARD_FUNC(log)
SWIZZLE_FORWARD_FUNC(exp2)
SWIZZLE_FORWARD_FUNC(log2)
SWIZZLE_FORWARD_FUNC(sqrt)
SWIZZLE_FORWARD_FUNC(inversesqrt)
SWIZZLE_FORWARD_FUNC(abs)
SWIZZLE_FORWARD_FUNC(sign)
SWIZZLE_FORWARD_FUNC(floor)
SWIZZLE_FORWARD_FUNC(ceil)
SWIZZLE_FORWARD_FUNC(fract)
SWIZZLE_FORWARD_FUNC(mod)
SWIZZLE_FORWARD_FUNC(min)
SWIZZLE_FORWARD_FUNC(max)
SWIZZLE_FORWARD_FUNC(clamp)
SWIZZLE_FORWARD_FUNC(mix)
SWIZZLE_FORWARD_FUNC(step)
SWIZZLE_FORWARD_FUNC(smoothstep)
SWIZZLE_FORWARD_FUNC(reflect)
SWIZZLE_FORWARD_FUNC(length)
SWIZZLE_FORWARD_FUNC(distance)
SWIZZLE_FORWARD_FUNC(dot)
SWIZZLE_FORWARD_FUNC(normalize)
SWIZZLE_FORWARD_FUNC(faceforward)
SWIZZLE_FORWARD_FUNC(cross)

SWIZZLE_FORWARD_FUNC(lessThan)
SWIZZLE_FORWARD_FUNC(lessThanEqual)
SWIZZLE_FORWARD_FUNC(greaterThan)
SWIZZLE_FORWARD_FUNC(greaterThanEqual)
SWIZZLE_FORWARD_FUNC(equal)
SWIZZLE_FORWARD_FUNC(notEqual)
SWIZZLE_FORWARD_FUNC(any)
SWIZZLE_FORWARD_FUNC(all)
SWIZZLE_FORWARD_FUNC(not)

#undef SWIZZLE_FORWARD_FUNC