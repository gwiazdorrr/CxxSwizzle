// CxxSwizzle
// Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <cassert>
#include <type_traits>
#include <iterator>
#include <swizzle/detail/utils.h>

namespace swizzle
{
    namespace detail
    {
        //! A very simple vector iterator. Uses subscript operator to access elements. The VectorType must
        //! also expose num_of_components static fields.
        template <class VectorType>
        class indexed_vector_iterator 
            : public std::iterator< 
                std::bidirectional_iterator_tag, 
                typename std::remove_reference< decltype(declval<VectorType>()[0]) >::type 
            > 
        {
        private:
            VectorType& m_vector;
            size_t m_index;

        public:
            indexed_vector_iterator(VectorType& vec, size_t i = 0) 
                : m_vector(vec)
                , m_index(i)
            {
                assert(m_index <= VectorType::num_of_components);
            }

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
                assert(m_index < VectorType::num_of_components);
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
                assert(m_index > 0);
                --m_index;
                return *this;
            }

            indexed_vector_iterator operator--(int) 
            {
                indexed_vector_iterator tmp (*this);
                --(*this);
                return tmp;
            }

            auto operator*() -> decltype( declval<VectorType>()[0] )
            {
                assert(m_index < VectorType::num_of_components);
                return m_vector[m_index];
            }

            auto operator->() -> decltype( &declval<VectorType>()[0] )
            {
                assert(m_index < VectorType::num_of_components);
                return &m_vector[m_index];
            }
        };

        //! A helper function to create a iterator.
        template <class VectorType>
        indexed_vector_iterator<VectorType> make_indexed_vector_iterator(VectorType& vector, size_t i = 0)
        {
            return indexed_vector_iterator<VectorType>(vector, i);
        }
    }
}