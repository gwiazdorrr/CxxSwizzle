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
        };
    }
}


#endif // HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_ADAPTER_TRAITS
