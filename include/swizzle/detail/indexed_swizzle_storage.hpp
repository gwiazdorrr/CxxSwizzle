// CxxSwizzle (c) 2013-2021 Piotr Gwiazdowski
#pragma once

#include <type_traits>
#include <swizzle/detail/utils.hpp>
#include <swizzle/detail/vector_traits.hpp>
#include <swizzle/detail/indexed_swizzle_swizzles.hpp>


#define CXXSWIZZLE_STORAGE_INDEXER_1 \
    constexpr TScalar operator[](size_t index) const noexcept \
    {                                                         \
        switch (index)                                        \
        {                                                     \
        case 0:  return x;                                    \
        default: return {};                                   \
        }                                                     \
    }

#define CXXSWIZZLE_STORAGE_INDEXER_2 \
    constexpr TScalar operator[](size_t index) const noexcept \
    {                                                         \
        switch (index)                                        \
        {                                                     \
        case 0:  return x;                                    \
        case 1:  return y;                                    \
        default: return {};                                   \
        }                                                     \
    }

#define CXXSWIZZLE_STORAGE_INDEXER_3 \
    constexpr TScalar operator[](size_t index) const noexcept \
    {                                                         \
        switch (index)                                        \
        {                                                     \
        case 0:  return x;                                    \
        case 1:  return y;                                    \
        case 2:  return z;                                    \
        default: return {};                                   \
        }                                                     \
    }

#define CXXSWIZZLE_STORAGE_INDEXER_4 \
    constexpr TScalar operator[](size_t index) const noexcept \
    {                                                         \
        switch (index)                                        \
        {                                                     \
        case 0:  return x;                                    \
        case 1:  return y;                                    \
        case 2:  return z;                                    \
        case 3:  return w;                                    \
        default: return {};                                   \
        }                                                     \
    }


namespace swizzle
{
    namespace detail
    {
        template <typename TData, typename TScalar, size_t TSize, size_t... TIndices>
        struct indexed_swizzle_storage;

        // thanks C++ for not supporting zero-sized arrays...

        template< typename TData, typename TScalar>
        struct indexed_swizzle_storage<TData, TScalar, 1, 0> : indexed_swizzle_swizzles<1>
        {
            union
            {
                TData data;
                TScalar x;
            };
            CXXSWIZZLE_STORAGE_INDEXER_1
        };

        template< typename TData, typename TScalar, size_t TI0>
        struct indexed_swizzle_storage<TData, TScalar, 1, TI0> : indexed_swizzle_swizzles<1>
        {
            union
            {
                TData data;
                struct { TScalar _padding_x[TI0]; TScalar x; };
            };
            CXXSWIZZLE_STORAGE_INDEXER_1
        };

        template< typename TData, typename TScalar>
        struct indexed_swizzle_storage<TData, TScalar, 2, 0, 0> : indexed_swizzle_swizzles<2>
        {
            union
            {
                TData data;
                TScalar x;
                TScalar y;
            };
            CXXSWIZZLE_STORAGE_INDEXER_2
        };

        template< typename TData, typename TScalar, size_t TI1>
        struct indexed_swizzle_storage<TData, TScalar, 2, 0, TI1> : indexed_swizzle_swizzles<2>
        {
            union
            {
                TData data;
                TScalar x;
                struct { TScalar _padding_y[TI1]; TScalar y; };
            };
            CXXSWIZZLE_STORAGE_INDEXER_2
        };

        template< typename TData, typename TScalar, size_t TI0>
        struct indexed_swizzle_storage<TData, TScalar, 2, TI0, 0> : indexed_swizzle_swizzles<2>
        {
            union
            {
                TData data;
                struct { TScalar _padding_x[TI0]; TScalar x; };
                TScalar y;
            };
            CXXSWIZZLE_STORAGE_INDEXER_2
        };

        template< typename TData, typename TScalar, size_t TI0, size_t TI1>
        struct indexed_swizzle_storage<TData, TScalar, 2, TI0, TI1> : indexed_swizzle_swizzles<2>
        {
            union
            {
                TData data;
                struct { TScalar _padding_x[TI0]; TScalar x; };
                struct { TScalar _padding_y[TI1]; TScalar y; };
            };
            CXXSWIZZLE_STORAGE_INDEXER_2
        };

