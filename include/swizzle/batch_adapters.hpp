// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <cmath>
#include <cstdint>
#include <swizzle/detail/utils.h>
#include <array>

#define CXXSWIZZLE_FORCE_INLINE

namespace swizzle
{
    struct construct_tag {};

    template <typename TData, typename TPrimitive, size_t... TIndices>
    struct batch_base
    {
        static constexpr size_t size = sizeof...(TIndices);
        using data_type = TData;
        using primitive_type = TPrimitive;

        using storage_type = std::conditional_t< size == 1, data_type, std::array<TData, size> >;

        storage_type storage;

        CXXSWIZZLE_FORCE_INLINE batch_base() noexcept
        {}

        CXXSWIZZLE_FORCE_INLINE batch_base(const typename detail::only_if<size != 1, storage_type&> storage) noexcept
            : storage(storage)
        {}

        CXXSWIZZLE_FORCE_INLINE batch_base(const data_type& d) noexcept
        {
            ((at<TIndices>() = d), ...);
        }

        CXXSWIZZLE_FORCE_INLINE batch_base(const batch_base& other) noexcept
            : storage(other.storage)
        {}

        CXXSWIZZLE_FORCE_INLINE batch_base(detail::only_if<!std::is_same_v<primitive_type, data_type>, primitive_type> value) noexcept
            : batch_base(batch_scalar_cast(value))
        {}

        template <typename TOtherBatch, typename TOtherPrimitive>
        CXXSWIZZLE_FORCE_INLINE explicit batch_base(const batch_base<TOtherBatch, TOtherPrimitive, TIndices...>& other) noexcept
        {
            ((at<TIndices>() = batch_cast<primitive_type>(other.template at<TIndices>())), ...);
        }

        template <typename... TTypes>
        CXXSWIZZLE_FORCE_INLINE explicit batch_base(construct_tag, TTypes&& ... values) noexcept
        {
            static_assert(sizeof...(TTypes) == sizeof...(TIndices));
            construct<0>(std::forward<TTypes>(values)...);
        }

        friend CXXSWIZZLE_FORCE_INLINE void load_aligned(batch_base& target, const TPrimitive* ptr) noexcept
        {
            target.load_aligned_internal(ptr);
        }

        friend CXXSWIZZLE_FORCE_INLINE void store_aligned(const batch_base& target, TPrimitive* ptr) noexcept
        {
            target.store_aligned_internal(ptr);
        }

        template <size_t TIndex, size_t TSize = size>
        CXXSWIZZLE_FORCE_INLINE std::enable_if_t< (TSize > 1), data_type>& at() noexcept
        {
            return storage[TIndex];
        }

        template <size_t TIndex, size_t TSize = size>
        CXXSWIZZLE_FORCE_INLINE std::enable_if_t< (TSize == 1), data_type>& at() noexcept
        {
            return storage;
        }

        template <size_t TIndex, size_t TSize = size>
        CXXSWIZZLE_FORCE_INLINE const std::enable_if_t< (TSize > 1), data_type>& at() const noexcept
        {
            return storage[TIndex];
        }

        template <size_t TIndex, size_t TSize = size>
        CXXSWIZZLE_FORCE_INLINE const std::enable_if_t< (TSize == 1), data_type>& at() const noexcept
        {
            return storage;
        }

        CXXSWIZZLE_FORCE_INLINE void assign(const batch_base& other) noexcept
        {
            (batch_masked_assign(at<TIndices>(), other.at<TIndices>()), ...);
        }

        CXXSWIZZLE_FORCE_INLINE void assign_fast(const batch_base& other) noexcept
        {
            ((at<TIndices>() = other.at<TIndices>()), ...);
        }

        template <size_t TIndex>
        CXXSWIZZLE_FORCE_INLINE void assign_at(const data_type& other) noexcept
        {
            batch_masked_assign(at<TIndex>(), other);
        }

        template <size_t TIndex>
        CXXSWIZZLE_FORCE_INLINE void assign_at_fast(const data_type& other) noexcept
        {
            at<TIndex>() = other;
        }

