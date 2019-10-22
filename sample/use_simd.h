// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

// VC need to come first or else VC is going to complain.
#include <Vc/vector.h>
#include <swizzle/setup_simd_vc.hpp>
// need to include scalars as well because we don't need literals
// to use simd (like sin(1))
// #include <swizzle/glsl/scalar_support.h>

namespace swizzle
{
    using write_mask = detail::batch_write_mask<::Vc::float_m, 16, false>;

    template <typename T>
    struct batch_assign_policy<::Vc::Vector<T>>
    {
        static void assign(::Vc::Vector<T>& target, const ::Vc::Vector<T>& src)
        {
            //target = src;
            target((typename ::Vc::Vector<T>::MaskType)write_mask::storage.masks[write_mask::storage.mask_index]) = src;
        }
    };

    template <typename T>
    struct batch_assign_policy<::Vc::Mask<T>>
    {
        static void assign(::Vc::Mask<T>& target, const ::Vc::Mask<T>& src)
        {
            target = src;
        }
    };

#define SIMD_IF(x)    if (::swizzle::write_mask current_mask_holder = ::swizzle::write_mask::push(x))
#define SIMD_ELSE     if (::swizzle::write_mask::invert_tag{})
#define SIMD_WHILE(x) while(::swizzle::write_mask current_mask_holder = ::swizzle::write_mask::push(x)) 
#define SIMD_CONDITION(x)    ::swizzle::write_mask current_mask_holder = ::swizzle::write_mask::push(x)

}

#ifdef SAMPLE_USE_SIMD_MASKING

typedef ::swizzle::detail::write_mask_scope<Vc::float_m, 16> pusher;
CXXSWIZZLE_WRITE_MASK_SCOPE_STORAGE(pusher);

#define SIMD_IF(x)    if (pusher current_mask_holder = ::pusher::push(x))
#define SIMD_WHILE(x) while(pusher current_mask_holder = ::pusher::push(x)) 
#define SIMD_CONDITION(x)    pusher current_mask_holder = ::pusher::push(x)
#define SIMD_ELSE     if(::pusher::invert_tag{})

typedef ::swizzle::detail::write_mask_assign_policy<pusher> assign_policy_type;

#else
//typedef ::swizzle::default_assign_policy assign_policy_type;

#endif

using batch_float_t = swizzle::float_type;
using batch_int_t = swizzle::int_type;
using batch_uint_t = swizzle::uint_type;
using batch_bool_t = swizzle::bool_type;

//template <typename T>
//inline void batch_store_aligned(const Vc::Vector<T>& value, T* target)
//{
//    value.store(target, Vc::Aligned);
//}
//
//template <typename T>
//inline void batch_load_aligned(Vc::Vector<T>& value, const T* data)
//{
//    value.load(data, Vc::Aligned);
//}
//
//template <typename To, typename From>
//To batch_cast(const From& value)
//{
//    return ::Vc::simd_cast<To>(value);
//}
//
