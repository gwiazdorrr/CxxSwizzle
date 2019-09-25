// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

// VC needs to come first or else it's going to complain (damn I hate these)
#include <xsimd/xsimd.hpp>
#include <type_traits>
#include <swizzle/detail/primitive_wrapper.h>
#include <swizzle/glsl/vector_helper.h>


namespace swizzle
{
    namespace glsl
    {
        typedef ::xsimd::simd_type<float> xsimd_float_vector;


        //! Specialise vector_helper so that it knows what to do.
        template <size_t Size>
        struct vector_helper<xsimd_float_vector, Size>
        {
            //! Array needs to be like a steak - the rawest possible
            //! (Wow - I managed to WTF myself upon reading the above after a week or two)
            typedef std::array<xsimd_float_vector, Size> data_type;

            template <size_t... indices>
            struct proxy_generator
            {
                typedef detail::indexed_proxy< vector<xsimd_float_vector, sizeof...(indices)>, data_type, indices...> type;
            };

            //! A factory of 1-component proxies.
            template <size_t x>
            struct proxy_generator<x>
            {
                typedef xsimd_float_vector type;
            };

            typedef detail::vector_base< Size, proxy_generator, data_type > base_type;
        };

    }

    namespace detail
    {
        //! CxxSwizzle needs to know which vector to create if it needs to
        struct get_vector_type_impl< ::swizzle::glsl::xsimd_float_vector >
        {
            typedef ::swizzle::glsl::vector<::swizzle::glsl::xsimd_float_vector, 1> type;
        };
    }
}