// CxxSwizzle (c) 2013-2021 Piotr Gwiazdowski

#include "shadertoy_sandbox.hpp"

using namespace swizzle;
using namespace shadertoy;

static_assert(sizeof(vec2) == sizeof(swizzle::float_type[2]), "Too big");
static_assert(sizeof(vec3) == sizeof(swizzle::float_type[3]), "Too big");
static_assert(sizeof(vec4) == sizeof(swizzle::float_type[4]), "Too big");

#include <ctime>
#include <functional>
#include <future>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <filesystem>
#include <optional>
#include <algorithm>

#ifdef CONFIG_ENABLE_PARALLELISM
#include <execution>
#endif

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_video.h>
#include <SDL_image.h>


#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>


#ifdef TRACY_ENABLE
#include <Tracy.hpp>
#endif

using swizzle::detail::nonmasked;

//-----------------------------------------------------------------------

#define CXXSWIZZLE_CAT(a,b)         CXXSWIZZLE_CAT_INNER(a,b) 
#define CXXSWIZZLE_CAT_INNER(a,b)   a ## b
#define CXXSWIZZLE_SCOPE_EXIT       auto CXXSWIZZLE_CAT(scope_exit_, __LINE__) = utils::make_scope_exit

namespace utils
{
    struct for_iterator
    {
        using iterator_category = std::forward_iterator_tag;
        using value_type = int;
        using difference_type = std::ptrdiff_t;
        using pointer = const int*;
        using reference = const int&;

        for_iterator() : index(0), step(0) {}
        for_iterator(int index, int step = 1) : index(index), step(step) {}

        int operator*() const
        {
            return index;
        }

        for_iterator& operator++()
        {
            index += step;
            return *this;
        }
        for_iterator operator++(int)
        {
            for_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator== (const for_iterator& a, const for_iterator& b)
        {
            return a.index == b.index;
        };

        friend bool operator!= (const for_iterator& a, const for_iterator& b)
        {
            return a.index != b.index;
        };

        int index;
        int step;
    };

    template <typename F>
    struct scope_exit 
    {
        F f;

        explicit scope_exit(F f) noexcept : f(std::move(f))
        {}

        ~scope_exit()
        {
            f();
        }
    };

    template <typename F>
    scope_exit<F> make_scope_exit(F&& f) noexcept
    {
        return scope_exit<F>{ std::forward<F>(f) };
    }

    std::string replace_all(std::string str, std::string token, std::string replacement)
    {
        size_t start_pos = 0;
        while ((start_pos = str.find(token, start_pos)) != std::string::npos)
        {
            str.replace(start_pos, token.length(), replacement);
            start_pos += replacement.length();
        }
        return str;
    }

    template <class Rep, class Period>
    constexpr auto duration_to_seconds(const std::chrono::duration<Rep, Period>& d)
    {
        return std::chrono::duration<double>(d).count();
    }

    template <typename T>
    bool from_string(const char* str, T& value)
    {
        std::stringstream s;
        s << str;
        return !!(s >> value);
    }
}

//-----------------------------------------------------------------------

namespace stl_utils
{
    // some SDL utils

    struct sdl_deleter
    {
        void operator()(SDL_Surface* ptr)
        {
            SDL_FreeSurface(ptr);
        }
        void operator()(SDL_Window* ptr)
        {
            SDL_DestroyWindow(ptr);
        }
        void operator()(SDL_Texture* ptr)
        {
            SDL_DestroyTexture(ptr);
        }
        void operator()(SDL_Renderer* ptr)
        {
            SDL_DestroyRenderer(ptr);
        }
    };

    template <typename T>
    using sdl_ptr = std::unique_ptr<T, sdl_deleter>;

    template <typename TSdlType>
    sdl_ptr<TSdlType> make_sdl_ptr(TSdlType* ptr)
    {
        return sdl_ptr<TSdlType>(ptr);
    }

    template <typename TFunc, typename... TArgs>
    auto create_sdl_object_or_abort(TFunc&& f, TArgs&&... args)
    {
        auto ptr = make_sdl_ptr(f(std::forward<TArgs>(args)...));
        if (!ptr)
        {
            fprintf(stderr, "SDL error: %s\n", SDL_GetError());
            abort();
        }
        return ptr;
    }

    template <typename TRenderTarget>
    sdl_ptr<SDL_Surface> create_matching_sdl_surface(TRenderTarget& target)
    {
        return create_sdl_object_or_abort(SDL_CreateRGBSurfaceWithFormatFrom, target.first_row, target.width, target.height, 32, target.pitch, target.sdl_pixelformat);
    }

    sdl_ptr<SDL_Texture> create_matching_sdl_texture(SDL_Renderer* renderer, int w, int h)
    {
        return create_sdl_object_or_abort(SDL_CreateTexture, renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, w, h);
    };

