#ifndef HEADER_GUARD_SWIZZLE_DETAIL_UTILS
#define HEADER_GUARD_SWIZZLE_DETAIL_UTILS

namespace swizzle
{
    namespace detail
    {
        template <size_t VectorSize, size_t Size1, size_t Size2, size_t Size3, size_t Size4>
        struct are_sizes_valid
        {
            static const bool value = 
                (VectorSize > Size1 || !Size2 && !Size3 && !Size4) &&
                (VectorSize > Size1 + Size2 || !Size3 && !Size4) &&
                (VectorSize > Size1 + Size2 + Size3 || !Size4) &&
                (VectorSize <= Size1 + Size2 + Size3 + Size4);
        };

        template <class T>
        struct scalar_proxy
        {
            T value;

            scalar_proxy(T value) : value(value)
            {}

            operator T() const
            {
                return value;
            }
        };
    }
}

#endif // HEADER_GUARD_SWIZZLE_DETAIL_UTILS
