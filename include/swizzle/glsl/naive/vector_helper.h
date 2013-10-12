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
                typedef vector<ScalarType, 2> vec2;
                typedef vector<ScalarType, 3> vec3;
                typedef vector<ScalarType, 4> vec4;
                typedef std::array<ScalarType, Size> data_type;

                //! The proxy factory. Had MSVC had full C++11 support this could be nicely done with
                //! variadic templates, but since that's not the case... it falls back to specialisations
                template <size_t x, size_t y = -1, size_t z = -1, size_t w = -1>
                struct proxy_generator
                {
                    typedef detail::indexed_proxy< vec4, data_type, x, y, z, w> type;
                    static_assert(sizeof(type) == Size * sizeof(ScalarType), "Size of the proxy is not equal to the size of the data");
                };
                //! A factory of 3 component proxies
                template <size_t x, size_t y, size_t z>
                struct proxy_generator<x, y, z, -1>
                {
                    typedef detail::indexed_proxy< vec3, data_type, x, y, z > type;
                    static_assert(sizeof(type) == Size * sizeof(ScalarType), "Size of the proxy is not equal to the size of the data");
                };
                //! A factory of 2-component proxies.
                template <size_t x, size_t y>
                struct proxy_generator<x, y, -1, -1>
                {
                    typedef detail::indexed_proxy< vec2, data_type, x, y > type;
                    static_assert(sizeof(type) == Size * sizeof(ScalarType), "Size of the proxy is not equal to the size of the data");
                };
                //! A factory of 1-component proxies.
                template <size_t x>
                struct proxy_generator<x, -1, -1, -1>
                {
                    typedef ScalarType type;
                };

                typedef detail::vector_base< Size, proxy_generator, data_type > base_type;
            };
        }
    }
}