    int sdl_keysym_to_ascii(SDL_Keysym keysym) {

        if (keysym.sym >= SDLK_a && keysym.sym <= SDLK_z)
        {
            if (keysym.mod & KMOD_SHIFT)
            {
                return int('A') + keysym.sym - 'a';
            }
            else
            {
                return keysym.sym;
            }
        }

        if (keysym.sym < 256)
        {
            return keysym.sym;
        }

        switch (keysym.sym)
        {
        case SDLK_PAGEUP:  return 33;
        case SDLK_PAGEDOWN:  return 34;
        case SDLK_END:  return 35;
        case SDLK_HOME: return 36;
        case SDLK_LEFT: return 37;
        case SDLK_UP: return 38;
        case SDLK_RIGHT: return 39;
        case SDLK_DOWN: return 40;
        default:
            return -1;
        }
    }
}

//-----------------------------------------------------------------------

namespace imgui_utils
{
    template <typename ... Ts>
    float imgui_text_centered(float width, bool enabled, const char* fmt, Ts ... ts)
    {
        char timeBuffer[64];
        int len = sprintf(timeBuffer, fmt, ts...);
        float textWidth = ImGui::CalcTextSize(timeBuffer).x;

        float padding = std::max(width / 2.0f - textWidth / 2.0f, 0.0f);

        if (padding > 0)
        {
            ImGui::Spacing();
            ImGui::SameLine(0, padding);
        }

        if (enabled)
        {
            ImGui::Text("%s", timeBuffer);
        }
        else
        {
            ImGui::TextDisabled("%s", timeBuffer);
        }

        if (padding > 0)
        {
            ImGui::SameLine(0, padding);
            ImGui::Spacing();
        }

        return padding;
    }
}

//-----------------------------------------------------------------------
// render targs; they need to be properly aligned to allow for SIMD variant
// rgba32 is used for, well, target, float - buffers, a8 - helps with keyboards.


constexpr auto columns_per_batch = static_cast<int>(::swizzle::detail::scalar_traits<float_type>::num_columns);
constexpr auto rows_per_batch = static_cast<int>(::swizzle::detail::scalar_traits<float_type>::num_rows);

struct aligned_render_target_base
{
    struct void_deleter
    {
        void operator()(void* ptr)
        {
            ::operator delete(ptr);
        }
    };

    std::unique_ptr<void, void_deleter> memory;
    void* first_row = nullptr;
    int pitch = 0;
    int width = 0;
    int height = 0;
    size_t pixels_size = 0;

    aligned_render_target_base() = default;
    aligned_render_target_base(int width, int height, int bpp, int align) : width(width), height(height)
    {
        const int mask = align - 1;

        // each row has to be aligned
        pitch = (width * bpp + mask) & (~mask);
        pixels_size = static_cast<size_t>(pitch * height);
        size_t alloc_size = pixels_size + align - 1;

        // alloc and align
        memory.reset(::operator new (alloc_size));
        first_row = memory.get();
        first_row = std::align(align, pixels_size, first_row, alloc_size);
        memset(memory.get(), 0, alloc_size);
    }


    bool is_valid()
    {
        return memory.get();
    }
};

struct render_target_rgba32 : aligned_render_target_base
{
    static constexpr int bytes_per_pixel = 4;
    static constexpr uint32_t sdl_pixelformat = SDL_PIXELFORMAT_RGBA32;

    render_target_rgba32() = default;
    render_target_rgba32(int width, int height) : aligned_render_target_base(width, height, bytes_per_pixel, 32) {}


    void store(uint8_t* ptr, const vec4& color, size_t pitch)
    {
        store_rgba8_aligned(color.r, color.g, color.b, color.a, ptr, pitch);
    }
};

struct render_target_a8 : aligned_render_target_base
{
    static constexpr int bytes_per_pixel = 1;
    static constexpr uint32_t sdl_pixelformat = SDL_PIXELFORMAT_INDEX8;

    render_target_a8() = default;
    render_target_a8(int width, int height) : aligned_render_target_base(width, height, bytes_per_pixel, 32) {}

    ptrdiff_t offset(int x, int y) const
    {
        return (height - 1 - y) * pitch + x * bytes_per_pixel;
    }

    uint8_t get(int x, int y) const
    {
        return *(reinterpret_cast<uint8_t*>(first_row) + offset(x, y));
    }

    void set(int x, int y, uint8_t value)
    {
        *(reinterpret_cast<uint8_t*>(first_row) + offset(x, y)) = value;
    }
};

struct render_target_float : aligned_render_target_base
{
    static constexpr int bytes_per_pixel = 4 * sizeof(float);

    render_target_float() = default;
    render_target_float(int width, int height) : aligned_render_target_base(width, height, bytes_per_pixel, 32) {}

    void store(uint8_t* ptr, const vec4& color, size_t pitch)
    {
        store_rgba32f_aligned(color.r, color.g, color.b, color.a, ptr, pitch);
    }

    void set(int x, int y, float r, float g, float b, float a)
    {
        float* p = reinterpret_cast<float*>(reinterpret_cast<uint8_t*>(first_row) + y * pitch + x * bytes_per_pixel);
        p[0] = r;
        p[1] = g;
        p[2] = b;
        p[3] = a;
    }

    std::tuple<float, float, float, float> get(int x, int y) const
    {
        float* p = reinterpret_cast<float*>(reinterpret_cast<uint8_t*>(first_row) + y * pitch + x * bytes_per_pixel);
        return std::make_tuple(p[0], p[1], p[2], p[3]);
    }

    void convert(render_target_rgba32& dst) const
    {
        using namespace std;

        assert(width == dst.width);
        assert(height == dst.height);

        auto work = [&](int y)
        {
            auto p = static_cast<uint8_t*>(dst.first_row) + y * dst.pitch;
            for (int x = 0; x < dst.width; ++x)
            {
                auto t = get(x, y);

                auto ir = max(0, min(255, static_cast<int>(std::get<0>(t) * 256)));
                auto ig = max(0, min(255, static_cast<int>(std::get<1>(t) * 256)));
                auto ib = max(0, min(255, static_cast<int>(std::get<2>(t) * 256)));
                auto ia = max(0, min(255, static_cast<int>(std::get<3>(t) * 256)));
                uint32_t rgba = ir | (ig << 8) | (ib << 16) | (static_cast<unsigned>(ia) << 24);
                *reinterpret_cast<uint32_t*>(p) = rgba;
                p += dst.bytes_per_pixel;
            }
        };

#ifdef CONFIG_ENABLE_PARALLELISM
        std::for_each(std::execution::par_unseq, utils::for_iterator(0), utils::for_iterator(height), std::move(work));
#else
        for (int y = 0; y < height; ++y) 
        {
            work(y);
        }
#endif
    }
};

//-----------------------------------------------------------------------

struct sampler_config
{
    std::string label;

    sampler_type type = sampler_type::none;
    std::vector<std::string> paths;

    texture_wrap_modes wrap_mode;
    texture_filter_modes filter_mode = texture_filter_modes::linear;
    bool vflip = true;

