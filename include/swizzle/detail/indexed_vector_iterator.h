// CxxSwizzle
// Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr.github@gmail.com>
#pragma once

#include <type_traits>
#include <swizzle/detail/utils.h>

namespace swizzle
{
    namespace detail
    {
        template <class VectorType>
        class indexed_vector_iterator 
            : public std::iterator< 
                std::bidirectional_iterator_tag, 
                typename std::remove_reference< decltype(declval<VectorType>()[0]) >::type 
            > 
        {
        private:
            size_t m_index;
            VectorType& m_vector;

        public:
            indexed_vector_iterator(VectorType& vec, size_t i = 0) 
                : m_vector(vec)
                , m_index(i)
            {}

            bool operator==(const indexed_vector_iterator& rhs) const 
            {
                return (&m_vector == &rhs.m_vector) && (m_index == rhs.m_index);
            }

            bool operator!=(const indexed_vector_iterator& rhs) const 
            {
                return !(*this == rhs);
            }

            indexed_vector_iterator& operator++() 
            {
                ++m_index;
                return *this;
            }   

            indexed_vector_iterator operator++(int) 
            {
                indexed_vector_iterator tmp(*this);
                ++(*this);
                return tmp;
            }

            indexed_vector_iterator& operator--() 
            {
                --m_index;
                return *this;
            }

            indexed_vector_iterator operator--(int) 
            {
                indexed_vector_iterator tmp (*this);
                --(*this);
                return tmp;
            }

            auto operator* () -> decltype( declval<VectorType>()[0] )
            {
                return m_vector[m_index];
            }

            auto operator->() -> decltype( &declval<VectorType>()[0] )
            {
                return &m_vector[m_index];
            }
        };

        template <class VectorType>
        indexed_vector_iterator<VectorType> make_vector_iterator(VectorType& vector, size_t i = 0)
        {
            return indexed_vector_iterator<VectorType>(vector, i);
        }
    }
}