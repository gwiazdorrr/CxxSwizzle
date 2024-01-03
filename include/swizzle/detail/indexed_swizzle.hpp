// CxxSwizzle (c) 2013-2021 Piotr Gwiazdowski
#pragma once

#include <type_traits>
#include <swizzle/detail/utils.hpp>
#include <swizzle/detail/vector_traits.hpp>
#include <swizzle/detail/indexed_swizzle_storage.hpp>

namespace swizzle
{
    namespace detail
    {
        // TODO: this comment's rubbish
        //! A TVector's proxy, using subscript operators to access components of both the vector and the
        //! TData. x, y, z & w template args define which components of the vector this proxy uses in place
        //! of its, with -1 meaning "don't use".
        //! The type is convertible to the vector. It also forwards all unary arithmetic operators. Binary
        //! operations hopefully fallback to the vector ones.
        template <typename TVector, typename TData, typename TScalar, size_t... TIndices>
        struct indexed_swizzle : indexed_swizzle_storage<TData, TScalar, sizeof...(TIndices), TIndices...>
        {
            // Can easily count now since -1 must be continuous
            static constexpr size_t num_components = sizeof...(TIndices);
            static_assert(num_components >= 2, "Must be at least 2 components");

            // Is this proxy writable? All TIndices must be different, except for -1s
            static constexpr bool is_writable = are_unique<TIndices...>::value;

            // Use the traits to define vector and scalar
            typedef TVector vector_type;
            typedef TVector decay_type;

            //! Convert proxy into a vector.
            vector_type decay() const
            {
                vector_type result;
                return decay_impl(result, std::make_index_sequence<num_components>{});
 
            }

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

            indexed_swizzle() = default;

            //! Allow only conversion to lexically preceeding proxy; this is to avoid ambiguity in ternary operators (g++/clang)
            template <size_t... OtherTIndices>
            indexed_swizzle(const indexed_swizzle_storage<TData, TScalar, sizeof...(TIndices), OtherTIndices...>& other,
                std::enable_if_t<(compare_sequence(std::index_sequence<TIndices...>{}, std::index_sequence<OtherTIndices...>{})<0) > * = nullptr) {
                assign_impl(other.data, std::index_sequence<OtherTIndices...>{});
            }

            //! Assignment only enabled if proxy is writable -> has unique indexes
            indexed_swizzle& operator=(const vector_type& vec)
            {
                return assign_impl(vec, std::make_index_sequence<num_components>{});
            }

            //! Assignment only enabled if proxy is writable -> has unique indexes
            template <size_t... OtherTIndices>
            indexed_swizzle& operator=(const indexed_swizzle_storage<TData, TScalar, sizeof...(TIndices), OtherTIndices...>& other) {
                return assign_impl(other.data, std::index_sequence<OtherTIndices...>{});
            }

            //! Forwarding operator. Global non-assignment operators depend on it.
            template <typename T>
            indexed_swizzle& operator+=(T && o)
            {
                return operator=(decay() + std::forward<T>(o));
            }

            //! Forwarding operator. Global non-assignment operators depend on it.
            template <typename T>
            indexed_swizzle& operator-=(T && o)
            {
                return operator=(decay() - std::forward<T>(o));
            }

            //! Forwarding operator. Global non-assignment operators depend on it.
            template <typename T>
            indexed_swizzle& operator*=(T && o)
            {
                return operator=(decay() * std::forward<T>(o));
            }

            //! Forwarding operator. Global non-assignment operators depend on it.
            template <typename T>
            indexed_swizzle& operator/=(T && o)
            {
                return operator=(decay() / std::forward<T>(o));
            
            }

            //! Forwarding operator. Global non-assignment operators depend on it.
            template <typename T>
            indexed_swizzle& operator^=(T&& o)
            {
                return operator=(decay() ^ std::forward<T>(o));
            }

            inline vector_type operator-() const
            {
                return -decay();
            }

        private:
            template <size_t... VectorTIndices>
            indexed_swizzle& assign_impl(const vector_type& vec, std::index_sequence<VectorTIndices...>)
            {
                return ((this->data[TIndices] = vec.data[VectorTIndices]), ..., *this);
            }

            template <size_t... VectorTIndices>
            indexed_swizzle& assign_impl(const TData& vec_data, std::index_sequence<VectorTIndices...>)
            {
                return ((this->data[TIndices] = vec_data[VectorTIndices]), ..., *this);
            }

            template <size_t... VectorTIndices>
            vector_type& decay_impl(vector_type& vec, std::index_sequence<VectorTIndices...>) const
            {
                return ((vec.data[VectorTIndices] = this->data[TIndices]), ..., vec);
            }
        };


        // TODO: incosnistent
        //! A specialisation for the indexed_swizzle, defines TVector as vector type.
        template <class TVector, class TData, class TScalar, size_t... TIndices>
        struct get_vector_type_impl< indexed_swizzle<TVector, TData, TScalar, TIndices...> >
        {
            typedef TVector type;
        };
    }
}