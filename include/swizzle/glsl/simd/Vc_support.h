#pragma once

// to get rid of automatic mask->bool conversions, define VC_NO_AUTOMATIC_BOOL_FROM_MASK somewhere
#include <vc/vector.h>


#include <swizzle/glsl/naive/vector_helper.h>
#include <swizzle/detail/primitive_wrapper.h>

namespace swizzle
{
    namespace glsl
    {
        namespace Vc
        {
            //! Create a mnemonic for a wrapped float_v.
            template<typename AssignPolicy = detail::nothing>
            using wrapped_float_v = detail::primitive_wrapper < ::Vc::float_v, ::Vc::float_v::EntryType, ::Vc::float_v::VectorType::Base, ::Vc::float_v::Mask, AssignPolicy >;
        }

        template <typename AssignPolicy, size_t Size>
        struct vector_helper<Vc::wrapped_float_v<AssignPolicy>, Size>
        {
            //! Array needs to be like a steak - the rawest possible
            typedef std::array<typename Vc::wrapped_float_v<AssignPolicy>::raw_internal_type, Size> data_type;

            template <size_t... indices>
            struct proxy_generator
            {
                typedef detail::indexed_proxy< vector<Vc::wrapped_float_v<AssignPolicy>, sizeof...(indices)>, data_type, indices...> type;
            };

            //! A factory of 1-component proxies.
            template <size_t x>
            struct proxy_generator<x>
            {
                typedef Vc::wrapped_float_v<AssignPolicy> type;
            };

            typedef detail::vector_base< Size, proxy_generator, data_type > base_type;
        };

    }

    namespace detail
    {
        //! CxxSwizzle needs to know which vector to create if it needs to
        template <typename AssignPolicy>
        struct get_vector_type_impl< ::swizzle::glsl::Vc::wrapped_float_v<AssignPolicy> >
        {
            typedef ::swizzle::glsl::vector<::swizzle::glsl::Vc::wrapped_float_v<AssignPolicy>, 1> type;
        };

        //! ... and if the type decays to something (it doesn't)
        template <typename AssignPolicy>
        inline ::swizzle::glsl::Vc::wrapped_float_v<AssignPolicy> decay(const ::swizzle::glsl::Vc::wrapped_float_v<AssignPolicy>& value)
        {
            return value;
        }
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
#ifdef VC_IMPL_Scalar
    namespace Scalar
#elif defined(VC_IMPL_AVX)
    namespace AVX
#elif defined(VC_IMPL_SSE)
    namespace SSE
#endif
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