#pragma once

#include <swizzle/glsl/vector_helper.h>
#include <swizzle/detail/common_binary_operators.h>

namespace swizzle
{
    namespace glsl
    {
        template <typename ScalarType, size_t... Index>
        struct vector_base;

        template <typename ScalarType, size_t... Index>
        struct vector_arithmetic_operations : detail::common_binary_operators<vector_base<ScalarType, Index...>, ScalarType>
        {
            using build_info = vector_build_info<ScalarType, sizeof...(Index)>;
            
            using this_type = vector_base<ScalarType, Index...>;
            using this_type_arg = const this_type&;
            
            //! Well, passing by value is not supported for aligned types in VC++, so let's pass by reference ;(
            using scalar_type = ScalarType;
            using scalar_arg = const scalar_type&;
            
            using bool_type = typename build_info::bool_type;
            using bool_vector_type = vector_base<bool_type, Index...>;

            // operators

            inline this_type& operator=(this_type_arg other)
            {
                return ((at(Index) = other.at(Index)), ..., *this);
            }
            inline this_type& operator+=(this_type_arg other)
            {
                return ((at(Index) += other.at(Index)), ..., *this);
            }
            inline this_type& operator-=(this_type_arg other)
            {
                return ((at(Index) -= other.at(Index)), ..., *this);
            }
            inline this_type& operator*=(this_type_arg other)
            {
                return ((at(Index) *= other.at(Index)), ..., *this);
            }
            inline this_type& operator/=(this_type_arg other)
            {
                return ((at(Index) /= other.at(Index)), ..., *this);
            }
            inline this_type& operator+=(scalar_arg other)
            {
                return ((at(Index) += other), ..., *this);
            }
            inline this_type& operator-=(scalar_arg other)
            {
                return ((at(Index) -= other), ..., *this);
            }
            inline this_type& operator*=(scalar_arg other)
            {
                return ((at(Index) *= other), ..., *this);
            }
            inline this_type& operator/=(scalar_arg other)
            {
                return ((at(Index) /= other), ..., *this);
            }

            inline this_type operator-() const
            {
                this_type result;
                ((result.at(Index) = -at(Index)), ...);
                return result;
            }

            // functions
            // 8.1

            static this_type call_radians(this_type_arg degrees) { return { (radians(degrees.data[Index]))... }; }
            static this_type call_degrees(this_type_arg radians) { return { (degrees(radians.data[Index]))... }; }
            static this_type call_cos(this_type_arg angle) { return { (cos(angle.data[Index]))... }; }
            static this_type call_tan(this_type_arg angle) { return { (tan(angle.data[Index]))... }; }
            static this_type call_sin(this_type_arg angle) { return { (sin(angle.data[Index]))... }; }
            static this_type call_asin(this_type_arg x) { return { (asin(x.data[Index]))... }; }
            static this_type call_acos(this_type_arg x) { return { (acos(x.data[Index]))... }; }
            static this_type call_atan(this_type_arg y_over_x) { return { (atan(y_over_x.data[Index]))... }; }
            static this_type call_atan(this_type_arg y, this_type_arg x) { return { (atan(y.data[Index], x.data[Index]))... }; }
            static this_type call_sinh(this_type_arg x) { return { (sinh(x.data[Index]))... }; }
            static this_type call_cosh(this_type_arg x) { return { (cosh(x.data[Index]))... }; }
            static this_type call_tanh(this_type_arg x) { return { (tanh(x.data[Index]))... }; }
            static this_type call_asinh(this_type_arg x) { return { (asinh(x.data[Index]))... }; }
            static this_type call_acosh(this_type_arg x) { return { (acosh(x.data[Index]))... }; }
            static this_type call_atanh(this_type_arg x) { return { (atanh(x.data[Index]))... }; }

            // 8.2

            static this_type call_pow(this_type_arg x, this_type_arg y) { return { (pow(x.data[Index], y.data[Index]))... }; }
            static this_type call_exp(this_type_arg x) { return { (exp(x.data[Index]))... }; }
            static this_type call_log(this_type_arg x) { return { (log(x.data[Index]))... }; }

