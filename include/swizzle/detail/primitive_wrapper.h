// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <cmath>
#include <swizzle/detail/utils.h>

namespace swizzle
{
    namespace detail
    {
        template <typename InternalType, typename ExternalType, typename BoolType = bool, typename AssignPolicy = nothing>
        class primitive_wrapper
        {
        public:
            typedef InternalType internal_type;
            typedef ExternalType external_type;
            typedef primitive_wrapper this_type;
            typedef AssignPolicy assign_policy_type;
            typedef BoolType bool_type;

            typedef const primitive_wrapper& this_arg;
            typedef const external_type& external_type_arg;

        private:
            internal_type data;

        public:
            primitive_wrapper()
            {}

            primitive_wrapper(const internal_type& data)
                : data(data)
            { }

            template <typename T>
            primitive_wrapper(T && data, typename std::enable_if< std::is_convertible<T, internal_type>::value >::type* = nullptr)
                : data(std::forward<T>(data))
            { }

            primitive_wrapper(external_type_arg data)
                : data(data)
            {}

            // functions

            inline friend this_type sin(this_arg x)
            {
                return sin(x.data);
            }
            inline friend this_type cos(this_arg x)
            {
                return cos(x.data);
            }
            inline friend this_type tan(this_arg x)
            {
                return tan(x.data);
            }
            inline friend this_type asin(this_arg x)
            {
                return asin(x.data);
            }
            inline friend this_type acos(this_arg x)
            {
                return acos(x.data);
            }
            inline friend this_type atan(this_arg x)
            {
                return atan(x.data);
            }
            inline friend this_type atan2(this_arg y, this_arg x)
            {
                return atan2(y.data, x.data);
            }

            inline friend this_type abs(this_arg x)
            {
                return abs(x.data);
            }
            inline friend this_type pow(this_arg x, this_arg n)
            {
                return pow(x.data, n.data);
            }
            inline friend this_type exp(this_arg x)
            {
                return exp(x.data);
            }
            inline friend this_type log(this_arg x)
            {
                return log(x.data);
            }
            inline friend this_type exp2(this_arg x)
            {
                return exp2(x.data);
            }
            inline friend this_type log2(this_arg x)
            {
                return log2(x.data);
            }
            inline friend this_type sqrt(this_arg x)
            {
                return sqrt(x.data);
            }
            inline friend this_type rsqrt(this_arg x)
            {
                return rsqrt(x.data);
            }

            inline friend this_type sign(this_arg x)
            {
                return sign(x.data);
            }
            inline friend this_type fract(this_arg x)
            {
                return fract(x.data);
            }
            inline friend this_type floor(this_arg x)
            {
                return floor(x.data);
            }
            inline friend this_type ceil(this_arg x)
            {
                return ceil(x.data);
            }
            inline friend this_type mod(this_arg x, this_arg y)
            {
                return mod(x.data, y.data);
            }

            inline friend this_type min(this_arg x, this_arg y)
            {
                return min(x.data, y.data);
            }
            inline friend this_type max(this_arg x, this_arg y)
            {
                return max(x.data, y.data);
            }

            inline friend this_type step(this_arg edge, this_arg x)
            {
                return step(edge.data, x.data);
            }

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

            this_type& operator+=(external_type_arg other)
            {
                return *this = *this + other;
            }
            this_type& operator-=(external_type_arg other)
            {
                return *this = *this - other;
            }
            this_type& operator*=(external_type_arg other)
            {
                return *this = *this * other;
            }
            this_type& operator/=(external_type_arg other)
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

            inline friend this_type operator+(this_arg a, external_type_arg b)
            {
                return a.data + b;
            }
            inline friend this_type operator+(external_type_arg a, this_arg b)
            {
                return b + a;
            }
            inline friend this_type operator-(this_arg a, external_type_arg b)
            {
                return a.data - b;
            }
            inline friend this_type operator-(external_type_arg a, this_arg b)
            {
                return internal_type(a) - b.data;
            }
            inline friend this_type operator*(this_arg a, external_type_arg b)
            {
                return a.data * b;
            }
            inline friend this_type operator*(external_type_arg a, this_arg b)
            {
                return b * a;
            }
            inline friend this_type operator/(this_arg a, external_type_arg b)
            {
                return a.data / b;
            }
            inline friend this_type operator/(external_type_arg a, this_arg b)
            {
                return internal_type(a) / b.data;
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
        };
    }
}