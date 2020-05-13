// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

// VC need to come first or else VC is going to complain.
#include <Vc/vector.h>
#include <swizzle/glsl/simd_support_vc.h>
// need to include scalars as well because we don't need literals
// to use simd (like sin(1))
#include <swizzle/glsl/scalar_support.h>



#ifndef CXX_SWIZZLE_MASKS_ENABLED
namespace Vc_VERSIONED_NAMESPACE
{
    template <typename MaskType>
    class wrapped_mask
    {
    public:
        typedef MaskType mask_type;
        typedef decltype(std::declval<mask_type>().data()) raw_internal_type;
        typedef wrapped_mask this_type;
        typedef const wrapped_mask& this_arg;

    private:
        mask_type data;

    public:
        wrapped_mask()
        {}

        wrapped_mask(const mask_type& data)
            : data(data)
        { }

        explicit wrapped_mask(bool data)
            : data(data)
        { }

        wrapped_mask(const raw_internal_type& data)
            : data(data)
        { }


        // logic operators

        inline friend this_type operator&(this_arg a, this_arg b)
        {
            return a.data & b.data;
        }
        inline this_type operator!() const
        {
            return !data;
        }

        inline bool operator==(const this_type& other)
        {
            return data == other.data;
        }
        inline bool operator!=(const this_type& other)
        {
            return data != other.data;
        }

        // casts

        //! To avoid ADL-hell, cast is explicit.
        inline explicit operator mask_type() const
        {
            return data;
        }

        //inline explicit operator raw_internal_type() const
        //{
        //    return data.data();
        //}

        // comparisons

        inline bool full() const
        {
            return a.data.isFull();
        }

        inline bool empty() const
        {
            return a.data.isEmpty();
        }

        inline explicit operator bool() const
        {
            return !data.isEmpty();
        }
    };
}

namespace
{
    typedef ::Vc::float_m  raw_mask_type;
    typedef ::Vc::wrapped_mask<raw_mask_type> mask_type;

    static const size_t c_max_depth = 16;

    //! Current mask, ready to use. Faster access than index based.
    thread_local raw_mask_type g_currentMask;

    //! Stack of all masks. When if "pops", it goes back to the previous one.
    thread_local raw_mask_type g_multipliedMasks[c_max_depth];

    //! Need to save unmultiplied ones too, as else needs to do reverse
    //! logical and with the last single mask. Need one less than multiplied masks.
    thread_local raw_mask_type g_singleMasks[c_max_depth - 1];

    //! Current mask position.
    thread_local int g_masksCount = 0;

}

//! A struct that will update the masks when needed.
struct mask_pusher
{
    //! Each thread should call this for masks to be initialised with correct values.
    static void init_for_thread()
    {
        g_currentMask = static_cast<raw_mask_type>(mask_type(true));
        g_multipliedMasks[0] = g_currentMask;
    }

    template <typename T>
    static mask_pusher push(T&& condition)
    {
        return{ static_cast<mask_type>(condition) };
    }

    struct invert {};
    static mask_pusher push(invert)
    {
        return{ !static_cast<const mask_type&>(g_singleMasks[g_masksCount]) };
    }

    struct lask_mask_all_true {};
    static bool push(lask_mask_all_true)
    {
        return static_cast<mask_type>(g_currentMask) == static_cast<mask_type>(true);
    }


    inline mask_pusher(const mask_type& mask)
    {
        assert(g_masksCount < c_max_depth - 1);

        // save the mask for later negation in "else"
        g_singleMasks[g_masksCount] = static_cast<raw_mask_type>(mask);

        // compute and store the new mask
        auto newMask = mask & static_cast<mask_type>(g_currentMask);
        g_currentMask = g_multipliedMasks[++g_masksCount] = static_cast<raw_mask_type>(newMask);
    }

    inline ~mask_pusher()
    {
        // pop!
        assert(g_masksCount > 0);
        g_currentMask = g_multipliedMasks[--g_masksCount];
    }

    inline operator bool() const
    {
        // let the mask type decide what it means by being "true"; for VC this means
        // at least one value being set IIRC
        return static_cast<bool>(static_cast<mask_type>(g_currentMask));
    }
};

//!
struct masked_assign_policy
{
    template<typename T>
    static void assign(T& target, const T& value)
    {
        if (g_masksCount == 0)
            target = value;
        else 
            target(g_currentMask) = value;
    }
};

#define MASKED_IF(x)    if(auto current_mask_holder = mask_pusher::push(x))
#define MASKED_ELSE     if(mask_pusher::invert{})
#define MASKED_WHILE(x) while(auto current_mask_holder = mask_pusher::push(x)) 
#define CONDITION(x)    auto current_mask_holder = mask_pusher::push(x)

typedef swizzle::glsl::vc_float<mask_type, masked_assign_policy> batch_float_t;
#else
typedef swizzle::glsl::vc_float<> batch_float_t;
#endif


typedef batch_float_t::internal_type raw_batch_float_t;
typedef Vc::uint_v raw_batch_uint32_t;

//! By default Vc's masks (result of comparisons) decay to bools but
//! are not implicitly constructible; hence defining bool_type as bool
//! is the safest bet here.
typedef bool bool_type;

static_assert(static_cast<size_t>(raw_batch_float_t::Size) == static_cast<size_t>(raw_batch_uint32_t::Size), "Both float and uint types need to have same number of entries");
const size_t batch_scalar_count = raw_batch_float_t::Size;
const size_t batch_float_align = Vc::VectorAlignment;
const size_t batch_uint32_align = Vc::VectorAlignment;

template <typename T>
inline void batch_store_aligned(const Vc::Vector<T>& value, T* target)
{
    value.store(target, Vc::Aligned);
}

template <typename T>
inline void batch_load_aligned(Vc::Vector<T>& value, const T* data)
{
    value.load(data, Vc::Aligned);
}

template <typename To, typename From>
To batch_cast(const From& value)
{
    return simd_cast<To>(value);
}

