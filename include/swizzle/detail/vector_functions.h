#ifndef HEADER_GUARD_SWIZZLE_DETAIL_FUNCTIONS
#define HEADER_GUARD_SWIZZLE_DETAIL_FUNCTIONS

namespace swizzle
{
    namespace detail
    {
        namespace glsl_functions
        {
            struct tag {};

#define SWIZZLE_FORWARD_FUNC_1(name)\
            template <class T> auto name(T&& t) -> decltype(typename get_vector_type<T>::type::name(t))\
            { return typename get_vector_type<T>::type::name(std::forward<T>(t)); }      

#define SWIZZLE_FORWARD_FUNC_2(name)\
            template <class T, class U> auto name(T&& t, U&& u) -> decltype(typename get_vector_type<T, U>::type::name(t, u))\
            { return typename get_vector_type<T, U>::type::name(std::forward<T>(t), std::forward<U>(u)); }                      

#define SWIZZLE_FORWARD_FUNC_3(name)\
            template <class T, class U, class V> auto name(T&& t, U&& u, V&& v) ->  decltype( typename get_vector_type<T, U, V>::type::name(t, u, v)  )\
            { return typename get_vector_type<T, U, V>::type::name(std::forward<T>(t), std::forward<U>(u), std::forward<V>(v)); }


            SWIZZLE_FORWARD_FUNC_1(radians)
            SWIZZLE_FORWARD_FUNC_1(degrees)
            SWIZZLE_FORWARD_FUNC_1(sin)
            SWIZZLE_FORWARD_FUNC_1(cos)
            SWIZZLE_FORWARD_FUNC_1(tan)
            SWIZZLE_FORWARD_FUNC_1(asin)
            SWIZZLE_FORWARD_FUNC_1(acos)
            SWIZZLE_FORWARD_FUNC_1(atan)
            SWIZZLE_FORWARD_FUNC_2(atan)
            SWIZZLE_FORWARD_FUNC_2(pow)
            SWIZZLE_FORWARD_FUNC_1(exp)
            SWIZZLE_FORWARD_FUNC_1(log)
            SWIZZLE_FORWARD_FUNC_1(exp2)
            SWIZZLE_FORWARD_FUNC_1(log2)
            SWIZZLE_FORWARD_FUNC_1(sqrt)
            SWIZZLE_FORWARD_FUNC_1(inversesqrt)
            SWIZZLE_FORWARD_FUNC_1(abs)
            SWIZZLE_FORWARD_FUNC_1(sign)
            SWIZZLE_FORWARD_FUNC_1(floor)
            SWIZZLE_FORWARD_FUNC_1(ceil)
            SWIZZLE_FORWARD_FUNC_1(fract)
            SWIZZLE_FORWARD_FUNC_2(mod)
            SWIZZLE_FORWARD_FUNC_2(min)
            SWIZZLE_FORWARD_FUNC_2(max)
            SWIZZLE_FORWARD_FUNC_3(clamp)
            SWIZZLE_FORWARD_FUNC_3(mix)
            SWIZZLE_FORWARD_FUNC_2(step)
            SWIZZLE_FORWARD_FUNC_3(smoothstep)
            SWIZZLE_FORWARD_FUNC_2(reflect)
            SWIZZLE_FORWARD_FUNC_1(length)
            SWIZZLE_FORWARD_FUNC_2(distance)
            SWIZZLE_FORWARD_FUNC_2(dot)
            SWIZZLE_FORWARD_FUNC_1(normalize)
            SWIZZLE_FORWARD_FUNC_3(faceforward)

#undef SWIZZLE_FORWARD_FUNC_1
#undef SWIZZLE_FORWARD_FUNC_2
#undef SWIZZLE_FORWARD_FUNC_3


        
        //    genType radians (genType degrees)
        //    genType degrees (genType radians)   
        //    genType sin (genType angle)         
        //    genType cos (genType angle)         
        //    genType tan (genType angle)         
        //    genType asin (genType x)            
        //    genType acos (genType x)            
        //    genType atan (genType y , genType x)
        //    genType atan (genType y_over_x) 

        //    // Exponential Functions 

        //    genType pow (genType x , genType y )
        //    genType exp (genType x)             
        //    genType log (genType x)             
        //    genType exp2 (genType x)            
        //    genType log2 (genType x)            
        //    genType sqrt (genType x)            
        //    genType inversesqrt (genType x)    

        //     Common Functions 

            //genType abs (genType x)                          
            //genType sign (genType x)                                                             
            //genType floor (genType x)                          
            //genType ceil (genType x)                           
            //genType fract (genType x)                          
            //genType mod (genType x , float y )                 
            //genType mod (genType x , genType y )               
            //genType min (genType x , genType y )               
            //genType min (genType x , float y) 
            //genType max (genType x , genType y )               
            //genType max (genType x , float y ) 
            //genType clamp (genType x ,  genType minVal, genType max Val) 
            //genType clamp (genType x , float minVal,  float max Val) 
            //genType mix (genType x , genType y , genType a) 
            //genType mix (genType x , genType y , float a) 
            //genType step (genType edge, genType x)             
            //genType step (float edge, genType x) 
            //genType smoothstep (genType edge0, genType edge1, genType x)      
            //genType smoothstep (float edge0, float edge1, genType x) 

        // Geometric functions
            //float length (genType x)                          
            //float distance (genType p0 , genType p1 )         
            //float dot (genType x , genType y )                
            //vec3 cross (vec3 x , vec3 y )                      
            //genType normalize (genType x)                     
            //vec4 ftransform()                                  
            //genType faceforward(genType N, genType I, genType Nref )
        }
    }
}

#endif  HEADER_GUARD_SWIZZLE_DETAIL_FUNCTIONS
