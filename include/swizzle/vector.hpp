// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <type_traits>
#include <iostream>

#include <swizzle/detail/fwd.h>
#include <swizzle/detail/utils.h>
#include <swizzle/detail/common_binary_operators.h>
#include <swizzle/detail/vector_storage.h>
#include <swizzle/detail/indexed_vector_iterator.h>
#include <swizzle/glsl/vector_helper.h>
//#include <swizzle/glsl/matrix.h>
#include <cstdint>

namespace swizzle
{
    template <typename ScalarType, size_t... Index>
    struct vector_
        : detail::vector_build_info<ScalarType, sizeof...(Index)>::base_type
        , std::conditional_t< detail::vector_build_info<ScalarType, sizeof...(Index)>::is_number && (sizeof...(Index) > 1), swizzle::detail::common_binary_operators< vector_<ScalarType, Index...>, ScalarType>, detail::nothing>
    {
    public:
        using build_info = detail::vector_build_info<ScalarType, sizeof...(Index)>;
        using storage_type = typename build_info::base_type;

        using this_type = vector_;
        using this_type_arg = const this_type&;

        using scalar_type = ScalarType;
        using scalar_arg = const scalar_type&;

        using number_scalar_arg = detail::only_if< build_info::is_number, scalar_arg >;
        using number_vector_arg = detail::only_if< build_info::is_number, this_type_arg >;
        
        using float_scalar_arg = detail::only_if< build_info::is_floating_point, scalar_arg >;
        using float_vector_arg = detail::only_if< build_info::is_floating_point, this_type_arg >;

        using bool_type = typename build_info::bool_type;
        using bool_vector_type = vector_<bool_type, Index...>;
        using bool_scalar_arg = detail::only_if< build_info::is_bool, this_type_arg >;
        using bool_vector_arg = detail::only_if< build_info::is_bool, this_type_arg >;

        //! Get the real, real internal scalar type. Useful when scalar visible
        //! externally is different than the internal one (well, hello SIMD)
        using internal_scalar_type = std::remove_reference_t<decltype(std::declval<storage_type>().data[0])>;

        static const size_t num_of_components = sizeof...(Index);
        static const bool are_scalar_types_same = std::is_same_v< internal_scalar_type, scalar_type>;
        static_assert(are_scalar_types_same || sizeof(scalar_type) == sizeof(internal_scalar_type), "scalar_type and internal_scalar_type can't be safely converted");

        //! Type static functions return; for single-component they decay to a scalar
        using decay_type = std::conditional_t<num_of_components == 1, scalar_type, this_type>;

        using number_scalar_arg_cond = detail::only_if <(num_of_components > 1), number_scalar_arg>;
        using float_scalar_arg_cond = detail::only_if <(num_of_components > 1), float_scalar_arg>;


        //! Decays the vector. For Size==1 this is going to return a scalar, for all other sizes - same vector
        inline decay_type decay() const
        {
            return static_cast<const decay_type&>(*this);
        }

        //! Default constructor.
        inline vector_()
        {
            //data = { };
        }

        //! Copy constructor
        inline vector_(this_type_arg other)
        {
            data = other.data;
        }

        //! Implicit constructor from scalar-convertible only for one-component vector
        inline vector_(detail::only_if<num_of_components == 1 && !std::is_same_v<scalar_type, int> && !std::is_same_v<scalar_type, double>, scalar_type> s)
        {
            at(0) = s;
        }

        //! Implicit from int (yep, it's needed for stuff like cos(0) to work
        inline vector_(detail::only_if<num_of_components == 1, int, __LINE__> s)
        {
            ((at(Index) = scalar_type(s)), ...);
        }

        //! Implicit from double (for stuff like cos(0.0) to work
        inline vector_(detail::only_if<num_of_components == 1 && build_info::is_floating_point, double, __LINE__> s)
        {
            ((at(Index) = scalar_type(s)), ...);
        }

        //! When more than 1 component implicit construction is not possible
        inline explicit vector_(detail::only_if<num_of_components != 1 && !std::is_same_v<scalar_type, int> && !std::is_same_v<scalar_type, double>, scalar_type, __LINE__> s)
        {
            ((at(Index) = s), ...);
        }

        //! For vec2(0)
        inline explicit vector_(detail::only_if<num_of_components != 1, int, __LINE__> s)
        {
            ((at(Index) = scalar_type(s)), ...);
        }

        //! For vec2(0.0)
        inline explicit vector_(detail::only_if<num_of_components != 1 && build_info::is_floating_point, double, __LINE__> s)
        {
            ((at(Index) = scalar_type(s)), ...);
        }

        // Block of generic proxy-constructors calling construct member function. Compiler
        // will likely optimise this.
        template <class T0, class T1, class... T,
            class = std::enable_if_t<
            (num_of_components <= detail::get_total_component_count_v<T0, T1, T...>) &&
            !(num_of_components <= detail::get_total_component_count_v<T0, T1, T...> -detail::get_total_component_count_v< detail::last_t<T0, T1, T...> >)
            >
        >
        inline vector_(T0&& t0, T1&& t1, T&&... ts)
        {
            construct<0>(std::forward<T0>(t0), std::forward<T1>(t1), std::forward<T>(ts)..., detail::nothing{});
        }

        inline scalar_type& operator[](size_t i)
        {
            return at(i);
        }

        inline const scalar_type& operator[](size_t i) const
        {
            return at(i);
        }

        //! These are chosen when internal_scalar_type and outside visible scalar type are same.
        inline internal_scalar_type& at(detail::only_if<are_scalar_types_same, size_t> i)
        {
            return data[i];
        }
        inline const internal_scalar_type& at(detail::only_if<are_scalar_types_same, size_t> i) const
        {
            return data[i];
        }

        //! These are chosen when internal_scalar_type and outside visible scalar type are not same.
        inline scalar_type& at(detail::only_if<!are_scalar_types_same, size_t> i)
        {
            return *reinterpret_cast<scalar_type*>(&data[i]);
        }

        inline const scalar_type& at(detail::only_if<!are_scalar_types_same, size_t> i) const
        {
            return *reinterpret_cast<const scalar_type*>(&data[i]);
        }


        // Auto-decay to scalar type only if this is a 1-sized vector
        inline operator detail::only_if<num_of_components == 1, scalar_type>() const
        {
            return at(0);
        }

    private:
        template <size_t Offset, typename T0, typename... Tail>
        void construct(T0&& t0, Tail&&... tail)
        {
            compose<Offset>(detail::decay(std::forward<T0>(t0)));
            construct<Offset + detail::get_total_component_count_v<T0>>(std::forward<Tail>(tail)...);
        }

        template <size_t>
        void construct(detail::nothing)
        {}

        //! Puts scalar at given position. Used only during construction.
        template <size_t Index>
        void compose(scalar_arg v)
        {
            data[Index] = static_cast<internal_scalar_type>(v);
        }

        //! For construction from int literal to work
        template <size_t Index>
        void compose(int v)
        {
            compose<Index>(scalar_type(v));
        }

        //! For construction from double literal to work
        template <size_t Index>
        void compose(detail::only_if<build_info::is_floating_point, double> v)
        {
            compose<Index>(scalar_type(v));
        }

        template <size_t Index, typename OtherScalarType, size_t... OtherIndex>
        void compose(const vector_<OtherScalarType, OtherIndex...>& v)
        {
            const size_t limit = sizeof...(OtherIndex) > num_of_components - Index ? (num_of_components - Index) : sizeof...(OtherIndex);
            compose_impl<Index>(v, detail::take_n<limit, OtherIndex...> {});
        }

        template <size_t Offset, typename DataTypeB, size_t... DataIndex>
        inline void compose_impl(const DataTypeB& src, std::index_sequence<DataIndex...>)
        {
            ((at(DataIndex + Offset) = src.at(DataIndex)), ...);
        }

    public:
        // functions
        // 8.1

        static this_type call_radians(float_vector_arg degrees)
        {
            return this_type(radians(degrees.at(Index))...);
        }
        static this_type call_degrees(float_vector_arg radians)
        {
            return this_type(degrees(radians.at(Index))...);
        }
        static this_type call_cos(float_vector_arg angle)
        {
            return this_type(cos(angle.at(Index))...);
        }
        static this_type call_tan(float_vector_arg angle)
        {
            return this_type(tan(angle.at(Index))...);
        }
        static this_type call_sin(float_vector_arg angle)
        {
            return this_type(sin(angle.at(Index))...);
        }
        static this_type call_asin(float_vector_arg x)
        {
            return this_type(asin(x.at(Index))...);
        }
        static this_type call_acos(float_vector_arg x)
        {
            return this_type(acos(x.at(Index))...);
        }
        static this_type call_atan(float_vector_arg y_over_x)
        {
            return this_type(atan(y_over_x.at(Index))...);
        }
        static this_type call_atan(float_vector_arg y, float_vector_arg x)
        {
            return this_type(atan(y.at(Index), x.at(Index))...);
        }
        static this_type call_sinh(float_vector_arg x)
        {
            return this_type(sinh(x.at(Index))...);
        }
        static this_type call_cosh(float_vector_arg x)
        {
            return this_type(cosh(x.at(Index))...);
        }
        static this_type call_tanh(float_vector_arg x)
        {
            return this_type(tanh(x.at(Index))...);
        }
        static this_type call_asinh(float_vector_arg x)
        {
            return this_type(asinh(x.at(Index))...);
        }
        static this_type call_acosh(float_vector_arg x)
        {
            return this_type(acosh(x.at(Index))...);
        }
        static this_type call_atanh(float_vector_arg x)
        {
            return this_type(atanh(x.at(Index))...);
        }

        // 8.2

        static this_type call_pow(float_vector_arg x, float_vector_arg y)
        {
            return this_type(pow(x.at(Index), y.at(Index))...);
        }
        static this_type call_exp(float_vector_arg x)
        {
            return this_type(exp(x.at(Index))...);
        }
        static this_type call_log(float_vector_arg x)
        {
            return this_type(log(x.at(Index))...);
        }

        static this_type call_exp2(float_vector_arg x)
        {
            return this_type(exp2(x.at(Index))...);
        }
        static this_type call_log2(float_vector_arg x)
        {
            return this_type(log2(x.at(Index))...);
        }
        static this_type call_sqrt(float_vector_arg x)
        {
            return this_type(sqrt(x.at(Index))...);
        }
        static this_type call_inversesqrt(float_vector_arg x)
        {
            return this_type(inversesqrt(x.at(Index))...);
        }

        // 8.3

        static this_type call_abs(number_vector_arg x)
        {
            return this_type(abs(x.at(Index))...);
        }
        static this_type call_sign(number_vector_arg x)
        {
            return this_type(sign(x.at(Index))...);
        }
        static this_type call_floor(float_vector_arg x)
        {
            return this_type(floor(x.at(Index))...);
        }
        static this_type call_trunc(float_vector_arg x)
        {
            return this_type(trunc(x.at(Index))...);
        }
        static this_type call_round(float_vector_arg x)
        {
            return this_type(round(x.at(Index))...);
        }
        static this_type call_roundEven(float_vector_arg x)
        {
            return this_type(roundEven(x.at(Index))...);
        }
        static this_type call_ceil(float_vector_arg x)
        {
            return this_type(ceil(x.at(Index))...);
        }
        static this_type call_fract(float_vector_arg x)
        {
            return this_type(fract(x.at(Index))...);
        }
        static this_type call_mod(float_vector_arg x, float_scalar_arg y)
        {
            return this_type(mod(x.at(Index), y)...);
        }
        static this_type call_mod(float_vector_arg x, float_vector_arg y)
        {
            return this_type(mod(x.at(Index), y.at(Index))...);
        }
        static this_type call_modf(float_vector_arg x, this_type& i)
        {
            return this_type(modf(x.at(Index), &i.at(Index))...);
        }
        static this_type call_min(number_vector_arg x, number_vector_arg y)
        {
            return this_type(min(x.at(Index), y.at(Index))...);
        }
        static this_type call_min(number_vector_arg x, number_scalar_arg y)
        {
            return this_type(min(x.at(Index), y)...);
        }
        static this_type call_max(number_vector_arg x, number_vector_arg y)
        {
            return this_type(max(x.at(Index), y.at(Index))...);
        }
        static this_type call_max(number_vector_arg x, number_scalar_arg y)
        {
            return this_type(max(x.at(Index), y)...);
        }
        static this_type call_clamp(number_vector_arg x, number_scalar_arg_cond minVal, number_scalar_arg_cond maxVal)
        {
            return this_type(max(min(x.at(Index), maxVal), minVal)...);
        }
        static this_type call_clamp(number_vector_arg x, number_vector_arg minVal, number_vector_arg maxVal)
        {
            return this_type(max(min(x.at(Index), maxVal.at(Index)), minVal.at(Index))...);
        }
        static this_type call_mix(float_vector_arg x, float_vector_arg y, float_scalar_arg_cond a)
        {
            return this_type(x.at(Index) + a * (y.at(Index) - x.at(Index))...);
        }
        static this_type call_mix(float_vector_arg x, float_vector_arg y, float_vector_arg a)
        {
            return this_type(x.at(Index) + a.at(Index) * (y.at(Index) - x.at(Index))...);
        }
        /*static this_type call_mix(float_vector_arg x, float_vector_arg y, const bool_vector_type& a)
        {
            this_type mask = call_step(this_type(scalar_type{ 0 }), this_type{ a });
            return call_mix(x, y, mask);
        }*/

        static this_type call_step(float_vector_arg edge, float_vector_arg x)
        {
            // we'll delegate step to "outside" because there can't be branches here
            return this_type(step(edge.at(Index), x.at(Index))...);
        }

        static this_type call_step(float_scalar_arg edge, float_vector_arg x)
        {
            // we'll delegate step to "outside" because there can't be branches here
            return this_type(step(edge, x.at(Index))...);
        }

        static this_type call_smoothstep(float_vector_arg edge0, float_scalar_arg edge1, float_vector_arg x)
        {
            return this_type(smoothstep_helper(edge0.at(Index), edge1.at(Index), x.at(Index))...);
        }

        static this_type call_smoothstep(float_scalar_arg edge0, float_scalar_arg edge1, float_vector_arg x)
        {
            return this_type(smoothstep_helper(edge0, edge1, x.at(Index))...);
        }

        static bool_vector_type call_isnan(float_vector_arg x)
        {
            return bool_vector_type(isnan(x.at(Index))...);
        }
        static bool_vector_type call_isinf(float_vector_arg p1)
        {
            return bool_vector_type(isinf(x.at(Index))...);
        }

        //static auto call_floatBitsToInt(float_vector_arg value)
        //{
        //    using int_vector_type = vector<decltype(floatBitsToInt(0)), num_of_components>;
        //    return int_vector_type{ (floatBitsToInt(value.data[Index]))... };
        //}

        //static uint_vector_type call_floatBitsToUint(float_vector_arg value)
        //{
        //    uint_vector_type result;
        //    ((bitcast(value.data[Index], &result.data[Index])), ...);
        //    return result;
        //}

        //static this_type call_intBitsToFloat(detail::only_if<build_info::is_floating_point, int_vector_type> value)
        //{
        //    this_type result;
        //    ((bitcast(value.data[Index], &result.data[Index])), ...);
        //    return result;
        //}

        //static uint_vector_type call_uintBitsToFloat(detail::only_if<build_info::is_floating_point, uint_vector_type> value)
        //{
        //    this_type result;
        //    ((bitcast(value.data[Index], &result.data[Index])), ...);
        //    return result;
        //}

        // 8.4

        static scalar_type call_length(float_vector_arg x)
        {
            return sqrt(call_dot(x, x));
        }

        static scalar_type call_distance(float_vector_arg p0, const float_vector_arg p1)
        {
            return call_length(p0 - p1);
        }

        static scalar_type call_dot(float_vector_arg x, float_vector_arg y)
        {
            return ((x.at(Index) * y.at(Index)) + ...);
        }

        static typename detail::only_if<num_of_components == 3, this_type> call_cross(float_vector_arg x, float_vector_arg y)
        {
            auto rx = x[1] * y[2] - x[2] * y[1];
            auto ry = x[2] * y[0] - x[0] * y[2];
            auto rz = x[0] * y[1] - x[1] * y[0];
            return this_type(rx, ry, rz);
        }

        static this_type call_normalize(float_vector_arg x)
        {
            return this_type(x) * inversesqrt(call_dot(x, x));
        }

        static this_type call_faceforward(float_vector_arg N, float_vector_arg I, float_vector_arg Nref)
        {
            scalar_type s = step(scalar_type(0), call_dot(Nref, I));
            s = s * (-2) + 1; // 0 -> 1, 1 -> -1
            return N * s;
        }

        static this_type call_reflect(float_vector_arg I, float_vector_arg N)
        {
            return (I - scalar_type(2) * call_dot(I, N) * N);
        }

        static this_type call_refract(float_vector_arg I, float_vector_arg N, float_scalar_arg eta)
        {
            scalar_type k = 1.0 - eta * eta * (1.0 - call_dot(N, I) * call_dot(N, I));
            scalar_type mask = step(scalar_type(0), k);
            return mask * (eta * I - (eta * call_dot(N, I) + sqrt(k)) * N);
        }

        // 8.6
        static bool_vector_type call_lessThan(number_vector_arg x, number_vector_arg y)
        {
            return bool_vector_type(x.at(Index) < y.at(Index)...);
        }
        static bool_vector_type call_lessThanEqual(number_vector_arg x, number_vector_arg y)
        {
            return bool_vector_type(x.at(Index) <= y.at(Index)...);
        }
        static bool_vector_type call_greaterThan(number_vector_arg x, number_vector_arg y)
        {
            return bool_vector_type(x.at(Index) > y.at(Index)...);
        }
        static bool_vector_type call_greaterThanEqual(number_vector_arg x, number_vector_arg y)
        {
            return bool_vector_type(x.at(Index) >= y.at(Index)...);
        }

        static bool_vector_type call_equal(this_type_arg x, this_type_arg y)
        {
            return bool_vector_type(x.at(Index) == y.at(Index)...);
        }
        static bool_vector_type call_notEqual(this_type_arg x, this_type_arg y)
        {
            return bool_vector_type(x.at(Index) != y.at(Index)...);
        }

        static scalar_type call_any(bool_vector_arg x)
        {
            return ((x.at(Index)) || ...);
        }
        static scalar_type call_all(bool_vector_arg x)
        {
            return ((x.at(Index)) && ...);
        }
        static this_type call_not(bool_vector_arg x)
        {
            return this_type(!x.at(Index)...);
        }

        // 8.8
        static this_type call_dFdx(float_vector_arg x)
        {
            return this_type(dFdx(x.at(Index))...);
        }
        static this_type call_dFdy(float_vector_arg x)
        {
            return this_type(dFdy(x.at(Index))...);
        }
        static this_type call_fwidth(float_vector_arg x)
        {
            return this_type(fwidth(x.at(Index))...);
        }

        // operators

        inline bool_type operator==(this_type_arg other) const
        {
            return ((at(Index) == other.at(Index)) && ...);
        }

        inline bool_type operator!=(this_type_arg other) const
        {
            return !(*this == other);
        }

        inline this_type& operator=(this_type_arg other)
        {
            return ((at(Index) = other.at(Index)), ..., *this);
        }
        inline this_type& operator+=(number_vector_arg other)
        {
            return ((at(Index) += other.at(Index)), ..., *this);
        }
        inline this_type& operator-=(number_vector_arg other)
        {
            return ((at(Index) -= other.at(Index)), ..., *this);
        }
        inline this_type& operator*=(number_vector_arg other)
        {
            return ((at(Index) *= other.at(Index)), ..., *this);
        }
        inline this_type& operator/=(number_vector_arg other)
        {
            return ((at(Index) /= other.at(Index)), ..., *this);
        }
        inline this_type& operator+=(number_scalar_arg other)
        {
            return ((at(Index) += other), ..., *this);
        }
        inline this_type& operator-=(number_scalar_arg other)
        {
            return ((at(Index) -= other), ..., *this);
        }
        inline this_type& operator*=(number_scalar_arg other)
        {
            return ((at(Index) *= other), ..., *this);
        }
        inline this_type& operator/=(number_scalar_arg other)
        {
            return ((at(Index) /= other), ..., *this);
        }
        inline detail::only_if<build_info::is_number, this_type> operator-() const
        {
            this_type result;
            ((result.at(Index) = -at(Index)), ...);
            return result;
        }

    private:
        static scalar_type smoothstep_helper(scalar_arg edge0, scalar_arg edge1, scalar_arg x)
        {
            auto t = (x - edge0) / (edge1 - edge0);
            t = min(max(t, scalar_type(0.0)), scalar_type(1.0));
            return t * t * (scalar_type(3.0) - scalar_type(2.0) * t);
        }

        // STL COMPABILITY (not needed, but useful for testing)
    public:

        //! Iterates over the vector, firing Func for each index
        template <class Func>
        static void visit_index(Func func)
        {
            ((func(Index)), ...);
        }

        //! As an inline friend function, because thanks to that all convertibles will use same function.
        friend std::ostream& operator<<(std::ostream& os, this_type_arg vec)
        {
            vec.visit_index([&](size_t i) -> void { os << vec[i] << (i == num_of_components - 1 ? "" : ","); });
            return os;
        }

        //! As an inline friend function, because thanks to that all convertibles will use same function.
        friend std::istream& operator>>(std::istream& is, this_type& vec)
        {
            vec.visit_index([&](size_t i) -> void
            {
                is >> vec[i];
                if (i < num_of_components - 1 && is.peek() == ',')
                {
                    is.ignore(1);
                }
            });
            return is;
        }

    };

    namespace detail
    {
        template <typename ScalarType, size_t... Index>
        struct get_vector_type_impl< ::swizzle::vector_<ScalarType, Index...> >
        {
            typedef ::swizzle::vector_<ScalarType, Index...> type;
        };
    }
}
