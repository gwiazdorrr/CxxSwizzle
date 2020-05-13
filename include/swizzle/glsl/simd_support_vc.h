// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

// VC needs to come first or else it's going to complain (damn I hate these)
#include <Vc/vector.h>
#include <Vc/global.h>
#include <type_traits>
#include <swizzle/detail/primitive_wrapper.h>
#include <swizzle/glsl/vector_helper.h>
#include <swizzle/detail/fwd.h>

namespace swizzle
{
    namespace detail
    {
//#ifdef VC_UNCONDITIONAL_AVX2_INTRINSICS
//        typedef ::Vc_VERSIONED_NAMESPACE::float_v::VectorType::Base raw_simd_type;
//#else
        typedef ::Vc::float_v raw_simd_type;
//#endif

        //! ::Vc::float_v has a tiny bit different semantics than what we need,
        //! so let's wrap it.
        using vc_bool = bool_wrapper<::Vc::float_m>;
        using vc_float = detail::floating_point_wrapper < ::Vc::float_v, ::Vc::float_v::EntryType, vc_bool, nothing >;
        
        template <>
        struct get_vector_type_impl< vc_float >
        {
            typedef ::swizzle::glsl::vector<vc_float, 1> type;
        };

        //template <>
        //struct get_vector_type_impl< raw_simd_type >
        //{
        //    typedef ::swizzle::glsl::vector<vc_float, 1> type;
        //};


        template <>
        struct get_vector_type_impl< vc_bool >
        {
            typedef ::swizzle::glsl::vector<vc_bool, 1> type;
        };


        void convert(const vc_bool& b, vc_float& a)
        {
            ::Vc::float_m mask = static_cast<::Vc::float_m>(b);
            ::Vc::float_v result(::Vc::Zero);
            result(mask) = 1.0f;
            a = result;
        }

        template <size_t Size>
        struct vector_build_info<vc_float, Size> : vector_build_info_base<vc_float, Size, std::array<raw_simd_type, Size>, vc_bool>
        {};

        template <size_t Size>
        struct vector_build_info<vc_bool, Size> : vector_build_info_base<vc_bool, Size, std::array<::Vc::float_m, Size>, vc_bool>
        {};

        //! CxxSwizzle needs to know which vector to create if it needs to

        //template <>
        //struct get_vector_type_impl< ::Vc::float_m >
        //{
        //    typedef ::swizzle::glsl::vector<vc_bool, 1> type;
        //};

        template <>
        constexpr bool is_scalar_floating_point_v<vc_float> = true;


        template <>
        constexpr bool is_scalar_bool_v<vc_bool> = true;

        //! If there's no decay function defined just return same object -- if it is a scalar.
        inline typename vc_bool decay(vc_bool t)
        {
            return t;
        }

    }
}


// Vc generally supports it all, but it lacks some crucial functions.
namespace Vc_VERSIONED_NAMESPACE
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
    inline Vector<T> exp2(const Vector<T>& x)
    {
        //! Vc doesn't come with pow function, so we're gonna go
        //! with the poor man's version of it.
        return exp(2 * log(x));
    }

    template <typename T>
    inline Vector<T> fract(const Vector<T>& x)
    {
        return x - floor(x);
    }

    template <typename T>
    inline Vector<T> inversesqrt(const Vector<T>& x)
    {
        return rsqrt(x);
    }

    template <typename T>
    inline Vector<T> mod(const Vector<T>& x, const Vector<T>& y)
    {
        return rsqrt(x);
    }

    template <typename T>
    inline Vector<T> sign(const Vector<T>& x)
    {
        auto m1 = x > 0;
        auto m2 = x < 0;
        Vector<T> result(::Vc::Zero);
        result(m1) = 1;
        result(m2) = -1;
        return result;
    }

#if Vc_IMPL_AVX 
    template <typename T>
    inline Vector<T> dFdx(const Vector<T>& x)
    {
        // this assumes vectors are row major and there are 2 rows
        auto data = x.data();
        float_v low =  Mem::shuffle<X0, X0, X2, X2, Y4, Y4, Y6, Y6>(data, data);
        float_v high = Mem::shuffle<X1, X1, X3, X3, Y5, Y5, Y7, Y7>(data, data);
        return high - low;
    }

    template <typename T>
    inline Vector<T> dFdy(const Vector<T>& x)
    {
        // this assumes vectors are row major and there are 2 rows
        auto data = x.data();
        float_v low =  Mem::shuffle<X0, X1, X2, X3, Y0, Y1, Y2, Y3>(data, data);
        float_v high = Mem::shuffle<X4, X5, X6, X7, Y4, Y5, Y6, Y7>(data, data);
        return high - low;
    }
#elif Vc_IMPL_SSE
    template <typename T>
    inline Vector<T> dFdx(const Vector<T>& x)
    {
        // this assumes vectors are row major and there are 2 rows
        auto data = x.data();
        float_v low =  Mem::shuffle<X0, X0, Y2, Y2>(data, data);
        float_v high = Mem::shuffle<X1, X1, Y3, Y3>(data, data);
        return high - low;
    }

    template <typename T>
    inline Vector<T> dFdy(const Vector<T>& x)
    {
        // this assumes vectors are row major and there are 2 rows
        auto data = x.data();
        float_v low =  Mem::shuffle<X0, X1, Y0, Y1>(data, data);
        float_v high = Mem::shuffle<X2, X3, Y2, Y3>(data, data);
        return high - low;
    }
#endif
}
