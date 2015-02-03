#pragma once

// VC needs to come first or else it's going to complain (damn I hate these)
#include <Vc/vector.h>
#include <type_traits>
#include <swizzle/glsl/vector_helper.h>


namespace swizzle
{
    namespace glsl
    {
        template <typename ScalarType, size_t Size>
        struct vector_helper<::Vc::Vector<ScalarType>, Size>
        {
            //! Array needs to be like a steak - the rawest possible
            //! (Wow - I managed to WTF myself upon reading the above after a week or two)
#ifdef VC_UNCONDITIONAL_AVX2_INTRINSICS
            typedef std::array<typename ::Vc::Vector<ScalarType>::VectorType::Base, Size> data_type;
#else
            typedef std::array<typename ::Vc::Vector<ScalarType>::VectorType, Size> data_type;
#endif

            template <size_t... indices>
            struct proxy_generator
            {
                typedef detail::indexed_proxy< vector<::Vc::Vector<ScalarType>, sizeof...(indices)>, data_type, indices...> type;
            };

            //! A factory of 1-component proxies.
            template <size_t x>
            struct proxy_generator<x>
            {
                typedef ::Vc::Vector<ScalarType> type;
            };

            typedef detail::vector_base< Size, proxy_generator, data_type > base_type;
        };
    }

    namespace detail
    {
        //! CxxSwizzle needs to know which vector to create if it needs to
        template <typename ScalarType>
        struct get_vector_type_impl< ::Vc::Vector<ScalarType> >
        {
            typedef ::swizzle::glsl::vector<::Vc::Vector<ScalarType>, 1> type;
        };

        //! For some ADL magic.
        inline ::Vc::float_v decay(const Vc::float_v& arg)
        {
            return arg;
        }
    }
}

// Vc generally supports it all, but it lacks some crucial functions.
// Also some binary operators need to be mention explicitly for ADL
// to kick in when there's implicit conversion going on.

// Also do keep in mind that Vc version needs to come with implicit
// from scalar constructors, otherwise it's just not going to work.

namespace Vc
{
    
#ifdef VC_IMPL_Scalar
    namespace Scalar
#elif defined(VC_IMPL_AVX)
    namespace AVX
#elif defined(VC_IMPL_SSE)
    namespace SSE
#endif
    {
        inline float_v operator+(float f, float_v::AsArg v)
        {
            return v + f;
        }

        inline float_v operator-(float f, float_v::AsArg v)
        {
            return float_v{ f } -v;
        }

        inline float_v operator*(float f, float_v::AsArg v)
        {
            return v * f;
        }

        inline float_v operator/(float f, float_v::AsArg v)
        {
            return float_v{ f } /v;
        }

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