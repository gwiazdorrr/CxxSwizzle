#ifndef HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_ADAPTER_TRAITS
#define HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_ADAPTER_TRAITS

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
    }
}


#endif  HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_ADAPTER_TRAITS