    int source_buffer_index;

    static sampler_config make_texture(std::string_view path) 
    {
        sampler_config result = make_base(path, sampler_type::texture);
        result.paths.push_back(static_cast<std::string>(path));
        return result;
    }

    static sampler_config make_cubemap(const std::array<std::string_view, 6>& faces)
    {
        sampler_config result = make_base(faces[0], sampler_type::cubemap);
        for (auto view : faces)
        {
            result.paths.push_back(static_cast<std::string>(view));
        }
        return result;
    }

    static sampler_config make_buffer(pass_type pass)
    {
        int index = static_cast<int>(pass);
        sampler_config result = make_base(std::string("buffer_") + static_cast<char>('a' + index), sampler_type::buffer);
        result.source_buffer_index = index;
        return result;
    }

    static sampler_config make_keyboard()
    {
        return make_base("keyboard", sampler_type::keyboard);
    }

    static sampler_config make_base(std::string_view label, sampler_type type)
    {
        sampler_config result;
        result.label = label;
        result.type = type;
        return result;
    }

    sampler_config init(texture_wrap_modes wrap_mode = texture_wrap_modes::clamp, texture_filter_modes filter_mode = texture_filter_modes::linear, bool vflip = true)
    {
        this->wrap_mode = wrap_mode;
        this->filter_mode = filter_mode;
        this->vflip = vflip;
        return *this;
    }

};

struct pass_config
{
    sampler_config samplers[shadertoy::num_samplers];
    sampler_config& get_sampler(int index)
    {
        return samplers[index];
    }
};

struct shadertoy_config
{
    pass_config passes[1 + shadertoy::num_buffers];
    pass_config& get_pass(pass_type pass)
    {
        return passes[static_cast<int>(pass)];
    }

    const pass_config& get_pass(pass_type pass) const
    {
        return passes[static_cast<int>(pass)];
    }

    static shadertoy_config make_default();
};

//-----------------------------------------------------------------------

using texture_map = std::unordered_map<std::string, stl_utils::sdl_ptr<SDL_Surface>>;

using buffer_render_target_list = std::array<render_target_float, shadertoy::num_buffers>;

struct shadertoy_input_textures
{
    const texture_map& textures;
    const render_target_a8* keyboard;
    std::array<const render_target_float*, shadertoy::num_buffers> buffers;
    
    shadertoy_input_textures(const texture_map& textures, const render_target_a8& keyboard, const buffer_render_target_list& buffers) : textures(textures), keyboard(&keyboard)
    {
        std::transform(buffers.begin(), buffers.end(), this->buffers.begin(), [](const render_target_float& f) -> const render_target_float* { return &f; });
    }
};

struct shadertoy_render_targets
{
    int width;
    int height;
    int num_passes;

    render_target_rgba32 target;
    render_target_rgba32 target_tmp;

    // buffers are double buffered (sic!)
    buffer_render_target_list buffers[2];

    shadertoy_render_targets(int width, int height)
        : width(width), height(height), num_passes(1)
    {
        // let's make life easier and make sure surfaces are aligned to the batch size
        auto aligned_w = ((width + columns_per_batch - 1) / columns_per_batch) * columns_per_batch;
        auto aligned_h = ((height + rows_per_batch - 1) / rows_per_batch) * rows_per_batch;

        for (pass_type pass : shadertoy::passes)
        {
            if (pass != pass_type::image)
            {
                buffers[0][static_cast<int>(pass)] = render_target_float(aligned_w, aligned_h);
                buffers[1][static_cast<int>(pass)] = render_target_float(aligned_w, aligned_h);
                ++num_passes;
            }
        }

        target = render_target_rgba32(aligned_w, aligned_h);
        target_tmp = render_target_rgba32(aligned_w, aligned_h);
    }
};

struct shadertoy_date
{
    std::chrono::system_clock::time_point time_point;

    void now()
    {
        time_point = std::chrono::system_clock::now();
    }

    swizzle::vec4 consume()
    {
        using sc = std::chrono::system_clock;
        auto now = time_point;
        auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
        auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(now - seconds).count();

        auto t = sc::to_time_t(now);
        std::tm* lt = std::localtime(&t);
        return {
            static_cast<float>(1900 + lt->tm_year),
            static_cast<float>(lt->tm_mon),
            static_cast<float>(lt->tm_mday),
            static_cast<float>((lt->tm_hour * 60 + lt->tm_min) * 60 + lt->tm_sec + microseconds / 1000000.0)
        };
    }
};

struct shadertoy_keyboard
{
    render_target_a8 data = { 256, 3 };

    static const int keyboard_row_down = 0;
    static const int keyboard_row_pressed = 1;
    static const int keyboard_row_toggle = 2;

    void consume(shadertoy_keyboard& target)
    {
        assert(target.data.pixels_size == data.pixels_size);
        memcpy(target.data.first_row, data.first_row, target.data.pixels_size);

        // clear pressed flag
        for (int i = 0; i < 256; ++i)
        {
            data.set(i, keyboard_row_pressed, 0);
        }
    }

    void key_up(uint8_t ascii)
    {
        data.set(ascii, keyboard_row_down, 0);
    }

    void key_down(uint8_t ascii)
    {
        data.set(ascii, keyboard_row_down, 0xFF);
        data.set(ascii, keyboard_row_pressed, 0xFF);
        data.set(ascii, keyboard_row_toggle, data.get(ascii, 2) ? 0 : 0xFF);
    }
};

struct shadertoy_mouse
{
    int mouse_x = 0;
    int mouse_y = 0;
    int mouse_press_x = 0;
    int mouse_press_y = 0;
    bool mouse_pressed = false;
    bool mouse_clicked = false;
    bool had_mouse_click = false;

    vec4 consume()
    {
        vec4 mouse(
            static_cast<float>(mouse_x),
            static_cast<float>(mouse_y),
            (mouse_pressed ? 1.0f : -1.0f) * mouse_press_x,
            had_mouse_click ? (mouse_clicked ? 1.0f : -1.0f) * (mouse_press_y) : 0
        );


        mouse_clicked = false;

        return mouse;
    }

