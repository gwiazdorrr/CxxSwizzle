#ifndef HEADER_GUARD_SWIZZLE_DETAIL_WRITABLE_WRAPPER
#define HEADER_GUARD_SWIZZLE_DETAIL_WRITABLE_WRAPPER

#include "read_only_wrapper.h"

namespace swizzle
{
    namespace detail
    {
        //! Unary and binary operators definitions.
        template <class TBase>
        struct writable_wrapper : read_only_wrapper<TBase>
        {
            typedef TBase base_type;
            typedef typename TBase::vector_type vector_type;
            typedef typename TBase::scalar_type scalar_type;
            

            writable_wrapper& operator=(const vector_type& o)
            {
                base_type::operator=(o);
                return *this;
            }

            writable_wrapper& operator+=(const vector_type& o)
            {
                base_type::operator=( base_type::operator vector_type() + o );
                return *this;
            }

            writable_wrapper& operator-=(const vector_type& o)
            {
                base_type::operator=( base_type::operator vector_type() - o );
                return *this;
            }

            writable_wrapper& operator*=(const vector_type& o)
            {
                base_type::operator=( base_type::operator vector_type() * o );
                return *this;
            }

            writable_wrapper& operator/=(const vector_type& o)
            {
                base_type::operator=( base_type::operator vector_type() / o );
                return *this;
            }

            writable_wrapper& operator+=(const scalar_type& o)
            {
                base_type::operator=( base_type::operator vector_type() + o );
                return *this;
            }

            writable_wrapper& operator-=(const scalar_type& o)
            {
                base_type::operator=( base_type::operator vector_type() - o );
                return *this;
            }

            writable_wrapper& operator*=(const scalar_type& o)
            {
                base_type::operator=( base_type::operator vector_type() * o );
                return *this;
            }

            writable_wrapper& operator/=(const scalar_type& o)
            {
                base_type::operator=( base_type::operator vector_type() / o );
                return *this;
            }
        };
    }
}


#endif  HEADER_GUARD_SWIZZLE_DETAIL_WRITABLE_WRAPPER
