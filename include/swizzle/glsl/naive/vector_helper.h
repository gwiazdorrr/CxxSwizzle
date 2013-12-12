// CxxSwizzle
// Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <array>
#include <swizzle/detail/indexed_proxy.h>
#include <swizzle/detail/vector_base.h>

namespace swizzle
{
    namespace glsl
    {
        namespace naive
        {
            template < class ScalarType, size_t Size >
            class vector;

            //! A type that simplifies creating proxies factories for the vector.
            template <class ScalarType, size_t Size>
            struct vector_helper
            {
                //! These can be incomplete types at this point.
                typedef std::array<ScalarType, Size> data_type;

                template <size_t... indices>
                struct proxy_generator
                {
                    typedef detail::indexed_proxy< vector<ScalarType, sizeof...(indices)>, data_type, indices...> type;
                };

                //! A factory of 1-component proxies.
                template <size_t x>
                struct proxy_generator<x>
                {
                    typedef ScalarType type;
                };

                typedef detail::vector_base< Size, proxy_generator, data_type > base_type;
            };
        }
    }
}