    void motion(int x, int y)
    {
        if (mouse_pressed)
        {
            mouse_x = x;
            mouse_y = y;
        }
    }

    void button_up(int x, int y)
    {
        mouse_pressed = false;
    }

    void button_down(int x, int y)
    {
        had_mouse_click = mouse_clicked = mouse_pressed = true;
        mouse_press_x = mouse_x = x;
        mouse_press_y = mouse_y = y;
    }
};

struct shadertoy_renderer
{
    struct progress
    {
        std::atomic_bool cancel;
        std::atomic_int num_pixels;
        std::atomic<pass_type> current_pass;
    };

    struct stats
    {
        std::chrono::microseconds duration;
        int num_pixels;
    };

    static stats render(const shadertoy_config& config, shader_inputs inputs, render_target_rgba32& target, buffer_render_target_list& buffers, shadertoy_input_textures context, progress& progress, bool parallel)
    {
        progress.num_pixels = 0;

        std::chrono::microseconds duration(0);

        duration += render_pass_if_enabled<pass_type::buffer_a>(config, inputs, buffers[0], context, progress, parallel).duration;
        context.buffers[0] = &buffers[0];

        duration += render_pass_if_enabled<pass_type::buffer_b>(config, inputs, buffers[1], context, progress, parallel).duration;
        context.buffers[1] = &buffers[1];

        duration += render_pass_if_enabled<pass_type::buffer_c>(config, inputs, buffers[2], context, progress, parallel).duration;
        context.buffers[2] = &buffers[2];

        duration += render_pass_if_enabled<pass_type::buffer_d>(config, inputs, buffers[3], context, progress, parallel).duration;
        context.buffers[3] = &buffers[3];

        auto result = render_pass_if_enabled<pass_type::image>(config, inputs, target, context, progress, parallel);
        result.duration += duration;

        return result;
    };

private:
    template <pass_type pass, typename TRenderTarget>
    static stats render_pass_if_enabled(const shadertoy_config& config, shader_inputs& inputs, TRenderTarget& render_target, shadertoy_input_textures& context, progress& progress, bool parallel)
    {
        constexpr shader_function fun = shadertoy::get_pass_func(pass);
        if constexpr (fun != nullptr)
        {
            sampler_generic_data data_buffer[6 * num_samplers];
            set_shader_inputs_samplers(inputs, data_buffer, config.get_pass(pass), context);

            progress.current_pass = pass;
            stats result = render_inner<fun>(inputs, render_target, progress.num_pixels, progress.cancel, parallel);
            return result;
        }
        else
        {
            return stats{ std::chrono::microseconds{}, 0 };
        }
    }

    static swizzle::sampler_generic_data make_sampler_data(const render_target_float& rt)
    {
        return sampler_generic_data::make(rt.first_row, rt.width, rt.height, rt.pitch, swizzle::texture_formats::r32g32b32a32_float);
    }

    static swizzle::sampler_generic_data make_sampler_data(const render_target_a8& rt)
    {
        return sampler_generic_data::make(rt.first_row, rt.width, rt.height, rt.pitch, swizzle::texture_formats::a8);
    }

    static swizzle::sampler_generic_data make_sampler_data(const SDL_Surface* surface)
    {
        auto sdl_format = surface->format->format;
        assert(sdl_format == SDL_PIXELFORMAT_INDEX8 || sdl_format == SDL_PIXELFORMAT_RGBA32);
        return sampler_generic_data::make(surface->pixels, surface->w, surface->h, surface->pitch,
            sdl_format == SDL_PIXELFORMAT_INDEX8 ? swizzle::texture_formats::a8 : swizzle::texture_formats::r8g8b8a8
        );
    }

    static void set_shader_inputs_samplers(shader_inputs& inputs, sampler_generic_data* data_buffer, const pass_config& pass, const shadertoy_input_textures& context)
    {
        std::array<sampler2D*, shadertoy::num_samplers> samplers =
        {
            &inputs.iChannel0,
            &inputs.iChannel1,
            &inputs.iChannel2,
            &inputs.iChannel3,
        };

        for (int sampler_no = 0; sampler_no < shadertoy::num_samplers; ++sampler_no)
        {
            const sampler_config& data = pass.samplers[sampler_no];

            samplers[sampler_no]->data = data_buffer;
            samplers[sampler_no]->face_count = 1;
            samplers[sampler_no]->wrap_mode = data.wrap_mode;
            samplers[sampler_no]->filter_mode = data.filter_mode;
            samplers[sampler_no]->vflip = data.vflip;

            if (data.type == sampler_type::buffer)
            {
                *data_buffer++ = make_sampler_data(*context.buffers[data.source_buffer_index]);
            }
            else if (data.type == sampler_type::keyboard)
            {
                *data_buffer++ = make_sampler_data(*context.keyboard);
            }
            else if (data.type == sampler_type::cubemap || data.type == sampler_type::texture)
            {
                samplers[sampler_no]->face_count = data.paths.size();
                for (auto& path : data.paths)
                {
                    auto found = context.textures.find(path);
                    if (found == context.textures.end())
                    {
                        fprintf(stderr, "ERROR: texture not found: %s\n", path.c_str());
                        abort();
                    }
                    *data_buffer++ = make_sampler_data(found->second.get());
                }
            }

            nonmasked(inputs.iChannelResolution[sampler_no]) = { samplers[sampler_no]->data->width, samplers[sampler_no]->data->height, 0 };
            nonmasked(inputs.iChannelTime[sampler_no]) = 0.0f;
        }
    }

