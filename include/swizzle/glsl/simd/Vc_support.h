#pragma once

#include <vc/vector.h>
#include <swizzle/glsl/naive/vector_helper.h>

namespace swizzle
{
    namespace glsl
    {
        typedef Vc::float_v float_v;

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
                typedef float_v type;
            };

            typedef detail::vector_base< Size, proxy_generator, data_type > base_type;

            static float_v::VectorType::Base convert_other_scalar_type(const float_v& other)
            {
                return other.data();
            }
        };

        template <typename BaseType, typename T, typename U>
        struct arithmetic_operators : BaseType
        {
            typedef T value_type;
            typedef const T& arg_type;
            typedef const U& other_arg_type;

            inline friend value_type operator+(arg_type a, other_arg_type b)
            {
                return add(a, b);
            }
            inline friend value_type operator+(other_arg_type a, arg_type b)
            {
                return add(b, a);
            }

            inline friend value_type operator-(arg_type a, other_arg_type b)
            {
                return sub(a, b);
            }
            inline friend value_type operator-(other_arg_type a, arg_type b)
            {
                return sub(value_type(a), b);
            }

            inline friend value_type operator*(arg_type a, other_arg_type b)
            {
                return mul(a, b);
            }
            inline friend value_type operator*(other_arg_type a, arg_type b)
            {
                return mul(a, b);
            }

            inline friend value_type operator/(arg_type a, other_arg_type b)
            {
                return div(a, b);
            }
            inline friend value_type operator/(other_arg_type a, arg_type b)
            {
                return div(value_type(a), b);
            }
        };

        template <typename BaseType, typename T>
        struct arithmetic_operators<BaseType, T, T>
        {
            typedef T value_type;
            typedef const T& arg_type;

            friend value_type operator+(arg_type a, arg_type b)
            {
                return add(a, b);
            }
            friend value_type operator-(arg_type a, arg_type b)
            {
                return sub(a, b);
            }
            friend value_type operator*(arg_type a, arg_type b)
            {
                return mul(a, b);
            }
            friend value_type operator/(arg_type a, arg_type b)
            {
                return div(a, b);
            }
        };

        template <typename T, typename PrimitiveType>
        struct maskable_type_wrapper
        {
            T data;
            typedef PrimitiveType primtive_type;

            typedef maskable_type_wrapper this_type;
            typedef const maskable_type_wrapper& arg_type;
            typedef const primtive_type& primitive_arg_type;

            maskable_type_wrapper() {}
            maskable_type_wrapper(const T& data) : data(data) { }

            operator T() const
            {
                return data;
            }

            // assignment -- this is where magic happens

            this_type& operator=(arg_type other)
            {
                if (auto mask = MaskPolicy::getMash())
                {

                }
            }

            // unary operators

            this_type& operator+=(arg_type other)
            {
                return *this = add(*this, other);
            }
            this_type& operator-=(arg_type other)
            {
                return *this = sub(*this, other);
            }
            this_type& operator*=(arg_type other)
            {
                return *this = mul(*this, other);
            }
            this_type& operator/=(arg_type other)
            {
                return *this = div(*this, other);
            }

            this_type& operator+=(primitive_arg_type other)
            {
                return *this = add(*this, other);
            }
            this_type& operator-=(primitive_arg_type other)
            {
                return *this = sub(*this, other);
            }
            this_type& operator*=(primitive_arg_type other)
            {
                return *this = mul(*this, other);
            }
            this_type& operator/=(primitive_arg_type other)
            {
                return *this = div(*this, other);
            }

            // binary operators

            friend this_type operator+(arg_type a, arg_type b)
            {
                return add(a, b);
            }
            friend this_type operator-(arg_type a, arg_type b)
            {
                return sub(a, b);
            }
            friend this_type operator*(arg_type a, arg_type b)
            {
                return mul(a, b);
            }
            friend this_type operator/(arg_type a, arg_type b)
            {
                return div(a, b);
            }

            inline friend this_type operator+(arg_type a, primitive_arg_type b)
            {
                return add(a, b);
            }
            inline friend this_type operator+(primitive_arg_type a, arg_type b)
            {
                return add(b, a);
            }
            inline friend this_type operator-(arg_type a, primitive_arg_type b)
            {
                return sub(a, b);
            }
            inline friend this_type operator-(primitive_arg_type a, arg_type b)
            {
                return sub(value_type(a), b);
            }
            inline friend this_type operator*(arg_type a, primitive_arg_type b)
            {
                return mul(a, b);
            }
            inline friend this_type operator*(primitive_arg_type a, arg_type b)
            {
                return mul(a, b);
            }
            inline friend this_type operator/(arg_type a, primitive_arg_type b)
            {
                return div(a, b);
            }
            inline friend this_type operator/(primitive_arg_type a, arg_type b)
            {
                return div(value_type(a), b);
            }


            template <typename Other>
            static this_type add(arg_type a, Other&& b)
            {
                return a.data + b;
            }
            template <typename Other>
            static this_type sub(arg_type a, Other&& b)
            {
                return a.data - b;
            }
            template <typename Other>
            static this_type mul(arg_type a, Other&& b)
            {
                return a.data * b;
            }
            template <typename Other>
            static this_type div(arg_type a, Other&& b)
            {
                return a.data / b;
            }
        };

    }

    namespace detail
    {
        inline const Vc::float_v& decay(const Vc::float_v& x) { return x; }
        inline Vc::float_v& decay(Vc::float_v& x) { return x; }
        inline Vc::float_v decay(Vc::float_v&& x) { return std::move(x); }

        template <>
        struct get_vector_type_impl<Vc::float_v>
        {
            typedef glsl::vector<Vc::float_v, 1> type;
        };

#ifdef ENABLE_SIMD

        template <>
        struct get_vector_type_impl<float> : get_vector_type_impl<Vc::float_v>
        {};

        template <>
        struct get_vector_type_impl<double> : get_vector_type_impl<Vc::float_v>
        {};

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
}