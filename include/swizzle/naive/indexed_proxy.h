#pragma once

#include <type_traits>
#include <iostream>

namespace swizzle
{
    namespace naive
    {
        template <class TVector, class TData, class TTag, size_t x, size_t y = -1, size_t z = -1, size_t w = -1>
        class indexed_proxy : private TTag
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
            typedef typename detail::remove_reference_cv< decltype((*(TData*)0)[0]) >::type  scalar_type;
            //! If only one component, decay to the scalar
            typedef typename std::conditional<num_of_components==1, scalar_type, vector_type>::type decay_type;
          
        public:
            decay_type decay() const
            {
                return decay( static_cast<decay_type*>(nullptr) );
            }

            operator decay_type() const
            {
                return decay();
            }

            indexed_proxy& operator=(const typename std::conditional<is_writable, decay_type, swizzle::detail::operation_not_available >::type& vec)
            {
                assign(vec);
                return *this;
            }

            template <class T>
            indexed_proxy& operator+=(T && o)
            {
                return operator=( operator decay_type() + std::forward<T>(o) );
            }

            template <class T>
            indexed_proxy& operator-=(T && o)
            {
                return operator=( operator decay_type() - std::forward<T>(o) );
            }

            template <class T>
            indexed_proxy& operator*=(T && o)
            {
                return operator=( operator decay_type() * std::forward<T>(o) );
            }

            template <class T>
            indexed_proxy& operator/=(T && o)
            {
                return operator=( operator decay_type() / std::forward<T>(o) );
            }

        private:
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

            void assign(const vector_type& vec)
            {
                detail::compile_time_for<0, num_of_components>([&](size_t i) -> void { m_data[get_index(i)] = vec[i]; } );
            }

            vector_type decay(vector_type*) const
            {
                vector_type result;
                detail::compile_time_for<0, num_of_components>([&](size_t i) -> void { result[i] = m_data[get_index(i)]; } );
                return result;
            }

            void assign(scalar_type value)
            {
                m_data[x] = value;
            }

            scalar_type decay(scalar_type*) const
            {
                return m_data[x];
            }
        };

        template <class TVector, class TData, class TTag, size_t x, size_t y, size_t z, size_t w>
        std::ostream& operator<<(std::ostream& os, const indexed_proxy<TVector, TData, TTag, x, y, z, w>& vec)
        {
            return os << vec.decay();
        }

        template <class TVector, class TData, class TTag, size_t x, size_t y, size_t z, size_t w>
        std::istream & operator>>(std::istream& is, indexed_proxy<TVector, TData, TTag, x, y, z, w>& vec)
        {
            TVector v;
            is >> v;
            vec = v;
            return is;
        }
    }
}