        template< typename TData, typename TScalar>
        struct indexed_swizzle_storage<TData, TScalar, 3, 0, 0, 0> : indexed_swizzle_swizzles<3>
        {
            union
            {
                TData data;
                TScalar x;
                TScalar y;
                TScalar z;
            };
            CXXSWIZZLE_STORAGE_INDEXER_3
        };

        template< typename TData, typename TScalar, size_t TI2>
        struct indexed_swizzle_storage<TData, TScalar, 3, 0, 0, TI2> : indexed_swizzle_swizzles<3>
        {
            union
            {
                TData data;
                TScalar x;
                TScalar y;
                struct { TScalar _padding_z[TI2]; TScalar z; };
            };
            CXXSWIZZLE_STORAGE_INDEXER_3
        };

        template< typename TData, typename TScalar, size_t TI1>
        struct indexed_swizzle_storage<TData, TScalar, 3, 0, TI1, 0> : indexed_swizzle_swizzles<3>
        {
            union
            {
                TData data;
                TScalar x;
                struct { TScalar _padding_y[TI1]; TScalar y; };
                TScalar z;
            };
            CXXSWIZZLE_STORAGE_INDEXER_3
        };

        template< typename TData, typename TScalar, size_t TI1, size_t TI2>
        struct indexed_swizzle_storage<TData, TScalar, 3, 0, TI1, TI2> : indexed_swizzle_swizzles<3>
        {
            union
            {
                TData data;
                TScalar x;
                struct { TScalar _padding_y[TI1]; TScalar y; };
                struct { TScalar _padding_z[TI2]; TScalar z; };
            };
            CXXSWIZZLE_STORAGE_INDEXER_3
        };

        template< typename TData, typename TScalar, size_t TI0>
        struct indexed_swizzle_storage<TData, TScalar, 3, TI0, 0, 0> : indexed_swizzle_swizzles<3>
        {
            union
            {
                TData data;
                struct { TScalar _padding_x[TI0]; TScalar x; };
                TScalar y;
                TScalar z;
            };
            CXXSWIZZLE_STORAGE_INDEXER_3
        };

        template< typename TData, typename TScalar, size_t TI0, size_t TI2>
        struct indexed_swizzle_storage<TData, TScalar, 3, TI0, 0, TI2> : indexed_swizzle_swizzles<3>
        {
            union
            {
                TData data;
                struct { TScalar _padding_x[TI0]; TScalar x; };
                TScalar y;
                struct { TScalar _padding_z[TI2]; TScalar z; };
            };
            CXXSWIZZLE_STORAGE_INDEXER_3
        };

        template< typename TData, typename TScalar, size_t TI0, size_t TI1>
        struct indexed_swizzle_storage<TData, TScalar, 3, TI0, TI1, 0> : indexed_swizzle_swizzles<3>
        {
            union
            {
                TData data;
                struct { TScalar _padding_x[TI0]; TScalar x; };
                struct { TScalar _padding_y[TI1]; TScalar y; };
                TScalar z;
            };
            CXXSWIZZLE_STORAGE_INDEXER_3
        };

        template< typename TData, typename TScalar, size_t TI0, size_t TI1, size_t TI2>
        struct indexed_swizzle_storage<TData, TScalar, 3, TI0, TI1, TI2> : indexed_swizzle_swizzles<3>
        {
            union
            {
                TData data;
                struct { TScalar _padding_x[TI0]; TScalar x; };
                struct { TScalar _padding_y[TI1]; TScalar y; };
                struct { TScalar _padding_z[TI2]; TScalar z; };
            };
            CXXSWIZZLE_STORAGE_INDEXER_3
        };

        template< typename TData, typename TScalar>
        struct indexed_swizzle_storage<TData, TScalar, 4, 0, 0, 0, 0> : indexed_swizzle_swizzles<4>
        {
            union
            {
                TData data;
                TScalar x;
                TScalar y;
                TScalar z;
                TScalar w;
            };
            CXXSWIZZLE_STORAGE_INDEXER_4
        };

