// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <cmath>
#include <swizzle/detail/utils.h>

namespace swizzle
{
    namespace detail
    {
        template <typename InternalType>
        struct bool_wrapper
        {
            typedef InternalType internal_type;
            typedef const bool_wrapper& this_arg;
            typedef bool_wrapper this_type;

        private:
            internal_type data;

        public:
            bool_wrapper()
            {}

            bool_wrapper(const internal_type& data)
                : data(data)
            {}

            template <typename T>
            bool_wrapper(T && data, std::enable_if_t< std::is_convertible_v<T, internal_type> >* = nullptr)
                : data(std::forward<T>(data))
            {}
            
            inline friend this_type operator||(this_arg a, this_arg b)
            {
                return a.data || b.data;
            }
            inline friend this_type operator&&(this_arg a, this_arg b)
            {
                return a.data && b.data;
            }

            //! To avoid ADL-hell, cast is explict.
            inline explicit operator internal_type() const
            {
                return data;
            }
            //! To avoid ADL-hell, cast is explict.
            inline operator bool() const
            {
                return data.isNotEmpty();
            }

        };

        template <typename InternalType, typename ExternalType, typename BoolType = bool, typename AssignPolicy = nothing>
        class floating_point_wrapper
        {
        public:
            typedef InternalType internal_type;
            typedef ExternalType external_type;
            typedef floating_point_wrapper this_type;
            typedef AssignPolicy assign_policy_type;
            typedef BoolType bool_type;

            typedef const floating_point_wrapper& this_arg;
            typedef const external_type& external_type_arg;

        private:
            internal_type data;

        public:
            floating_point_wrapper()
            {}

            floating_point_wrapper(const internal_type& data)
                : data(data)
            { }

            floating_point_wrapper(float data)
                : data(static_cast<internal_type>(data))
            {}

            floating_point_wrapper(double data)
                : data(static_cast<internal_type>(data))
            {}

            // functions

            

            // unary operators

            this_type operator-() const
            {
                return -data;
            }

            this_type& operator+=(this_arg other)
            {
                return *this = *this + other;
            }
            this_type& operator-=(this_arg other)
            {
                return *this = *this - other;
            }
            this_type& operator*=(this_arg other)
            {
                return *this = *this * other;
            }
            this_type& operator/=(this_arg other)
            {
                return *this = *this / other;
            }

            // binary operators

            inline friend this_type operator+(this_arg a, this_arg b)
            {
                return a.data + b.data;
            }
            inline friend this_type operator-(this_arg a, this_arg b)
            {
                return a.data - b.data;
            }
            inline friend this_type operator*(this_arg a, this_arg b)
            {
                return a.data * b.data;
            }
            inline friend this_type operator/(this_arg a, this_arg b)
            {
                return a.data / b.data;
            }

            // casts

            //! To avoid ADL-hell, cast is explict.
            inline explicit operator internal_type() const
            {
                return data;
            }

            // assignment

            inline this_type& operator=(this_arg other)
            {
                // this is where the masking magic may happen
                assign(other, std::is_same<assign_policy_type, nothing>());
                return *this;
            }

            inline this_type& operator=(const bool_type& other)
            {
                // this is where the masking magic may happen
                swizzle::detail::convert(other, *this);
                return *this;
            }

            // comparisons

            inline friend bool_type operator>(this_arg a, this_arg b)
            {
                return a.data > b.data;
            }
            inline friend bool_type operator>=(this_arg a, this_arg b)
            {
                return a.data >= b.data;
            }
            inline friend bool_type operator<(this_arg a, this_arg b)
            {
                return a.data < b.data;
            }
            inline friend bool_type operator<=(this_arg a, this_arg b)
            {
                return a.data <= b.data;
            }
            inline friend bool_type operator==(this_arg a, this_arg b)
            {
                return a.data == b.data;
            }
            inline friend bool_type operator!=(this_arg a, this_arg b)
            {
                return a.data != b.data;
            }

            // for CxxSwizzle ADL-magic

            this_type decay() const
            {
                return *this;
            }

        private:
            inline void assign(this_arg other, std::false_type)
            {
                assign_policy_type::assign(data, other.data);
            }

            inline void assign(this_arg other, std::true_type)
            {
                data = other.data;
            }

        public:

            // functions
            // 8.1

