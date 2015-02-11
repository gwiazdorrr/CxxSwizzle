// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <cmath>
#include <swizzle/detail/utils.h>

#define CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_V(name) \
    static vector_type call_##name(vector_arg_type x) { return construct_static(functor_##name{}, x); }

#define CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_VV(name) \
    static vector_type call_##name(vector_arg_type x, vector_arg_type y) { return construct_static(functor_##name{}, x, y); }

#define CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_VS(name) \
    static vector_type call_##name(vector_arg_type x, scalar_arg_type y) { return construct_static(functor_##name{}, x, y); }

#define CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_SV(name) \
    static vector_type call_##name(scalar_arg_type x, vector_arg_type y) { return construct_static(functor_##name{}, x, y); }

#define CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_VVV(name) \
    static vector_type call_##name(vector_arg_type x, vector_arg_type y, vector_arg_type z) { return construct_static(functor_##name{}, x, y, z); }

#define CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_VVS(name) \
    static vector_type call_##name(vector_arg_type x, vector_arg_type y, scalar_arg_type z) { return construct_static(functor_##name{}, x, y, z); }

#define CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_VSS(name) \
    static vector_type call_##name(vector_arg_type x, scalar_arg_type y, scalar_arg_type z) { return construct_static(functor_##name{}, x, y, z); }

#define CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_SSV(name) \
    static vector_type call_##name(scalar_arg_type x, scalar_arg_type y, vector_arg_type z) { return construct_static(functor_##name{}, x, y, z); }


namespace swizzle
{
    namespace detail
    {
        namespace glsl
        {
            //! A class providing static functions matching GLSL's vector functions. Uses naive approach, i.e.
            //! everything is done components-wise, using stdlib's math functions.
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

                template <typename Func, typename... Args>
                static vector_type construct_static(Func func, Args&&... args)
                {
                    vector_type result;
                    detail::static_for_with_static_call<0, Size>(func, result, std::forward<Args>(args)...);
                    return result;
                }

                template <class T, class Func>
                static VectorType<T, Size> construct(Func func)
                {
                    VectorType<T, Size> result;
                    detail::static_for<0, Size>([&](size_t i) -> void { result[i] = func(i); });
                    return result;
                }


            private:

                struct functor_radians
                {
                    template <size_t i> void operator()(vector_type& result, vector_arg_type x)
                    {
                        result.at(i) = x.at(i) * scalar_type(3.14159265358979323846 / 180);
                    }
                };

                struct functor_degrees
                {
                    template <size_t i> void operator()(vector_type& result, vector_arg_type x)
                    {
                        result.at(i) = x.at(i) * scalar_type(180 / 3.14159265358979323846);
                    }
                };

                struct functor_mul
                {
                    template <size_t i> void operator()(vector_type& result, vector_arg_type x, scalar_arg_type y)
                    {
                        result.at(i) = x.at(i) * y;
                    }
                };

                struct functor_sin
                {
                    template <size_t i> void operator()(vector_type& result, vector_arg_type x)
                    {
                        using namespace std;
                        result.at(i) = sin(x.at(i));
                    }
                };

                struct functor_cos
                {
                    template <size_t i> void operator()(vector_type& result, vector_arg_type x)
                    {
                        using namespace std;
                        result.at(i) = cos(x.at(i));
                    }
                };

                struct functor_tan
                {
                    template <size_t i> void operator()(vector_type& result, vector_arg_type x)
                    {
                        using namespace std;
                        result.at(i) = tan(x.at(i));
                    }
                };

                struct functor_asin
                {
                    template <size_t i> void operator()(vector_type& result, vector_arg_type x)
                    {
                        using namespace std;
                        result.at(i) = asin(x.at(i));
                    }
                };

                struct functor_acos
                {
                    template <size_t i> void operator()(vector_type& result, vector_arg_type x)
                    {
                        using namespace std;
                        result.at(i) = acos(x.at(i));
                    }
                };

                struct functor_atan
                {
                    template <size_t i> void operator()(vector_type& result, vector_arg_type x)
                    {
                        using namespace std;
                        result.at(i) = atan(x.at(i));
                    }

                    template <size_t i> void operator()(vector_type& result, vector_arg_type y, vector_arg_type x)
                    {
                        using namespace std;
                        result.at(i) = atan2(y.at(i), x.at(i));
                    }
                };

