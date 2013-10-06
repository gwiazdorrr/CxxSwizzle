//  CxxSwizzle
//  Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr.os@gmail.com>
#pragma once

#include <array>
#include <swizzle/naive/indexed_proxy.h>

namespace swizzle
{
    namespace naive
    {
        template < class TScalar, size_t Size >
        class vector_adapter;

        //! A type that simplifies creating proxies factories for the vector_adapter.
        template <class TScalar, size_t Size>
        struct vector_adapter_helper
        {
            //! It's all right, these can be incomplete types at this point.
            typedef vector_adapter<TScalar, 2> vec2;
            typedef vector_adapter<TScalar, 3> vec3;
            typedef vector_adapter<TScalar, 4> vec4;

            //! The proxy factory. Had MSVC had full C++11 support this could be nicely done with
            //! variadic templates, but since that's not the case... it falls back to specialisations
            template <size_t x, size_t y, size_t z, size_t w>
            struct proxy_factory
            {
                typedef indexed_proxy< vec4, std::array<TScalar, Size>, x, y, z, w> type;
                static_assert(sizeof(type) == Size * sizeof(TScalar), "Size of the proxy is not equal to the size of the data");
            };
            template <size_t x, size_t y, size_t z>
            struct proxy_factory<x, y, z, -1>
            {
                typedef indexed_proxy< vec3, std::array<TScalar, Size>, x, y, z > type;
                static_assert(sizeof(type) == Size * sizeof(TScalar), "Size of the proxy is not equal to the size of the data");
            };
            template <size_t x, size_t y>
            struct proxy_factory<x, y, -1, -1>
            {
                typedef indexed_proxy< vec2, std::array<TScalar, Size>, x, y > type;
                static_assert(sizeof(type) == Size * sizeof(TScalar), "Size of the proxy is not equal to the size of the data");
            };
            template <size_t x>
            struct proxy_factory<x, -1, -1, -1>
            {
                typedef TScalar type;
            };

            typedef detail::vector_base< Size, proxy_factory, std::array<TScalar, Size> > base_type;
        };
    }
}