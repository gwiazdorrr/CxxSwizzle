#ifndef HEADER_GUARD_SWIZZLE_DETAIL_FUNCTIONS
#define HEADER_GUARD_SWIZZLE_DETAIL_FUNCTIONS

namespace swizzle
{
    namespace detail
    {
        struct default_functions_tag {};

        // Angle and Trigonometry Functions

#define SWIZZLE_FORWARD_FUNCTION_VECTOR(name)    template <class T>                      SWIZZLE_DETAIL_RESULT_1(T)       name(T&& t)                 { return SWIZZLE_DETAIL_VECTOR(T)::name(std::forward<T>(t)); }
#define SWIZZLE_FORWARD_FUNCTION_BINARY(name)   template <class T, class U>             SWIZZLE_DETAIL_RESULT_2(T, U)    name(T&& t, U&& u)          { return SWIZZLE_DETAIL_VECTOR(T)::name(std::forward<T>(t), std::forward<U>(u)); }
#define SWIZZLE_FORWARD_FUNCTION_TERNARY(name)  template <class T, class U, class V>    SWIZZLE_DETAIL_RESULT_3(T, U, V) name(T&& t, U&& u, V&& v)   { return SWIZZLE_DETAIL_VECTOR(T)::name(std::forward<T>(t), std::forward<U>(u), std::forward<V>(v)); }
        

#define SWIZZLE_FORWARD_FUNC_V_V(name) \
    template <class T> SWIZZLE_DETAIL_RESULT_1(T) name(T&& t) { return SWIZZLE_DETAIL_VECTOR(T)::name(std::forward<T>(t)); }

#define SWIZZLE_FORWARD_FUNC_S_V(name) \
    template <class T> SWIZZLE_DETAIL_SCALAR_PROXY(T) name(T&& t) { return SWIZZLE_DETAIL_VECTOR(T)::name(std::forward<T>(t)); }


#define SWIZZLE_FORWARD_FUNC_V_VV(name) \
    template <class T, class U> SWIZZLE_DETAIL_RESULT_2(T, U) name(T&& t, U&& u) { return SWIZZLE_DETAIL_VECTOR(T)::name(std::forward<T>(t), std::forward<U>(u)); }

#define SWIZZLE_FORWARD_FUNC_S_VV(name) \
    template <class T, class U> typename std::enable_if< SWIZZLE_DETAIL_ARE_SAME(T, U), SWIZZLE_DETAIL_SCALAR_PROXY(T) >::type name(T&& t, U&& u) { return SWIZZLE_DETAIL_VECTOR(T)::name(std::forward<T>(t), std::forward<U>(u)); }



#define SWIZZLE_FORWARD_FUNC_V_VVV(name) \
    template <class T, class U, class V> SWIZZLE_DETAIL_RESULT_3(T, U, V) name(T&& t, U&& u, V&& v) { return SWIZZLE_DETAIL_VECTOR(T)::name(std::forward<T>(t), std::forward<U>(u), std::forward<V>(v)); }

#define SWIZZLE_FORWARD_FUNC_V_VS(name) \
    template <class T> SWIZZLE_DETAIL_RESULT_1(T) name(T&& t, SWIZZLE_DETAIL_SCALAR_PROXY(T) a) { return SWIZZLE_DETAIL_VECTOR(T)::name(std::forward<T>(t), a); }

#define SWIZZLE_FORWARD_FUNC_V_SV(name) \
    template <class T> SWIZZLE_DETAIL_RESULT_1(T) name(SWIZZLE_DETAIL_SCALAR_PROXY(T) a, T&& t) { return SWIZZLE_DETAIL_VECTOR(T)::name(a, std::forward<T>(t)); }


#define SWIZZLE_FORWARD_FUNC_V_VSS(name) \
    template <class T> SWIZZLE_DETAIL_RESULT_1(T) name(T&& t, SWIZZLE_DETAIL_SCALAR_PROXY(T) a, SWIZZLE_DETAIL_SCALAR_PROXY(T) b) { return SWIZZLE_DETAIL_VECTOR(T)::name(std::forward<T>(t), a, b); }

#define SWIZZLE_FORWARD_FUNC_V_SSV(name) \
    template <class T> SWIZZLE_DETAIL_RESULT_1(T) name(SWIZZLE_DETAIL_SCALAR_PROXY(T) a, SWIZZLE_DETAIL_SCALAR_PROXY(T) b, T&& t) { return SWIZZLE_DETAIL_VECTOR(T)::name(a, b, std::forward<T>(t)); }

#define SWIZZLE_FORWARD_FUNC_V_VVS(name) \
    template <class T, class U> SWIZZLE_DETAIL_RESULT_2(T, U) name(T&& t, U&& u, SWIZZLE_DETAIL_SCALAR_PROXY(T) a) { return SWIZZLE_DETAIL_VECTOR(T)::name(std::forward<T>(t), std::forward<U>(u), a); }


