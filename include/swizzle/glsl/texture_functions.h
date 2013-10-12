// CxxSwizzle
// Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

namespace swizzle
{
    namespace glsl
    {
        namespace texture_functions
        {
            //! Deriving from this struct will make sure all texture* calls are going to be ADL resolved, without
            //! "using" this namespace.
            struct tag {};

            template <class Sampler>
            auto texture(Sampler& sampler, typename Sampler::tex_coord_type coord) -> decltype ( sampler.sample(coord) )
            {
                return sampler.sample(coord);
            }

            template <class Sampler>
            auto texture(const Sampler& sampler, typename Sampler::tex_coord_type coord, float bias) -> decltype ( sampler.sample(coord, bias) )
            {
                return sampler.sample(coord, bias);
            }

            template <class Sampler>
            auto textureOffset(const Sampler& sampler, typename Sampler::tex_coord_type coord, typename Sampler::tex_offset_type offset) -> decltype( sampler.sampleOffset(coord, offset) )
            {
                return sampler.sample(coord, offset);
            }

            template <class Sampler>
            auto textureOffset(const Sampler& sampler, typename Sampler::tex_coord_type coord, typename Sampler::tex_offset_type offset, float bias) -> decltype( sampler.sampleOffset(coord, offset, bias) )
            {
                return sampler.sample(coord, offset, bias);
            }
        }
    }
}