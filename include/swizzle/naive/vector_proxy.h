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

        template <template <class> class TVector, class TTraits, size_t Size>
        struct vector_proxy_base : private TTraits::tag_type
        {
            typedef typename TTraits::change_num_of_components<Size>::type traits_type;
            typedef TVector< traits_type > vector_type;
            typedef typename traits_type::scalar_type scalar_type;
            static const size_t num_of_components = Size;

        protected:
            scalar_type data[TTraits::num_of_components];
        };

        template <template <class> class TVector, class TTraits, size_t x, size_t y = -1, size_t z = -1, size_t w = -1>
        struct vector_proxy;

        template <template <class> class TVector, class TTraits, size_t x>
        struct vector_proxy<TVector, TTraits, x, -1, -1, -1> : vector_proxy_base<TVector, TTraits, 1>
        {
            typedef vector_proxy_base<TVector, TTraits, 1> base_type;

            static const bool is_writable = true;

            operator base_type::vector_type() const
            {
                base_type::vector_type result;
                result._components[0] = data[x];
                return result;
            }

            vector_proxy& operator=(const typename std::conditional< is_writable, base_type::vector_type, private_dummy >::type& vec) 
            { 
                data[x] = vec._components[0]; 
                return *this; 
            }
        };

        template <template <class> class TVector, class TTraits, size_t x, size_t y>
        struct vector_proxy<TVector, TTraits, x, y, -1, -1> : vector_proxy_base<TVector, TTraits, 2>
        {
            typedef vector_proxy_base<TVector, TTraits, 2> base_type;

            static const bool is_writable = x != y;

            operator base_type::vector_type() const
            {
                base_type::vector_type result;
                result._components[0] = data[x];
                result._components[1] = data[y];
                return result;
            }

            vector_proxy& operator=(const typename std::conditional< is_writable, base_type::vector_type, private_dummy >::type& vec) 
            { 
                data[x] = vec._components[0]; 
                data[y] = vec._components[1]; 
                return *this; 
            }
        };

        template <template <class> class TVector, class TTraits, size_t x, size_t y, size_t z>
        struct vector_proxy<TVector, TTraits, x, y, z, -1> : vector_proxy_base<TVector, TTraits, 3>
        {
            typedef vector_proxy_base<TVector, TTraits, 3> base_type;

            static const bool is_writable = x != y && x != z && y != z;

            operator base_type::vector_type() const
            {
                base_type::vector_type result;
                result._components[0] = data[x];
                result._components[1] = data[y];
                result._components[2] = data[z];
                return result;
            }

            vector_proxy& operator=(const typename std::conditional< is_writable, base_type::vector_type, private_dummy >::type& vec) 
            { 
                data[x] = vec._components[0];
                data[y] = vec._components[1]; 
                data[z] = vec._components[2]; 
                return *this; 
            }
        };

        template <template <class> class TVector, class TTraits, size_t x, size_t y, size_t z, size_t w>
        struct vector_proxy : vector_proxy_base<TVector, TTraits, 4>
        {
            typedef vector_proxy_base<TVector, TTraits, 4> base_type;

            static const bool is_writable = x != y && x != z && x != w && y != z && y !=z && z != w;

            operator base_type::vector_type() const
            {
                base_type::vector_type result;
                result._components[0] = data[x];
                result._components[1] = data[y];
                result._components[2] = data[z];
                result._components[3] = data[w];
                return result;
            }

            vector_proxy& operator=(const typename std::conditional<is_writable, base_type::vector_type, private_dummy >::type& vec) 
            { 
                data[x] = vec._components[0]; 
                data[y] = vec._components[1]; 
                data[z] = vec._components[2]; 
                data[w] = vec._components[3]; 
                return *this; 
            }
        };
    }
}

#endif  HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_PROXY
