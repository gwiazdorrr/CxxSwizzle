#ifndef HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_TRAITS
#define HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_TRAITS

#include <type_traits>
#include "../detail/vector_traits.h"

namespace swizzle
{
    namespace naive
    {
        template <class TScalar, size_t NumComponents, class TTag>
        struct vector_adapter_traits;

        template <class TTraits>
        class vector_adapter;

        template <class TVector, class TData, class TTag, size_t x, size_t y, size_t z, size_t w>
        class indexed_proxy;
    }

    namespace detail
    {
        template <class T>
        struct get_vector_type_impl_for_scalar
        {
            typedef naive::vector_adapter< naive::vector_adapter_traits<T, 1, nothing> > type;
        };

        template <>
        struct get_vector_type_impl<float> : get_vector_type_impl_for_scalar<float>
        {};

        template <>
        struct get_vector_type_impl<double> : get_vector_type_impl_for_scalar<double>
        {};

        template <>
        struct get_vector_type_impl<int> : get_vector_type_impl_for_scalar<int>
        {};

        template <>
        struct get_vector_type_impl<unsigned short> : get_vector_type_impl_for_scalar<unsigned short>
        {};

        template <class TTraits>
        struct get_vector_type_impl< naive::vector_adapter<TTraits> >
        {
            typedef naive::vector_adapter<TTraits> type;
        };

        template <class TVector, class TData, class TTag, size_t x, size_t y, size_t z, size_t w>
        struct get_vector_type_impl< naive::indexed_proxy<TVector, TData, TTag, x, y, z, w> >
        {
            typedef TVector type;
        };

    }
}


#endif // HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_TRAITS
