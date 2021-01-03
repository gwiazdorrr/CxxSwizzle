// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

// define CXXSWIZZLE_WRITE_MASK_SCOPE_SINGLETHREADED if you want faster write_mask_scope.
// but unsafe to use in a multithreaded scenario

namespace swizzle
{
    namespace detail
    {
        template <typename TMask, size_t TStackSize>
        struct write_mask_scope
        {
            struct invert_tag {};
            using mask_type = TMask;

            struct storage_type
            {
                mask_type masks[TStackSize];
                mask_type single_masks[TStackSize];
                size_t mask_index;

                storage_type()
                {
                    masks[0] = mask_type(true);
                    mask_index = 0;
                }
            };

#if CXXSWIZZLE_WRITE_MASK_SCOPE_SINGLETHREADED
            static storage_type storage;
#else
            static thread_local storage_type storage;
#endif  

            template <typename T>
            static write_mask_scope push(T && condition)
            {
                return{ static_cast<mask_type>(condition) };
            }

            static write_mask_scope push(bool condition)
            {
                return{ static_cast<mask_type>(condition) };
            }

            static write_mask_scope push(invert_tag)
            {
                return{ !storage.single_masks[storage.mask_index] };
            }

            inline write_mask_scope(const mask_type& mask)
            {
                // save the mask for later negation in "else"
                storage.single_masks[storage.mask_index] = mask;

                // compute and store the new mask
                auto newMask = storage.mask_index == 0 ? mask : mask & storage.masks[storage.mask_index];
                storage.masks[++storage.mask_index] = newMask;
            }

            inline ~write_mask_scope()
            {
                // pop!
                --storage.mask_index;
            }

            inline operator bool() const
            {
                // let the mask type decide what it means by being "true"
                return simd_to_bool(storage.masks[storage.mask_index]);
            }
        };

        template <typename MaskPusher>
        struct write_mask_with_branch_assign_policy
        {
            template<typename T>
            static inline void assign(T& target, const T& value)
            {
                if (MaskPusher::storage.mask_index == 0)
                    target = value;
                else
                    target((typename T::TMask)MaskPusher::storage.masks[MaskPusher::storage.mask_index]) = value;
            }
        };

        template <typename MaskPusher>
        struct write_mask_assign_policy
        {
            template<typename T>
            static inline void assign(T& target, const T& value)
            {
                target((typename T::TMask)MaskPusher::storage.masks[MaskPusher::storage.mask_index]) = value;
            }
        };

#if CXXSWIZZLE_WRITE_MASK_SCOPE_SINGLETHREADED
#define CXXSWIZZLE_WRITE_MASK_SCOPE_STORAGE(type) type::storage_type type::storage
#else
#define CXXSWIZZLE_WRITE_MASK_SCOPE_STORAGE(type) thread_local type::storage_type type::storage
#endif  

    }
}