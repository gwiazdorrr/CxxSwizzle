// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <cstdint>
#include <type_traits>
#include <iostream>

#include <swizzle/detail/fwd.hpp>
#include <swizzle/detail/utils.h>
#include <swizzle/detail/vector_storage.hpp>
#include <swizzle/detail/indexed_vector_iterator.h>
#include <swizzle/detail/vector_base_type.hpp>

namespace swizzle
{
    template <typename TScalar, size_t... TIndices>
    struct vector_ : detail::vector_base_type<TScalar, sizeof...(TIndices)>
    {
    public:
        using base_type = detail::vector_base_type<TScalar, sizeof...(TIndices)>;
        using base_type::data;

        using this_type     = vector_;
        using this_type_arg = const this_type&;

        using scalar_type = TScalar;
        using scalar_arg  = const scalar_type&;

        using scalar_types = typename detail::scalar_traits<TScalar>::scalar_types;

        static constexpr bool scalar_is_bool           = detail::scalar_traits<TScalar>::is_bool;
        static constexpr bool scalar_is_integral       = detail::scalar_traits<TScalar>::is_integral;
        static constexpr bool scalar_is_floating_point = detail::scalar_traits<TScalar>::is_floating_point;
        static constexpr bool scalar_is_number         = scalar_is_integral || scalar_is_floating_point;
        static_assert(scalar_is_bool || scalar_is_integral || scalar_is_floating_point);

        using number_scalar_arg   = detail::only_if< scalar_is_number, scalar_arg >;
        using number_integral_arg = detail::only_if< scalar_is_integral, scalar_arg >;
        using number_vector_arg   = detail::only_if< scalar_is_number, this_type_arg >;
        
        using float_scalar_arg = detail::only_if< scalar_is_floating_point, scalar_arg >;
        using float_vector_arg = detail::only_if< scalar_is_floating_point, this_type_arg >;

        using integral_scalar_arg = detail::only_if< scalar_is_integral, scalar_arg >;
        using integral_vector_arg = detail::only_if< scalar_is_integral, this_type_arg >;

        using bool_type =  typename scalar_types::bool_type;
        using int_type =   typename scalar_types::int_type;
        using uint_type =  typename scalar_types::uint_type;
        using float_type = typename scalar_types::float_type;

        using int_vector_arg =  detail::only_if< std::is_same_v<int_type, scalar_type>, this_type_arg >;
        using uint_vector_arg = detail::only_if< std::is_same_v<uint_type, scalar_type>, this_type_arg >;

        using bool_vector_type = vector_<bool_type, TIndices...>;
        using bool_scalar_arg  = detail::only_if< scalar_is_bool, this_type_arg >;
        using bool_vector_arg  = detail::only_if< scalar_is_bool, this_type_arg >;

        using literal_arg = std::conditional_t<
            std::is_fundamental_v<scalar_type> || !scalar_is_number,
            detail::operation_not_available,
            std::conditional_t< scalar_is_floating_point, float, int >
        >;

        //! Get the real, real internal scalar type. Useful when scalar visible
        //! externally is different than the internal one (well, hello SIMD)
        using internal_scalar_type = std::remove_reference_t<decltype(std::declval<base_type>().data[0])>;

        static constexpr size_t num_components = sizeof...(TIndices);
        static constexpr bool are_scalar_types_same = std::is_same_v< internal_scalar_type, scalar_type>;
        static_assert(are_scalar_types_same || sizeof(scalar_type) == sizeof(internal_scalar_type), "scalar_type and internal_scalar_type can't be safely converted");

        //! Type static functions return; for single-component they decay to a scalar
        using decay_type = std::conditional_t<num_components == 1, scalar_type, this_type>;

        using number_scalar_arg_cond   = detail::only_if <(num_components > 1), number_scalar_arg>;
        using float_scalar_arg_cond    = detail::only_if <(num_components > 1), float_scalar_arg>;
        using number_vector_arg_cond   = detail::only_if <(num_components > 1), number_vector_arg>;
        using float_vector_arg_cond    = detail::only_if <(num_components > 1), float_vector_arg>;
        using integral_vector_arg_cond = detail::only_if <(num_components > 1), integral_vector_arg>;

        //!
        typedef detail::indexed_vector_iterator<const this_type> const_iterator;
        //!
        typedef detail::indexed_vector_iterator<this_type> iterator;

        //! Decays the vector. For Size==1 this is going to return a scalar, for all other sizes - same vector
        inline decay_type decay() const
        {
            return static_cast<const decay_type&>(*this);
        }

        //! Default constructor.
        inline vector_()
        {
            data = { };
        }

