//  CxxSwizzle
//  Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr.os@gmail.com>
#pragma once

#include <type_traits>
#include <iostream>
#include <swizzle/detail/utils.h>

namespace swizzle
{
    namespace naive
    {
        template <class TVector, class TData, size_t x, size_t y = -1, size_t z = -1, size_t w = -1>
        class indexed_proxy
        {
            //! The data.
            TData m_data;

        public:
            // Make sure -1 are always trailing, not in the middle
            static_assert ( (x!=-1) && (y!=-1 || (z==-1 && w==-1)) && (z!=-1 || w==-1 ), "Must be continuous" );

            // Can easily count now since -1 must be continuous
            static const size_t num_of_components = (x!=-1) + (y!=-1) + (z!=-1) + (w!=-1);

            // Is this proxy writable? All indices must be different, except for -1s
            static const bool is_writable = 
                (x != y  && x != z && x != w) && 
                (y == -1 || y != z && y != w) && 
                (z == -1 || z != w );

            // Use the traits to define vector and scalar
            typedef TVector vector_type;

            //! Since this is an indexed proxy, scalar type can be acquired straight from data's indexing operator.
            typedef typename detail::remove_reference_cv< decltype(detail::declval<TData>()[0]) >::type scalar_type;
            //! If only one component, decay to the scalar
            typedef typename std::conditional<num_of_components==1, scalar_type, vector_type>::type decay_type;
          
        public:
            //! Depending on size, converts proxy into scalar or vector
            decay_type decay() const
            {
                // select correct decay_impl based on decay type
                return decay_impl( static_cast<decay_type*>(nullptr) );
            }
            //! Auto-decaying where possible.
            operator decay_type() const
            {
                return decay();
            }

            //! Assignment only enabled if proxy is writable -> has unique indexes
            indexed_proxy& operator=(const typename std::conditional<is_writable, decay_type, swizzle::detail::operation_not_available >::type& vec)
            {
                assign_impl(vec);
                return *this;
            }

            //! Forwarding operator. Global non-assignment operators depend on it.
            template <class T>
            indexed_proxy& operator+=(T && o)
            {
                return operator=( operator decay_type() + std::forward<T>(o) );
            }
            //! Forwarding operator. Global non-assignment operators depend on it.
            template <class T>
            indexed_proxy& operator-=(T && o)
            {
                return operator=( operator decay_type() - std::forward<T>(o) );
            }
            //! Forwarding operator. Global non-assignment operators depend on it.
            template <class T>
            indexed_proxy& operator*=(T && o)
            {
                return operator=( operator decay_type() * std::forward<T>(o) );
            }
            //! Forwarding operator. Global non-assignment operators depend on it.
            template <class T>
            indexed_proxy& operator/=(T && o)
            {
                return operator=( operator decay_type() / std::forward<T>(o) );
            }

        private:
            
            //! Decays to scalar. Used in decay and conversion operator.
            scalar_type decay_impl(scalar_type*) const
            {
                return m_data[x];
            }
            //! Decays to vector. Used in decay and conversion operator.
            vector_type decay_impl(vector_type*) const
            {
                vector_type result;
                detail::compile_time_for<0, num_of_components>([&](size_t i) -> void { result[i] = m_data[get_index(i)]; } );
                return result;
            }

            //! Assigns data from vector. Used in assignment operator.
            void assign_impl(const vector_type& vec)
            {
                detail::compile_time_for<0, num_of_components>([&](size_t i) -> void { m_data[get_index(i)] = vec[i]; } );
            }
            //! Assigns data from scalar. Used in assignment operator.
            void assign_impl(scalar_type value)
            {
                m_data[x] = value;
            }

            //! Translates 0..3 into x..w. Had doubts how to do it, but MSVC seems to inline and compile-time optimise this call,
            //! regardless of whether it's a switch or a static array.
            static const size_t get_index(size_t i)
            {
                /*static const size_t s_indices[] = { x, y, z, w };
                return s_indices[i];*/
                switch (i)
                {
                case 0:
                    return x;
                case 1:
                    return y;
                case 2:
                    return z;
                default:
                    return w;
                }
            }
        };
    }
}