        template< typename TData, typename TScalar, size_t TI3>
        struct indexed_swizzle_storage<TData, TScalar, 4, 0, 0, 0, TI3> : indexed_swizzle_swizzles<4>
        {
            union
            {
                TData data;
                TScalar x;
                TScalar y;
                TScalar z;
                struct { TScalar _padding_w[TI3]; TScalar w; };
            };
            CXXSWIZZLE_STORAGE_INDEXER_4
        };

        template< typename TData, typename TScalar, size_t TI2>
        struct indexed_swizzle_storage<TData, TScalar, 4, 0, 0, TI2, 0> : indexed_swizzle_swizzles<4>
        {
            union
            {
                TData data;
                TScalar x;
                TScalar y;
                struct { TScalar _padding_z[TI2]; TScalar z; };
                TScalar w;
            };
            CXXSWIZZLE_STORAGE_INDEXER_4
        };

        template< typename TData, typename TScalar, size_t TI2, size_t TI3>
        struct indexed_swizzle_storage<TData, TScalar, 4, 0, 0, TI2, TI3> : indexed_swizzle_swizzles<4>
        {
            union
            {
                TData data;
                TScalar x;
                TScalar y;
                struct { TScalar _padding_z[TI2]; TScalar z; };
                struct { TScalar _padding_w[TI3]; TScalar w; };
            };
            CXXSWIZZLE_STORAGE_INDEXER_4
        };

        template< typename TData, typename TScalar, size_t TI1>
        struct indexed_swizzle_storage<TData, TScalar, 4, 0, TI1, 0, 0> : indexed_swizzle_swizzles<4>
        {
            union
            {
                TData data;
                TScalar x;
                struct { TScalar _padding_y[TI1]; TScalar y; };
                TScalar z;
                TScalar w;
            };
            CXXSWIZZLE_STORAGE_INDEXER_4
        };

        template< typename TData, typename TScalar, size_t TI1, size_t TI3>
        struct indexed_swizzle_storage<TData, TScalar, 4, 0, TI1, 0, TI3> : indexed_swizzle_swizzles<4>
        {
            union
            {
                TData data;
                TScalar x;
                struct { TScalar _padding_y[TI1]; TScalar y; };
                TScalar z;
                struct { TScalar _padding_w[TI3]; TScalar w; };
            };
            CXXSWIZZLE_STORAGE_INDEXER_4
        };

        template< typename TData, typename TScalar, size_t TI1, size_t TI2>
        struct indexed_swizzle_storage<TData, TScalar, 4, 0, TI1, TI2, 0> : indexed_swizzle_swizzles<4>
        {
            union
            {
                TData data;
                TScalar x;
                struct { TScalar _padding_y[TI1]; TScalar y; };
                struct { TScalar _padding_z[TI2]; TScalar z; };
                TScalar w;
            };
            CXXSWIZZLE_STORAGE_INDEXER_4
        };

        template< typename TData, typename TScalar, size_t TI1, size_t TI2, size_t TI3>
        struct indexed_swizzle_storage<TData, TScalar, 4, 0, TI1, TI2, TI3> : indexed_swizzle_swizzles<4>
        {
            union
            {
                TData data;
                TScalar x;
                struct { TScalar _padding_y[TI1]; TScalar y; };
                struct { TScalar _padding_z[TI2]; TScalar z; };
                struct { TScalar _padding_w[TI3]; TScalar w; };
            };
            CXXSWIZZLE_STORAGE_INDEXER_4
        };

        template< typename TData, typename TScalar, size_t TI0>
        struct indexed_swizzle_storage<TData, TScalar, 4, TI0, 0, 0, 0> : indexed_swizzle_swizzles<4>
        {
            union
            {
                TData data;
                struct { TScalar _padding_x[TI0]; TScalar x; };
                TScalar y;
                TScalar z;
                TScalar w;
            };
            CXXSWIZZLE_STORAGE_INDEXER_4
        };

