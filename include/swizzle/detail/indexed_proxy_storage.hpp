// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <type_traits>
#include <swizzle/detail/utils.h>
#include <swizzle/detail/vector_traits.h>

namespace swizzle
{
    namespace detail
    {
        template <typename TData, typename TScalar, size_t TSize, size_t... TIndices>
        struct indexed_proxy_storage;


        template< typename TData, typename TScalar>
        struct indexed_proxy_storage<TData, TScalar, 1, 0>
        {
            union
            {
                TData data;
                TScalar x;
            };
        };

        template< typename TData, typename TScalar, size_t TI0>
        struct indexed_proxy_storage<TData, TScalar, 1, TI0>
        {
            union
            {
                TData data;
                struct { TScalar _padding_x[TI0]; TScalar x; };
            };
        };

        template< typename TData, typename TScalar>
        struct indexed_proxy_storage<TData, TScalar, 2, 0, 0>
        {
            union
            {
                TData data;
                TScalar x;
                TScalar y;
            };
        };

        template< typename TData, typename TScalar, size_t TI1>
        struct indexed_proxy_storage<TData, TScalar, 2, 0, TI1>
        {
            union
            {
                TData data;
                TScalar x;
                struct { TScalar _padding_y[TI1]; TScalar y; };
            };
        };

        template< typename TData, typename TScalar, size_t TI0>
        struct indexed_proxy_storage<TData, TScalar, 2, TI0, 0>
        {
            union
            {
                TData data;
                struct { TScalar _padding_x[TI0]; TScalar x; };
                TScalar y;
            };
        };

        template< typename TData, typename TScalar, size_t TI0, size_t TI1>
        struct indexed_proxy_storage<TData, TScalar, 2, TI0, TI1>
        {
            union
            {
                TData data;
                struct { TScalar _padding_x[TI0]; TScalar x; };
                struct { TScalar _padding_y[TI1]; TScalar y; };
            };
        };

        template< typename TData, typename TScalar>
        struct indexed_proxy_storage<TData, TScalar, 3, 0, 0, 0>
        {
            union
            {
                TData data;
                TScalar x;
                TScalar y;
                TScalar z;
            };
        };

        template< typename TData, typename TScalar, size_t TI2>
        struct indexed_proxy_storage<TData, TScalar, 3, 0, 0, TI2>
        {
            union
            {
                TData data;
                TScalar x;
                TScalar y;
                struct { TScalar _padding_z[TI2]; TScalar z; };
            };
        };

        template< typename TData, typename TScalar, size_t TI1>
        struct indexed_proxy_storage<TData, TScalar, 3, 0, TI1, 0>
        {
            union
            {
                TData data;
                TScalar x;
                struct { TScalar _padding_y[TI1]; TScalar y; };
                TScalar z;
            };
        };

        template< typename TData, typename TScalar, size_t TI1, size_t TI2>
        struct indexed_proxy_storage<TData, TScalar, 3, 0, TI1, TI2>
        {
            union
            {
                TData data;
                TScalar x;
                struct { TScalar _padding_y[TI1]; TScalar y; };
                struct { TScalar _padding_z[TI2]; TScalar z; };
            };
        };

        template< typename TData, typename TScalar, size_t TI0>
        struct indexed_proxy_storage<TData, TScalar, 3, TI0, 0, 0>
        {
            union
            {
                TData data;
                struct { TScalar _padding_x[TI0]; TScalar x; };
                TScalar y;
                TScalar z;
            };
        };

        template< typename TData, typename TScalar, size_t TI0, size_t TI2>
        struct indexed_proxy_storage<TData, TScalar, 3, TI0, 0, TI2>
        {
            union
            {
                TData data;
                struct { TScalar _padding_x[TI0]; TScalar x; };
                TScalar y;
                struct { TScalar _padding_z[TI2]; TScalar z; };
            };
        };

        template< typename TData, typename TScalar, size_t TI0, size_t TI1>
        struct indexed_proxy_storage<TData, TScalar, 3, TI0, TI1, 0>
        {
            union
            {
                TData data;
                struct { TScalar _padding_x[TI0]; TScalar x; };
                struct { TScalar _padding_y[TI1]; TScalar y; };
                TScalar z;
            };
        };

        template< typename TData, typename TScalar, size_t TI0, size_t TI1, size_t TI2>
        struct indexed_proxy_storage<TData, TScalar, 3, TI0, TI1, TI2>
        {
            union
            {
                TData data;
                struct { TScalar _padding_x[TI0]; TScalar x; };
                struct { TScalar _padding_y[TI1]; TScalar y; };
                struct { TScalar _padding_z[TI2]; TScalar z; };
            };
        };

        template< typename TData, typename TScalar>
        struct indexed_proxy_storage<TData, TScalar, 4, 0, 0, 0, 0>
        {
            union
            {
                TData data;
                TScalar x;
                TScalar y;
                TScalar z;
                TScalar w;
            };
        };

        template< typename TData, typename TScalar, size_t TI3>
        struct indexed_proxy_storage<TData, TScalar, 4, 0, 0, 0, TI3>
        {
            union
            {
                TData data;
                TScalar x;
                TScalar y;
                TScalar z;
                struct { TScalar _padding_w[TI3]; TScalar w; };
            };
        };

        template< typename TData, typename TScalar, size_t TI2>
        struct indexed_proxy_storage<TData, TScalar, 4, 0, 0, TI2, 0>
        {
            union
            {
                TData data;
                TScalar x;
                TScalar y;
                struct { TScalar _padding_z[TI2]; TScalar z; };
                TScalar w;
            };
        };

