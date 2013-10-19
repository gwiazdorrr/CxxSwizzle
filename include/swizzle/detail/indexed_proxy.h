// CxxSwizzle
// Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <type_traits>
#include <swizzle/detail/utils.h>

namespace swizzle
{
    namespace detail
    {
        //! A VectorType's proxy, using subscript operators to access components of both the vector and the
        //! DataType. x, y, z & w template args define which components of the vector this proxy uses in place
        //! of its, with -1 meaning "don't use".
        //! The type is convertible to the vector. It also forwards all unary arithmetic operators. Binary
        //! operations hopefully fallback to the vector ones.
        template <class VectorType, class DataType, size_t x, size_t y, size_t z = -1, size_t w = -1>
        class indexed_proxy
        {
            //! The data. Must support subscript operator.
            DataType m_data;

        public:
            // Make sure -1 are always trailing, not in the middle
            static_assert ( (x!=-1) && (y!=-1) && (z!=-1 || w==-1), "Must be continuous" );

            // Can easily count now since -1 must be continuous
            static const size_t num_of_components = (x!=-1) + (y!=-1) + (z!=-1) + (w!=-1);
            static_assert(num_of_components >= 2, "Must be at least 2 components");

            // Is this proxy writable? All indices must be different, except for -1s
            static const bool is_writable = 
                (x != y  && x != z && x != w) && 
                (y == -1 || y != z && y != w) && 
                (z == -1 || z != w );

            // Use the traits to define vector and scalar
            typedef VectorType vector_type;
            typedef VectorType decay_type;
          
        public:
            //! Convert proxy into a vector.
            vector_type decay() const
            {
                vector_type result;
                // inlined, calls with x, y, z or w being -1 will get compiled out
                set_if_indices_are_valid<0, x>(result, m_data);
                set_if_indices_are_valid<1, y>(result, m_data);
                set_if_indices_are_valid<2, z>(result, m_data);
                set_if_indices_are_valid<3, w>(result, m_data);
                return result;
            }

            //! Auto-decaying where possible.
            operator vector_type() const
            {
                return decay();
            }

            //! Assignment only enabled if proxy is writable -> has unique indexes
            indexed_proxy& operator=(const typename std::conditional<is_writable, vector_type, operation_not_available>::type& vec)
            {
                // inlined, calls with x, y, z or w being -1 will get compiled out
                set_if_indices_are_valid<x, 0>(m_data, vec);
                set_if_indices_are_valid<y, 1>(m_data, vec);
                set_if_indices_are_valid<z, 2>(m_data, vec);
                set_if_indices_are_valid<w, 3>(m_data, vec);
                return *this;
            }

            //! Forwarding operator. Global non-assignment operators depend on it.
            template <class T>
            indexed_proxy& operator+=(T && o)
            {
                return operator=( decay() + std::forward<T>(o) );
            }

            //! Forwarding operator. Global non-assignment operators depend on it.
            template <class T>
            indexed_proxy& operator-=(T && o)
            {
                return operator=( decay() - std::forward<T>(o) );
            }

            //! Forwarding operator. Global non-assignment operators depend on it.
            template <class T>
            indexed_proxy& operator*=(T && o)
            {
                return operator=( decay() * std::forward<T>(o) );
            }

            //! Forwarding operator. Global non-assignment operators depend on it.
            template <class T>
            indexed_proxy& operator/=(T && o)
            {
                return operator=( decay() / std::forward<T>(o) );
            }
        };

        //! A specialisation for the indexed_proxy, defines TVector as vector type.
        template <class TVector, class TData, size_t x, size_t y, size_t z, size_t w>
        struct get_vector_type_impl< indexed_proxy<TVector, TData, x, y, z, w> >
        {
            typedef TVector type;
        };
    }
}