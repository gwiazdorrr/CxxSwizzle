#pragma once

#include <type_traits>

namespace swizzle
{
    namespace naive
    {
        template <template <class> class TVector, class TTraits, size_t x, size_t y = -1, size_t z = -1, size_t w = -1>
        struct vector_proxy : private TTraits::tag_type
        {
            // Make sure -1 are always trailing, not in the middle
            static_assert ( (x!=-1) && (y!=-1 || (z==-1 && w==-1)) && (z!=-1 || w==-1 ), "Must be continuous" );

            // Can easily count now since -1 must be continuous
            static const size_t num_of_components = (x!=-1) + (y!=-1) + (z!=-1) + (w!=-1);

            // Is this proxy writable? All indices must be different, except for -1s
            static const bool is_writable = 
                (x != y  && x != z && x != w) && 
                (y == -1 || y != z && y != w) && 
                (z == -1 || z != w );

            // Change the number of components for the vector
            typedef typename TTraits::template change_num_of_components<num_of_components>::type traits_type;

            // Use the traits to define vector and scalar
            typedef TVector< traits_type > vector_type;
            typedef typename traits_type::scalar_type scalar_type;

            //! The data.
            scalar_type data[TTraits::num_of_components];

            template <size_t Index1, size_t Index2, class A, class B>
            static typename std::enable_if<Index1 == -1 || Index2 == -1, void>::type set_component(A& a, const B& b, std::integral_constant<size_t, Index1>* = nullptr, std::integral_constant<size_t, Index2>* = nullptr)
            {
                // do nothing
            }

            template <size_t Index1, size_t Index2, class A, class B>
            static typename std::enable_if<Index1 != -1 && Index2 != -1, void>::type set_component(A& a, const B& b, std::integral_constant<size_t, Index1>* = nullptr, std::integral_constant<size_t, Index2>* = nullptr)
            {
                a[Index1] = b[Index2];
            }


            operator vector_type() const
            {
                return swizzle(data);
            }

            vector_proxy& operator=(const typename std::conditional<is_writable, vector_type, swizzle::detail::operation_not_available >::type& vec)
            {
                auto swizzled = swizzle(vec);
                detail::compile_time_for<0, num_of_components>([&](size_t i) -> void { data[i] = swizzled[i]; } );


                //set_component<x, 0>(data, vec);
                //set_component<y, 1>(data, vec);
                //set_component<z, 2>(data, vec);
                //set_component<w, 3>(data, vec);
                return *this;
            }

            template <class A>
            vector_type swizzle(const A& a) const
            {
                vector_type result;
                set_component<0, x>(result, a);
                set_component<1, y>(result, a);
                set_component<2, z>(result, a);
                set_component<3, w>(result, a);
                return result;
            }

            template <class T>
            vector_proxy& operator+=(T && o)
            {
                return operator=( operator vector_type() + std::forward<T>(o) );
            }

            template <class T>
            vector_proxy& operator-=(T && o)
            {
                return operator=( operator vector_type() - std::forward<T>(o) );
            }

            template <class T>
            vector_proxy& operator*=(T && o)
            {
                return operator=( operator vector_type() * std::forward<T>(o) );
            }

            template <class T>
            vector_proxy& operator/=(T && o)
            {
                return operator=( operator vector_type() / std::forward<T>(o) );
            }
        };
    }
}