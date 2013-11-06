// CxxSwizzle
// Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <swizzle/detail/utils.h>
#include <utility>
#include <type_traits>

//! "Invokes" macro what for each integer from range <1,count>, pasting separator
//! between each "invoke". Yes, indexed from 1. Turns out it is easier to write macros
//! this way, as first and last elements are accessible without using helper macros
#define CXXSWIZZLE_FOR(count, what, separator) CXXSWIZZLE_FOR_(count)(what, separator)
//! Expand count, please!
#define CXXSWIZZLE_FOR_(count) CXXSWIZZLE_FOR_##count
#define CXXSWIZZLE_FOR_0(x, s)
#define CXXSWIZZLE_FOR_1(x, s)  x(1)
#define CXXSWIZZLE_FOR_2(x, s)  CXXSWIZZLE_FOR_1(x, s)  s() x(2)
#define CXXSWIZZLE_FOR_3(x, s)  CXXSWIZZLE_FOR_2(x, s)  s() x(3)
#define CXXSWIZZLE_FOR_4(x, s)  CXXSWIZZLE_FOR_3(x, s)  s() x(4)
#define CXXSWIZZLE_FOR_5(x, s)  CXXSWIZZLE_FOR_4(x, s)  s() x(5)
#define CXXSWIZZLE_FOR_6(x, s)  CXXSWIZZLE_FOR_5(x, s)  s() x(6)
#define CXXSWIZZLE_FOR_7(x, s)  CXXSWIZZLE_FOR_6(x, s)  s() x(7)
#define CXXSWIZZLE_FOR_8(x, s)  CXXSWIZZLE_FOR_7(x, s)  s() x(8)
#define CXXSWIZZLE_FOR_9(x, s)  CXXSWIZZLE_FOR_8(x, s)  s() x(9)
#define CXXSWIZZLE_FOR_10(x, s) CXXSWIZZLE_FOR_9(x, s)  s() x(10)
#define CXXSWIZZLE_FOR_11(x, s) CXXSWIZZLE_FOR_10(x, s) s() x(11)
#define CXXSWIZZLE_FOR_12(x, s) CXXSWIZZLE_FOR_11(x, s) s() x(12)
#define CXXSWIZZLE_FOR_13(x, s) CXXSWIZZLE_FOR_12(x, s) s() x(13)
#define CXXSWIZZLE_FOR_14(x, s) CXXSWIZZLE_FOR_13(x, s) s() x(14)
#define CXXSWIZZLE_FOR_15(x, s) CXXSWIZZLE_FOR_14(x, s) s() x(15)
#define CXXSWIZZLE_FOR_16(x, s) CXXSWIZZLE_FOR_15(x, s) s() x(16)

//! A way of passing comma into a macro
#define CXXSWIZZLE_COMMA() ,
#define CXXSWIZZLE_EMPTY() 

//! A more convienient way of invoking CXXSWIZZLE_FOR, with default comma separator
#define CXXSWIZZLE_FOR_ITERATE(i, what) CXXSWIZZLE_FOR(i, what, CXXSWIZZLE_COMMA)
//! A more convienient way of invoking CXXSWIZZLE_FOR, with default without a separator
#define CXXSWIZZLE_FOR_ITERATE_NO_COMMA(i, what) CXXSWIZZLE_FOR(i, what, CXXSWIZZLE_EMPTY)

//! Macros used by the constructor generator, should be self explainatory
#define CXXSWIZZLE_ITH_TEMPLATE_TYPE(i) class T##i
#define CXXSWIZZLE_ITH_TEMPLATE_ARG(i) T##i
#define CXXSWIZZLE_ITH_FUNCTION_ARG(i) T##i && t##i
#define CXXSWIZZLE_ITH_FORWARD(i) std::forward<T##i>(t##i)
#define CXXSWIZZLE_CREATE_DUMMY(i) , ::swizzle::detail::nothing()

//! Defines a constructor of Type having Size components, acception ArgCount template parameters
//! and calling construct function forwarding all parameters and appending with FillersCount swizzle::detail::nothing instances.
//! Also makes sure that total number of components of parameters is equal or greater than Size and that
//! there are no surplus parameters passed.
//! Thanks to this macro only the user can focus on implementing construct function (accepting ArgCont + FillersCount arguments)
//! rather than an array of constructors.
#define CXXSWIZZLE_CONSTRUCTOR_GREATER_OR_EQUAL_COMPONENTS(Type, Size, ArgCount, FillersCount) \
    template < CXXSWIZZLE_FOR_ITERATE(ArgCount, CXXSWIZZLE_ITH_TEMPLATE_TYPE) > \
    explicit Type( CXXSWIZZLE_FOR_ITERATE(ArgCount, CXXSWIZZLE_ITH_FUNCTION_ARG), \
        typename std::enable_if< \
            !(Size <= ::swizzle::detail::get_total_size<CXXSWIZZLE_FOR_ITERATE(ArgCount, CXXSWIZZLE_ITH_TEMPLATE_ARG)>::value - ::swizzle::detail::get_total_size< CXXSWIZZLE_ITH_TEMPLATE_ARG(ArgCount) >::value) && \
            (Size <= ::swizzle::detail::get_total_size<CXXSWIZZLE_FOR_ITERATE(ArgCount, CXXSWIZZLE_ITH_TEMPLATE_ARG)>::value), \
            void \
        >::type* = 0 ) \
    {\
        construct( CXXSWIZZLE_FOR_ITERATE(ArgCount, CXXSWIZZLE_ITH_FORWARD) CXXSWIZZLE_FOR_ITERATE_NO_COMMA(FillersCount, CXXSWIZZLE_CREATE_DUMMY) ); \
    }

