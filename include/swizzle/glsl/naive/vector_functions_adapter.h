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
                typedef const VectorType<ScalarType, Size>& vector_arg_type;
                typedef VectorType<bool, Size> bool_vector_type;
                typedef ScalarType scalar_type;
                typedef const ScalarType& scalar_arg_type;

            private:

                struct not_available;

                //! Fires Func for each component an assigns back the result
                template <class Func>
                static vector_type transform(vector_arg_type x, Func func)
                {
                    vector_type result;
                    detail::static_for<0, Size>([&](size_t i) -> void { result[i] = func(x[i]); });
                    return result;
                }

                //! Fires Func for each pair of components an assigns back the result to the x
                template <class Func>
                static vector_type transform(vector_arg_type x, vector_arg_type y, Func func)
                {
                    vector_type result;
                    detail::static_for<0, Size>([&](size_t i) -> void { result[i] = func(x[i], y[i]); });
                    return result;
                }

                //! Helper that enables the use of overloaded functions as an argument.
                static vector_type transformFunc(vector_arg_type x, scalar_type(*func)(scalar_arg_type))
                {
                    vector_type result;
                    detail::static_for<0, Size>([&](size_t i) -> void { result[i] = func(x[i]); });
                    return result;
                }

                //! Helper that enables the use of overloaded functions as an argument.
                static vector_type transformFunc(vector_arg_type x, vector_arg_type y, scalar_type(*func)(scalar_arg_type, scalar_arg_type))
                {
                    vector_type result;
                    detail::static_for<0, Size>([&](size_t i) -> void { result[i] = func(x[i], y[i]); });
                    return result;
                }

                template <class T, class Func>
                static VectorType<T, Size> construct(Func func)
                {
                    VectorType<T, Size> result;
                    iterate([&](size_t i) -> void { result[i] = func(i); });
                    return result;
                }

                template <class Func>
                static void iterate(Func func)
                {
                    detail::static_for<0, Size>([&](size_t i) -> void { func(i); });
                }


            public:

                static vector_type call_radians(vector_arg_type degrees)
                {
                    static const scalar_type deg_to_rad = 3.14159265358979323846 / 180;
                    return transform(degrees, [&](scalar_arg_type a) -> scalar_type { return scalar_type(a * deg_to_rad); });
                }

                static vector_type call_degrees(vector_arg_type radians)
                {
                    static const scalar_type rad_to_deg = 180 / 3.14159265358979323846;
                    return transform(radians, [&](scalar_arg_type a) -> scalar_type { return scalar_type(a * rad_to_deg); });
                }

                #define ADL_FUNC_FORWARD(name)  \
                static vector_type call_##name(vector_arg_type x) \
                {\
                    return transform(x, [=](scalar_arg_type x) { using namespace std; return name(x); }); \
                }

                ADL_FUNC_FORWARD(sin)
                ADL_FUNC_FORWARD(cos)
                ADL_FUNC_FORWARD(abs)
                ADL_FUNC_FORWARD(tan)
                ADL_FUNC_FORWARD(asin)
                ADL_FUNC_FORWARD(acos)
                ADL_FUNC_FORWARD(atan)
                ADL_FUNC_FORWARD(pow)
                ADL_FUNC_FORWARD(exp)
                ADL_FUNC_FORWARD(log)
                ADL_FUNC_FORWARD(exp2)
                ADL_FUNC_FORWARD(log2)
                ADL_FUNC_FORWARD(sqrt)

                ADL_FUNC_FORWARD(floor)
                ADL_FUNC_FORWARD(ceil)


                static vector_type call_pow(vector_arg_type x, vector_arg_type y)
                {
                    return transform(x, y, [=](scalar_arg_type x, scalar_arg_type y) { using namespace std; return pow(x, y); });
                }


                static vector_type call_pow(vector_arg_type x, scalar_arg_type y)
                {
                    return transform(x, [&](scalar_arg_type x) { using namespace std; return pow(x, y); });
                }


                static vector_type call_atan(vector_arg_type y, vector_arg_type x)
                {
                    return transform(y, x, [=](scalar_arg_type y, scalar_arg_type x) { using namespace std; return atan2(y, x); });
                }

                static vector_type call_inversesqrt(vector_arg_type x)
                {
                    using namespace std;
                    return transformFunc(x, rsqrt);
                }


                static vector_type call_sign(vector_arg_type x)
                {
                    using namespace std;
                    return transformFunc(x, sign);
                }

                static vector_type call_fract(vector_arg_type x)
                {
                    using namespace std;
                    return transform(x, [=](scalar_arg_type a) -> scalar_type { return a - floor(a); });
                }

                static vector_type call_mod(vector_arg_type x, scalar_arg_type y)
                {
                    return call_mod(x, vector_type(y));
                }

                static vector_type call_mod(const vector_type& x, const vector_type& y)
                {
                    //return (x - y * call_floor(x / y));
                    vector_type xx = x;
                    vector_type temp = x;
                    temp /= y;
                    temp = call_floor(temp);
                    temp *= y;
                    xx -= temp;
                    return xx;
                }

                static vector_type call_min(vector_arg_type x, const vector_type& y)
                {
                    using namespace std;
                    return transformFunc(x, y, min);
                }

                static vector_type call_min(vector_arg_type x, scalar_arg_type y)
                {
                    return transform(x, [&](scalar_arg_type a) { using namespace std; return min(a, y); });
                }

                static vector_type call_max(vector_arg_type x, vector_arg_type y)
                {
                    using namespace std;
                    return transformFunc(x, y, max);
                }

                static vector_type call_max(vector_arg_type x, scalar_arg_type y)
                {
                    return transform(x, [&](scalar_arg_type a) { using namespace std; return max(a, y); });
                }

                static vector_type call_clamp(vector_arg_type x, const vector_type& minVal, const vector_type& maxVal)
                {
                    return call_max(call_min(x, maxVal), minVal);
                }

                static vector_type call_clamp(vector_arg_type x, scalar_arg_type minVal, scalar_arg_type maxVal)
                {
                    return call_max(call_min(x, maxVal), minVal);
                }

                static vector_type call_mix(const vector_type& x, const vector_type& y, const vector_type& a)
                {
                    return x + a * (y - x);
                }

                static vector_type call_mix(const vector_type& x, const vector_type& y, scalar_arg_type a)
                {
                    vector_type result = y;
                    result -= x;
                    result *= a;
                    result += x;
                    return result;

                    //return x + a * (y - x);
                }

                static vector_type call_step(vector_arg_type edge, vector_arg_type x)
                {
                    const scalar_type one(1);
                    const scalar_type zero(0.0f);
                    return transform(x, edge, [&](scalar_arg_type a, scalar_arg_type b) -> scalar_type { return a > b ? one : zero; });
                }

                static vector_type call_step(scalar_arg_type edge, vector_arg_type x)
                {
                    return call_step(vector_type(edge), x);
                }

                static vector_type call_smoothstep(vector_arg_type edge0, vector_arg_type edge1, vector_arg_type x)
                {
                    vector_type t = call_clamp((x - edge0) / (edge1 - edge0), 0, 1);
                    return (t * t * (3 - 2 * t));
                }

                static vector_type call_smoothstep(scalar_arg_type edge0, scalar_arg_type edge1, vector_arg_type x)
                {
                    vector_type t = x;
                    t -= edge0;

                    vector_type temp = edge1;
                    temp -= edge0;

                    t /= temp;
                    t = call_clamp(t, 0, 1);

                    temp = t;
                    temp *= 2;
                    vector_type temp2 = temp;
                    temp2 -= temp;

                    t *= t;
                    t *= temp2;

                    return t;


                    //vector_type t = call_clamp((x - edge0) / (edge1 - edge0), 0, 1);
                    //return t * t * (3 - 2 * t);
                }

                static vector_type call_reflect(vector_arg_type I, vector_arg_type N)
                {
                    return (I - 2 * call_dot(I, N) * N);
                }

                // Geometric functions
                static scalar_type call_length(const vector_type& x)
                {
                    scalar_type result = 0;
                    iterate([&](size_t i) -> void { result += x[i] * x[i]; });
                    return std::sqrt(result);
                }

                static scalar_type call_distance(vector_arg_type p0, const vector_arg_type p1)
                {
                    return call_length(p0 -= p1);
                }

                static scalar_type call_dot(const vector_type& x, const vector_type& y)
                {
                    scalar_type result = 0;
                    iterate([&](size_t i) -> void { result += x[i] * y[i]; });
                    return result;
                }

                static vector_type call_normalize(vector_arg_type x)
                {
                    return vector_type(x) /= call_length(x);
                }

                static typename std::conditional<Size == 3, vector_type, not_available>::type call_cross(const vector_type& x, const vector_type& y)
                {
                    auto rx = x[1] * y[2] - x[2] * y[1];
                    auto ry = x[2] * y[0] - x[0] * y[2];
                    auto rz = x[0] * y[1] - x[1] * y[0];
                    return vector_type(rx, ry, rz);
                }

                static bool_vector_type call_lessThan(vector_arg_type x, vector_arg_type y)
                {
                    return construct<bool>([&](size_t i) -> bool { return x[i] < y[i]; });
                }

                static bool_vector_type call_lessThanEqual(vector_arg_type x, vector_arg_type y)
                {
                    return construct<bool>([&](size_t i) -> bool { return x[i] <= y[i]; });
                }

                static bool_vector_type call_greaterThan(vector_arg_type x, vector_arg_type y)
                {
                    return construct<bool>([&](size_t i) -> bool { return x[i] > y[i]; });
                }

                static bool_vector_type call_greaterThanEqual(vector_arg_type x, vector_arg_type y)
                {
                    return construct<bool>([&](size_t i) -> bool { return x[i] >= y[i]; });
                }

                static bool_vector_type call_equal(vector_arg_type x, vector_arg_type y)
                {
                    return construct<bool>([&](size_t i) -> bool { return x[i] == y[i]; });
                }

                static bool_vector_type call_notEqual(vector_arg_type x, vector_arg_type y)
                {
                    return construct<bool>([&](size_t i) -> bool { return x[i] != y[i]; });
                }

                static bool call_any(typename std::conditional<std::is_same<ScalarType, bool>::value, vector_arg_type, not_available>::type x)
                {
                    bool result = false;
                    iterate([&](size_t i) -> void { result |= x[i]; });
                    return result;
                }

                static bool call_all(typename std::conditional< std::is_same<scalar_type, bool>::value, vector_arg_type, not_available>::type x)
                {
                    bool result = true;
                    iterate([&](size_t i) -> void { result &= x[i]; });
                    return result;
                }

                static vector_type call_not(typename std::conditional< std::is_same<scalar_type, bool>::value, vector_arg_type, not_available>::type x)
                {
                    return construct<bool>([&](size_t i) -> bool { return !x[i]; });
                }
            };
        }
    }
}