        //! Copy constructor
        inline vector_(this_type_arg other)
        {
            data = other.data;
        }

        //! Implicit constructor from scalar-convertible only for one-component vector
        inline vector_(detail::only_if<num_components == 1 && !std::is_same_v<scalar_type, int> && !std::is_same_v<scalar_type, double>, scalar_type> s)
        {
            at_rvalue(0) = s;
        }

        //! Implicit from int (yep, it's needed for stuff like cos(0) to work
        inline vector_(detail::only_if<num_components == 1, int, __LINE__> s)
        {
            ((at_rvalue(TIndices) = scalar_type(s)), ...);
        }

        //! Implicit from double (for stuff like cos(0.0) to work
        inline vector_(detail::only_if<num_components == 1 && scalar_is_floating_point, double, __LINE__> s)
        {
            ((at_rvalue(TIndices) = scalar_type(s)), ...);
        }

        //! When more than 1 component implicit construction is not possible
        inline explicit vector_(detail::only_if<num_components != 1 && !std::is_same_v<scalar_type, int> && !std::is_same_v<scalar_type, double>, scalar_type, __LINE__> s)
        {
            ((at_rvalue(TIndices) = s), ...);
        }

        //! For vec2(0)
        inline explicit vector_(detail::only_if<num_components != 1, int, __LINE__> s)
        {
            ((at_rvalue(TIndices) = scalar_type(s)), ...);
        }

        //! For vec2(0.0)
        inline explicit vector_(detail::only_if<num_components != 1 && scalar_is_floating_point, double, __LINE__> s)
        {
            ((at_rvalue(TIndices) = scalar_type(s)), ...);
        }

        template <class TLikelyOtherScalar, size_t TMoreIndexStart, size_t... TMoreIndices>
        inline explicit vector_(const vector_<TLikelyOtherScalar, TIndices..., TMoreIndexStart, TMoreIndices...>& t0)
        {
            construct_from_vector_helper<0>(t0);
        }

        template <class TLikelyOtherScalar>
        inline explicit vector_(const vector_<TLikelyOtherScalar, TIndices...>& t0)
        {
            construct_from_vector_helper<0>(t0);
        }

        template <class TProxyData, class TProxyScalar, size_t... TProxyIndices>
        inline explicit vector_(const detail::indexed_proxy_storage<TProxyData, TProxyScalar, num_components, TProxyIndices...>& proxy)
        {
            ((at_rvalue(TIndices) = proxy.data[TProxyIndices]), ...);
        }

        // Block of generic proxy-constructors calling construct member function. Compiler
        // will likely optimise this.
        template <class T0, class T1, class... T,
            class = std::enable_if_t<
            (num_components <= detail::get_total_component_count_v<T0, T1, T...>) &&
            !(num_components <= detail::get_total_component_count_v<T0, T1, T...> -detail::get_total_component_count_v< detail::last_t<T0, T1, T...> >)
            >
        >
        inline vector_(T0&& t0, T1&& t1, T&&... ts)
        {
            construct<0>(std::forward<T0>(t0), std::forward<T1>(t1), std::forward<T>(ts)...);
        }

        inline scalar_type& operator[](size_t i)
        {
            return at(i);
        }

        inline const scalar_type& operator[](size_t i) const
        {
            return at(i);
        }

        inline scalar_type& operator[](int i)
        {
            return at(i);
        }

        inline const scalar_type& operator[](int i) const
        {
            return at(i);
        }


        template <typename TSomeBatchedType, typename = std::enable_if_t<detail::scalar_traits<TSomeBatchedType>::is_integral && detail::scalar_traits<TSomeBatchedType>::size == detail::scalar_traits<scalar_type>::size> >
        scalar_type operator[](const TSomeBatchedType& b) const
        {
            scalar_type result;
            masked_read(b, result, at(TIndices)...);
            return result;
        }

        inline decltype(auto) at_rvalue(detail::only_if<std::is_fundamental_v<scalar_type>, size_t> i)
        {
            return at(i);
        }

