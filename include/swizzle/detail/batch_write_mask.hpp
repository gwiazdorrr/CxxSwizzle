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
            mask_type single_masks[TSize];
            // mask_type break_masks[TSize];
            // bool is_loop[TSize];
            // size_t next_break_mask_index;
            size_t mask_index;
            
            batch_write_mask_context()
            {
                masks[0] = mask_type(true);
                //break_masks[0] = mask_type(true);
                //break_masks[1] = mask_type(true);
                mask_index = 0;
                //break_mask_index = 0;
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
            struct invert_tag {};
            struct loop_tag {};

            using mask_type = TMask;
            using base_type = batch_write_mask_base<TMask, TStackSize, TIsSingleThreaded>;
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

            //static batch_write_mask push(bool condition, loop_tag)
            //{
            //    auto mask = static_cast<mask_type>(condition);

            //    mask &= storage.break_masks[storage.break_mask_index];

            //    // clean up the break mask for a nested loop
            //    storage.break_masks[storage.break_mask_index+1] = storage.break_masks[storage.break_mask_index];

            //    ++storage.break_mask_index;

            //    return{ mask };
            //}

            static batch_write_mask push(invert_tag)
            {
                return{ !storage.single_masks[storage.mask_index] };
            }

            static void do_break()
            {
                storage.break_mask[storage.break_mask_index] = !storage.masks[storage.mask_index];
            }

            inline batch_write_mask(const mask_type& mask, bool is_loop = false)
            {
                // save the mask for later negation in "else"
                storage.single_masks[storage.mask_index] = mask;
                //storage.is_loop[storage.mask_index] = is_loop;

                // compute and store the new mask
                // TODO: is the condition needed?
                auto newMask = storage.mask_index == 0 ? mask : mask & storage.masks[storage.mask_index];
                storage.masks[++storage.mask_index] = newMask;


                //storage.is_loop[storage.mask_index] = is_loop;
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