#pragma once

// get's rid of automatic 
//#define VC_NO_AUTOMATIC_BOOL_FROM_MASK
#include <vc/vector.h>


#include <swizzle/glsl/naive/vector_helper.h>
#include <swizzle/detail/primitive_wrapper.h>

namespace swizzle
{
    namespace glsl
    {
        typedef Vc::float_v float_v;

        struct no_assign_policy;

        struct masked_assign_policy
        {
            static void assign(Vc::float_v& target, const Vc::float_v& value);

            typedef Vc::float_v::Mask mask_type;

        };

        

        typedef detail::primitive_wrapper<Vc::float_v, Vc::float_v::EntryType, Vc::float_v::VectorType::Base, Vc::float_v::Mask, detail::nothing> masked_float_v;

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
    }
}


//! Vc's unary operators are defined in a weird way that
//! doesn't like ADL and can't do the implicit cast from
//! double. Hence this and following operators are
//! defined explicitly.
//inline Vc::float_v operator+(float x, Vc::float_v::AsArg y)
//{
//    return Vc::float_v(x) + y;
//}
//
//inline Vc::float_v operator-(float x, Vc::float_v::AsArg y)
//{
//    return Vc::float_v(x) - y;
//}
//
//inline Vc::float_v operator*(float x, Vc::float_v::AsArg y)
//{
//    return Vc::float_v(x) * y;
//}
//
//inline Vc::float_v operator/(float x, Vc::float_v::AsArg y)
//{
//    return Vc::float_v(x) / y;
//}
//

namespace Vc
{
    // Vc generally supports it all...
    namespace AVX
    {
        template <typename T>
        inline Vector<T> step(const Vector<T>& edge, const Vector<T>& x)
        {
            auto result = Vector<T>::One();
            result.setZero(x <= edge);
            return result;
        }

        template <typename T>
        inline Vector<T> pow(const Vector<T>& x, const Vector<T>& n)
        {
            //! Vc doesn't come with pow function, so we're gonna go
            //! with the poor man's version of it.
            return exp(n * log(x));
        }

        template <typename T>
        inline Vector<T> fract(const Vector<T>& x)
        {
            return x - floor(x);
        }
    }
}