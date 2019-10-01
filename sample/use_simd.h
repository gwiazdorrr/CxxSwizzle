// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

// VC need to come first or else VC is going to complain.
#include <Vc/vector.h>
#include <swizzle/glsl/simd_support_vc.h>
// need to include scalars as well because we don't need literals
// to use simd (like sin(1))
// #include <swizzle/glsl/scalar_support.h>

#ifdef SAMPLE_USE_SIMD_MASKING

typedef ::swizzle::detail::write_mask_scope<Vc::float_m, 16> pusher;
CXXSWIZZLE_WRITE_MASK_SCOPE_STORAGE(pusher);

#define SIMD_IF(x)    if (pusher current_mask_holder = ::pusher::push(x))
#define SIMD_WHILE(x) while(pusher current_mask_holder = ::pusher::push(x)) 
#define SIMD_CONDITION(x)    pusher current_mask_holder = ::pusher::push(x)
#define SIMD_ELSE     if(::pusher::invert_tag{})

typedef ::swizzle::detail::write_mask_assign_policy<pusher> assign_policy_type;

#else
typedef ::swizzle::default_assign_policy assign_policy_type;

#endif

using batch_float_t = swizzle::detail::vc_float<assign_policy_type>;
using batch_int_t = swizzle::detail::vc_int<assign_policy_type>;
using batch_uint_t = swizzle::detail::vc_uint<assign_policy_type>;
using batch_bool_t = swizzle::detail::vc_bool<assign_policy_type>; 

namespace swizzle
{
    namespace detail
    {
        template <> struct get_vector_type_impl<float> : get_vector_type_impl<batch_float_t> {};
        template <> struct get_vector_type_impl<double> : get_vector_type_impl<batch_float_t> {};
        template <> struct get_vector_type_impl<int> : get_vector_type_impl<batch_int_t> {};
    }
}

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