        template< typename TData, typename TScalar, size_t TI2, size_t TI3>
        struct indexed_proxy_storage<TData, TScalar, 4, 0, 0, TI2, TI3>
        {
            union
            {
                TData data;
                TScalar x;
                TScalar y;
                struct { TScalar _padding_z[TI2]; TScalar z; };
                struct { TScalar _padding_w[TI3]; TScalar w; };
            };
        };

        template< typename TData, typename TScalar, size_t TI1>
        struct indexed_proxy_storage<TData, TScalar, 4, 0, TI1, 0, 0>
        {
            union
            {
                TData data;
                TScalar x;
                struct { TScalar _padding_y[TI1]; TScalar y; };
                TScalar z;
                TScalar w;
            };
        };

        template< typename TData, typename TScalar, size_t TI1, size_t TI3>
        struct indexed_proxy_storage<TData, TScalar, 4, 0, TI1, 0, TI3>
        {
            union
            {
                TData data;
                TScalar x;
                struct { TScalar _padding_y[TI1]; TScalar y; };
                TScalar z;
                struct { TScalar _padding_w[TI3]; TScalar w; };
            };
        };

        template< typename TData, typename TScalar, size_t TI1, size_t TI2>
        struct indexed_proxy_storage<TData, TScalar, 4, 0, TI1, TI2, 0>
        {
            union
            {
                TData data;
                TScalar x;
                struct { TScalar _padding_y[TI1]; TScalar y; };
                struct { TScalar _padding_z[TI2]; TScalar z; };
                TScalar w;
            };
        };

        template< typename TData, typename TScalar, size_t TI1, size_t TI2, size_t TI3>
        struct indexed_proxy_storage<TData, TScalar, 4, 0, TI1, TI2, TI3>
        {
            union
            {
                TData data;
                TScalar x;
                struct { TScalar _padding_y[TI1]; TScalar y; };
                struct { TScalar _padding_z[TI2]; TScalar z; };
                struct { TScalar _padding_w[TI3]; TScalar w; };
            };
        };

        template< typename TData, typename TScalar, size_t TI0>
        struct indexed_proxy_storage<TData, TScalar, 4, TI0, 0, 0, 0>
        {
            union
            {
                TData data;
                struct { TScalar _padding_x[TI0]; TScalar x; };
                TScalar y;
                TScalar z;
                TScalar w;
            };
        };

        template< typename TData, typename TScalar, size_t TI0, size_t TI3>
        struct indexed_proxy_storage<TData, TScalar, 4, TI0, 0, 0, TI3>
        {
            union
            {
                TData data;
                struct { TScalar _padding_x[TI0]; TScalar x; };
                TScalar y;
                TScalar z;
                struct { TScalar _padding_w[TI3]; TScalar w; };
            };
        };

        template< typename TData, typename TScalar, size_t TI0, size_t TI2>
        struct indexed_proxy_storage<TData, TScalar, 4, TI0, 0, TI2, 0>
        {
            union
            {
                TData data;
                struct { TScalar _padding_x[TI0]; TScalar x; };
                TScalar y;
                struct { TScalar _padding_z[TI2]; TScalar z; };
                TScalar w;
            };
        };

        template< typename TData, typename TScalar, size_t TI0, size_t TI2, size_t TI3>
        struct indexed_proxy_storage<TData, TScalar, 4, TI0, 0, TI2, TI3>
        {
            union
            {
                TData data;
                struct { TScalar _padding_x[TI0]; TScalar x; };
                TScalar y;
                struct { TScalar _padding_z[TI2]; TScalar z; };
                struct { TScalar _padding_w[TI3]; TScalar w; };
            };
        };

        template< typename TData, typename TScalar, size_t TI0, size_t TI1>
        struct indexed_proxy_storage<TData, TScalar, 4, TI0, TI1, 0, 0>
        {
            union
            {
                TData data;
                struct { TScalar _padding_x[TI0]; TScalar x; };
                struct { TScalar _padding_y[TI1]; TScalar y; };
                TScalar z;
                TScalar w;
            };
        };

        template< typename TData, typename TScalar, size_t TI0, size_t TI1, size_t TI3>
        struct indexed_proxy_storage<TData, TScalar, 4, TI0, TI1, 0, TI3>
        {
            union
            {
                TData data;
                struct { TScalar _padding_x[TI0]; TScalar x; };
                struct { TScalar _padding_y[TI1]; TScalar y; };
                TScalar z;
                struct { TScalar _padding_w[TI3]; TScalar w; };
            };
        };

        template< typename TData, typename TScalar, size_t TI0, size_t TI1, size_t TI2>
        struct indexed_proxy_storage<TData, TScalar, 4, TI0, TI1, TI2, 0>
        {
            union
            {
                TData data;
                struct { TScalar _padding_x[TI0]; TScalar x; };
                struct { TScalar _padding_y[TI1]; TScalar y; };
                struct { TScalar _padding_z[TI2]; TScalar z; };
                TScalar w;
            };
        };

        template< typename TData, typename TScalar, size_t TI0, size_t TI1, size_t TI2, size_t TI3>
        struct indexed_proxy_storage<TData, TScalar, 4, TI0, TI1, TI2, TI3>
        {
            union
            {
                TData data;
                struct { TScalar _padding_x[TI0]; TScalar x; };
                struct { TScalar _padding_y[TI1]; TScalar y; };
                struct { TScalar _padding_z[TI2]; TScalar z; };
                struct { TScalar _padding_w[TI3]; TScalar w; };
            };
        };
    }
}