    template <shader_function func, typename TRenderTarget>
    static stats render_inner(shader_inputs uniforms, TRenderTarget& rt, std::atomic_int& num_pixels, const std::atomic_bool& cancelled, bool parallel)
    {
        assert(rt.width % columns_per_batch == 0);
        assert(rt.height % rows_per_batch == 0);

        using ::swizzle::detail::static_for;
        using namespace ::swizzle;
        using float_traits = ::swizzle::detail::scalar_traits<swizzle::float_type>;

        auto render_begin = std::chrono::steady_clock::now();

        // if there are more than 1 scalars in a vector, work on two rows with half width at the same time
        swizzle::float_type x_offsets(0);
        swizzle::float_type y_offsets(0);

        {
            float_traits::aligned_storage_type aligned_storage;
            float* aligned = reinterpret_cast<float*>(&aligned_storage);

            static_for<0, float_traits::size>([&](size_t i) { aligned[i] = static_cast<float>(i % columns_per_batch) + 0.5f; });
            load_aligned(x_offsets, aligned);

            static_for<0, float_traits::size>([&](size_t i) { aligned[i] = static_cast<float>(i / columns_per_batch) + 0.5f; });
            load_aligned(y_offsets, aligned);
        }


        const int max_x = rt.width;
        const int max_y = rt.height;

        auto work = [&](int y)
        {
            if (cancelled)
                return;

            swizzle::float_type frag_coord_y = static_cast<float>(max_y - y) - y_offsets;

            uint8_t* ptr = reinterpret_cast<uint8_t*>(rt.first_row) + y * rt.pitch;

            int x;
            for (x = 0; x < max_x; x += columns_per_batch)
            {
                swizzle::bool_type discarded = false;
                swizzle::vec4 color = func(uniforms, vec2(static_cast<float>(x) + x_offsets, frag_coord_y), {}, &discarded);

                if (!discarded)
                {
                    rt.store(ptr, color, rt.pitch);
                }

                ptr += rt.bytes_per_pixel * columns_per_batch;
            }

            num_pixels += (max_x)*rows_per_batch;
        };

#ifdef CONFIG_ENABLE_PARALLELISM
        if (parallel)
        {
            std::for_each(std::execution::par_unseq, utils::for_iterator(0, rows_per_batch), utils::for_iterator(max_y), std::move(work));
        }
        else
#endif
        {
            for (int y = 0; y < max_y; y += rows_per_batch)
            {
                work(y);
            }
        }

        return stats{ std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - render_begin), num_pixels };
    }
};

//-----------------------------------------------------------------------

struct cmdline_options
{
    int resolution_x = 512;
    int resolution_y = 288;
    int output_frames = 1;

    float time = 0.0f;
    std::optional<float> time_delta = std::nullopt;

    bool had_error = false;
    bool show_help = false;
    bool multi_threaded = true;

    const char* output_path = nullptr;
    std::filesystem::path textures_root = CONFIG_SAMPLE_TEXTURES_ROOT;

    static cmdline_options parse(int argc, char* argv[])
    {
        using namespace utils;

        cmdline_options result = {};
        
        for (int i = 1; i < argc; ++i)
        {
            char* arg = argv[i];
            if (!strcmp(arg, "-r"))
            {
                if (i + 2 < argc && from_string(argv[++i], result.resolution_x) && from_string(argv[++i], result.resolution_y))
                {
                    continue;
                }
            }
            else if (!strcmp(arg, "-n"))
            {
                if (i + 1 < argc && from_string(argv[++i], result.output_frames))
                {
                    continue;
                }
            }
            else if (!strcmp(arg, "-d"))
            {
                float delta = 0.0f;
                if (i + 1 < argc && from_string(argv[++i], delta))
                {
                    result.time_delta = delta;
                    continue;
                }
            }
            else if (!strcmp(arg, "-t"))
            {
                if (i + 1 < argc && from_string(argv[++i], result.time))
                {
                    continue;
                }
            }
            else if (!strcmp(arg, "-o"))
            {
                if (i + 1 < argc)
                {
                    result.output_path = argv[++i];
                    continue;
                }
            }
            else if (!strcmp(arg, "-h"))
            {
                result.show_help = true;
                continue;
            }
            else if (!strcmp(arg, "-s"))
            {
                result.multi_threaded = false;
                continue;
            }
            else if (!strcmp(arg, "--textures-root"))
            {
                if (i + 1 < argc)
                {
                    result.textures_root = argv[++i];
                    continue;
                }
            }

            result.had_error = true;
            break;
        }

        return result;
    }

    static void print_help()
    {
        printf("-h                          show this message\n");
        printf("-o <path>                   render one frame, save to <path> (PNG) and quit.\n");
        printf("-r <width> <height>         set initial resolution\n");
        printf("--textures-root <path>      config path (default: %s)\n", CONFIG_SAMPLE_TEXTURES_ROOT);
        printf("-t <time>                   set initial time & pause\n");
    }
};

//-----------------------------------------------------------------------

