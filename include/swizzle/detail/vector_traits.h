// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <type_traits>
#include <swizzle/detail/utils.h>
#include <swizzle/detail/fwd.h>

namespace swizzle
{
    namespace detail
    {
        //! Type to specialise; it should define a nested type 'type' being a vector
        //! Non-specialised version does not define it, failing any function relying on it.
        template <class T>
        struct get_vector_type_impl
        {};

        template <class T>
        struct default_vector_type_impl
        {
            typedef swizzle::vector<T, 1> type;
        };

        //! Used for graceful SFINAE - becomes true_type if get_vector_type_impl defines nested type.
        template <class T>
        struct has_vector_type_impl : std::integral_constant<bool, has_type< get_vector_type_impl< typename remove_reference_cv<T>::type > >::value >
        {};



        //! A generic common vector type implementation. Defines a type if:
        //! 1) scalars have a common type and it must be either one of them
        //! 2) Sizes are equal
        //! This type is specialised to handle cases of 1-sized vectors.
        template <class VectorType1, class ScalarType1, size_t Size1, class VectorType2, class ScalarType2, size_t Size2>
        struct common_vector_type_generic_fallback
        {
        private:
            typedef typename std::common_type<ScalarType1, ScalarType2>::type common_scalar_type;
            static_assert( std::is_same<common_scalar_type, ScalarType1>::value || std::is_same<common_scalar_type, ScalarType2>::value, 
                "Common type must be same as at least one of scalar types");
            static_assert( Size1 == Size2 || Size1 == 1 || Size2 == 1,
                "Either both vectors must have same size or either one of them has to have a size equal to 1 (auto promotion to the bigger vector)");

        public:
            typedef typename std::conditional< Size1 == Size2,
                typename std::conditional< std::is_same<common_scalar_type, ScalarType1>::value,
                    VectorType1,
                    VectorType2
                >::type,
                typename std::conditional< Size2 == 1,
                    VectorType1,
                    VectorType2
                >::type
            >::type type;
        };



        //! Defines a common type for two vectors. By default falls back to common_vector_type_generic_fallback,
        //! but can be easily specialised to handle vectors differently.
        template <class VectorType1, class VectorType2>
        struct common_vector_type : common_vector_type_generic_fallback<
            VectorType1, typename VectorType1::scalar_type, VectorType1::num_of_components, 
            VectorType2, typename VectorType2::scalar_type, VectorType2::num_of_components>
        {};



        //! Defines common vector type for given combination of input types.Scalars are treated as one-component vector.
        template <class T, class... U>
        struct get_vector_type
            : std::conditional<
                has_vector_type_impl<T>::value,
                common_vector_type< typename get_vector_type<T>::type, typename get_vector_type<U...>::type >,
                nothing
            >::type
        {};

        //! A specialisation for one type; redirects to get_vector_type_impl<T>
        template <class T>
        struct get_vector_type<T> 
            : std::conditional<
                has_vector_type_impl<T>::value,
                get_vector_type_impl<typename remove_reference_cv<T>::type>,
                nothing
            >::type
        {};


        template <class T>
        struct get_batch_size : std::integral_constant<size_t, 1> {};

        //! A shortcut for getting the number of vector's components.
        template <class T, class = std::true_type >
        struct get_vector_size;

        //! 
        template <class T>
        struct get_vector_size<T, std::integral_constant<bool, is_greater<get_vector_type<T>::type::num_of_components, 0>::value> >
            :  std::integral_constant< size_t, get_vector_type<T>::type::num_of_components >
        {};

        struct one_of_the_types_does_not_have_get_vector_type_impl
        {};

        //! To get SFINAE-usable total vector size, we need to introduce helper type, or else g++ will fall on it's face.
        template <bool DoAllHaveVectorSizes, class... T>
        struct get_total_component_count_helper {
            static const one_of_the_types_does_not_have_get_vector_type_impl value;
        };

        template <class... T>
        struct get_total_component_count_helper<true, T...> 
            : std::integral_constant<size_t, accumulate<get_vector_size<T>::value...>::value>{};

        //! Only expand get_vector_size if *all* types have vector type impl defined.
        template <class... T>
        struct get_total_component_count 
            : get_total_component_count_helper< 
                all<has_vector_type_impl, T...>::value, 
                T...
            > 
        {};

        template <class... T>
        constexpr size_t get_total_component_count_v = get_total_component_count<T...>::value;

        template <typename StorageType, size_t Align, size_t Size, typename BoolType, bool IsBool, bool IsIntegral, bool IsFloatingPoint>
        struct batch_traits_builder
        {
            using bool_type = BoolType;
            using storage_type = StorageType;
            static const bool is_bool = IsBool;
            static const bool is_integral = IsIntegral;
            static const bool is_floating_point = IsFloatingPoint;
            static const size_t size = Size;
            static const size_t align = Align;
        };

        template <typename T>
        struct batch_traits : batch_traits_builder<
            T,
            alignof(T), 
            sizeof(T), 
            bool,
            std::is_same_v<T, bool>,
            std::is_integral_v<T> && !std::is_same_v<T, bool>,
            std::is_floating_point_v<T>
        >
        {};
        
    }
}