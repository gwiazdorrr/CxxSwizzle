// CxxSwizzle (c) 2013-2021 Piotr Gwiazdowski
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
            mask_type single_masks[TSize];
            int mask_index;
            
            batch_write_mask_context()
            {
                masks[0] = mask_type(true);
                single_masks[0] = mask_type(true);
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

            struct invert_tag   {};
            struct loop_tag     {};
            struct else_if_tag  {};
            struct if_tag       {};
            struct else_tag     {};
   
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

                for (int i = mask.index; i < storage.mask_index; ++i) 
                {
                    storage.masks[i] &= top_level_mask;
                }

                return mask; 
            }

            batch_write_mask() = delete;
            batch_write_mask(const batch_write_mask&) = delete;

            inline batch_write_mask(else_tag) noexcept
            {
                auto& storage = base_type::storage;
                index = ++storage.mask_index;

                mask_type if_mask = storage.single_masks[index];
                storage.masks[index] = (!if_mask) & storage.masks[index - 1];
            } 

            inline batch_write_mask(loop_tag) noexcept
            {
                auto& storage = base_type::storage;
                index = ++storage.mask_index;
                storage.masks[index]        = storage.masks[index - 1];
            }

            inline batch_write_mask(const mask_type& mask, if_tag) noexcept
            {
                auto& storage = base_type::storage;
                index = ++storage.mask_index;
                storage.masks[index] = mask & storage.masks[index - 1];
                storage.single_masks[index] = mask;
            }

            inline batch_write_mask(const mask_type& mask, else_if_tag) noexcept
            {
                auto& storage = base_type::storage;
                index = ++storage.mask_index;
                
                mask_type if_mask = storage.single_masks[index];
                storage.masks[index] = (!if_mask) & storage.masks[index - 1] & mask;
                storage.single_masks[index] = if_mask | mask;
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
    }
}