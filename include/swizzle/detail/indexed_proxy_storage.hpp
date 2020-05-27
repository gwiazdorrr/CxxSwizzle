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
        template <typename DataType, typename ScalarType, size_t Size, size_t... Indices>
        struct indexed_proxy_storage;


        template< typename DataType, typename ScalarType>
        struct indexed_proxy_storage<DataType, ScalarType, 1, 0>
        {
            union
            {
                DataType data;
                ScalarType x;
            };
        };

        template< typename DataType, typename ScalarType, size_t I0>
        struct indexed_proxy_storage<DataType, ScalarType, 1, I0>
        {
            union
            {
                DataType data;
                struct { ScalarType _padding_x[I0]; ScalarType x; };
            };
        };

        template< typename DataType, typename ScalarType>
        struct indexed_proxy_storage<DataType, ScalarType, 2, 0, 0>
        {
            union
            {
                DataType data;
                ScalarType x;
                ScalarType y;
            };
        };

        template< typename DataType, typename ScalarType, size_t I1>
        struct indexed_proxy_storage<DataType, ScalarType, 2, 0, I1>
        {
            union
            {
                DataType data;
                ScalarType x;
                struct { ScalarType _padding_y[I1]; ScalarType y; };
            };
        };

        template< typename DataType, typename ScalarType, size_t I0>
        struct indexed_proxy_storage<DataType, ScalarType, 2, I0, 0>
        {
            union
            {
                DataType data;
                struct { ScalarType _padding_x[I0]; ScalarType x; };
                ScalarType y;
            };
        };

        template< typename DataType, typename ScalarType, size_t I0, size_t I1>
        struct indexed_proxy_storage<DataType, ScalarType, 2, I0, I1>
        {
            union
            {
                DataType data;
                struct { ScalarType _padding_x[I0]; ScalarType x; };
                struct { ScalarType _padding_y[I1]; ScalarType y; };
            };
        };

        template< typename DataType, typename ScalarType>
        struct indexed_proxy_storage<DataType, ScalarType, 3, 0, 0, 0>
        {
            union
            {
                DataType data;
                ScalarType x;
                ScalarType y;
                ScalarType z;
            };
        };

        template< typename DataType, typename ScalarType, size_t I2>
        struct indexed_proxy_storage<DataType, ScalarType, 3, 0, 0, I2>
        {
            union
            {
                DataType data;
                ScalarType x;
                ScalarType y;
                struct { ScalarType _padding_z[I2]; ScalarType z; };
            };
        };

        template< typename DataType, typename ScalarType, size_t I1>
        struct indexed_proxy_storage<DataType, ScalarType, 3, 0, I1, 0>
        {
            union
            {
                DataType data;
                ScalarType x;
                struct { ScalarType _padding_y[I1]; ScalarType y; };
                ScalarType z;
            };
        };

        template< typename DataType, typename ScalarType, size_t I1, size_t I2>
        struct indexed_proxy_storage<DataType, ScalarType, 3, 0, I1, I2>
        {
            union
            {
                DataType data;
                ScalarType x;
                struct { ScalarType _padding_y[I1]; ScalarType y; };
                struct { ScalarType _padding_z[I2]; ScalarType z; };
            };
        };

        template< typename DataType, typename ScalarType, size_t I0>
        struct indexed_proxy_storage<DataType, ScalarType, 3, I0, 0, 0>
        {
            union
            {
                DataType data;
                struct { ScalarType _padding_x[I0]; ScalarType x; };
                ScalarType y;
                ScalarType z;
            };
        };

        template< typename DataType, typename ScalarType, size_t I0, size_t I2>
        struct indexed_proxy_storage<DataType, ScalarType, 3, I0, 0, I2>
        {
            union
            {
                DataType data;
                struct { ScalarType _padding_x[I0]; ScalarType x; };
                ScalarType y;
                struct { ScalarType _padding_z[I2]; ScalarType z; };
            };
        };

        template< typename DataType, typename ScalarType, size_t I0, size_t I1>
        struct indexed_proxy_storage<DataType, ScalarType, 3, I0, I1, 0>
        {
            union
            {
                DataType data;
                struct { ScalarType _padding_x[I0]; ScalarType x; };
                struct { ScalarType _padding_y[I1]; ScalarType y; };
                ScalarType z;
            };
        };

        template< typename DataType, typename ScalarType, size_t I0, size_t I1, size_t I2>
        struct indexed_proxy_storage<DataType, ScalarType, 3, I0, I1, I2>
        {
            union
            {
                DataType data;
                struct { ScalarType _padding_x[I0]; ScalarType x; };
                struct { ScalarType _padding_y[I1]; ScalarType y; };
                struct { ScalarType _padding_z[I2]; ScalarType z; };
            };
        };

        template< typename DataType, typename ScalarType>
        struct indexed_proxy_storage<DataType, ScalarType, 4, 0, 0, 0, 0>
        {
            union
            {
                DataType data;
                ScalarType x;
                ScalarType y;
                ScalarType z;
                ScalarType w;
            };
        };

        template< typename DataType, typename ScalarType, size_t I3>
        struct indexed_proxy_storage<DataType, ScalarType, 4, 0, 0, 0, I3>
        {
            union
            {
                DataType data;
                ScalarType x;
                ScalarType y;
                ScalarType z;
                struct { ScalarType _padding_w[I3]; ScalarType w; };
            };
        };

        template< typename DataType, typename ScalarType, size_t I2>
        struct indexed_proxy_storage<DataType, ScalarType, 4, 0, 0, I2, 0>
        {
            union
            {
                DataType data;
                ScalarType x;
                ScalarType y;
                struct { ScalarType _padding_z[I2]; ScalarType z; };
                ScalarType w;
            };
        };

        template< typename DataType, typename ScalarType, size_t I2, size_t I3>
        struct indexed_proxy_storage<DataType, ScalarType, 4, 0, 0, I2, I3>
        {
            union
            {
                DataType data;
                ScalarType x;
                ScalarType y;
                struct { ScalarType _padding_z[I2]; ScalarType z; };
                struct { ScalarType _padding_w[I3]; ScalarType w; };
            };
        };

        template< typename DataType, typename ScalarType, size_t I1>
        struct indexed_proxy_storage<DataType, ScalarType, 4, 0, I1, 0, 0>
        {
            union
            {
                DataType data;
                ScalarType x;
                struct { ScalarType _padding_y[I1]; ScalarType y; };
                ScalarType z;
                ScalarType w;
            };
        };

        template< typename DataType, typename ScalarType, size_t I1, size_t I3>
        struct indexed_proxy_storage<DataType, ScalarType, 4, 0, I1, 0, I3>
        {
            union
            {
                DataType data;
                ScalarType x;
                struct { ScalarType _padding_y[I1]; ScalarType y; };
                ScalarType z;
                struct { ScalarType _padding_w[I3]; ScalarType w; };
            };
        };

        template< typename DataType, typename ScalarType, size_t I1, size_t I2>
        struct indexed_proxy_storage<DataType, ScalarType, 4, 0, I1, I2, 0>
        {
            union
            {
                DataType data;
                ScalarType x;
                struct { ScalarType _padding_y[I1]; ScalarType y; };
                struct { ScalarType _padding_z[I2]; ScalarType z; };
                ScalarType w;
            };
        };

        template< typename DataType, typename ScalarType, size_t I1, size_t I2, size_t I3>
        struct indexed_proxy_storage<DataType, ScalarType, 4, 0, I1, I2, I3>
        {
            union
            {
                DataType data;
                ScalarType x;
                struct { ScalarType _padding_y[I1]; ScalarType y; };
                struct { ScalarType _padding_z[I2]; ScalarType z; };
                struct { ScalarType _padding_w[I3]; ScalarType w; };
            };
        };

        template< typename DataType, typename ScalarType, size_t I0>
        struct indexed_proxy_storage<DataType, ScalarType, 4, I0, 0, 0, 0>
        {
            union
            {
                DataType data;
                struct { ScalarType _padding_x[I0]; ScalarType x; };
                ScalarType y;
                ScalarType z;
                ScalarType w;
            };
        };

        template< typename DataType, typename ScalarType, size_t I0, size_t I3>
        struct indexed_proxy_storage<DataType, ScalarType, 4, I0, 0, 0, I3>
        {
            union
            {
                DataType data;
                struct { ScalarType _padding_x[I0]; ScalarType x; };
                ScalarType y;
                ScalarType z;
                struct { ScalarType _padding_w[I3]; ScalarType w; };
            };
        };

        template< typename DataType, typename ScalarType, size_t I0, size_t I2>
        struct indexed_proxy_storage<DataType, ScalarType, 4, I0, 0, I2, 0>
        {
            union
            {
                DataType data;
                struct { ScalarType _padding_x[I0]; ScalarType x; };
                ScalarType y;
                struct { ScalarType _padding_z[I2]; ScalarType z; };
                ScalarType w;
            };
        };

        template< typename DataType, typename ScalarType, size_t I0, size_t I2, size_t I3>
        struct indexed_proxy_storage<DataType, ScalarType, 4, I0, 0, I2, I3>
        {
            union
            {
                DataType data;
                struct { ScalarType _padding_x[I0]; ScalarType x; };
                ScalarType y;
                struct { ScalarType _padding_z[I2]; ScalarType z; };
                struct { ScalarType _padding_w[I3]; ScalarType w; };
            };
        };

        template< typename DataType, typename ScalarType, size_t I0, size_t I1>
        struct indexed_proxy_storage<DataType, ScalarType, 4, I0, I1, 0, 0>
        {
            union
            {
                DataType data;
                struct { ScalarType _padding_x[I0]; ScalarType x; };
                struct { ScalarType _padding_y[I1]; ScalarType y; };
                ScalarType z;
                ScalarType w;
            };
        };

        template< typename DataType, typename ScalarType, size_t I0, size_t I1, size_t I3>
        struct indexed_proxy_storage<DataType, ScalarType, 4, I0, I1, 0, I3>
        {
            union
            {
                DataType data;
                struct { ScalarType _padding_x[I0]; ScalarType x; };
                struct { ScalarType _padding_y[I1]; ScalarType y; };
                ScalarType z;
                struct { ScalarType _padding_w[I3]; ScalarType w; };
            };
        };

        template< typename DataType, typename ScalarType, size_t I0, size_t I1, size_t I2>
        struct indexed_proxy_storage<DataType, ScalarType, 4, I0, I1, I2, 0>
        {
            union
            {
                DataType data;
                struct { ScalarType _padding_x[I0]; ScalarType x; };
                struct { ScalarType _padding_y[I1]; ScalarType y; };
                struct { ScalarType _padding_z[I2]; ScalarType z; };
                ScalarType w;
            };
        };

        template< typename DataType, typename ScalarType, size_t I0, size_t I1, size_t I2, size_t I3>
        struct indexed_proxy_storage<DataType, ScalarType, 4, I0, I1, I2, I3>
        {
            union
            {
                DataType data;
                struct { ScalarType _padding_x[I0]; ScalarType x; };
                struct { ScalarType _padding_y[I1]; ScalarType y; };
                struct { ScalarType _padding_z[I2]; ScalarType z; };
                struct { ScalarType _padding_w[I3]; ScalarType w; };
            };
        };
    }
}