            static this_type call_exp2(this_type_arg x) { return { (exp2(x.data[Index]))... }; }
            static this_type call_log2(this_type_arg x) { return { (log2(x.data[Index]))... }; }
            static this_type call_sqrt(this_type_arg x) { return { (sqrt(x.data[Index]))... }; }
            static this_type call_inversesqrt(this_type_arg x) { return { (inversesqrt(x.data[Index]))... }; }

            // 8.3

            static this_type call_abs(this_type_arg x) { return { (abs(x.data[Index]))... }; }
            static this_type call_sign(this_type_arg x) { return { (sign(x.data[Index]))... }; }
            static this_type call_floor(this_type_arg x) { return { (floor(x.data[Index]))... }; }
            static this_type call_trunc(this_type_arg x) { return { (trunc(x.data[Index]))... }; }
            static this_type call_round(this_type_arg x) { return { (round(x.data[Index]))... }; }
            static this_type call_roundEven(this_type_arg x) { return { (roundEven(x.data[Index]))... }; }
            static this_type call_ceil(this_type_arg x) { return { (ceil(x.data[Index]))... }; }
            static this_type call_fract(this_type_arg x) { return { (fract(x.data[Index]))... }; }
            static this_type call_mod(this_type_arg x, scalar_arg y) { return { (mod(x.data[Index], y))... }; }
            static this_type call_mod(this_type_arg x, this_type_arg y) { return { (mod(x.data[Index], y.data[Index]))... }; }
            //CXXSWIZZLE_COMPONENTWISE_FUNC_V_modf(genType x, out genType i)
            static this_type call_min(this_type_arg x, this_type_arg y) { return { (min(x.data[Index], y.data[Index]))... }; }
            static this_type call_min(this_type_arg x, scalar_arg y) { return { (min(x.data[Index], y))... }; }
            static this_type call_max(this_type_arg x, this_type_arg y) { return { (max(x.data[Index], y.data[Index]))... }; }
            static this_type call_max(this_type_arg x, scalar_arg y) { return { (max(x.data[Index], y))... }; }

            static this_type call_clamp(this_type_arg x, scalar_arg minVal, scalar_arg maxVal)
            {
                return { (max(min(x.data[Index], maxVal), minVal))... };
            }

            static this_type call_clamp(this_type_arg x, this_type_arg minVal, this_type_arg maxVal)
            {
                return { (max(min(x.data[Index], maxVal.data[Index]), minVal.data[Index]))... };
            }

            static this_type call_mix(this_type_arg x, this_type_arg y, scalar_arg a)
            {
                return { (x.data[Index] + a * (y.data[Index] - x.data[Index]))... };
            }

            static this_type call_mix(this_type_arg x, this_type_arg y, this_type_arg a)
            {
                return { (x.data[Index] + a.data[Index] * (y.data[Index] - x.data[Index]))... };
            }

            //CXXSWIZZLE_COMPONENTWISE_FUNC_V_VVB(mix, x, y, a)

            // we'll delegate step to "outside" because there can't be branches here
            static this_type call_step(this_type_arg edge, this_type_arg x) { return { (step(edge.data[Index], x.data[Index]))... }; }
            static this_type call_step(scalar_arg edge, this_type_arg x) { return { (step(edge, x.data[Index]))... }; }

            //CXXSWIZZLE_COMPONENTWISE_FUNC_V_VVV(smoothstep, edge0, edge1, x)
            //CXXSWIZZLE_COMPONENTWISE_FUNC_V_SSV(smoothstep, edge0, edge1, x)

            static this_type call_smoothstep(this_type_arg edge0, scalar_arg edge1, this_type_arg x)
            {
                return { (smoothstep_helper(edge0.data[Index], edge1.data[Index], x.data[Index]))... };
            }

            static this_type call_smoothstep(scalar_arg edge0, scalar_arg edge1, this_type_arg x)
            {
                return { (smoothstep_helper(edge0, edge1, x.data[Index]))... };
            }

            static bool_vector_type call_isnan(this_type_arg x) { return bool_vector_type((isnan(x.data[Index]))...); }
            static bool_vector_type call_isinf(this_type_arg p1) { return bool_vector_type((isinf(x.data[Index]))...); }