        inline decltype(auto) at_rvalue(detail::only_if<!std::is_fundamental_v<scalar_type>, size_t> i)
        {
            return std::move(at(i));
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
        inline operator detail::only_if<num_components == 1, scalar_type, __LINE__>() const
        {
            return at(0);
        }

        // implicit cast

        explicit operator detail::only_if<!std::is_same_v<float_type, scalar_type>, float_type, __LINE__>() const
        {
            return this->at(0);
        }

    private:
        template <size_t TIndex, typename T0, typename... Tail>
        void construct(T0&& t0, Tail&&... tail)
        {
            using raw_t = std::remove_reference_t<T0>;
            if constexpr (std::is_constructible_v<scalar_type, raw_t>)
            {
                at_rvalue(TIndex) = scalar_type(std::forward<T0>(t0));
            }
            else
            {
                construct_from_vector_helper<TIndex>(detail::decay(std::forward<T0>(t0)));
            }

            if constexpr (sizeof...(Tail) > 0)
            {
                construct<TIndex + detail::get_total_component_count_v<T0> >(std::forward<Tail>(tail)...);
            }
        }

        template <size_t TIndex, typename TOtherScalar, size_t... TOtherIndices>
        void construct_from_vector_helper(const vector_<TOtherScalar, TOtherIndices...>& v)
        {
            const size_t limit = sizeof...(TOtherIndices) > num_components - TIndex ? (num_components - TIndex) : sizeof...(TOtherIndices);
            ((TOtherIndices < limit ? at_rvalue(TIndex + TOtherIndices) = scalar_type(v.at(TOtherIndices)), (void)0 : (void)0), ...);
        }


    public:
        constexpr int length() const noexcept
        {
            return num_components;
        }

        constexpr int _cxxswizzle_func_length() const noexcept
        {
            return length();
        }

        static matrix<scalar_type, num_components, num_components> call_outerProduct(this_type_arg c, this_type_arg r)
        {
            matrix<scalar_type, num_components, num_components> m;
            ((m.column(TIndices) = c * (r.at(TIndices))), ...);
            return m;
        }


        // functions
        // 8.1

        static this_type call_radians(float_vector_arg degrees)
        {
            return this_type(radians(degrees.at(TIndices))...);
        }
        static this_type call_degrees(float_vector_arg radians)
        {
            return this_type(degrees(radians.at(TIndices))...);
        }
        static this_type call_cos(float_vector_arg angle)
        {
            return this_type(cos(angle.at(TIndices))...);
        }
        static this_type call_tan(float_vector_arg angle)
        {
            return this_type(tan(angle.at(TIndices))...);
        }
        static this_type call_sin(float_vector_arg angle)
        {
            return this_type(sin(angle.at(TIndices))...);
        }
        static this_type call_asin(float_vector_arg x)
        {
            return this_type(asin(x.at(TIndices))...);
        }
        static this_type call_acos(float_vector_arg x)
        {
            return this_type(acos(x.at(TIndices))...);
        }
        static this_type call_atan(float_vector_arg y_over_x)
        {
            return this_type(atan(y_over_x.at(TIndices))...);
        }
        static this_type call_atan(float_vector_arg y, float_vector_arg x)
        {
            return this_type(atan(y.at(TIndices), x.at(TIndices))...);
        }
        static this_type call_sinh(float_vector_arg x)
        {
            return this_type(sinh(x.at(TIndices))...);
        }
        static this_type call_cosh(float_vector_arg x)
        {
            return this_type(cosh(x.at(TIndices))...);
        }
        static this_type call_tanh(float_vector_arg x)
        {
            return this_type(tanh(x.at(TIndices))...);
        }
        static this_type call_asinh(float_vector_arg x)
        {
            return this_type(asinh(x.at(TIndices))...);
        }
        static this_type call_acosh(float_vector_arg x)
        {
            return this_type(acosh(x.at(TIndices))...);
        }
        static this_type call_atanh(float_vector_arg x)
        {
            return this_type(atanh(x.at(TIndices))...);
        }

        // 8.2

        static this_type call_pow(float_vector_arg x, float_vector_arg y)
        {
            return this_type(pow(x.at(TIndices), y.at(TIndices))...);
        }
        static this_type call_exp(float_vector_arg x)
        {
            return this_type(exp(x.at(TIndices))...);
        }
        static this_type call_log(float_vector_arg x)
        {
            return this_type(log(x.at(TIndices))...);
        }

        static this_type call_exp2(float_vector_arg x)
        {
            return this_type(exp2(x.at(TIndices))...);
        }
        static this_type call_log2(float_vector_arg x)
        {
            return this_type(log2(x.at(TIndices))...);
        }
        static this_type call_sqrt(float_vector_arg x)
        {
            return this_type(sqrt(x.at(TIndices))...);
        }
        static this_type call_inversesqrt(float_vector_arg x)
        {
            return this_type(inversesqrt(x.at(TIndices))...);
        }

        // 8.3

        static this_type call_abs(number_vector_arg x)
        {
            return this_type(abs(x.at(TIndices))...);
        }
        static this_type call_sign(number_vector_arg x)
        {
            return this_type(sign(x.at(TIndices))...);
        }
        static this_type call_floor(float_vector_arg x)
        {
            return this_type(floor(x.at(TIndices))...);
        }
        static this_type call_trunc(float_vector_arg x)
        {
            return this_type(trunc(x.at(TIndices))...);
        }
        static this_type call_round(float_vector_arg x)
        {
            return this_type(round(x.at(TIndices))...);
        }
        static this_type call_roundEven(float_vector_arg x)
        {
            return this_type(roundEven(x.at(TIndices))...);
        }
        static this_type call_ceil(float_vector_arg x)
        {
            return this_type(ceil(x.at(TIndices))...);
        }
        static this_type call_fract(float_vector_arg x)
        {
            return this_type(fract(x.at(TIndices))...);
        }
        static this_type call_mod(float_vector_arg x, float_scalar_arg y)
        {
            return this_type(mod(x.at(TIndices), y)...);
        }
        static this_type call_mod(float_vector_arg x, double y)
        {
            return this_type(mod(x.at(TIndices), scalar_type(y))...);
        }
        static this_type call_mod(float_vector_arg x, float_vector_arg y)
        {
            return this_type(mod(x.at(TIndices), y.at(TIndices))...);
        }
        static this_type call_modf(float_vector_arg x, this_type& i)
        {
            return this_type(modf(x.at(TIndices), &i.at(TIndices))...);
        }
        static this_type call_modf(detail::only_if<num_components == 1, float_vector_arg> x, float& i)
        {
            // TODO: batch detection
            return this_type(modf(x.at(TIndices), &i)...);
        }

        static this_type call_min(number_vector_arg x, detail::only_if<(num_components>1), number_vector_arg> y)
        {
            return this_type(min(x.at(TIndices), y.at(TIndices))...);
        }
        static this_type call_min(number_vector_arg x, number_scalar_arg y)
        {
            return this_type(min(x.at(TIndices), y)...);
        }
        static this_type call_max(number_vector_arg x, detail::only_if<(num_components>1), number_vector_arg> y)
        {
            return this_type(max(x.at(TIndices), y.at(TIndices))...);
        }
        static this_type call_max(number_vector_arg x, number_scalar_arg y)
        {
            return this_type(max(x.at(TIndices), y)...);
        }
        static this_type call_clamp(number_vector_arg x, number_scalar_arg_cond minVal, number_scalar_arg_cond maxVal)
        {
            return this_type(min(max(x.at(TIndices), minVal), maxVal)...);
        }
        static this_type call_clamp(number_vector_arg x, number_vector_arg minVal, number_vector_arg maxVal)
        {
            return this_type(min(max(x.at(TIndices), minVal.at(TIndices)), maxVal.at(TIndices))...);
        }
        static this_type call_mix(float_vector_arg x, float_vector_arg y, float_scalar_arg_cond a)
        {
            return this_type(x.at(TIndices) + a * (y.at(TIndices) - x.at(TIndices))...);
        }
        static this_type call_mix(float_vector_arg x, float_vector_arg y, double a)
        {
            return this_type(x.at(TIndices) + scalar_type(a) * (y.at(TIndices) - x.at(TIndices))...);
        }
        static this_type call_mix(number_vector_arg x, number_vector_arg y, number_vector_arg a)
        {
            return this_type(x.at(TIndices) + a.at(TIndices) * (y.at(TIndices) - x.at(TIndices))...);
        }
        static this_type call_mix(number_vector_arg x, number_vector_arg y, const bool_vector_type& a)
        {
            return this_type(mix(x.at(TIndices), y.at(TIndices), a.at(TIndices))...);
        }

        static this_type call_step(float_vector_arg edge, float_vector_arg x)
        {
            // we'll delegate step to "outside" because there can't be branches here
            return this_type(step(edge.at(TIndices), x.at(TIndices))...);
        }

        static this_type call_step(float_scalar_arg edge, float_vector_arg x)
        {
            // we'll delegate step to "outside" because there can't be branches here
            return this_type(step(edge, x.at(TIndices))...);
        }

        static this_type call_step(double edge, float_vector_arg x)
        {
            // we'll delegate step to "outside" because there can't be branches here
            return this_type(step(scalar_type(edge), x.at(TIndices))...);
        }

        static this_type call_smoothstep(float_vector_arg_cond edge0, float_vector_arg_cond edge1, float_vector_arg x)
        {
            return this_type(smoothstep_helper(edge0.at(TIndices), edge1.at(TIndices), x.at(TIndices))...);
        }
        static this_type call_smoothstep(float_scalar_arg edge0, float_scalar_arg edge1, float_vector_arg x)
        {
            return this_type(smoothstep_helper(edge0, edge1, x.at(TIndices))...);
        }

        static bool_vector_type call_isnan(float_vector_arg x)
        {
            return bool_vector_type(isnan(x.at(TIndices))...);
        }
        static bool_vector_type call_isinf(float_vector_arg x)
        {
            return bool_vector_type(isinf(x.at(TIndices))...);
        }

        static vector<int_type, num_components> call_floatBitsToInt(float_vector_arg value)
        {
            return { bit_cast<int_type>(value.at(TIndices))... };
        }

        static vector<uint_type, num_components> call_floatBitsToUint(float_vector_arg value)
        {
            return { bit_cast<uint_type>(value.at(TIndices))... };
        }

        static vector<float_type, num_components> call_uintBitsToFloat(uint_vector_arg value)
        {
            return { bit_cast<float_type>(value.at(TIndices))... };
        }

        static vector<float_type, num_components> call_intBitsToFloat(int_vector_arg value)
        {
            return { bit_cast<float_type>(value.at(TIndices))... };
        }

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
            return ((x.at(TIndices) * y.at(TIndices)) + ...);
        }

