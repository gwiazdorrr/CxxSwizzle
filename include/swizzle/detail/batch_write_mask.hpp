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
        template <typename TMask, size_t TSize>
        struct batch_write_mask_context
        {
            using mask_type = TMask;

            mask_type masks[TSize];
            int mask_index;
            
            batch_write_mask_context()
            {
                masks[0] = mask_type(true);
                mask_index = 0;
            }
        };

        template <typename TMask, size_t TStackSize, bool TIsSingleThreaded = false>
        struct batch_write_mask_base
        {
            inline static thread_local batch_write_mask_context<TMask, TStackSize> storage;
        };

        template <typename TMask, size_t TStackSize>
        struct batch_write_mask_base<TMask, TStackSize, true>
        {
            inline static batch_write_mask_context<TMask, TStackSize> storage;
        };



        template <typename TMask, size_t TStackSize, bool TIsSingleThreaded = false>
        struct batch_write_mask : batch_write_mask_base<TMask, TStackSize, TIsSingleThreaded>
        {
            int index;

            struct invert_tag {};
            struct copy_tag   {};
   
            using mask_type = TMask;
            using base_type = batch_write_mask_base<TMask, TStackSize, TIsSingleThreaded>;
            using this_type = batch_write_mask;


            static void reset()
            {
                auto& storage = base_type::storage;
                storage.masks[0] = mask_type(true);
                storage.mask_index = 0;
            }

            static const bool multiply_to(batch_write_mask& mask)
            {
                auto& storage = base_type::storage;
                auto top_level_mask = !storage.masks[storage.mask_index];

                for (int i = mask.index; i < storage.mask_index; ++i) {
                    storage.masks[i] &= top_level_mask;
                }

                return mask; 
            }

            batch_write_mask() = delete;
            batch_write_mask(const batch_write_mask&) = delete;

            inline batch_write_mask(invert_tag) noexcept
            {
                auto& storage = base_type::storage;
                index = ++storage.mask_index;
                // invert the previous mask leftover
                storage.masks[index]        = !storage.masks[index] & storage.masks[index - 1];
            } 

            inline batch_write_mask(copy_tag) noexcept
            {
                auto& storage = base_type::storage;
                index = ++storage.mask_index;
                storage.masks[index]        = storage.masks[index - 1];
            }

            inline batch_write_mask(bool b) noexcept
                : this_type(static_cast<mask_type>(b))
            {}

            inline batch_write_mask(const mask_type& mask) noexcept
            {
                auto& storage = base_type::storage;
                index = ++storage.mask_index;
                storage.masks[index]        = mask & storage.masks[index - 1];
            }

            inline ~batch_write_mask()
            {
                auto& storage = base_type::storage;
                assert(storage.mask_index == index);
                storage.mask_index = index - 1;
            }

            inline operator bool() const
            {
                auto& storage = base_type::storage;
                // let the mask type decide what it means by being "true"
                return batch_collapse(storage.masks[index]);
            }

            inline batch_write_mask& operator&=(const mask_type& mask)
            {
                auto& storage = base_type::storage;
                assert(index == storage.mask_index);
                storage.masks[index] &= mask;
                return *this;
            }

        };

        //template <typename TMask, size_t TStackSize, bool TIsSingleThreaded = false>
        //struct batch_write_loop_mask : batch_write_mask<TMask, TStackSize, TIsSingleThreaded>
        //{
        //    inline batch_write_loop_mask(const mask_type& mask)
        //        : this(mask & storage.batch_masks[++storage.batch_mask_index])
        //    {
        //        // prepare for a nested loop
        //        storage.break_masks[storage.break_mask_index + 1] = storage.break_masks[storage.break_mask_index];
        //        storage.break_masks[storage.break_mask_index + 1] = storage.break_masks[storage.break_mask_index];
        //    }

        //    inline ~batch_write_loop_mask()
        //    {
        //        // pop!
        //        --storage.mask_index;
        //    }
        //};
    }
}