int main(int argc, char* argv[])
{
    using namespace std;
    using namespace stl_utils;
    
    auto options = cmdline_options::parse(argc, argv);
    if (options.had_error)
    {
        options.print_help();
        return 1;
    }
    if (options.resolution_x <= 0 || options.resolution_y < 0)
    {
        fprintf(stderr, "ERROR: invalid resolution: %dx%d\n", options.resolution_x, options.resolution_y);
        return 1;
    }
    if (options.show_help)
    {
        options.print_help();
        return 0;
    }

    static_assert(::shadertoy::num_samplers >= 4);

    shadertoy_config config = shadertoy_config::make_default();
        
    // load textures
    bool img_initialized = false;
    auto ensure_img_initialized = [&img_initialized](int img_flags)
    {
        if (!img_initialized) return;

        img_initialized = true;
        if (IMG_Init(img_flags) != img_flags)
        {
            fprintf(stderr, "WARNING: failed to initialize required jpg and png support: %s\n", IMG_GetError());
        }
    };
    CXXSWIZZLE_SCOPE_EXIT([&img_initialized]() { if (img_initialized) IMG_Quit(); });

    texture_map textures;
    for (pass_config& pass : config.passes)
    {
        for (sampler_config& sampler : pass.samplers)
        {
            if (sampler.type != sampler_type::texture && sampler.type != sampler_type::cubemap)
                continue;

            for (auto& path : sampler.paths)
            {
                if (textures.find(path) != textures.end())
                    continue;

                std::filesystem::path p = path;

                auto str = p.string();

                if (!exists(p) && p.is_relative())
                {
                    // try loading from config's dir
                    if (!options.textures_root.empty())
                    {
                        p = options.textures_root / p;
                    }

                    if (!exists(p))
                    {
                        fprintf(stderr, "ERROR: texture does not exist: %s\n", str.c_str());
                        textures[path] = stl_utils::create_sdl_object_or_abort(SDL_CreateRGBSurfaceWithFormat, 0, 1, 1, 8, SDL_PIXELFORMAT_INDEX8);
                        continue;
                    }

                    str = p.string();
                }

                ensure_img_initialized(IMG_INIT_JPG | IMG_INIT_PNG);
                auto image = stl_utils::make_sdl_ptr(IMG_Load(str.c_str()));
                if (!image)
                {
                    fprintf(stderr, "ERROR: failed to load %s: %s\n", str.c_str(), IMG_GetError());
                    textures[path] = stl_utils::create_sdl_object_or_abort(SDL_CreateRGBSurfaceWithFormat, 0, 1, 1, 8, SDL_PIXELFORMAT_INDEX8);
                    continue;
                }

                if (image->format->format == SDL_PIXELFORMAT_INDEX8)
                {
                    textures[path] = std::move(image);
                }
                else
                {
                    textures[path] = stl_utils::create_sdl_object_or_abort(SDL_ConvertSurfaceFormat, image.get(), SDL_PIXELFORMAT_RGBA32, 0);
                }
            }
        }
    }

    shadertoy_render_targets render_targets(options.resolution_x, options.resolution_y);
    shadertoy_renderer::stats last_render_stats = { };

    bool multi_threaded = options.multi_threaded;

    if (options.output_path != nullptr)
    {
        shadertoy_keyboard keyboard;
        shadertoy_date date;

        auto& rt = render_targets.target;
        auto target_surface = create_sdl_object_or_abort(SDL_CreateRGBSurfaceFrom, rt.first_row, rt.width, rt.height, 32, rt.pitch, 0x000000FF, 0x0000FF00, 0x00FF0000, 0x0);

        float delta = 0.0f;
        int fps = 0;
        float time = options.time;

        shadertoy_renderer::progress progress = {};

        for (int i = 0; i < options.output_frames; ++i)
        {
            date.now();

            shader_inputs inputs;
            nonmasked(inputs.iTime) = time;
            nonmasked(inputs.iTimeDelta) = delta;
            nonmasked(inputs.iFrameRate) = fps;
            nonmasked(inputs.iFrame) = i;
            nonmasked(inputs.iResolution) = vec3(static_cast<float>(render_targets.width), static_cast<float>(render_targets.height), 0.0f);
            nonmasked(inputs.iMouse) = vec4(0);
            nonmasked(inputs.iDate) = date.consume();

            shadertoy_input_textures context(textures, keyboard.data, render_targets.buffers[1 - i & 1]);
            last_render_stats = shadertoy_renderer::render(config, inputs, render_targets.target, render_targets.buffers[i & 1], context, progress, multi_threaded);
                
            std::string output = utils::replace_all(options.output_path, "%n", std::to_string(i));
            output = utils::replace_all(output, "%t", std::to_string(time));

            ensure_img_initialized(IMG_INIT_PNG);
            IMG_SavePNG(target_surface.get(), output.c_str());

            delta = options.time_delta.value_or(static_cast<float>(utils::duration_to_seconds(last_render_stats.duration)));
            time += delta;
            fps  = static_cast<int>(1 / utils::duration_to_seconds(last_render_stats.duration));
        }
    }
    else
    {
        float last_frame_timestamp = 0.0f;

        std::chrono::microseconds fps_frames_duration = {};
        int fps_frames_num = 0;
        double current_fps = 0;

        int num_frames = 0;
        float current_frame_timestamp = 0.0f;
            
        bool pending_resize = false;

        float time = options.time;

        shadertoy_renderer::progress progress = {};

        shadertoy_keyboard keyboard_copy;
        shadertoy_keyboard keyboard;
        shadertoy_mouse mouse;
        shadertoy_date date;

        auto render_async = [&]() -> std::future<shadertoy_renderer::stats>
        {
            progress.cancel = false;

            // bring about keyboard changes
            keyboard.consume(keyboard_copy);
            date.now();

            shader_inputs inputs;
            nonmasked(inputs.iTime) = time;
            nonmasked(inputs.iTimeDelta) = static_cast<float>(utils::duration_to_seconds(last_render_stats.duration));
            nonmasked(inputs.iFrameRate) = static_cast<int>(current_fps);
            nonmasked(inputs.iFrame) = num_frames;
            nonmasked(inputs.iResolution) = vec3(static_cast<float>(render_targets.width), static_cast<float>(render_targets.height), 0.0f);
            nonmasked(inputs.iMouse) = mouse.consume();
            nonmasked(inputs.iDate) = date.consume();

            shadertoy_input_textures context(textures, keyboard_copy.data, render_targets.buffers[1 - num_frames & 1]);
            return std::async([&, inputs, context]() { return shadertoy_renderer::render(config, inputs, render_targets.target, render_targets.buffers[num_frames & 1], context, progress, multi_threaded); });
        };


        // initial setup
        SDL_SetMainReady();
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            fprintf(stderr, "ERROR: Unable to init SDL\n");
            return 1;
        }
        CXXSWIZZLE_SCOPE_EXIT([]() { SDL_Quit(); });

        const int status_bar_height = 24;

        sdl_ptr<SDL_Window> window          = create_sdl_object_or_abort(SDL_CreateWindow, "CxxSwizzle sample", 
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
            render_targets.width, render_targets.height + status_bar_height, 
            SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

        sdl_ptr<SDL_Renderer> renderer      = create_sdl_object_or_abort(SDL_CreateRenderer, window.get(), -1, 0);
        sdl_ptr<SDL_Texture> target_texture = create_matching_sdl_texture(renderer.get(), render_targets.width, render_targets.height);
        std::unordered_map<std::string, sdl_ptr<SDL_Texture>> ui_textures;


        ImGui::CreateContext();
        CXXSWIZZLE_SCOPE_EXIT([=]() { ImGui::DestroyContext(); });

        ImGui_ImplSDL2_InitForSDLRenderer(window.get(), renderer.get());
        CXXSWIZZLE_SCOPE_EXIT([]() { ImGui_ImplSDL2_Shutdown(); });

        ImGui_ImplSDLRenderer2_Init(renderer.get());

        printf("shift + p           - pause/unpause\n");
        printf("shift + left arrow  - decrease time by 1 s\n");
        printf("shift + right arrow - increase time by 1 s\n");
        printf("shift + space       - blit now! (show incomplete render)\n");
        printf("esc                 - quit\n");
        printf("\n");
            
        ImGuiIO& io = ImGui::GetIO();

        std::future<shadertoy_renderer::stats> render_task = render_async();


        bool paused = false;
        pass_type selected_pass = pass_type::image;
        SDL_SetWindowMinimumSize(window.get(), status_bar_height * 2, status_bar_height * 2);
        for (auto update_begin = chrono::steady_clock::now();;)
        {
            bool blit_now = false;
            bool quit = false;

            // process events
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                ImGui_ImplSDL2_ProcessEvent(&event);

                switch (event.type)
                {
                case SDL_WINDOWEVENT:
                    switch (event.window.event)
                    {
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        progress.cancel = pending_resize = true;
                        break;
                    }
                    break;
                case SDL_QUIT:
                    progress.cancel = quit = true;
                    break;

                case SDL_KEYDOWN:
                case SDL_KEYUP:
                {
                    int ascii = sdl_keysym_to_ascii(event.key.keysym);
                    if (ascii > 0)
                    {
                        if (event.type == SDL_KEYDOWN)
                        {
                            keyboard.key_down(ascii);
                        }
                        else
                        {
                            keyboard.key_up(ascii);
                        }
                    }
                    break;
                }

                case SDL_MOUSEMOTION:
                    if (!io.WantCaptureMouse)
                    {
                        mouse.motion(event.button.x, event.button.y);
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (!io.WantCaptureMouse)
                    {
                        mouse.button_down(event.button.x, event.button.y);
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (!io.WantCaptureMouse)
                    {
                        mouse.button_up(event.button.x, event.button.y);
                    }
                    break;

                default:
                    break;
                }
            }

            if (quit)
            {
                break;
            }


            // imgui
            {
                ImGui_ImplSDLRenderer2_NewFrame();
                ImGui_ImplSDL2_NewFrame();
                ImGui::NewFrame();

                bool is_shift_down = ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift);

                if (is_shift_down && ImGui::IsKeyPressed(ImGuiKey_Space))
                {
                    blit_now = true;
                }

                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0f, 2.0f));
                ImGui::Begin("StatusBar", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
                {
                    const float padding = 5.0f;
                    auto button_size = ImVec2(22.0f, 20.0f);

                    ImGui::SetWindowPos(ImVec2(0, io.DisplaySize.y - status_bar_height));
                    ImGui::SetWindowSize(ImVec2(io.DisplaySize.x, status_bar_height));

                    if (ImGui::Button("|<", button_size))
                    {
                        time = 0.0f;
                        progress.cancel = true;
                        num_frames = 0;
                        // force render buffers flush
                        pending_resize = true;
                    }

                    ImGui::SameLine();
                    if (ImGui::Button(paused ? ">" : "||", button_size) || is_shift_down && ImGui::IsKeyPressed(ImGuiKey_P))
                    {
                        paused = !paused;
                    }

                    ImGui::SameLine();
                    if (ImGui::ArrowButton("SeekBack", ImGuiDir_Left) || is_shift_down && ImGui::IsKeyPressed(ImGuiKey_LeftArrow))
                    {
                        time = std::max(0.0f, time - 1.0f);
                        progress.cancel = true;
                    }

                    ImGui::SameLine();
                    imgui_utils::imgui_text_centered(40.0f, true, "%.2f", time);

                    ImGui::SameLine();
                    if (ImGui::ArrowButton("SeekForward", ImGuiDir_Right) || is_shift_down && ImGui::IsKeyPressed(ImGuiKey_RightArrow))
                    {
                        time += 1.0f;
                        progress.cancel = true;
                    }

                    ImGui::SameLine();
                    imgui_utils::imgui_text_centered(90.0f, true, "%lG fps", current_fps);

                    ImGui::SameLine();
                    imgui_utils::imgui_text_centered(60.0f, true, "%dx%d\n", render_targets.width, render_targets.height);

                    ImGui::SameLine();
                    if (ImGui::RadioButton("MT", multi_threaded))
                    {
                        multi_threaded = !multi_threaded;
                        progress.cancel = true;
                    }
                    if (ImGui::IsItemHovered()) 
                    {
                        ImGui::SetTooltip("Enable/disable multithreading");
                    }

                    ImGui::SameLine();
                    if (ImGui::Button("TEX")) 
                    {
                        ImGui::OpenPopup("Details");
                    }

                        

                    auto pass_type_to_label = [](pass_type pass)
                    {
                        switch (pass)
                        {
                        case pass_type::buffer_a: return "buffer_a";
                        case pass_type::buffer_b: return "buffer_b";
                        case pass_type::buffer_c: return "buffer_c";
                        case pass_type::buffer_d: return "buffer_d";
                        case pass_type::image:    return "image";
                        default:                  return "?";
                        }
                    };
                    auto pass_type_to_label_short = [](pass_type pass)
                    {
                        switch (pass)
                        {
                        case pass_type::buffer_a: return "A";
                        case pass_type::buffer_b: return "B";
                        case pass_type::buffer_c: return "C";
                        case pass_type::buffer_d: return "D";
                        case pass_type::image:    return "I";
                        default:                  return "?";
                        }
                    };


                    if (render_targets.num_passes > 1)
                    {
                        ImGui::SameLine();
                        ImGui::PushItemWidth(15.0f);

                        if (ImGui::BeginCombo("##PassCombo", pass_type_to_label_short(selected_pass), ImGuiComboFlags_NoArrowButton))
                        {
                            for (pass_type pass : shadertoy::passes)
                            {
                                const bool is_selected = (pass == selected_pass);
                                if (ImGui::Selectable(pass_type_to_label(pass), is_selected))
                                {
                                    selected_pass = pass;
                                }

                                if (is_selected)
                                {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }
                            ImGui::EndCombo();
                        }
                    }

                    // a low effort way of getting this aligned to the right, if there's a space
                    ImGui::SameLine(io.DisplaySize.x - 85 > ImGui::GetItemRectMax().x ? io.DisplaySize.x - 80.0f : 0.0f);
                    imgui_utils::imgui_text_centered(37.0f, false, "%.2f", current_frame_timestamp);
                    ImGui::SameLine();
                    ImGui::ProgressBar(progress.num_pixels / static_cast<float>(render_targets.width * render_targets.height * render_targets.num_passes), ImVec2(28.0f, button_size.y), "");

                    if (ImGui::BeginPopup("Details"))
                    {
                        if (ImGui::BeginTabBar("Passes"))
                        {
                            for (pass_type pass : shadertoy::passes)
                            {
                                if (ImGui::BeginTabItem(pass_type_to_label(pass)))
                                {
                                    int sampler_idx = 0;
                                    for (auto& sampler : config.get_pass(pass).samplers)
                                    {
                                        ImVec2 thumb_size = { 100, 75 };

                                        if (sampler_idx != 0)
                                        {
                                            ImGui::SameLine();
                                        }

                                        ImGui::BeginGroup();

                                        if (sampler.type == sampler_type::none)
                                        {
                                            ImGui::Button("None", thumb_size);
                                        }
                                        else if (sampler.type == sampler_type::texture)
                                        {
                                            auto path = sampler.paths[0];
                                            if (ui_textures.find(path) == ui_textures.end())
                                            {
                                                ui_textures.emplace(path, create_sdl_object_or_abort(SDL_CreateTextureFromSurface, renderer.get(), textures[path].get()));
                                            }
                                            ImGui::Image(ui_textures[path].get(), thumb_size);
                                        }
                                        else
                                        {
                                            ImGui::Button(sampler.label.c_str(), thumb_size);
                                        }
                                        ImGui::Text("iChannel%d", sampler_idx++);
                                        ImGui::EndGroup();
                                    }


                                    ImGui::EndTabItem();
                                }
                            }
                            ImGui::EndTabBar();
                        }

                        ImGui::EndPopup();
                    }

                }

                ImGui::End();
                ImGui::PopStyleColor();
                ImGui::PopStyleVar(2);
            }
                

            // handle frame flushing

            bool frame_ready = !(bool)render_task.wait_for(chrono::microseconds{ 16 });
            if (pending_resize)
            {
                if (frame_ready)
                {
                    render_targets = shadertoy_render_targets(static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y) - status_bar_height);
                    target_texture = create_matching_sdl_texture(renderer.get(), render_targets.width, render_targets.height);
                    pending_resize = false;

                    current_frame_timestamp = time;
                    render_task = render_async();
                }
            }
            else
            {
                if (blit_now || frame_ready)
                {
                    SDL_Rect rect{ 0, 0, render_targets.width, render_targets.height };
                    if (selected_pass != pass_type::image)
                    {
                        int index = static_cast<int>(selected_pass);
                        render_targets.buffers[num_frames & 1][index].convert(render_targets.target_tmp);
                        SDL_UpdateTexture(target_texture.get(), &rect, render_targets.target_tmp.first_row, render_targets.target_tmp.pitch);
                    }
                    else 
                    {
                        SDL_UpdateTexture(target_texture.get(), &rect, render_targets.target.first_row, render_targets.target.pitch);
                    }
                }

                if (frame_ready)
                {
#ifdef TRACY_ENABLE
                    FrameMark;
#endif

                    if (!progress.cancel)
                    {
                        ++num_frames;
                        last_render_stats = render_task.get();
                        last_frame_timestamp = current_frame_timestamp;

                        // update fps
                        ++fps_frames_num;
                        fps_frames_duration += last_render_stats.duration;
                        if (fps_frames_duration >= std::chrono::milliseconds(500))
                        {
                            current_fps = fps_frames_num / utils::duration_to_seconds(fps_frames_duration);
                            fps_frames_num = 0;
                            fps_frames_duration = {};
                        }
                    }


                    current_frame_timestamp = time;
                    render_task = render_async();
                }
            }

            ImGui::Render();
            SDL_RenderClear(renderer.get());

            SDL_Rect rect_image = { 0, 0, static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y) - status_bar_height };
            SDL_RenderCopy(renderer.get(), target_texture.get(), NULL, &rect_image);

            ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());

            SDL_RenderPresent(renderer.get());

            // update timers
            auto now = chrono::steady_clock::now();
            auto delta = chrono::duration_cast<chrono::microseconds>(now - update_begin);

            if (!paused)
            {
                time += static_cast<float>(utils::duration_to_seconds(delta));
            }
            update_begin = now;
        }

        printf("\nwaiting for the worker thread to finish...");
        render_task.wait();
    }

    return 0; 
}


#include <shadertoy_config.hpp>

#if WIN32
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

static int result_SetProcessDPIAware = []() -> int {
    if (!SetProcessDPIAware()) 
    {
        return GetLastError();
    }
    return 0;
}();


#endif