        static typename detail::only_if<num_components == 3, this_type> call_cross(float_vector_arg x, float_vector_arg y)
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
            scalar_type k = scalar_type(1) - eta * eta * (scalar_type(1) - call_dot(N, I) * call_dot(N, I));
            scalar_type mask = step(scalar_type(0), k);
            return mask * (eta * I - (eta * call_dot(N, I) + sqrt(k)) * N);
        }

        // 8.6
        static bool_vector_type call_lessThan(number_vector_arg x, number_vector_arg y)
        {
            return bool_vector_type(x.at(TIndices) < y.at(TIndices)...);
        }
        static bool_vector_type call_lessThanEqual(number_vector_arg x, number_vector_arg y)
        {
            return bool_vector_type(x.at(TIndices) <= y.at(TIndices)...);
        }
        static bool_vector_type call_greaterThan(number_vector_arg x, number_vector_arg y)
        {
            return bool_vector_type(x.at(TIndices) > y.at(TIndices)...);
        }
        static bool_vector_type call_greaterThanEqual(number_vector_arg x, number_vector_arg y)
        {
            return bool_vector_type(x.at(TIndices) >= y.at(TIndices)...);
        }

        static bool_vector_type call_equal(this_type_arg x, this_type_arg y)
        {
            return bool_vector_type(x.at(TIndices) == y.at(TIndices)...);
        }
        static bool_vector_type call_notEqual(this_type_arg x, this_type_arg y)
        {
            return bool_vector_type(x.at(TIndices) != y.at(TIndices)...);
        }

