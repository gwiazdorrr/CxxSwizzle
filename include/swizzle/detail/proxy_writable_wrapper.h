#pragma once

namespace swizzle
{
    namespace detail
    {
        //! A helper class simplyfing 
        template <class TBase>
        struct proxy_writable_wrapper : TBase
        {
            typedef TBase base_type;
            typedef typename TBase::vector_type vector_type;
            typedef typename TBase::scalar_type scalar_type;

            proxy_writable_wrapper& operator=(const vector_type& o)
            {
                base_type::operator=(o);
                return *this;
            }

            proxy_writable_wrapper& operator+=(const vector_type& o)
            {
                base_type::operator=( base_type::operator vector_type() + o );
                return *this;
            }

            proxy_writable_wrapper& operator-=(const vector_type& o)
            {
                base_type::operator=( base_type::operator vector_type() - o );
                return *this;
            }

            proxy_writable_wrapper& operator*=(const vector_type& o)
            {
                base_type::operator=( base_type::operator vector_type() * o );
                return *this;
            }

            proxy_writable_wrapper& operator/=(const vector_type& o)
            {
                base_type::operator=( base_type::operator vector_type() / o );
                return *this;
            }

            proxy_writable_wrapper& operator+=(const scalar_type& o)
            {
                base_type::operator=( base_type::operator vector_type() + o );
                return *this;
            }

            proxy_writable_wrapper& operator-=(const scalar_type& o)
            {
                base_type::operator=( base_type::operator vector_type() - o );
                return *this;
            }

            proxy_writable_wrapper& operator*=(const scalar_type& o)
            {
                base_type::operator=( base_type::operator vector_type() * o );
                return *this;
            }

            proxy_writable_wrapper& operator/=(const scalar_type& o)
            {
                base_type::operator=( base_type::operator vector_type() / o );
                return *this;
            }
        };
    }
}