    private:
        template <size_t TIndex, typename... TTypes>
        CXXSWIZZLE_FORCE_INLINE void construct(const data_type& first, TTypes&& ... others) noexcept
        {
            at<TIndex>() = first;
            construct<TIndex + 1>(std::forward<TTypes>(others)...);
        }

        template <size_t TIndex>
        CXXSWIZZLE_FORCE_INLINE void construct() noexcept
        {}

        CXXSWIZZLE_FORCE_INLINE void load_aligned_internal(const TPrimitive* ptr) noexcept
        {
            (batch_load_aligned(at<TIndices>(), ptr + TIndices * sizeof(data_type) / sizeof(primitive_type)), ...);
        }

        CXXSWIZZLE_FORCE_INLINE void store_aligned_internal(TPrimitive* ptr) const noexcept
        {
            (batch_store_aligned(at<TIndices>(), ptr + TIndices * sizeof(data_type) / sizeof(primitive_type)), ...);
        }
    };


    template <typename TData, size_t... TIndices>
    struct bool_batch : batch_base<TData, bool, TIndices...>
    {
        using base_type = batch_base<TData, bool, TIndices...>;
        using batch_base<TData, bool, TIndices...>::batch_base;
        using this_type = bool_batch;
        using this_arg = const this_type&;
        using primitive_type = typename base_type::primitive_type;

        bool_batch(bool b) noexcept : bool_batch(batch_scalar_cast(b)) {}

        CXXSWIZZLE_FORCE_INLINE this_type& operator=(this_arg other)& noexcept
        {
            this->assign(other);
            return *this;
        }

        CXXSWIZZLE_FORCE_INLINE this_type& operator=(this_arg other)&& noexcept
        {
            this->assign_fast(other);
            return *this;
        }

        // needed to avoid template as qualifier
        template <size_t TIndex>
        CXXSWIZZLE_FORCE_INLINE const typename base_type::data_type& at() const noexcept
        {
            return base_type::template at<TIndex>();
        }

        // for CxxSwizzle ADL-magic
        CXXSWIZZLE_FORCE_INLINE this_type decay() const noexcept
        {
            return *this;
        }

        // implement if you really need to.
        CXXSWIZZLE_FORCE_INLINE explicit operator bool() const { return (batch_collapse(at<TIndices>()) || ...); }

        CXXSWIZZLE_FORCE_INLINE explicit operator TData() const { return at<0>(); }


