#ifndef HEADER_GUARD_SWIZZLE_DETAIL_READ_ONLY_WRAPPER
#define HEADER_GUARD_SWIZZLE_DETAIL_READ_ONLY_WRAPPER

namespace swizzle
{
    namespace detail
    {
        //! Binary operators definitions.
        template <class TBase>
        struct read_only_wrapper : TBase
        {
            typedef TBase base_type;
            typedef typename TBase::vector_type vector_type;
            typedef typename TBase::scalar_type scalar_type;

            operator vector_type() const
            {
                return base_type::operator vector_type();
            }

            vector_type operator+(const vector_type& o) const
            {
                return base_type::operator vector_type() + o;
            }

            vector_type operator-(const vector_type& o) const
            {
                return base_type::operator vector_type() - o;
            }

            vector_type operator*(const vector_type& o) const
            {
                return base_type::operator vector_type() * o;
            }

            vector_type operator/(const vector_type& o) const
            {
                return base_type::operator vector_type() / o;
            }

            vector_type operator+(const scalar_type & o) const
            {
                return base_type::operator vector_type() + o;
            }

            vector_type operator-(const scalar_type & o) const
            {
                return base_type::operator vector_type() - o;
            }

            vector_type operator*(const scalar_type & o) const
            {
                return base_type::operator vector_type() * o;
            }

            vector_type operator/(const scalar_type & o) const
            {
                return base_type::operator vector_type() / o;
            }
        };

        template<class TBase>
        typename read_only_wrapper<TBase>::vector_type operator+(const typename read_only_wrapper<TBase>::scalar_type& v, read_only_wrapper<TBase> vec)
        {
            return vec + v;
        }

        template<class TBase>
        typename read_only_wrapper<TBase>::vector_type operator-(const typename read_only_wrapper<TBase>::scalar_type& v, read_only_wrapper<TBase> vec)
        {
            return v - static_cast<typename read_only_wrapper<TBase>::vector_type>(vec);
        }

        template<class TBase>
        typename read_only_wrapper<TBase>::vector_type operator*(const typename read_only_wrapper<TBase>::scalar_type& v, read_only_wrapper<TBase> vec)
        {
            return vec * v;
        }

        template<class TBase>
        typename read_only_wrapper<TBase>::vector_type operator/(const typename read_only_wrapper<TBase>::scalar_type& v, read_only_wrapper<TBase> vec)
        {
            return v / static_cast<typename read_only_wrapper<TBase>::vector_type>(vec);
        }

    }
}


#endif  HEADER_GUARD_SWIZZLE_DETAIL_READ_ONLY_WRAPPER