                struct functor_pow
                {
                    template <size_t i> void operator()(vector_type& result, vector_arg_type x, scalar_arg_type y)
                    {
                        using namespace std;
                        result.at(i) = pow(x.at(i), y);
                    }

                    template <size_t i> void operator()(vector_type& result, vector_arg_type x, vector_arg_type y)
                    {
                        using namespace std;
                        result.at(i) = pow(x.at(i), y.at(i));
                    }
                };

                struct functor_abs
                {
                    template <size_t i> void operator()(vector_type& result, vector_arg_type x)
                    {
                        using namespace std;
                        result.at(i) = abs(x.at(i));
                    }
                };

                struct functor_exp
                {
                    template <size_t i> void operator()(vector_type& result, vector_arg_type x)
                    {
                        using namespace std;
                        result.at(i) = exp(x.at(i));
                    }
                };

                struct functor_log
                {
                    template <size_t i> void operator()(vector_type& result, vector_arg_type x)
                    {
                        using namespace std;
                        result.at(i) = log(x.at(i));
                    }
                };

                struct functor_exp2
                {
                    template <size_t i> void operator()(vector_type& result, vector_arg_type x)
                    {
                        using namespace std;
                        result.at(i) = exp2(x.at(i));
                    }
                };

                struct functor_log2
                {
                    template <size_t i> void operator()(vector_type& result, vector_arg_type x)
                    {
                        using namespace std;
                        result.at(i) = log2(x.at(i));
                    }
                };

                struct functor_sqrt
                {
                    template <size_t i> void operator()(vector_type& result, vector_arg_type x)
                    {
                        using namespace std;
                        result.at(i) = sqrt(x.at(i));
                    }
                };

                struct functor_inversesqrt
                {
                    template <size_t i> void operator()(vector_type& result, vector_arg_type x)
                    {
                        using namespace std;
                        result.at(i) = rsqrt(x.at(i));
                    }
                };

                struct functor_sign
                {
                    template <size_t i> void operator()(vector_type& result, vector_arg_type x)
                    {
                        using namespace std;
                        result.at(i) = sign(x.at(i));
                    }
                };

                struct functor_fract
                {
                    template <size_t i> void operator()(vector_type& result, vector_arg_type x)
                    {
                        using namespace std;
                        scalar_type xx = x.at(i);
                        result.at(i) = xx - floor(xx);
                    }
                };

                struct functor_mod
                {
                    template <size_t i> void operator()(vector_type& result, vector_arg_type x, vector_arg_type y)
                    {
                        operator() < i > (result, x, y.at(i));
                    }

                    template <size_t i> void operator()(vector_type& result, vector_arg_type x, scalar_arg_type y)
                    {
                        using namespace std;
                        auto xx = x.at(i);
                        result.at(i) = xx - y * floor(xx / y);
                    }
                };

                struct functor_min
                {
                    template <size_t i> void operator()(vector_type& result, vector_arg_type x, vector_arg_type y)
                    {
                        operator() < i > (result, x, y.at(i));
                    }

                    template <size_t i> void operator()(vector_type& result, vector_arg_type x, scalar_arg_type y)
                    {
                        using namespace std;
                        result.at(i) = min(x.at(i), y);
                    }
                };

                struct functor_max
                {
                    template <size_t i> void operator()(vector_type& result, vector_arg_type x, vector_arg_type y)
                    {
                        operator() < i > (result, x, y.at(i));
                    }

                    template <size_t i> void operator()(vector_type& result, vector_arg_type x, scalar_arg_type y)
                    {
                        using namespace std;
                        result.at(i) = max(x.at(i), y);
                    }
                };

                struct functor_clamp
                {
                    template <size_t i> void operator()(vector_type& result, vector_arg_type x, vector_arg_type a, vector_arg_type b)
                    {
                        operator() < i > (result, x, a.at(i), b.at(i));
                    }

                    template <size_t i> void operator()(vector_type& result, vector_arg_type x, scalar_arg_type a, scalar_arg_type b)
                    {
                        using namespace std;
                        result.at(i) = max(min(x.at(i), b), a);
                    }
                };

                struct functor_mix
                {
                    template <size_t i> void operator()(vector_type& result, vector_arg_type x, vector_arg_type y, vector_arg_type a)
                    {
                        operator() < i > (result, x, y, a.at(i));
                    }

                    template <size_t i> void operator()(vector_type& result, vector_arg_type x, vector_arg_type y, scalar_arg_type a)
                    {
                        using namespace std;
                        result.at(i) = x.at(i) + a * (y.at(i) - x.at(i));
                    }
                };

