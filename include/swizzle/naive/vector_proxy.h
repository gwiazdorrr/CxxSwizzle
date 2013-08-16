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

            template <size_t Index1, size_t Index2>
            static typename std::enable_if<Index1 == -1 || Index2 == -1, void>::type set_component(scalar_type* a, const scalar_type* b, std::integral_constant<size_t, Index1>* = nullptr, std::integral_constant<size_t, Index2>* = nullptr)
            {
                // do nothing
            }

            template <size_t Index1, size_t Index2>
            static typename std::enable_if<Index1 != -1 && Index2 != -1, void>::type set_component(scalar_type* a, const scalar_type* b, std::integral_constant<size_t, Index1>* = nullptr, std::integral_constant<size_t, Index2>* = nullptr)
            {
                a[Index1] = b[Index2];
            }


            operator vector_type() const
            {
                vector_type result;
                set_component<0, x>(result._components, data);
                set_component<1, y>(result._components, data);
                set_component<2, z>(result._components, data);
                set_component<3, w>(result._components, data);
                return result;
            }

            vector_proxy& operator=(const typename std::conditional<is_writable, vector_type, swizzle::detail::operation_not_available >::type& vec)
            {
                set_component<x, 0>(data, result._components);
                set_component<y, 1>(data, result._components);
                set_component<z, 2>(data, result._components);
                set_component<w, 3>(data, result._components);
                return *this;
            }
        };
    }
}