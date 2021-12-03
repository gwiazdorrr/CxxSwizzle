shadertoy_config shadertoy_config::make_default()
{
    // url: https://www.shadertoy.com/view/lldGDr
    shadertoy_config config;

    config.get_pass(pass_type::image).get_sampler(0) = sampler_config::make_buffer(pass_type::buffer_a)
        .init(texture_wrap_modes::clamp, texture_filter_modes::nearest, true);

    config.get_pass(pass_type::image).get_sampler(2) = sampler_config::make_buffer(pass_type::buffer_b)
        .init(texture_wrap_modes::clamp, texture_filter_modes::nearest, true);

    config.get_pass(pass_type::image).get_sampler(1) = sampler_config::make_buffer(pass_type::buffer_c)
        .init(texture_wrap_modes::clamp, texture_filter_modes::nearest, true);

    config.get_pass(pass_type::image).get_sampler(3) = sampler_config::make_buffer(pass_type::buffer_d)
        .init(texture_wrap_modes::clamp, texture_filter_modes::nearest, true);


    config.get_pass(pass_type::buffer_a).get_sampler(0) = sampler_config::make_buffer(pass_type::buffer_a)
        .init(texture_wrap_modes::clamp, texture_filter_modes::nearest, true);

    config.get_pass(pass_type::buffer_a).get_sampler(1) = sampler_config::make_buffer(pass_type::buffer_c)
        .init(texture_wrap_modes::clamp, texture_filter_modes::nearest, true);


    config.get_pass(pass_type::buffer_b).get_sampler(1) = sampler_config::make_buffer(pass_type::buffer_a)
        .init(texture_wrap_modes::clamp, texture_filter_modes::nearest, true);

    config.get_pass(pass_type::buffer_b).get_sampler(0) = sampler_config::make_buffer(pass_type::buffer_b)
        .init(texture_wrap_modes::clamp, texture_filter_modes::nearest, true);


    config.get_pass(pass_type::buffer_c).get_sampler(3) = sampler_config::make_keyboard()
        .init(texture_wrap_modes::clamp, texture_filter_modes::nearest, true);

    config.get_pass(pass_type::buffer_c).get_sampler(0) = sampler_config::make_buffer(pass_type::buffer_a)
        .init(texture_wrap_modes::clamp, texture_filter_modes::nearest, true);

    config.get_pass(pass_type::buffer_c).get_sampler(1) = sampler_config::make_buffer(pass_type::buffer_c)
        .init(texture_wrap_modes::clamp, texture_filter_modes::nearest, true);


    config.get_pass(pass_type::buffer_d).get_sampler(0) = sampler_config::make_buffer(pass_type::buffer_a)
        .init(texture_wrap_modes::clamp, texture_filter_modes::nearest, true);

    config.get_pass(pass_type::buffer_d).get_sampler(2) = sampler_config::make_buffer(pass_type::buffer_b)
        .init(texture_wrap_modes::clamp, texture_filter_modes::nearest, true);

    config.get_pass(pass_type::buffer_d).get_sampler(1) = sampler_config::make_buffer(pass_type::buffer_c)
        .init(texture_wrap_modes::clamp, texture_filter_modes::nearest, true);



    return config;
}