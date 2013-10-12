// CxxSwizzle
// Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <cmath>
#include <swizzle/detail/utils.h>

namespace swizzle
{
    namespace glsl
    {
        namespace naive
        {
            //! A class providing static functions matching GLSL's vector functions. Uses naive approach, i.e.
            //! everything is done components-wise, using stdlib's math functions.
            //! VectorType must have arithmetic operators, binary and unary.
            template <class Base, template <class, size_t> class VectorType, class ScalarType, size_t Size>
            class vector_functions_adapter : public Base
            {
            public:
                typedef VectorType<ScalarType, Size> vector_type;
                typedef VectorType<bool, Size> bool_vector_type;
                typedef ScalarType scalar_type;

            private:

                struct not_available;

                //! Fires Func for each component an assigns back the result
                template <class Func>
                static vector_type& transform(vector_type& x, Func func)
                {
                    detail::static_for<0, Size>([&](size_t i) -> void { x[i] = func(x[i]); } );
                    return x;
                }

                //! Fires Func for each pair of components an assigns back the result to the x
                template <class Func>
                static vector_type& transform(vector_type& x, const vector_type& y, Func func)
                {
                    detail::static_for<0, Size>([&](size_t i) -> void { x[i] = func(x[i], y[i]); } );
                    return x;
                }

                //! Helper that enables the use of overloaded functions as an argument.
                static vector_type& transform(vector_type& x, scalar_type (*func)(scalar_type))
                {
                    return transform<scalar_type (*)(scalar_type)>(x, func);
                }

                //! Helper that enables the use of overloaded functions as an argument.
                static vector_type& transform(vector_type& x, const vector_type& y, scalar_type (*func)(scalar_type, scalar_type))
                {
                    return transform<scalar_type (*)(scalar_type, scalar_type)>(x, y, func);
                }

                template <class T, class Func>
                static VectorType<T, Size> construct(Func func)
                {
                    VectorType<T, Size> result;
                    iterate([&](size_t i) -> void { result[i] = func(i); } );
                    return result;
                }

                template <class Func>
                static void iterate(Func func)
                {
                    detail::static_for<0, Size>([&](size_t i) -> void { func(i); } );
                }


            public:

                static vector_type radians(vector_type degrees)
                {
                    static const scalar_type pi = 3.14159265358979323846;
                    return transform(degrees, [](scalar_type a) -> scalar_type { return (pi * a / 180); } );
                }

                static vector_type degrees(vector_type radians)
                {
                    static const scalar_type pi = 3.14159265358979323846;
                    return transform(radians, [](scalar_type a) -> scalar_type { return scalar_type(180 * a / pi); } );
                }

                static vector_type sin(vector_type x)
                {
                    return transform(x, std::sin);
                }

                static vector_type cos(vector_type x)
                {
                    return transform(x, std::cos);
                }

                static vector_type tan(vector_type x)
                {
                    return transform(x, std::tan);
                }

                static vector_type asin(vector_type x)
                {
                    return transform(x, std::asin);
                }

                static vector_type acos(vector_type x)
                {
                    return transform(x, std::acos);
                }

                static vector_type atan(vector_type y, const vector_type& x)
                {
                    return transform(y, x, std::atan2);
                }

                static vector_type atan(vector_type y_over_x)
                {
                    return transform(y_over_x, std::atan);
                }

                // Exponential Functions
                static vector_type pow(vector_type x, const vector_type& y)
                {
                    return transform(x, y, std::pow );
                }

                static vector_type exp(vector_type x)
                {
                    return transform(x, std::exp );
                }

                static vector_type log(vector_type x)
                {
                    return transform(x, std::log );
                }

                static vector_type exp2(const vector_type& x)
                {
                    return pow( vector_type(2), x);
                }

                static vector_type log2(vector_type x)
                {
                    const scalar_type c_log2e = 1.44269504088896340736;
                    return transform(x, [=](scalar_type a) -> scalar_type { return scalar_type( std::log(a) / c_log2e); } );
                }

                static vector_type sqrt(vector_type x)
                {
                    return transform(x, std::sqrt );
                }

                static vector_type inversesqrt(vector_type x)
                {
                    return (vector_type(1) /= sqrt(x));
                }

                static vector_type abs(vector_type x)
                {
                    return transform(x, std::abs );
                }

                static vector_type sign(vector_type x)
                {
                    return transform(x,  [](scalar_type a) -> scalar_type { return ( scalar_type(0) < a ) - ( a < scalar_type(0) ); } );
                }

                static vector_type floor(vector_type x)
                {
                    return transform(x, std::floor );
                }

                static vector_type ceil(vector_type x)
                {
                    return transform(x, std::ceil );
                }

                static vector_type fract(vector_type x)
                {
                    return transform(x, [=](scalar_type a) -> scalar_type { return a - std::floor(a); } );
                }

                static vector_type mod(vector_type x, scalar_type y)
                {
                    return mod(x, vector_type(y));
                }

                static vector_type mod(const vector_type& x, const vector_type& y)
                {
                    return (x - y * floor( x / y ));
                }

