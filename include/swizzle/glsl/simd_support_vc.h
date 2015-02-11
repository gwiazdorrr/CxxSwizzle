// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

// VC needs to come first or else it's going to complain (damn I hate these)
#include <Vc/vector.h>
#include <type_traits>
#include <swizzle/detail/primitive_wrapper.h>
#include <swizzle/glsl/vector_helper.h>


namespace swizzle
{
    namespace glsl
    {
#ifdef VC_UNCONDITIONAL_AVX2_INTRINSICS
        typedef ::Vc::float_v::VectorType::Base raw_simd_type;
#else
        typedef ::Vc::float_v::VectorType raw_simd_type;
#endif

        //! ::Vc::float_v has a tiny bit different semantics than what we need,
        //! so let's wrap it.
        template<typename BoolType = ::Vc::float_m, typename AssignPolicy = detail::nothing>
        using vc_float = detail::primitive_wrapper < ::Vc::float_v, ::Vc::float_v::EntryType, BoolType, AssignPolicy >;


        //! Specialise vector_helper so that it knows what to do.
        template <typename BoolType, typename AssignPolicy, size_t Size>
        struct vector_helper<vc_float<BoolType, AssignPolicy>, Size>
        {
            //! Array needs to be like a steak - the rawest possible
            //! (Wow - I managed to WTF myself upon reading the above after a week or two)
            typedef std::array<raw_simd_type, Size> data_type;

            template <size_t... indices>
            struct proxy_generator
            {
                typedef detail::indexed_proxy< vector<vc_float<BoolType, AssignPolicy>, sizeof...(indices)>, data_type, indices...> type;
            };

            //! A factory of 1-component proxies.
            template <size_t x>
            struct proxy_generator<x>
            {
                typedef vc_float<BoolType, AssignPolicy> type;
            };

            typedef detail::vector_base< Size, proxy_generator, data_type > base_type;
        };

    }

    namespace detail
    {
        //! CxxSwizzle needs to know which vector to create if it needs to
        template <typename BoolType, typename AssignPolicy>
        struct get_vector_type_impl< ::swizzle::glsl::vc_float<BoolType, AssignPolicy> >
        {
            typedef ::swizzle::glsl::vector<::swizzle::glsl::vc_float<BoolType, AssignPolicy>, 1> type;
        };
    }
}

// Vc generally supports it all, but it lacks some crucial functions.

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