        SWIZZLE_FORWARD_FUNC_V_V(radians)
        SWIZZLE_FORWARD_FUNC_V_V(degrees)
        SWIZZLE_FORWARD_FUNC_V_V(sin)
        SWIZZLE_FORWARD_FUNC_V_V(cos)
        SWIZZLE_FORWARD_FUNC_V_V(tan)
        SWIZZLE_FORWARD_FUNC_V_V(asin)
        SWIZZLE_FORWARD_FUNC_V_V(acos)
        SWIZZLE_FORWARD_FUNC_V_V(atan)
        SWIZZLE_FORWARD_FUNC_V_VV(atan)
        SWIZZLE_FORWARD_FUNC_V_VV(pow)
        SWIZZLE_FORWARD_FUNC_V_V(exp)
        SWIZZLE_FORWARD_FUNC_V_V(log)
        SWIZZLE_FORWARD_FUNC_V_V(exp2)
        SWIZZLE_FORWARD_FUNC_V_V(log2)
        SWIZZLE_FORWARD_FUNC_V_V(sqrt)
        SWIZZLE_FORWARD_FUNC_V_V(inversesqrt)
        SWIZZLE_FORWARD_FUNC_V_V(abs)
        SWIZZLE_FORWARD_FUNC_V_V(sign)
        SWIZZLE_FORWARD_FUNC_V_V(floor)
        SWIZZLE_FORWARD_FUNC_V_V(ceil)
        SWIZZLE_FORWARD_FUNC_V_V(fract)
        SWIZZLE_FORWARD_FUNC_V_VS(mod)
        SWIZZLE_FORWARD_FUNC_V_VV(mod)
        SWIZZLE_FORWARD_FUNC_V_VS(min)
        SWIZZLE_FORWARD_FUNC_V_VV(min)
        SWIZZLE_FORWARD_FUNC_V_VS(max)
        SWIZZLE_FORWARD_FUNC_V_VV(max)
        SWIZZLE_FORWARD_FUNC_V_VVV(clamp)
        SWIZZLE_FORWARD_FUNC_V_VSS(clamp)
        SWIZZLE_FORWARD_FUNC_V_VVV(mix)
        SWIZZLE_FORWARD_FUNC_V_VVS(mix)
        SWIZZLE_FORWARD_FUNC_V_VV(step)
        SWIZZLE_FORWARD_FUNC_V_SV(step)
        SWIZZLE_FORWARD_FUNC_V_VVV(smoothstep)
        SWIZZLE_FORWARD_FUNC_V_SSV(smoothstep)
        SWIZZLE_FORWARD_FUNC_V_VV(reflect)
        SWIZZLE_FORWARD_FUNC_S_V(length)
        SWIZZLE_FORWARD_FUNC_S_VV(distance)
        SWIZZLE_FORWARD_FUNC_S_VV(dot)
        SWIZZLE_FORWARD_FUNC_V_V(normalize)
        SWIZZLE_FORWARD_FUNC_V_VVV(faceforward)


        
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

#endif  HEADER_GUARD_SWIZZLE_DETAIL_FUNCTIONS
