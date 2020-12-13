// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <type_traits>
#include <cstdint>
#include <swizzle/detail/utils.h>
#include <swizzle/detail/vector_traits.h>
#include <swizzle/detail/fwd.hpp>

namespace swizzle
{
    namespace detail
    {
        template <> struct get_vector_type_impl<float_type> : default_vector_type_impl<float_type> {};
        template <> struct get_vector_type_impl<int_type> : default_vector_type_impl<int_type> {};
        template <> struct get_vector_type_impl<uint_type> : default_vector_type_impl<uint_type> {};
        template <> struct get_vector_type_impl<bool_type> : default_vector_type_impl<bool_type> {};

        // now make sure fundamentals get the right treatment too
        template <> struct get_vector_type_impl<only_if<!std::is_same_v<float_type, float>, float, __LINE__>>   : get_vector_type_impl<float_type> {};
        template <> struct get_vector_type_impl<only_if<!std::is_same_v<float_type, double>, double, __LINE__>> : get_vector_type_impl<float_type> {};

        template <> struct get_vector_type_impl<only_if<!std::is_same_v<int_type, int16_t>, int16_t, __LINE__>> : get_vector_type_impl<int_type> {};
        template <> struct get_vector_type_impl<only_if<!std::is_same_v<int_type, int32_t>, int32_t, __LINE__>> : get_vector_type_impl<int_type> {};
        template <> struct get_vector_type_impl<only_if<!std::is_same_v<int_type, int64_t>, int64_t, __LINE__>> : get_vector_type_impl<int_type> {};

        template <> struct get_vector_type_impl<only_if<!std::is_same_v<uint_type, uint16_t>, uint16_t, __LINE__>> : get_vector_type_impl<uint_type> {};
        template <> struct get_vector_type_impl<only_if<!std::is_same_v<uint_type, uint32_t>, uint32_t, __LINE__>> : get_vector_type_impl<uint_type> {};
        template <> struct get_vector_type_impl<only_if<!std::is_same_v<uint_type, uint64_t>, uint64_t, __LINE__>> : get_vector_type_impl<uint_type> {};
    }

    using vec2 = vector<float_type, 2>;
    using vec3 = vector<float_type, 3>;
    using vec4 = vector<float_type, 4>;

    using ivec2 = vector<int_type, 2>;
    using ivec3 = vector<int_type, 3>;
    using ivec4 = vector<int_type, 4>;

    using uvec2 = vector<uint_type, 2>;
    using uvec3 = vector<uint_type, 3>;
    using uvec4 = vector<uint_type, 4>;

    using bvec2 = vector<bool_type, 2>;
    using bvec3 = vector<bool_type, 3>;
    using bvec4 = vector<bool_type, 4>;

    using mat2x2 = matrix<float_type, 2, 2>;
    using mat3x3 = matrix<float_type, 3, 3>;
    using mat4x4 = matrix<float_type, 4, 4>;

    using mat2 = mat2x2;
    using mat3 = mat3x3;
    using mat4 = mat4x4;

    using sampler2D = naive_sampler_generic<float_type, int_type, uint_type>;
}