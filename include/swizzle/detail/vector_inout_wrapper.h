// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <type_traits>
#include <swizzle/detail/utils.h>
#include <swizzle/detail/vector_traits.h>
#include <functional>

namespace swizzle
{
    namespace detail
    {
        //! A wrapper for a vector reference. Makes inout and out parameters work with proxies, but at a cost
        //! of std::function call (and likely an alloc, too).
        template <class VectorType>
        struct vector_inout_wrapper : public VectorType
        {
            typedef VectorType vector_type;

        private:
            std::function<void(const vector_type&)> m_cleanup;

        public:

            vector_inout_wrapper(vector_type& value)
                : vector_type(value)
            {
                m_cleanup = [&](const vector_type& v) -> void { value = v; };
            }

            vector_inout_wrapper(const vector_type& value, std::function<void(const vector_type&)> cleanup)
                : vector_type(value)
                , m_cleanup(cleanup)
            {
            }

            ~vector_inout_wrapper()
            {
                if (m_cleanup)
                {
                    m_cleanup(*this);
                }
            }
        };

        template <class TVector>
        struct get_vector_type_impl< vector_inout_wrapper<TVector> >
        {
            typedef TVector type;
        };
    }
}