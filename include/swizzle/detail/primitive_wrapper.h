// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <cmath>
#include <swizzle/detail/utils.h>

#define CXXSWIZZLE_FORCE_INLINE __forceinline

namespace swizzle
{
    namespace detail
    {
        struct default_assign_policy
        {
            template <typename T>
            static void assign(T& a, const T& b)
            {
                a = b;
            }
        };

        template <typename WrapperType, typename BoolType>
        struct adapter_binary_operators
        {
            using this_type = WrapperType;
            using bool_type = BoolType;
            using this_arg = const this_type&;
  
            // binary operators

            CXXSWIZZLE_FORCE_INLINE friend this_type operator+(this_arg a, this_arg b)   { return static_cast<this_type>(a.data + b.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type operator-(this_arg a, this_arg b)   { return static_cast<this_type>(a.data - b.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type operator*(this_arg a, this_arg b)   { return static_cast<this_type>(a.data * b.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type operator/(this_arg a, this_arg b)   { return static_cast<this_type>(a.data / b.data); }

            CXXSWIZZLE_FORCE_INLINE friend bool_type operator>(this_arg a, this_arg b)   { return static_cast<bool_type::data_type>(a.data > b.data); }
            CXXSWIZZLE_FORCE_INLINE friend bool_type operator>=(this_arg a, this_arg b)  { return static_cast<bool_type::data_type>(a.data >= b.data); }
            CXXSWIZZLE_FORCE_INLINE friend bool_type operator<(this_arg a, this_arg b)   { return static_cast<bool_type::data_type>(a.data < b.data); }
            CXXSWIZZLE_FORCE_INLINE friend bool_type operator<=(this_arg a, this_arg b)  { return static_cast<bool_type::data_type>(a.data <= b.data); }
            CXXSWIZZLE_FORCE_INLINE friend bool_type operator==(this_arg a, this_arg b)  { return static_cast<bool_type::data_type>(a.data == b.data); }
            CXXSWIZZLE_FORCE_INLINE friend bool_type operator!=(this_arg a, this_arg b)  { return static_cast<bool_type::data_type>(a.data != b.data); }
        };


        template <typename AssignPolicy, typename InternalFloatType, typename InternalIntType, typename InternalUintType, typename InternalBoolType>
        struct adapter_traits
        {
            using internal_float_type = InternalFloatType;
            using internal_int_type = InternalIntType;
            using internal_uint_type = InternalUintType;
            using internal_bool_type = InternalBoolType;
            using assign_policy = AssignPolicy;
        };

        template <typename DataType, typename AssignPolicy>
        struct adapter_base
        {
            using data_type = DataType;

            data_type data;

            adapter_base()
            {}

            adapter_base(typename data_type& data)
                : data(data)
            {}

            //! To avoid ADL-hell, cast is explict.
            CXXSWIZZLE_FORCE_INLINE explicit operator data_type() const
            {
                return data;
            }

            CXXSWIZZLE_FORCE_INLINE void assign(const adapter_base& other)
            {
                AssignPolicy::assign(data, other.data);
            }
        };


        template <typename DataType, typename AssignPolicy = default_assign_policy>
        struct bool_adapter : adapter_base<DataType, AssignPolicy>
        {
            using adapter_base::adapter_base;
            using this_type = bool_adapter;
            using this_arg = const this_type&;

            bool_adapter(bool b) : bool_adapter(bool_to_simd(b)) {}

            CXXSWIZZLE_FORCE_INLINE this_type& operator=(this_arg other)
            {
                assign(other);
                return *this;
            }

            // for CxxSwizzle ADL-magic

            this_type decay() const
            {
                return *this;
            }

            // implement if you really need to.
            CXXSWIZZLE_FORCE_INLINE operator bool() const { return simd_to_bool(data); }

            CXXSWIZZLE_FORCE_INLINE friend this_type operator||(this_arg a, this_arg b)   { return static_cast<this_type>(a.data || b.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type operator&&(this_arg a, this_arg b)   { return static_cast<this_type>(a.data && b.data); }
        };

        template <typename DataType, typename BoolType, typename AssignPolicy = default_assign_policy>
        struct int_adapter : adapter_base<DataType, AssignPolicy>, adapter_binary_operators<int_adapter<DataType, BoolType, AssignPolicy>, BoolType>
        {
            using adapter_base::adapter_base;
            using this_type = int_adapter;
            using this_arg = const this_type&;

            int_adapter(int value): int_adapter(int_to_simd(value)) {}

            CXXSWIZZLE_FORCE_INLINE this_type& operator=(this_arg other)
            {
                assign(other);
                return *this;
            }

            CXXSWIZZLE_FORCE_INLINE this_type& operator++()
            {
                assign(data + int_to_simd(1));
                return *this;
            }

            CXXSWIZZLE_FORCE_INLINE this_type operator++(int)
            {
                this_type result = *this;
                assign(data + int_to_simd(1));
                return result;
            }

            CXXSWIZZLE_FORCE_INLINE this_type& operator--()
            {
                assign(data - int_to_simd(1));
                return *this;
            }

            CXXSWIZZLE_FORCE_INLINE this_type operator--(int)
            {
                this_type result = *this;
                assign(data - int_to_simd(1));
                return result;
            }

            // for CxxSwizzle ADL-magic

            this_type decay() const
            {
                return *this;
            }

            this_type operator-() const             { return -data; }
            this_type& operator+=(this_arg other)   { return *this = *this + other; }
            this_type& operator-=(this_arg other)   { return *this = *this - other; }
            this_type& operator*=(this_arg other)   { return *this = *this * other; }
            this_type& operator/=(this_arg other)   { return *this = *this / other; }

            CXXSWIZZLE_FORCE_INLINE friend this_type min(this_arg x, this_arg y) { return min(x.data, y.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type max(this_arg x, this_arg y) { return max(x.data, y.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type clamp(this_arg x, this_arg minVal, this_arg maxVal) { return clamp(x.data, minVal.data, maxVal.data); }
        };

        template <typename DataType, typename BoolType, typename AssignPolicy = default_assign_policy>
        struct uint_adapter : adapter_base<DataType, AssignPolicy>, adapter_binary_operators<uint_adapter<DataType, BoolType, AssignPolicy>, BoolType>
        {
            using adapter_base::adapter_base;
            using this_type = uint_adapter;
            using this_arg = const this_type&;

            uint_adapter(unsigned value) : uint_adapter(uint_to_simd(value)) {}

            CXXSWIZZLE_FORCE_INLINE this_type& operator=(this_arg other)
            {
                assign(other);
                return *this;
            }

            // for CxxSwizzle ADL-magic

            this_type decay() const
            {
                return *this;
            }

            this_type operator-() const             { return -data; }
            this_type& operator+=(this_arg other)   { return *this = *this + other; }
            this_type& operator-=(this_arg other)   { return *this = *this - other; }
            this_type& operator*=(this_arg other)   { return *this = *this * other; }
            this_type& operator/=(this_arg other)   { return *this = *this / other; }

            CXXSWIZZLE_FORCE_INLINE friend this_type min(this_arg x, this_arg y) { return min(x.data, y.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type max(this_arg x, this_arg y) { return max(x.data, y.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type clamp(this_arg x, this_arg minVal, this_arg maxVal) { return clamp(x.data, minVal.data, maxVal.data); }
        };


        template <typename DataType, typename BoolType, typename AssignPolicy = default_assign_policy>
        struct float_adapter : adapter_base<DataType, AssignPolicy>, adapter_binary_operators<float_adapter<DataType, BoolType, AssignPolicy>, BoolType>
        {
            using adapter_base::adapter_base;
            using this_type = float_adapter;
            using this_arg = const this_type&;
            using bool_type = BoolType;

            float_adapter(float value) : float_adapter(float_to_simd(value)) {}
            float_adapter(double value): float_adapter(float_to_simd(static_cast<float>(value))) {}
            float_adapter(const bool_type& value) : float_adapter(simd_bool_to_simd_float(value.data)) {}

            CXXSWIZZLE_FORCE_INLINE this_type& operator=(this_arg other)
            {
                assign(other);
                return *this;
            }

            // for CxxSwizzle ADL-magic

            this_type decay() const
            {
                return *this;
            }

            this_type operator-() const             { return -data; }
            this_type& operator+=(this_arg other)   { return *this = *this + other; }
            this_type& operator-=(this_arg other)   { return *this = *this - other; }
            this_type& operator*=(this_arg other)   { return *this = *this * other; }
            this_type& operator/=(this_arg other)   { return *this = *this / other; }

            // functions
            // 8.1

            CXXSWIZZLE_FORCE_INLINE friend this_type radians(this_arg degrees) { return radians(degrees.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type degrees(this_arg radians) { return degrees(radians.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type cos(this_arg angle) { return cos(angle.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type tan(this_arg angle) { return tan(angle.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type sin(this_arg angle) { return sin(angle.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type asin(this_arg x) { return asin(x.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type acos(this_arg x) { return acos(x.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type atan(this_arg y_over_x) { return atan(y_over_x.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type atan(this_arg y, this_arg x) { return atan(y.data, x.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type sinh(this_arg x) { return sinh(x.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type cosh(this_arg x) { return cosh(x.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type tanh(this_arg x) { return tanh(x.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type asinh(this_arg x) { return asinh(x.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type acosh(this_arg x) { return acosh(x.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type atanh(this_arg x) { return atanh(x.data); }

            // 8.2

            CXXSWIZZLE_FORCE_INLINE friend this_type pow(this_arg x, this_arg y) { return pow(x.data, y.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type exp(this_arg x) { return exp(x.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type log(this_arg x) { return log(x.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type exp2(this_arg x) { return exp2(x.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type log2(this_arg x) { return log2(x.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type sqrt(this_arg x) { return sqrt(x.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type inversesqrt(this_arg x) { return inversesqrt(x.data); }

            // 8.3

            CXXSWIZZLE_FORCE_INLINE friend this_type abs(this_arg x) { return abs(x.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type sign(this_arg x) { return sign(x.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type floor(this_arg x) { return floor(x.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type trunc(this_arg x) { return trunc(x.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type round(this_arg x) { return round(x.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type roundEven(this_arg x) { return roundEven(x.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type ceil(this_arg x) { return ceil(x.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type fract(this_arg x) { return fract(x.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type mod(this_arg x, this_arg y) { return mod(x.data, y.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type modf(this_arg x, this_type& i) { return modf(x.data, i.data); }

            CXXSWIZZLE_FORCE_INLINE friend this_type min(this_arg x, this_arg y) { return min(x.data, y.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type max(this_arg x, this_arg y) { return max(x.data, y.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type clamp(this_arg x, this_arg minVal, this_arg maxVal) { return clamp(x.data, minVal.data, maxVal.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type mix(this_arg x, this_arg y, this_arg a) { return mix(x.data, y.data, a.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type mix(this_arg x, this_arg y, const bool_type& a) { return mix(x.data, y.data, a.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type step(this_arg edge, this_arg x) { return step(edge.data, x.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type smoothstep(this_arg edge0, this_arg edge1, this_arg x) { return smoothstep(edge0.data, edge1.data, x.data); }

            CXXSWIZZLE_FORCE_INLINE friend bool_type isnan(this_arg x) { return isnan(x.data); }
            CXXSWIZZLE_FORCE_INLINE friend bool_type isinf(this_arg x) { return isinf(x.data); }

            //genIType floatBitsToInt(genType value)
            //genUType floatBitsToUint(genType value)

            // 8.4
            CXXSWIZZLE_FORCE_INLINE friend this_type length(this_arg x) { return x; }
            CXXSWIZZLE_FORCE_INLINE friend this_type distance(this_arg p0, this_arg p1) { return abs(p0 - p1); }
            CXXSWIZZLE_FORCE_INLINE friend this_type dot(this_arg x, this_arg y) { return x * y; }
            CXXSWIZZLE_FORCE_INLINE friend this_type normalize(this_arg x) { return step(this_type(0.0f), x) * 2.0f - 1.0f; }
            CXXSWIZZLE_FORCE_INLINE friend this_type faceforward(this_arg N, this_arg I, this_arg Nref) { return (step(this_type(0.0f), dot(Nref, I)) * (-2.0f) + 1.0f) * N; }
            CXXSWIZZLE_FORCE_INLINE friend this_type reflect(this_arg N, this_arg I) { return (I - 2.0f * dot(I, N) * N); }
            CXXSWIZZLE_FORCE_INLINE friend this_type refract(this_arg I, this_arg N, this_arg eta)
            {
                this_type k = 1.0 - eta * eta * (1.0 - dot(N, I) * dot(N, I));
                this_type mask = step(this_type(0), k);
                return mask * (eta * I - (eta * dot(N, I) + sqrt(k)) * N);
            }

            // 8.8
            CXXSWIZZLE_FORCE_INLINE friend this_type dFdx(this_arg p) { return dFdx(p.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type dFdy(this_arg p) { return dFdy(p.data); }
            CXXSWIZZLE_FORCE_INLINE friend this_type fwidth(this_arg p)
            {
                return abs(dFdx(p)) + abs(dFdy(p));
            }

        };
    }
}