        CXXSWIZZLE_FORCE_INLINE friend this_type operator||(this_arg a, this_arg b) { return this_type(construct_tag{}, a.at<TIndices>() || b.at<TIndices>()...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type operator&&(this_arg a, this_arg b) { return this_type(construct_tag{}, a.at<TIndices>() && b.at<TIndices>()...); }
        CXXSWIZZLE_FORCE_INLINE this_type operator!() const { return this_type(construct_tag{}, !at<TIndices>()...); }
    };


    template <typename TData, typename TBool, size_t... TIndices>
    struct int_batch : batch_base<TData, int, TIndices...>
    {
        using base_type = batch_base<TData, int, TIndices...>;
        using batch_base<TData, int, TIndices...>::batch_base;
        using this_type = int_batch;
        using this_arg = const this_type&;
        using primitive_type = typename base_type::primitive_type;
        using bool_type = bool_batch<TBool, TIndices...>;

        explicit int_batch(double value) : int_batch(static_cast<int>(value)) {}

        // needed to avoid template as qualifier
        template <size_t TIndex>
        CXXSWIZZLE_FORCE_INLINE const typename base_type::data_type& at() const
        {
            return base_type::template at<TIndex>();
        }

        // for CxxSwizzle ADL-magic
        CXXSWIZZLE_FORCE_INLINE this_type decay() const
        {
            return *this;
        }

#include <swizzle/detail/common_batch_operators.hpp>

        // this type specific operators
        CXXSWIZZLE_FORCE_INLINE this_type& operator%=(this_arg other) { return *this = *this % other; }
        CXXSWIZZLE_FORCE_INLINE this_type& operator^=(this_arg other) { return *this = *this ^ other; }

        // functions: 8.3
        CXXSWIZZLE_FORCE_INLINE friend this_type min(this_arg x, this_arg y) { return this_type(construct_tag{}, min(x.at<TIndices>(), y.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type max(this_arg x, this_arg y) { return this_type(construct_tag{}, max(x.at<TIndices>(), y.at<TIndices>())...); }


        template <typename TOtherBatch, typename TOtherPrimitive, typename... TOtherTypes>
        CXXSWIZZLE_FORCE_INLINE friend void masked_read(const this_type& mask, ::swizzle::batch_base<TOtherBatch, TOtherPrimitive, TIndices...>& result, TOtherTypes&&... others)
        {
            batch_masked_read(mask.storage, result.storage, others.storage...);
        }
    };

    template <typename TData, typename TBool, size_t... TIndices>
    struct uint_batch : batch_base<TData, uint32_t, TIndices...>
    {
        using base_type = batch_base<TData, uint32_t, TIndices...>;
        using batch_base<TData, uint32_t, TIndices...>::batch_base;
        using this_type = uint_batch;
        using this_arg = const this_type&;
        using primitive_type = typename base_type::primitive_type;
        using bool_type = bool_batch<TBool, TIndices...>;

        // needed to avoid template as qualifier
        template <size_t TIndex>
        CXXSWIZZLE_FORCE_INLINE const typename base_type::data_type& at() const
        {
            return base_type::template at<TIndex>();
        }

        // for CxxSwizzle ADL-magic
        CXXSWIZZLE_FORCE_INLINE this_type decay() const
        {
            return *this;
        }

#include <swizzle/detail/common_batch_operators.hpp>

        // this type specific operators
        CXXSWIZZLE_FORCE_INLINE this_type& operator%=(this_arg other) { return *this = *this % other; }
        CXXSWIZZLE_FORCE_INLINE this_type& operator^=(this_arg other) { return *this = *this ^ other; }

        // functions: 8.3
        CXXSWIZZLE_FORCE_INLINE friend this_type min(this_arg x, this_arg y) { return this_type(construct_tag{}, min(x.at<TIndices>(), y.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type max(this_arg x, this_arg y) { return this_type(construct_tag{}, max(x.at<TIndices>(), y.at<TIndices>())...); }

        template <typename TOtherBatch, typename TOtherPrimitive, typename... TOtherTypes>
        CXXSWIZZLE_FORCE_INLINE friend void masked_read(const this_type& mask, ::swizzle::batch_base<TOtherBatch, TOtherPrimitive, TIndices...>& result, TOtherTypes&&... others)
        {
            batch_masked_read(mask.storage, result.storage, others.storage...);
        }
    };


    template <typename TData, typename TBool, size_t... TIndices>
    struct float_batch : batch_base<TData, float, TIndices...>
    {
        using base_type = batch_base<TData, float, TIndices...>;
        using batch_base<TData, float, TIndices...>::batch_base;
        using this_type = float_batch;
        using this_arg = const this_type&;
        using primitive_type = typename base_type::primitive_type;
        using data_type = typename base_type::data_type;
        using bool_type = bool_batch<TBool, TIndices...>;


        CXXSWIZZLE_FORCE_INLINE float_batch(double value) : float_batch(batch_scalar_cast(static_cast<float>(value))) {}
        CXXSWIZZLE_FORCE_INLINE float_batch(const bool_type& value) : base_type(value) {}
        CXXSWIZZLE_FORCE_INLINE explicit float_batch(int32_t value) : float_batch(static_cast<float>(value)) {}
        CXXSWIZZLE_FORCE_INLINE explicit float_batch(uint32_t value) : float_batch(static_cast<float>(value)) {}

        // needed to avoid template as qualifier
        template <size_t TIndex>
        CXXSWIZZLE_FORCE_INLINE const typename base_type::data_type& at() const
        {
            return base_type::template at<TIndex>();
        }

        // for CxxSwizzle ADL-magic
        CXXSWIZZLE_FORCE_INLINE this_type decay() const
        {
            return *this;
        }

        friend CXXSWIZZLE_FORCE_INLINE void store_rgba8_aligned(this_arg r, this_arg g, this_arg b, this_arg a, uint8_t* ptr, size_t pitch)
        {
            store_rgba8_aligned_internal(r, g, b, a, ptr, pitch);
        }

        static CXXSWIZZLE_FORCE_INLINE void store_rgba8_aligned_internal(this_arg r, this_arg g, this_arg b, this_arg a, uint8_t* ptr, size_t pitch)
        {
            ((ptr = batch_store_rgba8_aligned(r.at<TIndices>(), g.at<TIndices>(), b.at<TIndices>(), a.at<TIndices>(), ptr, pitch)), ...);
        }

        friend CXXSWIZZLE_FORCE_INLINE void store_rgba32f_aligned(this_arg r, this_arg g, this_arg b, this_arg a, uint8_t* ptr, size_t pitch)
        {
            store_rgba32f_aligned_internal(r, g, b, a, ptr, pitch);
        }

        static CXXSWIZZLE_FORCE_INLINE void store_rgba32f_aligned_internal(this_arg r, this_arg g, this_arg b, this_arg a, uint8_t* ptr, size_t pitch)
        {
            ((ptr = batch_store_rgba32f_aligned(r.at<TIndices>(), g.at<TIndices>(), b.at<TIndices>(), a.at<TIndices>(), ptr, pitch)), ...);
        }

#include <swizzle/detail/common_batch_operators.hpp>

        // functions
        // 8.1

        CXXSWIZZLE_FORCE_INLINE friend this_type radians(this_arg degrees) { return this_type(construct_tag{}, radians(degrees.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type degrees(this_arg radians) { return this_type(construct_tag{}, degrees(radians.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type cos(this_arg angle) { return this_type(construct_tag{}, cos(angle.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type tan(this_arg angle) { return this_type(construct_tag{}, tan(angle.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type sin(this_arg angle) { return this_type(construct_tag{}, sin(angle.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type asin(this_arg x) { return this_type(construct_tag{}, asin(x.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type acos(this_arg x) { return this_type(construct_tag{}, acos(x.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type atan(this_arg y_over_x) { return this_type(construct_tag{}, atan(y_over_x.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type atan(this_arg y, this_arg x) { return this_type(construct_tag{}, atan(y.at<TIndices>(), x.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type sinh(this_arg x) { return this_type(construct_tag{}, sinh(x.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type cosh(this_arg x) { return this_type(construct_tag{}, cosh(x.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type tanh(this_arg x) { return this_type(construct_tag{}, tanh(x.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type asinh(this_arg x) { return this_type(construct_tag{}, asinh(x.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type acosh(this_arg x) { return this_type(construct_tag{}, acosh(x.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type atanh(this_arg x) { return this_type(construct_tag{}, atanh(x.at<TIndices>())...); }

        // 8.2

        CXXSWIZZLE_FORCE_INLINE friend this_type pow(this_arg x, this_arg y) { return this_type(construct_tag{}, pow(x.at<TIndices>(), y.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type exp(this_arg x) { return this_type(construct_tag{}, exp(x.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type log(this_arg x) { return this_type(construct_tag{}, log(x.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type exp2(this_arg x) { return this_type(construct_tag{}, exp2(x.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type log2(this_arg x) { return this_type(construct_tag{}, log2(x.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type sqrt(this_arg x) { return this_type(construct_tag{}, sqrt(x.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type inversesqrt(this_arg x) { return this_type(construct_tag{}, inversesqrt(x.at<TIndices>())...); }

        // 8.3

        CXXSWIZZLE_FORCE_INLINE friend this_type abs(this_arg x) { return this_type(construct_tag{}, abs(x.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type sign(this_arg x) { return this_type(construct_tag{}, sign(x.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type floor(this_arg x) { return this_type(construct_tag{}, floor(x.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type trunc(this_arg x) { return this_type(construct_tag{}, trunc(x.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type round(this_arg x) { return this_type(construct_tag{}, round(x.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type roundEven(this_arg x) { return this_type(construct_tag{}, roundEven(x.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type ceil(this_arg x) { return this_type(construct_tag{}, ceil(x.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type fract(this_arg x) { return this_type(construct_tag{}, fract(x.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type mod(this_arg x, this_arg y) { return this_type(construct_tag{}, mod(x.at<TIndices>(), y.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type modf(this_arg x, this_type& i) { return this_type(construct_tag{}, modf(x.at<TIndices>(), i.at<TIndices>())...); }

        CXXSWIZZLE_FORCE_INLINE friend this_type min(this_arg x, this_arg y) { return this_type(construct_tag{}, min(x.at<TIndices>(), y.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type max(this_arg x, this_arg y) { return this_type(construct_tag{}, max(x.at<TIndices>(), y.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type mix(this_arg x, this_arg y, const bool_type& a) { return this_type(construct_tag{}, mix(x.at<TIndices>(), y.at<TIndices>(), a.template at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type step(this_arg edge, this_arg x) { return this_type(construct_tag{}, step(edge.at<TIndices>(), x.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type smoothstep(this_arg edge0, this_arg edge1, this_arg x) { return smoothstep_helper(edge0, edge1, x); }

        CXXSWIZZLE_FORCE_INLINE friend bool_type isnan(this_arg x) { return this_type(construct_tag{}, isnan(x.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend bool_type isinf(this_arg x) { return this_type(construct_tag{}, isinf(x.at<TIndices>())...); }

        //genIType floatBitsToInt(genType value)
        //genUType floatBitsToUint(genType value)

        // 8.4
        CXXSWIZZLE_FORCE_INLINE friend this_type length(this_arg x) { return x; }
        CXXSWIZZLE_FORCE_INLINE friend this_type distance(this_arg p0, this_arg p1) { return abs(p0 - p1); }
        CXXSWIZZLE_FORCE_INLINE friend this_type dot(this_arg x, this_arg y) { return x * y; }
        CXXSWIZZLE_FORCE_INLINE friend this_type normalize(this_arg x) { return step(this_type(construct_tag{}, 0.0f), x) * 2.0f - 1.0f; }
        CXXSWIZZLE_FORCE_INLINE friend this_type faceforward(this_arg N, this_arg I, this_arg Nref) { return (step(this_type(construct_tag{}, 0.0f), dot(Nref, I)) * (-2.0f) + 1.0f) * N; }
        CXXSWIZZLE_FORCE_INLINE friend this_type reflect(this_arg N, this_arg I) { return (I - 2.0f * dot(I, N) * N); }
        CXXSWIZZLE_FORCE_INLINE friend this_type refract(this_arg I, this_arg N, this_arg eta)
        {
            this_type k = 1.0 - eta * eta * (1.0 - dot(N, I) * dot(N, I));
            this_type mask = step(this_type(0), k);
            return mask * (eta * I - (eta * dot(N, I) + sqrt(k)) * N);
        }

        // 8.8
        CXXSWIZZLE_FORCE_INLINE friend this_type dFdx(this_arg p) { return this_type(construct_tag{}, dFdx(p.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type dFdy(this_arg p) { return this_type(construct_tag{}, dFdy(p.at<TIndices>())...); }
        CXXSWIZZLE_FORCE_INLINE friend this_type fwidth(this_arg p)
        {
            return abs(dFdx(p)) + abs(dFdy(p));
        }

        static this_type smoothstep_helper(this_arg edge0, this_arg edge1, this_arg x)
        {
            auto t = (x - edge0) / (edge1 - edge0);
            t = min(max(t, this_type(0.0)), this_type(1.0));
            return t * t * (this_type(3.0) - this_type(2.0) * t);
        }

    };
}