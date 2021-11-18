// CxxSwizzle (c) 2013-2021 Piotr Gwiazdowski
#pragma once

// turn of some annoying VC warnings
#pragma warning(push)
#pragma warning(disable: 4244)
#pragma warning(disable: 4146)
// VC needs to come first or else it's going to complain (damn I hate these)
#include <Vc/vector.h>
#include <Vc/global.h>
#pragma warning(pop)

#include <type_traits>
#include <iostream>

namespace swizzle
{
    // needed for the batch_write_mask to work
    inline bool batch_collapse(const ::Vc::float_m& value) noexcept
    {
        return value.isNotEmpty();
    }

    template <typename U, typename T>
    inline U bit_cast(T src)
    {
        return ::Vc::reinterpret_components_cast<U>(src);
    }
}

#include <swizzle/detail/batch_write_mask.hpp>

namespace swizzle
{
    // free functions needed for wrappers to work
#ifdef CXXSWIZZLE_SIMD_MASKING
#define CXXSWIZZLE_INTERNAL_BATCH_WRITE_MASK_TYPE detail::batch_write_mask<::Vc::float_m, 16, false>

    template <typename T>
    inline void batch_masked_assign(::Vc::Vector<T>& target, const ::Vc::Vector<T>& src) noexcept
    {
        using write_mask = detail::batch_write_mask<::Vc::float_m, 16, false>;
        target(simd_cast<typename ::Vc::Vector<T>::MaskType>(write_mask::storage.masks[write_mask::storage.mask_index])) = src;
    }

    template <typename T>
    inline void batch_masked_assign(::Vc::Mask<T>& target, const ::Vc::Mask<T>& src) noexcept
    {
        using write_mask = detail::batch_write_mask<::Vc::float_m, 16, false>;
        auto m = simd_cast<typename ::Vc::Vector<T>::MaskType>(write_mask::storage.masks[write_mask::storage.mask_index]);
        target = (target & (!m)) | (src & m);
    }

#else
    template <typename T>
    inline void batch_masked_assign(::Vc::Vector<T>& target, const ::Vc::Vector<T>& src) noexcept
    {
        target = src;
    }

    template <typename T>
    inline void batch_masked_assign(::Vc::Mask<T>& target, const ::Vc::Mask<T>& src) noexcept
    {
        target = src;
    }
#endif



    template <typename To, typename From>
    inline auto batch_cast(const ::Vc::Mask<From>& value) noexcept
    {
        if constexpr (std::is_same_v<To, bool>)
        {
            return ::Vc::simd_cast<::Vc::float_m>(value);
        }
        else
        {
            ::Vc::Vector<To> result(::Vc::Zero);
            result(::Vc::simd_cast<result.mask_type>(value)) = ::Vc::One;
            return result;
        }
    }


    template <typename To, typename From>
    inline ::Vc::Vector<To> batch_cast(const ::Vc::Vector<From>& value) noexcept
    {
        return simd_cast<::Vc::Vector<To>>(value);
    }

    template <typename T>
    inline ::Vc::Vector<T> batch_scalar_cast(T value) noexcept
    {
        return ::Vc::Vector<T>(value);
    }

    inline ::Vc::float_m batch_scalar_cast(bool value) noexcept
    {
        return ::Vc::float_m(value);
    }

    template <typename T>
    inline void batch_load_aligned(::Vc::Vector<T>& value, const T* data) noexcept
    {
        value.load(data, Vc::Aligned);
    }

    template <typename T>
    inline void batch_store_aligned(const ::Vc::Vector<T>& value, T* data) noexcept
    {
        value.store(data, Vc::Aligned);
    }

    template <size_t TIndex, typename TResult>
    inline void batch_masked_read_internal(const ::Vc::Vector<TResult>& mask, ::Vc::Vector<TResult>& result) noexcept
    {}

    template <size_t TIndex, typename TResult, typename... TTypes>
    inline void batch_masked_read_internal(const ::Vc::Vector<TResult>& mask, ::Vc::Vector<TResult>& result, const ::Vc::Vector<TResult>& vec, TTypes&&... types) noexcept
    {
        result.assign(vec, (mask == TIndex));
        batch_masked_read_internal<TIndex + 1>(mask, result, std::forward<TTypes>(types)...);
    }

    template <typename TMask, typename TResult, typename... TTypes>
    inline void batch_masked_read(const ::Vc::Vector<TMask>& mask, ::Vc::Vector<TResult>& result, TTypes&&... types) noexcept
    {
        batch_masked_read_internal<0, TResult>(simd_cast<::Vc::Vector<TResult>>(mask), result, std::forward<TTypes>(types)...);
    }