        static scalar_type call_any(bool_vector_arg x)
        {
            return ((x.at(TIndices)) || ...);
        }
        static scalar_type call_all(bool_vector_arg x)
        {
            return ((x.at(TIndices)) && ...);
        }
        static this_type call_not(bool_vector_arg x)
        {
            return this_type(!x.at(TIndices)...);
        }

        // 8.8
        static this_type call_dFdx(float_vector_arg x)
        {
            return this_type(dFdx(x.at(TIndices))...);
        }
        static this_type call_dFdy(float_vector_arg x)
        {
            return this_type(dFdy(x.at(TIndices))...);
        }
        static this_type call_fwidth(float_vector_arg x)
        {
            return this_type(fwidth(x.at(TIndices))...);
        }


        // operators

        friend bool_type operator==(this_type_arg a, this_type_arg b)
        {
            return a.are_components_equal(b);
        }

        friend bool_type operator!=(this_type_arg a, this_type_arg b)
        {
            return !a.are_components_equal(b);
        }


        this_type& operator=(this_type_arg other)      & { return ((at(TIndices) = other.at(TIndices)), ..., *this);  }
        this_type& operator+=(number_vector_arg other) & { return ((at(TIndices) += other.at(TIndices)), ..., *this); }
        this_type& operator-=(number_vector_arg other) & { return ((at(TIndices) -= other.at(TIndices)), ..., *this); }
        this_type& operator*=(number_vector_arg other) & { return ((at(TIndices) *= other.at(TIndices)), ..., *this); }
        this_type& operator/=(number_vector_arg other) & { return ((at(TIndices) /= other.at(TIndices)), ..., *this); }
        this_type& operator+=(number_scalar_arg other) & { return ((at(TIndices) += other), ..., *this); }
        this_type& operator-=(number_scalar_arg other) & { return ((at(TIndices) -= other), ..., *this); }
        this_type& operator*=(number_scalar_arg other) & { return ((at(TIndices) *= other), ..., *this); }
        this_type& operator/=(number_scalar_arg other) & { return ((at(TIndices) /= other), ..., *this); }