                static vector_type min(vector_type x, const vector_type& y)
                {
                    const scalar_type& (*func)(const scalar_type&, const scalar_type&) = std::min<scalar_type>;
                    return transform(x, y, func);
                }

                static vector_type min(vector_type x, scalar_type y)
                {
                    return transform(x, [=](scalar_type a) -> scalar_type { return std::min(a, y); } );
                }

                static vector_type max(vector_type x, const vector_type& y)
                {
                    const scalar_type& (*func)(const scalar_type&, const scalar_type&) = std::max<scalar_type>;
                    return transform(x, y, func);
                }

                static vector_type max(vector_type x, scalar_type y)
                {
                    return transform(x, [=](scalar_type a) -> scalar_type { return std::max(a, y); } );
                }

                static vector_type clamp(vector_type x, const vector_type& minVal, const vector_type& maxVal)
                {
                    return max( min(x, maxVal), minVal);
                }

                static vector_type clamp(vector_type x, scalar_type minVal, scalar_type maxVal)
                {
                    return max( min(x, maxVal), minVal);
                }

                static vector_type mix(const vector_type& x, const vector_type& y, const vector_type& a)
                {
                    return x + a * (y - x);
                }

                static vector_type mix(const vector_type& x, const vector_type& y, scalar_type a)
                {
                    return x + a * (y - x);
                }

                static vector_type step(const vector_type& edge, vector_type x)
                {
                    const scalar_type one(1);
                    const scalar_type zero(0);
                    return transform(x, edge, [&](scalar_type a, scalar_type b) -> scalar_type { return a > b ? one : zero; });
                }

                static vector_type step(scalar_type edge, vector_type x)
                {
                    return step(vector_type(edge), x);
                }

                static vector_type smoothstep(vector_type edge0, vector_type edge1, vector_type x)
                {
                    vector_type t = clamp((x - edge0) / (edge1 - edge0), 0, 1);
                    return (t * t * (3 - 2 * t));
                }

                static vector_type smoothstep(scalar_type edge0, scalar_type edge1, vector_type x)
                {
                    vector_type t = clamp((x - edge0) / (edge1 - edge0), 0, 1);
                    return t * t * (3.0 - 2.0 * t);
                }

                static vector_type reflect(vector_type I, vector_type N)
                {
                    return (I - 2 * dot(I, N) * N);
                }

                // Geometric functions
                static scalar_type length(const vector_type& x)
                {
                    scalar_type result = 0;
                    iterate([&](size_t i) -> void { result += x[i] * x[i]; } );
                    return std::sqrt(result);
                }

                static scalar_type distance(vector_type p0 , const vector_type& p1 )
                {
                    return length(p0 -= p1);
                }

                static scalar_type dot(const vector_type& x, const vector_type& y)
                {
                    scalar_type result = 0;
                    iterate( [&](size_t i) -> void { result += x[i] * y[i]; } );
                    return result;
                }

                static vector_type normalize(vector_type x)
                {
                    x /= length(x);
                    return x;
                }

                static typename std::conditional<Size==3, vector_type, not_available>::type cross(const vector_type& x, const vector_type& y)
                {
                    auto rx = x[1]*y[2] - x[2]*y[1];
                    auto ry = x[2]*y[0] - x[0]*y[2];
                    auto rz = x[0]*y[1] - x[1]*y[0];
                    return vector_type(rx, ry, rz);
                }

                static bool_vector_type lessThan(vector_type x, vector_type y)
                {
                    return construct<bool>([&](size_t i) -> bool { return x[i] < y[i]; });
                }

                static bool_vector_type lessThanEqual(vector_type x, vector_type y)
                {
                    return construct<bool>([&](size_t i) -> bool { return x[i] <= y[i]; });
                }

                static bool_vector_type greaterThan(vector_type x, vector_type y)
                {
                    return construct<bool>([&](size_t i) -> bool { return x[i] > y[i]; });
                }

                static bool_vector_type greaterThanEqual(vector_type x, vector_type y)
                {
                    return construct<bool>([&](size_t i) -> bool { return x[i] >= y[i]; });
                }

                static bool_vector_type equal(vector_type x, vector_type y)
                {
                    return construct<bool>([&](size_t i) -> bool { return x[i] == y[i]; });
                }

                static bool_vector_type notEqual(vector_type x, vector_type y)
                {
                    return construct<bool>([&](size_t i) -> bool { return x[i] != y[i]; });
                }

                static bool any( typename std::conditional<std::is_same<ScalarType, bool>::value, vector_type, not_available>::type x )
                {
                    bool result = false;
                    iterate([&](size_t i) -> void { result |= x[i]; } );
                    return result;
                }

                static bool all( typename std::conditional< std::is_same<scalar_type, bool>::value, vector_type, not_available>::type x )
                {
                    bool result = true;
                    iterate([&](size_t i) -> void { result &= x[i]; } );
                    return result;
                }

                static vector_type not( typename std::conditional< std::is_same<scalar_type, bool>::value, vector_type, not_available>::type x )
                {
                    return construct<bool>([&](size_t i) -> bool { return !x[i]; });
                }
            };
        }
    }
}