// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <type_traits>
#include <swizzle/detail/utils.h>
#include <swizzle/detail/vector_traits.h>
#include <swizzle/detail/indexed_proxy.hpp>

namespace swizzle
{
    //! A wrapper for a vector reference. Makes inout and out parameters work with proxies, but at a cost
    //! of std::function call (and likely an alloc, too).
    template <class VectorType>
    struct inout_wrapper : public detail::clean_up_before_other_destructors<inout_wrapper<VectorType>>, public VectorType
    {
        typedef VectorType vector_type;
        using this_type = inout_wrapper;

        inout_wrapper(vector_type& value)
            : vector_type(value)
        {
            this->cleanup = [&value](this_type& v) -> void 
            { 
                value = v; 
            };
        }

        template <typename SomeVectorType, typename SomeDataType, typename SomeScalarType, size_t... Index>
        inout_wrapper(detail::indexed_proxy<SomeVectorType, SomeDataType, SomeScalarType, Index...>& value,
            std::enable_if_t<sizeof...(Index) == vector_type::num_of_components && std::is_same_v<SomeScalarType, typename vector_type::scalar_type>, bool> = false)
            : vector_type(value.decay())
        {
            this->cleanup = [&value](this_type& v) -> void
            { 
                value = v; 
            };
        }

        inout_wrapper(inout_wrapper& value)
            : vector_type(value)
        {
            this->cleanup = [&value](this_type& v) -> void
            { 
                value = v;
            };
        }

        inout_wrapper(const inout_wrapper&) = delete;

        this_type& operator=(const vector_type& other)
        {
            vector_type::operator=(other);
            return *this;
        }

        this_type& operator=(const this_type& other)
        {
            vector_type::operator=(other);
            return *this;
        }
    };

    namespace detail
    {
        //! Make this type usable in ADL-magic
        template <class TVector>
        struct get_vector_type_impl< inout_wrapper<TVector> >
        {
            typedef TVector type;
        };
    }
}