#ifndef HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_PROXY
#define HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_PROXY

#include <type_traits>
#include "../detail/vector_binary_operators.h"

namespace swizzle
{
    namespace naive
    {
        namespace
        {
            class private_dummy 
            {
                private_dummy();
            };
        }

        template <class TVector, class TScalar, size_t NumOfComponents, size_t Size>
        struct vector_proxy_base : detail::default_vector_unary_operators_tag
        {
            typedef TVector vector_type;
            typedef TScalar scalar_type;
            static const size_t num_of_components = Size;

        protected:
            scalar_type data[NumOfComponents];
        };

        template <class TVector, class TScalar, size_t NumOfComponents, size_t x, size_t y = -1, size_t z = -1, size_t w = -1>
        struct vector_proxy;

        template <class TVector, class TScalar, size_t NumOfComponents, size_t x>
        struct vector_proxy<TVector, TScalar, NumOfComponents, x, -1, -1, -1> : vector_proxy_base<TVector, TScalar, NumOfComponents, 1>
        {
            static const bool is_writable = true;
            typedef TVector vector_type;
            typedef TScalar scalar_type;
            static const size_t num_of_components = 1;


            operator TVector() const
            {
                return TVector()._set(data[x]);
            }

            vector_proxy& operator=(const typename std::conditional< is_writable, TVector, private_dummy >::type& vec) 
            { 
                data[x] = vec._components[0]; 
                return *this; 
            }

            operator scalar_type() const
            {
                return data[x];
            }
        };

        template <class TVector, class TScalar, size_t NumOfComponents, size_t x, size_t y>
        struct vector_proxy<TVector, TScalar, NumOfComponents, x, y, -1, -1> : vector_proxy_base<TVector, TScalar, NumOfComponents, 2>
        {
            static const bool is_writable = x != y;
            typedef TVector vector_type;
            typedef TScalar scalar_type;
            static const size_t num_of_components = 2;


            operator TVector() const
            {
                return TVector()._set(data[x], data[y]);
            }

            vector_proxy& operator=(const typename std::conditional< is_writable, TVector, private_dummy >::type& vec) 
            { 
                data[x] = vec._components[0]; 
                data[y] = vec._components[1]; 
                return *this; 
            }

            template <size_t i>
            scalar_type at()
            {
                return at(std::integral_constant<size_t, i>());
            }
            scalar_type at(std::integral_constant<size_t, 0>)
            {
                return data[x];
            }
            scalar_type at(std::integral_constant<size_t, 1>)
            {
                return data[y];
            }
        };

        template <class TVector, class TScalar, size_t NumOfComponents, size_t x, size_t y, size_t z>
        struct vector_proxy<TVector, TScalar, NumOfComponents, x, y, z, -1> : vector_proxy_base<TVector, TScalar, NumOfComponents, 3>
        {
            static const bool is_writable = x != y && x != z && y != z;
            typedef TVector vector_type;
            typedef TScalar scalar_type;
            static const size_t num_of_components = 3;

            operator TVector() const
            {
                return TVector()._set(data[x], data[y], data[z]);
            }

            vector_proxy& operator=(const typename std::conditional< is_writable, TVector, private_dummy >::type& vec) 
            { 
                data[x] = vec._components[0];
                data[y] = vec._components[1]; 
                data[z] = vec._components[2]; 
                return *this; 
            }

            template <size_t i>
            scalar_type at()
            {
                return at(std::integral_constant<size_t, i>());
            }
            scalar_type at(std::integral_constant<size_t, 0>)
            {
                return data[x];
            }
            scalar_type at(std::integral_constant<size_t, 1>)
            {
                return data[y];
            }
            scalar_type at(std::integral_constant<size_t, 2>)
            {
                return data[z];
            }
        };

        template <class TVector, class TScalar, size_t NumOfComponents, size_t x, size_t y, size_t z, size_t w>
        struct vector_proxy : vector_proxy_base<TVector, TScalar, NumOfComponents, 4>
        {
            static const bool is_writable = x != y && x != z && x != w && y != z && y !=z && z != w;
            typedef TVector vector_type;
            typedef TScalar scalar_type;
            static const size_t num_of_components = 4;

            operator TVector() const
            {
                return TVector()._set(data[x], data[y], data[z], data[w]);
            }

            vector_proxy& operator=(const typename std::conditional<is_writable, TVector, private_dummy >::type& vec) 
            { 
                data[x] = vec._components[0]; 
                data[y] = vec._components[1]; 
                data[z] = vec._components[2]; 
                data[w] = vec._components[3]; 
                return *this; 
            }

            template <size_t i>
            scalar_type at()
            {
                return at(std::integral_constant<size_t, i>());
            }
            scalar_type at(std::integral_constant<size_t, 0>)
            {
                return data[x];
            }
            scalar_type at(std::integral_constant<size_t, 1>)
            {
                return data[y];
            }
            scalar_type at(std::integral_constant<size_t, 2>)
            {
                return data[z];
            }
            scalar_type at(std::integral_constant<size_t, 3>)
            {
                return data[w];
            }
        };
    }
}

#endif  HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_PROXY
