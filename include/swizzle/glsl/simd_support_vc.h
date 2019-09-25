// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

// VC needs to come first or else it's going to complain (damn I hate these)
#include <Vc/vector.h>
#include <Vc/global.h>
#include <type_traits>
#include <swizzle/detail/primitive_adapters.h>
#include <swizzle/glsl/vector_helper.h>
#include <swizzle/detail/fwd.h>
#include <swizzle/detail/simd_mask.h>

namespace swizzle
{
    namespace detail
    {
        // batch types definitions

        template <typename AssignPolicy = default_assign_policy>
        using vc_bool = bool_batch<::Vc::float_m, AssignPolicy>;

        template <typename AssignPolicy = default_assign_policy>
        using vc_int = int_batch<::Vc::int32_v, vc_bool<AssignPolicy>, AssignPolicy>;

        template <typename AssignPolicy = default_assign_policy>
        using vc_uint = uint_batch<::Vc::uint32_v, vc_bool<AssignPolicy>, AssignPolicy>;

        template <typename AssignPolicy = default_assign_policy>
        using vc_float = float_batch<::Vc::float_v, vc_bool<AssignPolicy>, AssignPolicy>;


        // batch types traits definitions

        template <typename AssignPolicy>
        struct batch_traits<vc_float<AssignPolicy>> : batch_traits_builder<
            ::Vc::float_v,
            ::Vc::VectorAlignment, ::Vc::float_v::Size,
            vc_bool<AssignPolicy>,
            false, false, true>
        {};

        template <typename AssignPolicy>
        struct batch_traits<vc_int<AssignPolicy>> : batch_traits_builder<
            ::Vc::int32_v,
            ::Vc::VectorAlignment, ::Vc::int32_v::Size,
            vc_bool<AssignPolicy>,
            false, true, false>
        {};

        template <typename AssignPolicy>
        struct batch_traits<vc_uint<AssignPolicy>> : batch_traits_builder<
            ::Vc::uint32_v,
            ::Vc::VectorAlignment, ::Vc::uint32_v::Size,
            vc_bool<AssignPolicy>,
            false, true, false>
        {};

        template <typename AssignPolicy>
        struct batch_traits<vc_bool<AssignPolicy>> : batch_traits_builder<
            ::Vc::float_m,
            ::Vc::VectorAlignment, ::Vc::float_m::Size,
            vc_bool<AssignPolicy>,
            true, false, false>
        {};

        // make sure single batch will get converted to "vec1" to have access to math functions

        template <typename AssignPolicy>
        struct get_vector_type_impl<vc_float<AssignPolicy>> : default_vector_type_impl<vc_float<AssignPolicy>>
        {};
        template <typename AssignPolicy>
        struct get_vector_type_impl<vc_int<AssignPolicy>> : default_vector_type_impl< vc_int<AssignPolicy>>
        {};
        template <typename AssignPolicy>
        struct get_vector_type_impl<vc_uint<AssignPolicy>> : default_vector_type_impl<vc_uint<AssignPolicy>>
        {};
        template <typename AssignPolicy>
        struct get_vector_type_impl<vc_bool<AssignPolicy>> : default_vector_type_impl<vc_bool<AssignPolicy>>
        {};

        // free functions needed for wrappers to work

        template <typename T>
        ::Vc::Vector<T> batch_cast(const ::Vc::float_m& value)
        {
            ::Vc::Vector<T> result(::Vc::Zero);
            result(value) = 1.0f;
            return result;
        }

        template <typename T>
        inline ::Vc::Vector<T> batch_scalar_cast(T value)
        {
            return ::Vc::Vector<T>(value);
        }

        inline ::Vc::float_m batch_scalar_cast(bool value)
        {
            return ::Vc::float_m(value);
        }

        inline bool batch_collapse(const ::Vc::float_m& value)
        {
            return value.isNotEmpty();
        }

        template <typename T>
        inline void batch_load_aligned(::Vc::Vector<T>& value, const T* data)
        {
            value.load(data, Vc::Aligned);
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

    /*inline Vector<T> mix(const Vector<T>& x, const Vector<T>& y, const Vector<T>& a)
    {
        auto result = Vector<T>::One();
        result.setZero(x <= edge);
        return result;
    }*/

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
        return x - y * floor(x / y);
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
        float_v low = Mem::shuffle<X0, X0, X2, X2, Y4, Y4, Y6, Y6>(data, data);
        float_v high = Mem::shuffle<X1, X1, X3, X3, Y5, Y5, Y7, Y7>(data, data);
        return high - low;
    }

    template <typename T>
    inline Vector<T> dFdy(const Vector<T>& x)
    {
        // this assumes vectors are row major and there are 2 rows
        auto data = x.data();
        float_v low = Mem::shuffle<X0, X1, X2, X3, Y0, Y1, Y2, Y3>(data, data);
        float_v high = Mem::shuffle<X4, X5, X6, X7, Y4, Y5, Y6, Y7>(data, data);
        return high - low;
    }
#elif Vc_IMPL_SSE
    template <typename T>
    inline Vector<T> dFdx(const Vector<T>& x)
    {
        // this assumes vectors are row major and there are 2 rows
        auto data = x.data();
        float_v low = Mem::shuffle<X0, X0, Y2, Y2>(data, data);
        float_v high = Mem::shuffle<X1, X1, Y3, Y3>(data, data);
        return high - low;
    }

    template <typename T>
    inline Vector<T> dFdy(const Vector<T>& x)
    {
        // this assumes vectors are row major and there are 2 rows
        auto data = x.data();
        float_v low = Mem::shuffle<X0, X1, Y0, Y1>(data, data);
        float_v high = Mem::shuffle<X2, X3, Y2, Y3>(data, data);
        return high - low;
    }
#endif
}
