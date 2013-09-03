#pragma once

#include <type_traits>
#include <swizzle/detail/vector_traits.h>

namespace swizzle
{
    namespace naive
    {
        template <class TScalar, size_t Size>
        class vector_adapter;

        template <class TVector, class TData, class TTag, size_t x, size_t y, size_t z, size_t w>
        class indexed_proxy;
    }

    namespace detail
    {
        template <class T>
        struct get_vector_type_impl_for_scalar
        {
            typedef naive::vector_adapter< T, 1 > type;
        };

        template <>
        struct get_vector_type_impl<float> : get_vector_type_impl_for_scalar<float>
        {};

        template <>
        struct get_vector_type_impl<double> : get_vector_type_impl_for_scalar<double>
        {};

        template <>
        struct get_vector_type_impl<int> : get_vector_type_impl_for_scalar<int>
        {};

        template <>
        struct get_vector_type_impl<unsigned short> : get_vector_type_impl_for_scalar<unsigned short>
        {};

        template <class TScalar, size_t Size>
        struct get_vector_type_impl< naive::vector_adapter<TScalar, Size> >
        {
            typedef naive::vector_adapter<TScalar, Size> type;
        };

        template <class TVector, class TData, class TTag, size_t x, size_t y, size_t z, size_t w>
        struct get_vector_type_impl< naive::indexed_proxy<TVector, TData, TTag, x, y, z, w> >
        {
            typedef TVector type;
        };
/*
        template <class TScalar, size_t Size>
        struct common_vector_type< naive::vector_adapter<TScalar, Size>, naive::vector_adapter<TScalar, Size> >
        {
            typedef naive::vector_adapter<TScalar, Size> type;
        };

        template <class TScalar1, class TScalar2, size_t Size>
        struct common_vector_type< naive::vector_adapter<TScalar1, Size>, naive::vector_adapter<TScalar2, 1> >
        {
            typedef naive::vector_adapter<TScalar1, Size> type;
        };

        template <class TScalar1, class TScalar2, size_t Size>
        struct common_vector_type< naive::vector_adapter<TScalar1, 1>, naive::vector_adapter<TScalar2, Size> >
        {
            typedef naive::vector_adapter<TScalar2, Size> type;
        };

        template <class TCommonScalar, class TScalar1, class TScalar2, size_t Size> 
        struct common_vector_type_helper
        {};
        template <class TScalar1, class TScalar2, size_t Size> 
        struct common_vector_type_helper<TScalar1, TScalar1, TScalar2, Size>
        {
            typedef naive::vector_adapter<TScalar1, Size> type;
        };
        template <class TScalar1, class TScalar2, size_t Size> 
        struct common_vector_type_helper<TScalar2, TScalar1, TScalar2, Size>
        {
            typedef naive::vector_adapter<TScalar2, Size> type;
        };



        template <class TScalar1, class TScalar2, size_t Size>
        struct common_vector_type< naive::vector_adapter<TScalar1, Size>, naive::vector_adapter<TScalar2, Size> >
            : common_vector_type_helper< typename std::common_type<TScalar1, TScalar2>::type, TScalar1, TScalar2, Size >
        {};
*/
        

        ////! Rules are following:
        ////! - if either of vector for input types has more components it is considered a result
        ////! - if sizes are same then vector that defines "broader" scalar type is considered a result
        ////! - otherwise they are considered same and makes no difference which is returned
        //template <class TScalar1, size_t Size1, class TScalar2, size_t Size2>
        //struct common_vector_type< naive::vector_adapter<TScalar1, Size1>, naive::vector_adapter<TScalar2, Size2> > :
        //    std::enable_if<
        //        Size1 == 1 || Size2 == 1 || Size1 == Size2,
        //        typename std::conditional<
        //            is_greater<Size1, Size2>::value ||
        //            std::is_same<
        //                typename std::common_type<TScalar1, TScalar2>::type, 
        //                TScalar1
        //            >::value && Size1 == Size2,
        //            // use the first one if it has more components or sizes are same but common type is equal to its scalar type;
        //            naive::vector_adapter<TScalar1, Size1>,
        //            // use second vector if its bigger or same, but only if common scalar is its scalar
        //            typename std::enable_if< 
        //                std::is_same<
        //                    typename std::common_type<TScalar1, TScalar2>::type, 
        //                    TScalar2
        //                >::value, 
        //                naive::vector_adapter<TScalar2, Size2> 
        //            >::type
        //        >::type
        //    >
        //{};

    }
}