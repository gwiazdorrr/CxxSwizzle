#pragma once

namespace swizzle
{
    namespace naive
    {
        template <class TexCoordType>
        struct sampler_base
        {
            typedef TexCoordType tex_coord_type;
        };

        template <class Sampler>
        auto texture(Sampler& sampler, typename Sampler::tex_coord_type coord) -> decltype ( sampler.sample(coord) )
        {
            return sampler.sample(coord);
        }

        //template <class Sampler>
        //auto texture(const Sampler& sampler, typename Sampler::tex_coord_type coord, float bias) -> decltype ( sampler.sample(coord, bias) )
        //{
        //    return sampler.sample(coord, bias);
        //}

        //template <class Sampler>
        //auto textureOffset(const Sampler& sampler, typename Sampler::tex_coord_type coord, typename Sampler::tex_offset_type offset) -> decltype( sampler.sampleOffset(coord, offset) )
        //{
        //    return sampler.sample(coord, offset);
        //}

        //template <class Sampler>
        //auto textureOffset(const Sampler& sampler, typename Sampler::tex_coord_type coord, typename Sampler::tex_offset_type offset, float bias) -> decltype( sampler.sampleOffset(coord, offset, bias) )
        //{
        //    return sampler.sample(coord, offset, bias);
        //}

    }
}