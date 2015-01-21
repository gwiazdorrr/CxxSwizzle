#pragma once

#include <vc/vector.h>
#include <swizzle/glsl/naive/vector_helper.h>

namespace swizzle
{
    namespace glsl
    {
        typedef Vc::float_v float_v;

       
        template <typename T, typename PrimitiveType>
        struct maskable_type_wrapper
        {
            static const size_t Size = T::Size;

            T data;
            typedef PrimitiveType primtive_type;

            typedef maskable_type_wrapper this_type;
            typedef const maskable_type_wrapper& arg_type;
            typedef const primtive_type& primitive_arg_type;

            maskable_type_wrapper() {}
            maskable_type_wrapper(const T& data) : data(data) { }
            maskable_type_wrapper(const float_v::VectorType::Base& data) : data(data) { }
            maskable_type_wrapper(primitive_arg_type data) : data(data) {}

            explicit operator const T&() const
            {
                return data;
            }

            //operator vector<T, 1>() const
            //{
            //    return data;
            //}

            // assignment -- this is where magic happens

            this_type& operator=(arg_type other)
            {
                data = other.data;
                return *this;
            }

            // functions

            inline friend this_type sin(arg_type x)
            {
                return sin(x.data);
            }
            inline friend this_type cos(arg_type x)
            {
                return cos(x.data);
            }
            inline friend this_type tan(arg_type x)
            {
                return tan(x.data);
            }
            inline friend this_type asin(arg_type x)
            {
                return asin(x.data);
            }
            inline friend this_type acos(arg_type x)
            {
                return acos(x.data);
            }
            inline friend this_type atan(arg_type x)
            {
                return atan(x.data);
            }
            inline friend this_type atan(arg_type y, arg_type x)
            {
                return atan(y.data, x.data);
            }

            inline friend this_type abs(arg_type x)
            {
                return abs(x.data);
            }
            inline friend this_type pow(arg_type x, arg_type n)
            {
                return pow(x.data, n.data);
            }
            inline friend this_type exp(arg_type x)
            {
                return exp(x.data);
            }
            inline friend this_type log(arg_type x)
            {
                return log(x.data);
            }
            inline friend this_type exp2(arg_type x)
            {
                return exp2(x.data);
            }
            inline friend this_type log2(arg_type x)
            {
                return log2(x.data);
            }
            inline friend this_type sqrt(arg_type x)
            {
                return sqrt(x.data);
            }
            inline friend this_type rsqrt(arg_type x)
            {
                return rsqrt(x.data);
            }

            inline friend this_type sign(arg_type x)
            {
                return sign(x.data);
            }
            inline friend this_type fract(arg_type x)
            {
                return fract(x.data);
            }
            inline friend this_type floor(arg_type x)
            {
                return floor(x.data);
            }
            inline friend this_type ceil(arg_type x)
            {
                return ceil(x.data);
            }
            inline friend this_type mod(arg_type x, arg_type y)
            {
                return mod(x.data, y.data);
            }

            inline friend this_type min(arg_type x, arg_type y)
            {
                return x;
            }
            inline friend this_type max(arg_type x, arg_type y)
            {
                return max(x.data, y.data);
            }

            inline friend this_type step(arg_type edge, arg_type x)
            {
                auto result = T::One();
                result.setZero(x.data <= edge.data);
                return result;
            }

            // unary operators

            this_type operator-() const
            {
                return -data;
            }

            this_type& operator+=(arg_type other)
            {
                return *this = *this + other;
            }
            this_type& operator-=(arg_type other)
            {
                return *this = *this - other;
            }
            this_type& operator*=(arg_type other)
            {
                return *this = *this * other;
            }
            this_type& operator/=(arg_type other)
            {
                return *this = *this / other;
            }

            this_type& operator+=(primitive_arg_type other)
            {
                return *this = *this + other;
            }
            this_type& operator-=(primitive_arg_type other)
            {
                return *this = *this - other;
            }
            this_type& operator*=(primitive_arg_type other)
            {
                return *this = *this * other;
            }
            this_type& operator/=(primitive_arg_type other)
            {
                return *this = *this / other;
            }

            // binary operators

            friend this_type operator+(arg_type a, arg_type b)
            {
                return a.data + b.data;
            }
            friend this_type operator-(arg_type a, arg_type b)
            {
                return a.data - b.data;
            }
            friend this_type operator*(arg_type a, arg_type b)
            {
                return a.data * b.data;
            }
            friend this_type operator/(arg_type a, arg_type b)
            {
                return a.data / b.data;
            }