            //CXXSWIZZLE_COMPONENTWISE_FUNC_I_V(floatBitsToInt, value)
            //CXXSWIZZLE_COMPONENTWISE_FUNC_U_V(floatBitsToUint, value)
            //CXXSWIZZLE_COMPONENTWISE_FUNC_V_intBitsToFloat(genIType value)
            //CXXSWIZZLE_COMPONENTWISE_FUNC_V_uintBitsToFloat(genUType value)

            // 8.4

            static scalar_type call_length(this_type_arg x)
            {
                return sqrt(call_dot(x, x));
            }

            static scalar_type call_distance(this_type_arg p0, const this_type_arg p1)
            {
                return call_length(p0 - p1);
            }

            static scalar_type call_dot(this_type_arg x, this_type_arg y)
            {
                return ((x.data[Index] * y.data[Index]) + ...);
            }

            static typename only_if<sizeof...(Index) == 3, this_type> call_cross(this_type_arg x, this_type_arg y)
            {
                auto rx = x[1] * y[2] - x[2] * y[1];
                auto ry = x[2] * y[0] - x[0] * y[2];
                auto rz = x[0] * y[1] - x[1] * y[0];
                return this_type(rx, ry, rz);
            }

            static this_type call_normalize(this_type_arg x)
            {
                return this_type(x) * inversesqrt(call_dot(x, x));
            }

            static this_type call_faceforward(this_type_arg N, this_type_arg I, this_type_arg Nref)
            {
                scalar_type s = step(scalar_type(0), call_dot(Nref, I));
                s = s * (-2) + 1; // 0 -> 1, 1 -> -1
                return N * s;
            }

            static this_type call_reflect(this_type_arg I, this_type_arg N)
            {
                return (I - 2 * call_dot(I, N) * N);
            }

            static this_type call_refract(this_type_arg I, this_type_arg N, scalar_arg eta)
            {
                scalar_type k = 1.0 - eta * eta * (1.0 - call_dot(N, I) * call_dot(N, I));
                scalar_type mask = step(scalar_type(0), k);
                return mask * (eta * I - (eta * call_dot(N, I) + sqrt(k)) * N);
            }


            // 8.5
            static bool_vector_type call_lessThan(this_type_arg x, this_type_arg y) { return { (x.data[Index] < y.data[Index])... }; }
            static bool_vector_type call_lessThanEqual(this_type_arg x, this_type_arg y) { return { (x.data[Index] <= y.data[Index])... }; }
            static bool_vector_type call_greaterThan(this_type_arg x, this_type_arg y) { return { (x.data[Index] > y.data[Index])... }; }
            static bool_vector_type call_greaterThanEqual(this_type_arg x, this_type_arg y) { return { (x.data[Index] >= y.data[Index])... }; }
            static bool_vector_type call_equal(this_type_arg x, this_type_arg y) { return { (x.data[Index] == y.data[Index])... }; }
            static bool_vector_type call_notEqual(this_type_arg x, this_type_arg y) { return { (x.data[Index] != y.data[Index])... }; }

        private:
            static scalar_type smoothstep_helper(scalar_arg edge0, scalar_arg edge1, scalar_arg x)
            {
                auto t = (x - edge0) / (edge1 - edge0);
                t = min(max(t, scalar_arg(0)), scalar_arg(1));
                return t * t * (3 - 2 * t);
            }

        };
    }

    //namespace detail
    //{
    //    template <typename ScalarType, size_t... Index>
    //    struct get_vector_type_impl< ::swizzle::glsl::number_vector<ScalarType, Index...> >
    //    {
    //        typedef ::swizzle::glsl::number_vector<ScalarType, Index...> type;
    //    };

    //    template <typename ScalarType, size_t... Index> number_vector<ScalarType, Index...> number_vector_type_builder(std::index_sequence<Index...>);
    //}

    //namespace glsl
    //{
    //    template <typename ScalarType, size_t Size>
    //    using vector = decltype(detail::number_vector_type_builder<BoolType>(std::make_index_sequence<Size>{}));
    //}
}