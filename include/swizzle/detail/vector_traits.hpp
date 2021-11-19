// CxxSwizzle (c) 2013-2021 Piotr Gwiazdowski
#pragma once

#include <type_traits>
#include <swizzle/detail/utils.hpp>
#include <swizzle/detail/fwd.hpp>

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
            using type = swizzle::vector_<T, 0>;
        };

        //! Used for graceful SFINAE - becomes true_type if get_vector_type_impl defines nested type.
        template <class T>
        struct has_vector_type_impl : std::bool_constant< has_type_v< get_vector_type_impl< remove_reference_cv_t<T> > > >
        {};


        //! A generic common vector type implementation. Defines a type if:
        //! 1) scalars have a common type and it must be either one of them
        //! 2) Sizes are equal
        //! This type is specialised to handle cases of 1-sized vectors.
        template <class TVector1, class TScalar1, size_t TSize1, class TVector2, class TScalar2, size_t TSize2>
        struct common_vector_type_generic_fallback
        {
        private:
            using common_scalar_type = std::common_type_t<TScalar1, TScalar2>;
            static_assert( std::is_same_v<common_scalar_type, TScalar1> || std::is_same_v<common_scalar_type, TScalar2>, 
                "Common type must be same as at least one of scalar types");
            static_assert( TSize1 == TSize2 || TSize1 == 1 || TSize2 == 1,
                "Either both vectors must have same size or either one of them has to have a size equal to 1 (auto promotion to the bigger vector)");

        public:
            using type = std::conditional_t<TSize1 == TSize2,
                std::conditional_t< std::is_same_v<common_scalar_type, TScalar1>, TVector1, TVector2>,
                std::conditional_t< TSize2 == 1, TVector1, TVector2>
            >;
        };



        //! Defines a common type for two vectors. By default falls back to common_vector_type_generic_fallback,
        //! but can be easily specialised to handle vectors differently.
        template <class TVector1, class TVector2>
        struct common_vector_type : common_vector_type_generic_fallback<
            TVector1, typename TVector1::scalar_type, TVector1::num_components, 
            TVector2, typename TVector2::scalar_type, TVector2::num_components>
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
        struct get_vector_size<T, std::integral_constant<bool, is_greater<get_vector_type<T>::type::num_components, 0>::value> >
            :  std::integral_constant< size_t, get_vector_type<T>::type::num_components >
        {};

        struct one_of_the_types_does_not_have_get_vector_type_impl
        {};

        //! To get SFINAE-usable total vector size, we need to introduce helper type, or else g++ will fall on it's face.
        template <bool DoAllHaveVectorSizes, class... T>
        struct get_total_component_count_helper {
            static constexpr one_of_the_types_does_not_have_get_vector_type_impl value = {};
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

        template <typename TScalarType>
        struct scalar_type_storage
        {
            using type = TScalarType;
        };

        template <typename TScalarType>
        struct scalar_type_align : std::integral_constant<size_t, alignof(TScalarType)> {};

        template <typename TScalarTypesInfo, typename T, typename = std::enable_if_t<TScalarTypesInfo::template is_scalar_type<T>::value> >
        struct scalar_traits_builder
        {
            using scalar_types = TScalarTypesInfo;
            
            static constexpr bool is_floating_point = scalar_types::template is_floating_point<T>::value;
            static constexpr bool is_bool           = scalar_types::template is_bool<T>::value;
            static constexpr bool is_integral       = scalar_types::template is_integral<T>::value;
            static constexpr bool is_unsigned       = scalar_types::template is_unsigned<T>::value;

            static constexpr size_t size        = scalar_types::size;
            static constexpr size_t num_rows    = scalar_types::num_rows;
            static constexpr size_t num_columns = scalar_types::num_columns;
            static constexpr size_t align       = scalar_type_align<T>::value;

            using storage_type         = conditional_array_t<typename scalar_type_storage<T>::type, scalar_types::batch_count>;
            using primitive_type       = typename scalar_types::template primitive_type_t<T>;
            using aligned_storage_type = std::aligned_storage_t<sizeof(storage_type), align>;
        };

        template <typename TFloat, typename TInt, typename TUInt, typename TBool, size_t TBatchSize = 1, size_t TBatchCount = 1>
        struct scalar_types_info_builder
        {
            using float_type = TFloat;
            using int_type   = TInt;
            using uint_type  = TUInt;
            using bool_type  = TBool;

            static constexpr size_t batch_count = TBatchCount;
            static constexpr size_t batch_size  = TBatchSize;
            static constexpr size_t size        = TBatchSize * batch_count;
            static constexpr size_t num_rows    = size > 1 ? 2 : 1;
            static constexpr size_t num_columns = size > 1 ? size / 2 : 1;
            static_assert(size == 1 || size % 2 == 0, "1 or even scalar count");
            
            template <typename T>
            using primitive_type_t = 
                std::conditional_t< std::is_same_v<T, float_type>, float,
                    std::conditional_t< std::is_same_v<T, int_type>, int,
                        std::conditional_t< std::is_same_v<T, uint_type>, unsigned, 
                            std::conditional_t<std::is_same_v<T, bool_type>, bool, void> 
                        >
                    >
                >;

            template <typename T>
            using is_scalar_type = std::integral_constant<bool, std::is_same_v<T, float_type> || std::is_same_v<T, int_type> || std::is_same_v<T, uint_type> || std::is_same_v<T, bool_type>>;
        
            template <typename T>
            using is_integral = std::bool_constant<std::is_same_v<T, int_type> || std::is_same_v<T, uint_type>>;

            template <typename T>
            using is_unsigned = std::is_same<T, uint_type>;

            template <typename T>
            using is_floating_point = std::is_same<T, float_type>;

            template <typename T>
            using is_bool = std::is_same<T, bool_type>;
        };

        // default scalar types need to be extended to handle all fundamental C++ types
        struct default_scalar_types : scalar_types_info_builder<float, int32_t, uint32_t, bool>
        {
            template <typename T>
            using is_scalar_type = std::integral_constant<bool, std::is_arithmetic_v<T> || std::is_same_v<bool, T>>;

            template <typename T>
            using is_integral = std::bool_constant<std::is_integral_v<T> && !std::is_same_v<bool, T>>;

            template <typename T>
            using is_unsigned = std::is_unsigned<T>;

            template <typename T>
            using is_floating_point = std::is_floating_point<T>;

            template <typename T>
            using is_bool = std::is_same<bool, T>;
        };
        
        template <typename T>
        struct scalar_traits : scalar_traits_builder<default_scalar_types, T> {};
        
    }
}