            inline friend this_type operator+(arg_type a, primitive_arg_type b)
            {
                return a.data + b;
            }
            inline friend this_type operator+(primitive_arg_type a, arg_type b)
            {
                return b.data + a;
            }
            inline friend this_type operator-(arg_type a, primitive_arg_type b)
            {
                return a.data - b;
            }
            inline friend this_type operator-(primitive_arg_type a, arg_type b)
            {
                return (T)a - b.data;
            }
            inline friend this_type operator*(arg_type a, primitive_arg_type b)
            {
                return a.data * b;
            }
            inline friend this_type operator*(primitive_arg_type a, arg_type b)
            {
                return b.data * a;
            }
            inline friend this_type operator/(arg_type a, primitive_arg_type b)
            {
                return a.data / b;
            }
            inline friend this_type operator/(primitive_arg_type a, arg_type b)
            {
                return (T)a / b.data;
            }


            inline this_type decay() const
            {
                return *this;
            }
        };

        typedef maskable_type_wrapper<Vc::float_v, float> masked_float_v;

        template <size_t Size>
        struct vector_helper<masked_float_v, Size>
        {
            //! These can be incomplete types at this point.
            typedef std::array<float_v::VectorType::Base, Size> data_type;

            template <size_t... indices>
            struct proxy_generator
            {
                typedef detail::indexed_proxy< vector<masked_float_v, sizeof...(indices)>, data_type, indices...> type;
            };

            //! A factory of 1-component proxies.
            template <size_t x>
            struct proxy_generator<x>
            {
                typedef masked_float_v type;
            };

            typedef detail::vector_base< Size, proxy_generator, data_type > base_type;
            
            typedef Vc::float_v static_at_type;
        };



        template <size_t Size>
        struct vector_helper<float_v, Size>
        {
            //! These can be incomplete types at this point.
            typedef std::array<float_v::VectorType::Base, Size> data_type;

            template <size_t... indices>
            struct proxy_generator
            {
                typedef detail::indexed_proxy< vector<float_v, sizeof...(indices)>, data_type, indices...> type;
            };

            //! A factory of 1-component proxies.
            template <size_t x>
            struct proxy_generator<x>
            {
                typedef masked_float_v type;
            };

            typedef detail::vector_base< Size, proxy_generator, data_type > base_type;
            typedef masked_float_v masked_scalar_type;
            static float_v::VectorType::Base convert_other_scalar_type(const float_v& other)
            {
                return other.data();
            }
        };

    }

    namespace detail
    {
        template <>
        struct get_vector_type_impl<glsl::masked_float_v>
        {
            typedef glsl::vector<glsl::masked_float_v, 1> type;
        };



#ifdef ENABLE_SIMD

        template <>
        struct get_vector_type_impl<float> : get_vector_type_impl<glsl::masked_float_v>
        {};

        template <>
        struct get_vector_type_impl<double> : get_vector_type_impl<glsl::masked_float_v>
        {};

        //template <>
        //struct get_vector_type_impl<float> : get_vector_type_impl<glsl::masked_float_v>
        //{};

        //template <>
        //struct get_vector_type_impl<double> : get_vector_type_impl<glsl::masked_float_v>
        //{};


#endif
    }
}


//! Vc's unary operators are defined in a weird way that
//! doesn't like ADL and can't do the implicit cast from
//! double. Hence this and following operators are
//! defined explicitly.
inline Vc::float_v operator+(float x, Vc::float_v::AsArg y)
{
    return Vc::float_v(x) + y;
}

inline Vc::float_v operator-(float x, Vc::float_v::AsArg y)
{
    return Vc::float_v(x) - y;
}

inline Vc::float_v operator*(float x, Vc::float_v::AsArg y)
{
    return Vc::float_v(x) * y;
}

inline Vc::float_v operator/(float x, Vc::float_v::AsArg y)
{
    return Vc::float_v(x) / y;
}


namespace Vc
{
    //! Vc doesn't come with pow function, so we're gonna go
    //! with the poor man's version of it.
    inline float_v pow(float_v::AsArg x, float_v::AsArg y)
    {
        return exp(y * log(x));
    }

    namespace AVX
    {
        struct tag {};
    }
}