    inline uint8_t* batch_store_rgba8_aligned(const ::Vc::float_v& r, const ::Vc::float_v& g, const ::Vc::float_v& b, const ::Vc::float_v& a, uint8_t* ptr, size_t pitch) noexcept
    {
        using namespace Vc;

        uint16_v sr = simd_cast<uint16_v>(max(0.0f, min(255.0f, r * 256.0f)));
        uint16_v sg = simd_cast<uint16_v>(max(0.0f, min(255.0f, g * 256.0f)));
        uint16_v sb = simd_cast<uint16_v>(max(0.0f, min(255.0f, b * 256.0f)));
        uint16_v sa = simd_cast<uint16_v>(max(0.0f, min(255.0f, a * 256.0f)));

        uint16_v srg = sr | (sg << 8);
        uint16_v sba = sb | (sa << 8);

        uint16_v srgba = srg.interleaveLow(sba);
        auto data = srgba.data();
#if Vc_IMPL_AVX2 
        static_assert(uint16_v::Size == 16);
        SSE::uint_v(AVX::lo128(data)).store(reinterpret_cast<uint16_t*>(ptr));
        SSE::uint_v(AVX::hi128(data)).store(reinterpret_cast<uint16_t*>(ptr + pitch));
        return ptr + 16;
#else
        static_assert(uint16_v::Size == 8);
        auto i1 = _mm_extract_epi64(data, 0);
        auto i2 = _mm_extract_epi64(data, 1);
        *reinterpret_cast<::int64_t*>(ptr) = i1;
        *reinterpret_cast<::int64_t*>(ptr + pitch) = i2;
        return ptr + 8;
#endif
    }

    inline uint8_t* batch_store_rgba32f_aligned(const ::Vc::float_v& r, const ::Vc::float_v& g, const ::Vc::float_v& b, const ::Vc::float_v& a, uint8_t* ptr, size_t pitch) noexcept
    {
        using namespace Vc;

        float_v rbrb = r.interleaveLow(b); // r0b0r1b1...
        float_v gaga = b.interleaveLow(a); // g0a0g1a1...

        float* p = reinterpret_cast<float*>(ptr);
        rbrb.interleaveLow(gaga).store(p, Aligned); // r0g0b0a0...
        rbrb.interleaveHigh(gaga).store(p + float_v::Size, Aligned);

        rbrb = r.interleaveHigh(g);
        gaga = b.interleaveHigh(a);
        
        p = reinterpret_cast<float*>(ptr + pitch);
        rbrb.interleaveLow(gaga).store(p, Aligned);
        rbrb.interleaveHigh(gaga).store(p + float_v::Size, Aligned);

        return ptr + sizeof(float) * float_v::Size * 2;
    }
}

// Vc generally supports it all, but it lacks some crucial functions.
namespace Vc_VERSIONED_NAMESPACE
{
    template <typename T>
    inline Vector<T> step(const Vector<T>& edge, const Vector<T>& x) noexcept
    {
        auto result = Vector<T>::One();
        result.setZero(x <= edge);
        return result;
    }

    inline float_v mix(const float_v& x, const float_v& y, const float_m& a) noexcept
    {
        float_v result = x;
        result(a) = y;
        return result;
    }

    template <typename T, typename F> Vc_INTRINSIC Vector<T> apply(const Vector<T>& x, const Vector<T>& y, F&& f)
    {
        Vector<T> r;
        ::Vc::Common::for_all_vector_entries<x.Size>(
            [&](size_t i) {
                r[i] = f(x[i], y[i]);
            }
        );
        return r;
    }


    template <typename T>
    inline Vector<T> pow(const Vector<T>& x, const Vector<T>& n) noexcept
    {
        return apply(x, n, [](float _x, float _n) { return std::pow(_x, _n); });
    }

    template <typename T>
    inline Vector<T> exp2(const Vector<T>& x) noexcept
    {
        return x.apply([](float _x) { return std::exp2(_x); });
    }

    template <typename T>
    inline Vector<T> fract(const Vector<T>& x) noexcept
    {
        return x - floor(x);
    }

    template <typename T>
    inline Vector<T> inversesqrt(const Vector<T>& x) noexcept
    {
        return rsqrt(x);
    }

    template <typename T>
    inline Vector<T> mod(const Vector<T>& x, const Vector<T>& y) noexcept
    { 
        return x - y * floor(x / y);
    }

    template <typename T>
    inline Vector<T> sign(const Vector<T>& x) noexcept
    {
        auto m1 = x > 0;
        auto m2 = x < 0;
        Vector<T> result(::Vc::Zero);
        result(m1) = 1;
        result(m2) = -1;
        return result;
    }