        this_type& operator=(this_type_arg other)      && { return ((at_rvalue(TIndices) = other.at(TIndices)), ..., *this);  }
        this_type& operator+=(number_vector_arg other) && { return ((at_rvalue(TIndices) += other.at(TIndices)), ..., *this); }
        this_type& operator-=(number_vector_arg other) && { return ((at_rvalue(TIndices) -= other.at(TIndices)), ..., *this); }
        this_type& operator*=(number_vector_arg other) && { return ((at_rvalue(TIndices) *= other.at(TIndices)), ..., *this); }
        this_type& operator/=(number_vector_arg other) && { return ((at_rvalue(TIndices) /= other.at(TIndices)), ..., *this); }
        this_type& operator+=(number_scalar_arg other) && { return ((at_rvalue(TIndices) += other), ..., *this); }
        this_type& operator-=(number_scalar_arg other) && { return ((at_rvalue(TIndices) -= other), ..., *this); }
        this_type& operator*=(number_scalar_arg other) && { return ((at_rvalue(TIndices) *= other), ..., *this); }
        this_type& operator/=(number_scalar_arg other) && { return ((at_rvalue(TIndices) /= other), ..., *this); }

        friend this_type operator+(number_vector_arg_cond a, number_vector_arg_cond b) { return this_type(a.at(TIndices)+b.at(TIndices)...); }
        friend this_type operator-(number_vector_arg_cond a, number_vector_arg_cond b) { return this_type(a.at(TIndices)-b.at(TIndices)...); }
        friend this_type operator*(number_vector_arg_cond a, number_vector_arg_cond b) { return this_type(a.at(TIndices)*b.at(TIndices)...); }
        friend this_type operator/(number_vector_arg_cond a, number_vector_arg_cond b) { return this_type(a.at(TIndices)/b.at(TIndices)...); }

        friend this_type operator+(number_vector_arg_cond a, number_scalar_arg b) { return this_type(a.at(TIndices)+b...); }
        friend this_type operator-(number_vector_arg_cond a, number_scalar_arg b) { return this_type(a.at(TIndices)-b...); }
        friend this_type operator*(number_vector_arg_cond a, number_scalar_arg b) { return this_type(a.at(TIndices)*b...); }
        friend this_type operator/(number_vector_arg_cond a, number_scalar_arg b) { return this_type(a.at(TIndices)/b...); }
        friend this_type operator+(number_scalar_arg a, number_vector_arg_cond b) { return b + a; }
        friend this_type operator-(number_scalar_arg a, number_vector_arg_cond b) { return this_type(a-b.at(TIndices)...); }
        friend this_type operator*(number_scalar_arg a, number_vector_arg_cond b) { return b * a; }
        friend this_type operator/(number_scalar_arg a, number_vector_arg_cond b) { return this_type(a/b.at(TIndices)...); }

        friend this_type operator+(number_vector_arg_cond a, literal_arg b) { return this_type(a.at(TIndices)+b...); }
        friend this_type operator-(number_vector_arg_cond a, literal_arg b) { return this_type(a.at(TIndices)-b...); }
        friend this_type operator*(number_vector_arg_cond a, literal_arg b) { return this_type(a.at(TIndices)*b...); }
        friend this_type operator/(number_vector_arg_cond a, literal_arg b) { return this_type(a.at(TIndices)/b...); }
        friend this_type operator+(literal_arg a, number_vector_arg_cond b) { return b + a; }
        friend this_type operator-(literal_arg a, number_vector_arg_cond b) { return this_type(a-b.at(TIndices)...); }
        friend this_type operator*(literal_arg a, number_vector_arg_cond b) { return b * a; }
        friend this_type operator/(literal_arg a, number_vector_arg_cond b) { return this_type(a/b.at(TIndices)...); }


        this_type& operator%=(integral_vector_arg other)& { return ((at(TIndices) %= other.at(TIndices)), ..., *this); }
        this_type& operator&=(integral_vector_arg other)& { return ((at(TIndices) &= other.at(TIndices)), ..., *this); }
        this_type& operator|=(integral_vector_arg other)& { return ((at(TIndices) |= other.at(TIndices)), ..., *this); }
        this_type& operator^=(integral_vector_arg other)& { return ((at(TIndices) ^= other.at(TIndices)), ..., *this); }