        template< typename TData, typename TScalar, size_t TI0, size_t TI3>
        struct indexed_swizzle_storage<TData, TScalar, 4, TI0, 0, 0, TI3> : indexed_swizzle_swizzles<4>
        {
            union
            {
                TData data;
                struct { TScalar _padding_x[TI0]; TScalar x; };
                TScalar y;
                TScalar z;
                struct { TScalar _padding_w[TI3]; TScalar w; };
            };
            CXXSWIZZLE_STORAGE_INDEXER_4
        };

        template< typename TData, typename TScalar, size_t TI0, size_t TI2>
        struct indexed_swizzle_storage<TData, TScalar, 4, TI0, 0, TI2, 0> : indexed_swizzle_swizzles<4>
        {
            union
            {
                TData data;
                struct { TScalar _padding_x[TI0]; TScalar x; };
                TScalar y;
                struct { TScalar _padding_z[TI2]; TScalar z; };
                TScalar w;
            };
            CXXSWIZZLE_STORAGE_INDEXER_4
        };

        template< typename TData, typename TScalar, size_t TI0, size_t TI2, size_t TI3>
        struct indexed_swizzle_storage<TData, TScalar, 4, TI0, 0, TI2, TI3> : indexed_swizzle_swizzles<4>
        {
            union
            {
                TData data;
                struct { TScalar _padding_x[TI0]; TScalar x; };
                TScalar y;
                struct { TScalar _padding_z[TI2]; TScalar z; };
                struct { TScalar _padding_w[TI3]; TScalar w; };
            };
            CXXSWIZZLE_STORAGE_INDEXER_4
        };

        template< typename TData, typename TScalar, size_t TI0, size_t TI1>
        struct indexed_swizzle_storage<TData, TScalar, 4, TI0, TI1, 0, 0> : indexed_swizzle_swizzles<4>
        {
            union
            {
                TData data;
                struct { TScalar _padding_x[TI0]; TScalar x; };
                struct { TScalar _padding_y[TI1]; TScalar y; };
                TScalar z;
                TScalar w;
            };
            CXXSWIZZLE_STORAGE_INDEXER_4
        };

        template< typename TData, typename TScalar, size_t TI0, size_t TI1, size_t TI3>
        struct indexed_swizzle_storage<TData, TScalar, 4, TI0, TI1, 0, TI3> : indexed_swizzle_swizzles<4>
        {
            union
            {
                TData data;
                struct { TScalar _padding_x[TI0]; TScalar x; };
                struct { TScalar _padding_y[TI1]; TScalar y; };
                TScalar z;
                struct { TScalar _padding_w[TI3]; TScalar w; };
            };
            CXXSWIZZLE_STORAGE_INDEXER_4
        };

        template< typename TData, typename TScalar, size_t TI0, size_t TI1, size_t TI2>
        struct indexed_swizzle_storage<TData, TScalar, 4, TI0, TI1, TI2, 0> : indexed_swizzle_swizzles<4>
        {
            union
            {
                TData data;
                struct { TScalar _padding_x[TI0]; TScalar x; };
                struct { TScalar _padding_y[TI1]; TScalar y; };
                struct { TScalar _padding_z[TI2]; TScalar z; };
                TScalar w;
            };
            CXXSWIZZLE_STORAGE_INDEXER_4
        };

        template< typename TData, typename TScalar, size_t TI0, size_t TI1, size_t TI2, size_t TI3>
        struct indexed_swizzle_storage<TData, TScalar, 4, TI0, TI1, TI2, TI3> : indexed_swizzle_swizzles<4>
        {
            union
            {
                TData data;
                struct { TScalar _padding_x[TI0]; TScalar x; };
                struct { TScalar _padding_y[TI1]; TScalar y; };
                struct { TScalar _padding_z[TI2]; TScalar z; };
                struct { TScalar _padding_w[TI3]; TScalar w; };
            };
            CXXSWIZZLE_STORAGE_INDEXER_4
        };
    }
}

#undef CXXSWIZZLE_STORAGE_INDEXER_1
#undef CXXSWIZZLE_STORAGE_INDEXER_2
#undef CXXSWIZZLE_STORAGE_INDEXER_3
#undef CXXSWIZZLE_STORAGE_INDEXER_4