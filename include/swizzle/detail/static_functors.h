// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

namespace swizzle
{
    namespace detail
    {
        template <typename VectorType>
        struct functor_assign
        {
            template <size_t i> inline void operator()(VectorType& result, const VectorType& other)
            {
                operator()<i>(result, other.at(i));
            }

            template <size_t i> inline void operator()(VectorType& result, const typename VectorType::scalar_type& other)
            {
                result.at(i) = other;
            }
        };

        template <typename VectorType>
        struct functor_add
        {
            template <size_t i> inline void operator()(VectorType& result, const VectorType& other)
            {
                operator()<i>(result, other.at(i));
            }

            template <size_t i> inline void operator()(VectorType& result, const typename VectorType::scalar_type& other)
            {
                result.at(i) += other;
            }
        };

        template <typename VectorType>
        struct functor_sub
        {
            template <size_t i> void operator()(VectorType& result, const VectorType& other)
            {
                operator()<i>(result, other.at(i));
            }

            template <size_t i> void operator()(VectorType& result, const typename VectorType::scalar_type& other)
            {
                result.at(i) -= other;
            }
        };

        template <typename VectorType>
        struct functor_mul
        {
            template <size_t i> void operator()(VectorType& result, const VectorType& other)
            {
                operator()<i>(result, other.at(i));
            }

            template <size_t i> void operator()(VectorType& result, const typename VectorType::scalar_type& other)
            {
                result.at(i) *= other;
            }
        };

        template <typename VectorType>
        struct functor_div
        {
            template <size_t i> void operator()(VectorType& result, const VectorType& other)
            {
                operator()<i>(result, other.at(i));
            }

            template <size_t i> void operator()(VectorType& result, const typename VectorType::scalar_type& other)
            {
                result.at(i) /= other;
            }
        };

        template <typename VectorType>
        struct functor_neg
        {
            template <size_t i> void operator()(VectorType& result, const VectorType& other)
            {
                operator()<i>(result, other.at(i));
            }

            template <size_t i> void operator()(VectorType& result, const typename VectorType::scalar_type& other)
            {
                result.at(i) = -other;
            }
        };

        template <typename VectorType>
        struct functor_equals
        {
            template <size_t i> void operator()(const VectorType& a, const VectorType& b, typename VectorType::bool_type& result)
            {
                result &= (a.at(i) == b.at(i));
            }
        };

        template <typename VectorType, size_t offset, typename OtherVectorType>
        struct functor_compose_from_other_vector
        {
            template <size_t i> void operator()(VectorType& result, const OtherVectorType& other)
            {
                result.at(i) = other.at(i - offset);
            }
        };
    }


}