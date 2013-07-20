#ifndef HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_TRAITS
#define HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_TRAITS

#include <type_traits>
#include "../detail/writable_wrapper.h"
#include "../detail/vector_traits.h"

namespace swizzle
{
    namespace naive
    {
        template <class TScalar, size_t NumComponents, class TTag>
        struct vector_adapter_traits
        {
            typedef TTag tag_type;
            typedef TScalar scalar_type;
            static const size_t num_of_components = NumComponents;

            template <size_t NewNumComponents>
            struct change_num_of_components
            {
                typedef ::swizzle::naive::vector_adapter_traits<TScalar, NewNumComponents, TTag> type;
            };

            template <class TTraits>
            struct is_compatible
            {
                static const bool value = false;
            };

            template <class TTag2>
            struct is_compatible< vector_adapter_traits<scalar_type, num_of_components, TTag2> >
            {
                static const bool value = true;
            };
        };

        template <class TTraits>
        class vector_adapter;

        template <template <class> class TVector, class TTraits, size_t x, size_t y, size_t z, size_t w>
        struct vector_proxy;
    }

    namespace detail
    {
        struct empty
        {

        };

        struct not_available
        {
        private:
            not_available();
        };




        template <class T, bool isArithmetic>
        struct vector_traits_fallback;
        //{};


        template <class T>
        struct vector_traits_fallback<T, true>
            : vector_traits_base< naive::vector_adapter< naive::vector_adapter_traits<T, 1, empty> > >
        {};

        template <class T>
        struct vector_traits /*vector_traits_fallback<T, std::is_arithmetic<T>::value>*/
            : std::enable_if< std::is_arithmetic<T>::value, 
                vector_traits_base< naive::vector_adapter< naive::vector_adapter_traits<T, 1, empty> > >
            >::type
        {
        };

        template <class TTraits>
        struct vector_traits< naive::vector_adapter<TTraits> > 
            : vector_traits_base< naive::vector_adapter<TTraits>  >
        {};

        template <template <class> class TVector, class TTraits, size_t x, size_t y, size_t z, size_t w>
        struct vector_traits< naive::vector_proxy<TVector, TTraits, x, y, z, w> > 
            : vector_traits_base< typename naive::vector_proxy<TVector, TTraits, x, y, z, w>::vector_type >
        {};

        template <class TBase>
        struct vector_traits< detail::writable_wrapper<TBase> >
            : vector_traits< TBase >
        {};


    }
}


#endif  HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_TRAITS