        this_type& operator%=(integral_scalar_arg other)& { return ((at(TIndices) %= other), ..., *this); }
        this_type& operator&=(integral_scalar_arg other)& { return ((at(TIndices) &= other), ..., *this); }
        this_type& operator|=(integral_scalar_arg other)& { return ((at(TIndices) |= other), ..., *this); }
        this_type& operator^=(integral_scalar_arg other)& { return ((at(TIndices) ^= other), ..., *this); }

        this_type& operator<<=(integral_scalar_arg other)& { return ((at(TIndices) <<= other), ..., *this); }
        this_type& operator>>=(integral_scalar_arg other)& { return ((at(TIndices) >>= other), ..., *this); }

        this_type& operator%=(integral_vector_arg other)&& { return ((at_rvalue(TIndices) %= other.at(TIndices)), ..., *this); }
        this_type& operator&=(integral_vector_arg other)&& { return ((at_rvalue(TIndices) &= other.at(TIndices)), ..., *this); }
        this_type& operator|=(integral_vector_arg other)&& { return ((at_rvalue(TIndices) |= other.at(TIndices)), ..., *this); }
        this_type& operator^=(integral_vector_arg other)&& { return ((at_rvalue(TIndices) ^= other.at(TIndices)), ..., *this); }

        this_type& operator%=(integral_scalar_arg other)&& { return ((at_rvalue(TIndices) %= other), ..., *this); }
        this_type& operator&=(integral_scalar_arg other)&& { return ((at_rvalue(TIndices) &= other), ..., *this); }
        this_type& operator|=(integral_scalar_arg other)&& { return ((at_rvalue(TIndices) |= other), ..., *this); }
        this_type& operator^=(integral_scalar_arg other)&& { return ((at_rvalue(TIndices) ^= other), ..., *this); }

        this_type& operator<<=(integral_scalar_arg other)&& { return ((at_rvalue(TIndices) <<= other), ..., *this); }
        this_type& operator>>=(integral_scalar_arg other)&& { return ((at_rvalue(TIndices) >>= other), ..., *this); }

        friend this_type operator%(integral_vector_arg_cond a, integral_vector_arg_cond b) { return this_type(a.at(TIndices) % b.at(TIndices)...); }
        friend this_type operator&(integral_vector_arg_cond a, integral_vector_arg_cond b) { return this_type(a.at(TIndices) & b.at(TIndices)...); }
        friend this_type operator|(integral_vector_arg_cond a, integral_vector_arg_cond b) { return this_type(a.at(TIndices) | b.at(TIndices)...); }
        friend this_type operator^(integral_vector_arg_cond a, integral_vector_arg_cond b) { return this_type(a.at(TIndices) ^ b.at(TIndices)...); }

        friend this_type operator<<(integral_vector_arg_cond a, integral_vector_arg_cond b) { return this_type(a.at(TIndices) << b.at(TIndices)...); }
        friend this_type operator>>(integral_vector_arg_cond a, integral_vector_arg_cond b) { return this_type(a.at(TIndices) >> b.at(TIndices)...); }

        friend this_type operator%(integral_vector_arg_cond a, integral_scalar_arg b) { return this_type(a.at(TIndices) % b...); }
        friend this_type operator&(integral_vector_arg_cond a, integral_scalar_arg b) { return this_type(a.at(TIndices) & b...); }
        friend this_type operator|(integral_vector_arg_cond a, integral_scalar_arg b) { return this_type(a.at(TIndices) | b...); }
        friend this_type operator^(integral_vector_arg_cond a, integral_scalar_arg b) { return this_type(a.at(TIndices) ^ b...); }

        friend this_type operator<<(integral_vector_arg_cond a, integral_scalar_arg b) { return this_type(a.at(TIndices) << b...); }
        friend this_type operator>>(integral_vector_arg_cond a, integral_scalar_arg b) { return this_type(a.at(TIndices) >> b...); }

        friend this_type operator%(integral_scalar_arg a, integral_vector_arg_cond b) { return b % a; }
        friend this_type operator&(integral_scalar_arg a, integral_vector_arg_cond b) { return b & a; }
        friend this_type operator|(integral_scalar_arg a, integral_vector_arg_cond b) { return b | a; }
        friend this_type operator^(integral_scalar_arg a, integral_vector_arg_cond b) { return b ^ a; }

        friend this_type operator<<(integral_scalar_arg a, integral_vector_arg_cond b) { return b << a; }
        friend this_type operator>>(integral_scalar_arg a, integral_vector_arg_cond b) { return b >> a; }

