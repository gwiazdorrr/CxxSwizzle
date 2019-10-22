// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <type_traits>
#include <cstdint>
#include <swizzle/detail/utils.h>
#include <swizzle/detail/vector_traits.h>

namespace swizzle
{
    namespace detail
    {
        template <typename MaskType, size_t Size>
        struct batch_write_mask_context
        {
            using mask_type = MaskType;

            mask_type masks[Size];
            mask_type single_masks[Size];
            size_t mask_index;

            batch_write_mask_context()
            {
                masks[0] = mask_type(true);
                mask_index = 0;
            }
        };

        template <typename MaskType, size_t StackSize, bool IsSingleThreaded = false>
        struct batch_write_mask_base
        {
            inline static thread_local batch_write_mask_context<MaskType, StackSize> storage;
        };

        template <typename MaskType, size_t StackSize>
        struct batch_write_mask_base<MaskType, StackSize, true>
        {
            inline static batch_write_mask_context<MaskType, StackSize> storage;
        };

        template <typename MaskType, size_t StackSize, bool IsSingleThreaded = false>
        struct batch_write_mask : batch_write_mask_base<MaskType, StackSize, IsSingleThreaded>
        {
            struct invert_tag {};

            using mask_type = MaskType;
            using base_type = batch_write_mask_base<MaskType, StackSize, IsSingleThreaded>;
            using base_type::storage;

            template <typename T>
            static batch_write_mask push(T && condition)
            {
                return{ static_cast<mask_type>(condition) };
            }

            static batch_write_mask push(bool condition)
            {
                return{ static_cast<mask_type>(condition) };
            }

            static batch_write_mask push(invert_tag)
            {
                return{ !storage.single_masks[storage.mask_index] };
            }

            inline batch_write_mask(const mask_type& mask)
            {
                // save the mask for later negation in "else"
                storage.single_masks[storage.mask_index] = mask;

                // compute and store the new mask
                auto newMask = storage.mask_index == 0 ? mask : mask & storage.masks[storage.mask_index];
                storage.masks[++storage.mask_index] = newMask;
            }

            inline ~batch_write_mask()
            {
                // pop!
                --storage.mask_index;
            }

            inline operator bool() const
            {
                // let the mask type decide what it means by being "true"
                return batch_collapse(storage.masks[storage.mask_index]);
            }
        };
    }
}