                struct functor_step
                {
                    template <size_t i> void operator()(vector_type& result, vector_arg_type edge, vector_arg_type x)
                    {
                        operator() < i > (result, edge.at(i), x);
                    }

                    template <size_t i> void operator()(vector_type& result, scalar_arg_type edge, vector_arg_type x)
                    {
                        using namespace std;
                        result.at(i) = step(edge, x.at(i));
                    }
                };


                struct functor_smoothstep
                {
                    template <size_t i> void operator()(vector_type& result, vector_arg_type edge0, vector_arg_type edge1, vector_arg_type x)
                    {
                        operator() < i > (result, edge0.at(i), edge1.at(i), x);
                    }

                    template <size_t i> void operator()(vector_type& result, scalar_arg_type edge0, scalar_arg_type edge1, vector_arg_type x)
                    {
                        using namespace std;
                        auto t = (x.at(i) - edge0) / (edge1 - edge0);
                        t = min(max(t, scalar_arg_type(0)), scalar_arg_type(1));
                        result.at(i) = t * t * (3 - 2 * t);
                    }
                };

                struct functor_dot
                {
                    template <size_t i> void operator()(scalar_type& result, vector_arg_type x, vector_arg_type y)
                    {
                        result += x.at(i) * y.at(i);
                    }
                };

                struct functor_floor
                {
                    template <size_t i> void operator()(vector_type& result, vector_arg_type x)
                    {
                        using namespace std;
                        result.at(i) = floor(x.at(i));
                    }
                };

                struct functor_ceil
                {
                    template <size_t i> void operator()(vector_type& result, vector_arg_type x)
                    {
                        using namespace std;
                        result.at(i) = ceil(x.at(i));
                    }
                };


            public:

                // these functions do component-wise transform

                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_V(degrees)
                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_V(radians)

                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_V(sin)
                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_V(cos)
                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_V(tan)
                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_V(asin)
                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_V(acos)
                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_V(atan)
                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_VV(atan)
                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_V(abs)
                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_VV(pow)
                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_VS(pow)
                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_V(exp)
                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_V(log)
                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_V(exp2)
                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_V(log2)
                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_V(sqrt)
                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_V(inversesqrt)
                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_V(sign)
                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_V(fract)
                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_V(floor)
                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_V(ceil)
                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_VV(mod)
                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_VS(mod)

                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_VV(min)
                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_VS(min)
                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_VV(max)
                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_VS(max)

                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_VVV(clamp)
                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_VSS(clamp)
                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_VVV(mix)
                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_VVS(mix)

                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_VV(step)
                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_SV(step)
                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_VVV(smoothstep)
                CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_SSV(smoothstep)

                // these are more complex

                static vector_type call_reflect(vector_arg_type I, vector_arg_type N)
                {
                    //return (I - 2 * call_dot(I, N) * N);
                    scalar_type dot2 = 2 * call_dot(I, N);
                    vector_type n = N;
                    n *= dot2;

                    vector_type i = I;
                    i -= n;
                    return i;
                }

                // Geometric functions
                static scalar_type call_length(vector_arg_type x)
                {
                    using namespace std;
                    scalar_type dot = call_dot(x, x);
                    return sqrt(dot);
                }

                static scalar_type call_distance(vector_arg_type p0, const vector_arg_type p1)
                {
                    return call_length(p0 - p1);
                }

                static scalar_type call_dot(vector_arg_type x, vector_arg_type y)
                {
                    scalar_type result = 0;
                    detail::static_for_with_static_call<0, Size>(functor_dot{}, result, x, y);
                    return result;
                }

                static vector_type call_normalize(vector_arg_type x)
                {
                    using namespace std;
                    scalar_type dot = call_dot(x, x);
                    return vector_type(x) * rsqrt(dot);
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
                    detail::static_for<0, Size>([&](size_t i) -> void { result |= x[i]; });
                    return result;
                }

                static bool call_all(typename std::conditional< std::is_same<scalar_type, bool>::value, vector_arg_type, not_available>::type x)
                {
                    bool result = true;
                    detail::static_for<0, Size>([&](size_t i) -> void { result &= x[i]; });
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

#undef CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_V
#undef CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_VV
#undef CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_VS
#undef CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_SV
#undef CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_VVV
#undef CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_VVS
#undef CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_VSS
#undef CXXSWIZZLE_DETAIL_SIMPLE_TRANSFORM_SSV