        friend this_type operator%(integral_vector_arg_cond a, literal_arg b) { return this_type(a.at(TIndices) % b...); }
        friend this_type operator&(integral_vector_arg_cond a, literal_arg b) { return this_type(a.at(TIndices) & b...); }
        friend this_type operator|(integral_vector_arg_cond a, literal_arg b) { return this_type(a.at(TIndices) | b...); }
        friend this_type operator^(integral_vector_arg_cond a, literal_arg b) { return this_type(a.at(TIndices) ^ b...); }

        friend this_type operator<<(integral_vector_arg_cond a, literal_arg b) { return this_type(a.at(TIndices) << b...); }
        friend this_type operator>>(integral_vector_arg_cond a, literal_arg b) { return this_type(a.at(TIndices) >> b...); }

        friend this_type operator%(literal_arg a, integral_vector_arg_cond b) { return b % a; }
        friend this_type operator&(literal_arg a, integral_vector_arg_cond b) { return b & a; }
        friend this_type operator|(literal_arg a, integral_vector_arg_cond b) { return b | a; }
        friend this_type operator^(literal_arg a, integral_vector_arg_cond b) { return b ^ a; }
        friend this_type operator>>(literal_arg a, integral_vector_arg_cond b) { return b >> a; }
        friend this_type operator<<(literal_arg a, integral_vector_arg_cond b) { return b << a; }


        inline detail::only_if<scalar_is_number, this_type> operator-() const
        {
            this_type result;
            ((result.at_rvalue(TIndices) = -at(TIndices)), ...);
            return result;
        }

        inline detail::only_if<scalar_is_number, this_type> operator+() const
        {
            return *this;
        }

        inline this_type& operator*=(const matrix<scalar_type, num_components, num_components>& mat)
        {
            return *this = mat.mul(*this, mat);
        }

        inline detail::only_if<scalar_is_number, this_type>& operator++()
        {
            return ((at(TIndices) += 1), ..., *this);
        }

        inline detail::only_if<scalar_is_number, this_type> operator++(int)
        {
            this_type result(*this);
            ++(*this);
            return result;
        }

        inline detail::only_if<scalar_is_number, this_type>& operator--()
        {
            return ((at(TIndices) -= 1), ..., *this);
        }

        inline detail::only_if<scalar_is_number, this_type> operator--(int)
        {
            this_type result(*this);
            --(*this);
            return result;
        }



    private:
        static scalar_type smoothstep_helper(scalar_arg edge0, scalar_arg edge1, scalar_arg x)
        {
            auto t = (x - edge0) / (edge1 - edge0);
            detail::nonmasked(t) = min(max(t, scalar_type(0.0)), scalar_type(1.0));
            return t * t * (scalar_type(3.0) - scalar_type(2.0) * t);
        }

        inline bool_type are_components_equal(this_type_arg other) const
        {
            // for some reason this can't be used in friend inline function
            return ((at(TIndices) == other.at(TIndices)) && ...);
        }

        // STL COMPABILITY (not needed, but useful for testing)
    public:

        const_iterator begin() const
        {
            return detail::make_indexed_vector_iterator(*this);
        }
        //! \return Immutable iterator.
        const_iterator end() const
        {
            return detail::make_indexed_vector_iterator(*this, num_components);
        }
        //! \return Mutable iterator.
        iterator begin()
        {
            return detail::make_indexed_vector_iterator(*this);
        }
        //! \return Mutable iterator.
        iterator end()
        {
            return detail::make_indexed_vector_iterator(*this, num_components);
        }


        //! Iterates over the vector, firing Func for each index
        template <class Func>
        static void visit_index(Func func)
        {
            ((func(TIndices)), ...);
        }

        //! As an inline friend function, because thanks to that all convertibles will use same function.
        friend std::ostream& operator<<(std::ostream& os, this_type_arg vec)
        {
            vec.visit_index([&](size_t i) -> void { os << vec[i] << (i == num_components - 1 ? "" : ","); });
            return os;
        }

        //! As an inline friend function, because thanks to that all convertibles will use same function.
        friend std::istream& operator>>(std::istream& is, this_type& vec)
        {
            vec.visit_index([&](size_t i) -> void
            {
                is >> vec[i];
                if (i < num_components - 1 && is.peek() == ',')
                {
                    is.ignore(1);
                }
            });
            return is;
        }

    };

    namespace detail
    {
        template <typename TScalar, size_t... TIndices>
        struct get_vector_type_impl< ::swizzle::vector_<TScalar, TIndices...> >
        {
            typedef ::swizzle::vector_<TScalar, TIndices...> type;
        };
    }
}
