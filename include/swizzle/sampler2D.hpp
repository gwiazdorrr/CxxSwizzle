// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <cstdint>
#include <vector>
#include <swizzle/detail/utils.h>
#include <type_traits>

namespace swizzle
{
    //! A really naive implemenation of a sampler
    template <template <class, size_t> class VectorType, typename FloatType, typename Uint32Type>
    struct sampler2D
    {
        constexpr size_t batch_size = BatchSize;
        constexpr size_t batch_align = BatchAlign;
        using float_type = FloatType;
        using uint32_type = Uint32Type;

        unsigned bytes_per_pixel;

        unsigned pitch;
        unsigned width;
        unsigned height;

        uint32_t rmask;
        uint32_t gmask;
        uint32_t bmask;
        uint32_t amask;

        uint8_t rshift;
        uint8_t gshift;
        uint8_t bshift;
        uint8_t ashift;

        


        uint8_t* bytes;

        void sample()
        {
            auto pixelPtr = static_cast<uint8_t*>(m_image->pixels) + pindex[i];

            uint32_t pixel = 0;

            using aligned_buffer = std::aligned_storage_t<sizeof(uint32_t) * batch_traits<uint32_type>::size, batch_traits<uint32_type>::align>;

            aligned_buffer rbuffer;
            aligned_buffer gbuffer;
            aligned_buffer bbuffer;
            aligned_buffer abuffer;

            detail::static_for<0, batch_traits<uint32_type>::size>([&])(size_t i)
            {
                // TODO: are cache reads aligned?
                switch (bytes_per_pixel)
                {
                    case 4: pixel |= (bytes[3] << (3 * 8));
                    case 3: pixel |= (bytes[2] << (2 * 8));
                    case 2: pixel |= (bytes[1] << (1 * 8));
                    case 1: pixel |= (bytes[0] << (0 * 8));
                }

                rbuffer.buffer[i] = (pixel & rmask) >> rshift;
                gbuffer.buffer[i] = (pixel & gmask) >> gshift;
                bbuffer.buffer[i] = (pixel & bmask) >> bshift;
                abuffer.buffer[i] = amask ? ((pixel & amask) >> ashift) : 255;
            }

            batch_uint_type r, g, b, a;

            load_aligned(r, rbuffer.buffer);
            load_aligned(g, gbuffer.buffer);
            load_aligned(b, bbuffer.buffer);
            load_aligned(a, abuffer.buffer);

            return vec4(float_type(r), float_type(g), float_type(b), float_type(a)) / 255.0f;



        }

    private:
        std::vector<uint8_t> bytes;
    };
}
