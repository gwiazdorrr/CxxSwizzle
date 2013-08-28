#include <swizzle/naive/vector_adapter.h>
#include <swizzle/detail/vector_binary_operators.h>
#include <swizzle/detail/glsl_functions.h>

struct wtf {};

class tag_1 : swizzle::detail::glsl_functions::tag {};


struct tag : 
    private swizzle::detail::binary_operators::tag
    //private swizzle::detail::glsl_functions::tag
    //private swizzle::detail::binary_operators::tag
{};


typedef swizzle::naive::vector_adapter< swizzle::naive::vector_adapter_traits<float, 1, tag> > vec1;
typedef swizzle::naive::vector_adapter< swizzle::naive::vector_adapter_traits<float, 2, tag> > vec2;
typedef swizzle::naive::vector_adapter< swizzle::naive::vector_adapter_traits<float, 3, tag> > vec3;
typedef swizzle::naive::vector_adapter< swizzle::naive::vector_adapter_traits<float, 4, tag> > vec4;

