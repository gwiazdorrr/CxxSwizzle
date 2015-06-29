// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <type_traits>
#include <swizzle/detail/utils.h>
#include <swizzle/detail/vector_traits.h>
#include <swizzle/detail/vector_inout_wrapper.h>

namespace swizzle
{
    namespace detail
    {
        //! A VectorType's proxy, using subscript operators to access components of both the vector and the
        //! DataType. x, y, z & w template args define which components of the vector this proxy uses in place
        //! of its, with -1 meaning "don't use".
        //! The type is convertible to the vector. It also forwards all unary arithmetic operators. Binary
        //! operations hopefully fallback to the vector ones.
        template <class VectorType, class DataType, size_t... indices>
        class indexed_proxy
        {
            //! The data. Must support subscript operator.
            DataType m_data;

        public:
            // Can easily count now since -1 must be continuous
            static const size_t num_of_components = sizeof...(indices);
            static_assert(num_of_components >= 2, "Must be at least 2 components");

            // Is this proxy writable? All indices must be different, except for -1s
            static const bool is_writable = are_unique<indices...>::value;

            // Use the traits to define vector and scalar
            typedef VectorType vector_type;
            typedef VectorType decay_type;
          
        public:

            //! Convert proxy into a vector.
            vector_type decay() const
            {
                vector_type result;
                decay_helper<0, indices...> {result, m_data};
                return result;
            }

        #ifdef CXXSWIZZLE_VECTOR_INOUT_WRAPPER_ENABLED
            //! If enabled, it will decay to a reference wrapper, if needed (inout and out parameters); on destruction
            //! the wrapper will copy its value back into the proxy
            operator const typename std::conditional<is_writable, vector_inout_wrapper<vector_type>, operation_not_available>::type()
            {
                return vector_inout_wrapper<vector_type>(decay(), [this](const vector_type& v) -> void { *this = v; });
            }
        #endif

            //! Auto-decaying where possible.
            operator vector_type()
            {
                return decay();
            }

            //! Auto-decaying where possible.
            operator vector_type() const
            {
                return decay();
            }

            //! Assignment only enabled if proxy is writable -> has unique indexes
            indexed_proxy& operator=(const typename std::conditional<is_writable, vector_type, operation_not_available>::type& vec)
            {
                decay_helper<0, indices...> {vec, m_data};
                return *this;
            }

            //! Forwarding operator. Global non-assignment operators depend on it.
            template <class T>
            indexed_proxy& operator+=(T && o)
            {
                return operator=( decay() + std::forward<T>(o) );
            }

            //! Forwarding operator. Global non-assignment operators depend on it.
            template <class T>
            indexed_proxy& operator-=(T && o)
            {
                return operator=( decay() - std::forward<T>(o) );
            }

            //! Forwarding operator. Global non-assignment operators depend on it.
            template <class T>
            indexed_proxy& operator*=(T && o)
            {
                return operator=( decay() * std::forward<T>(o) );
            }

            //! Forwarding operator. Global non-assignment operators depend on it.
            template <class T>
            indexed_proxy& operator/=(T && o)
            {
                return operator=( decay() / std::forward<T>(o) );
            }

        private:
            //! Helper type, converts vector to or from data
            template <size_t VectorIndex, size_t DataIndex, size_t... DataIndexTail>
            struct decay_helper
            {
                //! Convert data into vector.
                decay_helper(VectorType& vec, const DataType& data)
                {
                    vec[VectorIndex] = data[DataIndex];
                    decay_helper<VectorIndex + 1, DataIndexTail...>(vec, data);
                }
                //! Convert vector into data.
                decay_helper(const VectorType& vec, DataType& data)
                {
                    data[DataIndex] = vec.at(VectorIndex, std::true_type());
                    decay_helper<VectorIndex + 1, DataIndexTail...>(vec, data);
                }
            };

            //! Terminator.
            template <size_t VectorIndex, size_t DataIndex>
            struct decay_helper<VectorIndex, DataIndex>
            {
                decay_helper(VectorType& vec, const DataType& data)
                {
                    vec.at(VectorIndex, std::true_type()) = data[DataIndex];
                }

                decay_helper(const VectorType& vec, DataType& data)
                {
                    data[DataIndex] = vec.at(VectorIndex, std::true_type());
                }
            };
        };


        //! A specialisation for the indexed_proxy, defines TVector as vector type.
        template <class TVector, class TData, size_t... indices>
        struct get_vector_type_impl< indexed_proxy<TVector, TData, indices...> >
        {
            typedef TVector type;
        };
    }
}