    template <typename T>
    inline Vector<T> atan(const Vector<T>& y, const Vector<T>& x) noexcept
    {
        return atan2(y, x);
    }

    template <typename T>
    inline Vector<T> tan(const Vector<T>& x) noexcept
    {
        Vector<T> sn, cs;
        ::Vc::sincos(x, &sn, &cs);
        return sn / cs;
    }

    //! https://developer.download.nvidia.com/cg/acos.html
    template <typename T>
    inline Vector<T> acos(const Vector<T>& _x) noexcept
    {
        Vector<T> negate = Vector<T>::Zero();
        negate(_x < 0) = 1;
        auto x = abs(_x);
        Vector<T> ret = -0.0187293;
        ret = ret * x;
        ret = ret + 0.0742610f;
        ret = ret * x;
        ret = ret - 0.2121144f;
        ret = ret * x;
        ret = ret + 1.5707288f;
        ret = ret * sqrt(1.0f - x);
        ret = ret - 2 * negate * ret;
        return negate * 3.14159265358979f + ret;
    }


    template <typename T>
    inline Vector<T> degrees(const Vector<T>& x) noexcept
    {
        return  x * (180.0f / 3.14159265358979323846f);
    }

    template <typename T>
    inline Vector<T> radians(const Vector<T>& x) noexcept
    {
        return x * (3.14159265358979323846f / 180.0f);
    }

    template <typename T>
    inline Vector<T> dFdx(const Vector<T>& x) noexcept
    {
        // this assumes vectors are row major and there are 2 rows
        auto data = x.data();
        float_v low = Mem::shuffle<X0, X0, Y2, Y2>(data, data);
        float_v high = Mem::shuffle<X1, X1, Y3, Y3>(data, data);
        return high - low;
    }

    template <typename T>
    inline Vector<T> dFdy(const Vector<T>& x) noexcept
    {
        // this assumes vectors are row major and there are 2 rows
        auto data = x.data();
#if Vc_IMPL_AVX 
        float_v high = ::Vc::Mem::permute128<X0, X0>(data);
        float_v low = ::Vc::Mem::permute128<X1, X1>(data);
#else
        float_v high = Mem::shuffle<X0, X1, Y0, Y1>(data, data);
        float_v low = Mem::shuffle<X2, X3, Y2, Y3>(data, data);
#endif
        return high - low;
    }

    template <typename T>
    std::ostream& operator<<(std::ostream& os, const Vector<T>& v)
    {
        alignas(v.MemoryAlignment) typename Vector<T>::EntryType entries[v.Size];
        v.store(entries);
        os << "{";
        os << entries[0];
        for (int i = 1; i < v.Size; ++i)
        {
            os << ", " << entries[i];
        }
        os << "}";
        return os;
    }
}


#include <swizzle/batch_adapters.hpp>
#include <swizzle/detail/fwd.hpp>
#include <swizzle/detail/simd_mask.h>
#include <swizzle/detail/vector_traits.h>
#include <swizzle/detail/batch_write_mask.hpp>

namespace swizzle
{
    using float_type = float_batch<::Vc::float_v, ::Vc::float_m, 1>;
    using int_type   = int_batch<::Vc::int32_v, ::Vc::float_m, 1>;
    using uint_type  = uint_batch<::Vc::uint32_v, ::Vc::float_m, 1>;
    using bool_type  = bool_batch<::Vc::float_m, 1>;

    // batch types traits definitions
    namespace detail
    {
        template <> struct scalar_type_storage<float_type> { using type = ::Vc::float_v; };
        template <> struct scalar_type_storage<int_type>   { using type = ::Vc::int32_v; };
        template <> struct scalar_type_storage<uint_type>  { using type = ::Vc::uint32_v; };
        template <> struct scalar_type_storage<bool_type>  { using type = ::Vc::float_m; };

        template <> struct scalar_type_align<float_type> : std::integral_constant<size_t, ::Vc::VectorAlignment> {};
        template <> struct scalar_type_align<int_type>   : std::integral_constant<size_t, ::Vc::VectorAlignment> {};
        template <> struct scalar_type_align<uint_type>  : std::integral_constant<size_t, ::Vc::VectorAlignment> {};
        template <> struct scalar_type_align<bool_type>  : std::integral_constant<size_t, ::Vc::VectorAlignment> {};

        static_assert(::Vc::float_v::Size == ::Vc::int_v::Size);
        static_assert(::Vc::float_v::Size == ::Vc::uint_v::Size);
        struct scalar_types_info : scalar_types_info_builder<float_type, int_type, uint_type, bool_type, ::Vc::float_v::Size, float_type::size> {};
    }
}

#include <swizzle/detail/setup_common.hpp>