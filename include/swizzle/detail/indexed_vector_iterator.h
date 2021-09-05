// CxxSwizzle (c) 2013-2021 Piotr Gwiazdowski
#pragma once

#include <cassert>
#include <type_traits>
#include <iterator>
#include <swizzle/detail/utils.h>

namespace swizzle
{
    namespace detail
    {
        //! A very simple vector iterator. Uses subscript operator to access elements. The TVector must
        //! also expose num_components static fields.
        template <class TVector>
        struct indexed_vector_iterator 
        {
            using iterator_category = std::bidirectional_iterator_tag;
            using value_type = typename std::remove_reference< decltype(std::declval<TVector>()[0]) >::type;
            using difference_type = ptrdiff_t;
            using pointer = value_type*;
            using reference = value_type&;

        private:
            TVector& vector;
            size_t index;

        public:
            indexed_vector_iterator(TVector& vec, size_t i = 0) 
                : vector(vec)
                , index(i)
            {
                assert(index <= TVector::num_components);
            }

            bool operator==(const indexed_vector_iterator& rhs) const 
            {
                return (&vector == &rhs.vector) && (index == rhs.index);
            }

            bool operator!=(const indexed_vector_iterator& rhs) const 
            {
                return !(*this == rhs);
            }

            indexed_vector_iterator& operator++() 
            {
                assert(index < TVector::num_components);
                ++index;
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
                assert(index > 0);
                --index;
                return *this;
            }

            indexed_vector_iterator operator--(int) 
            {
                indexed_vector_iterator tmp (*this);
                --(*this);
                return tmp;
            }

            auto operator*() -> decltype( std::declval<TVector>()[0] )
            {
                assert(index < TVector::num_components);
                return vector[index];
            }

            auto operator->() -> decltype( &std::declval<TVector>()[0] )
            {
                assert(index < TVector::num_components);
                return &vector[index];
            }
        };

        //! A helper function to create a iterator.
        template <class TVector>
        indexed_vector_iterator<TVector> make_indexed_vector_iterator(TVector& vector, size_t i = 0)
        {
            return indexed_vector_iterator<TVector>(vector, i);
        }
    }
}