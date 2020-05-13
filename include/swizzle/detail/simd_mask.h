// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

namespace swizzle
{
    namespace detail
    {
        struct mask_invert_tag {};

        template <typename MaskType, typename RawMaskType, size_t StackSize>
        struct mask_pusher
        {
            using mask_type = MaskType;
            using raw_mask_type = RawMaskType;

            struct storage_type
            {
                raw_mask_type masks[StackSize];
                raw_mask_type single_masks[StackSize];
                size_t mask_index;
            };

            static thread_local storage_type storage;

            static void init_for_thread()
            {
                storage.masks[0] = static_cast<raw_mask_type>(mask_type(true));
                storage.mask_index = 0;
            }

            template <typename T>
            static mask_pusher push(T&& condition)
            {
                return{ static_cast<mask_type>(condition) };
            }

            static mask_pusher push(mask_invert_tag)
            {
                return{ !static_cast<const mask_type&>(storage.single_masks[storage.mask_index]) };
            }

            inline mask_pusher(const mask_type& mask)
            {
                // save the mask for later negation in "else"
                storage.single_masks[storage.mask_index] = static_cast<raw_mask_type>(mask);

                // compute and store the new mask
                auto newMask = mask & static_cast<mask_type>(storage.masks[storage.mask_index]);
                storage.masks[++storage.mask_index] = static_cast<raw_mask_type>(newMask);
            }

            inline ~mask_pusher()
            {
                // pop!
                --storage.mask_index;
            }

            inline operator bool() const
            {
                // let the mask type decide what it means by being "true"; for VC this means
                // at least one value being set IIRC
                return static_cast<bool>(static_cast<mask_type>(storage.masks[storage.mask_index]));
            }

        };

        template <typename MaskPusher>
        struct masked_assign_policy
        {
            template<typename T>
            static void assign(T& target, const T& value)
            {
                if (MaskPusher::storage.mask_index == 0)
                    target = value;
                else
                    target(MaskPusher::storage.masks[MaskPusher::storage.mask_index]) = value;
            }
        };
    }
}