            inline friend this_type radians(this_arg degrees) { return radians(degrees.data); }
            inline friend this_type degrees(this_arg radians) { return degrees(radians.data); }
            inline friend this_type cos(this_arg angle) { return cos(angle.data); }
            inline friend this_type tan(this_arg angle) { return tan(angle.data); }
            inline friend this_type sin(this_arg angle) { return sin(angle.data); }
            inline friend this_type asin(this_arg x) { return asin(x.data); }
            inline friend this_type acos(this_arg x) { return acos(x.data); }
            inline friend this_type atan(this_arg y_over_x) { return atan(y_over_x.data); }
            inline friend this_type atan(this_arg y, this_arg x) { return atan(y.data, x.data); }
            inline friend this_type sinh(this_arg x) { return sinh(x.data); }
            inline friend this_type cosh(this_arg x) { return cosh(x.data); }
            inline friend this_type tanh(this_arg x) { return tanh(x.data); }
            inline friend this_type asinh(this_arg x) { return asinh(x.data); }
            inline friend this_type acosh(this_arg x) { return acosh(x.data); }
            inline friend this_type atanh(this_arg x) { return atanh(x.data); }

            // 8.2

            inline friend this_type pow(this_arg x, this_arg y) { return pow(x.data, y.data); }
            inline friend this_type exp(this_arg x) { return exp(x.data); }
            inline friend this_type log(this_arg x) { return log(x.data); }
            inline friend this_type exp2(this_arg x) { return exp2(x.data); }
            inline friend this_type log2(this_arg x) { return log2(x.data); }
            inline friend this_type sqrt(this_arg x) { return sqrt(x.data); }
            inline friend this_type inversesqrt(this_arg x) { return inversesqrt(x.data); }

            // 8.3

            inline friend this_type abs(this_arg x) { return abs(x.data); }
            inline friend this_type sign(this_arg x) { return sign(x.data); }
            inline friend this_type floor(this_arg x) { return floor(x.data); }
            inline friend this_type trunc(this_arg x) { return trunc(x.data); }
            inline friend this_type round(this_arg x) { return round(x.data); }
            inline friend this_type roundEven(this_arg x) { return roundEven(x.data); }
            inline friend this_type ceil(this_arg x) { return ceil(x.data); }
            inline friend this_type fract(this_arg x) { return fract(x.data); }
            inline friend this_type mod(this_arg x, this_arg y) { return mod(x.data, y.data); }
            inline friend this_type modf(this_arg x, this_type& i) { return modf(x.data, i.data); }

            inline friend this_type min(this_arg x, this_arg y) { return min(x.data, y.data); }
            inline friend this_type max(this_arg x, this_arg y) { return max(x.data, y.data); }
            inline friend this_type clamp(this_arg x, this_arg minVal, this_arg maxVal) { return clamp(x.data, minVal.data, maxVal.data); }
            inline friend this_type mix(this_arg x, this_arg y, this_arg a) { return mix(x.data, y.data, a.data); }
            // inline friend this_type mix(this_arg x, this_arg y, genBType_arg a) { return mix(x.data, y.data, a.data); }
            inline friend this_type step(this_arg edge, this_arg x) { return step(edge.data, x.data); }
            inline friend this_type smoothstep(this_arg edge0, this_arg edge1, this_arg x) { return smoothstep(edge0.data, edge1.data, x.data); }

            //genBType isnan(genType x)
            //genBType isinf(genType x)

            //genIType floatBitsToInt(genType value)
            //genUType floatBitsToUint(genType value)

            //// 8.4
            //float length(genType x)
            //float distance(genType p0, genType p1)
            //float dot(genType x, genType y)
            //inline friend this_type normalize(this_arg x) { return normalize(x.data); }
            //inline friend this_type faceforward(this_arg N, this_arg I, this_arg Nref) { return faceforward(N.data, I.data, Nref.data); }
            //inline friend this_type reflect(this_arg N, this_arg I) { return reflect(N.data, I.data); }
            //inline friend this_type refract(this_arg N, this_arg I, float_arg Nref) { return refract(N.data, I.data, Nref.data); }

            // 8.8
            inline friend this_type dFdx(this_arg p) { return dFdx(p.data); }
            inline friend this_type dFdy(this_arg p) { return dFdy(p.data); }
            inline friend this_type fwidth(this_arg p) { return fwidth(p.data); }

        };
    }
}