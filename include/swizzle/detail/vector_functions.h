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
        

        template <class T>
        SWIZZLE_DETAIL_RESULT_1(T) radians(T&& degrees)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::radians(std::forward<T>(degrees));
        }
        template <class T>
        SWIZZLE_DETAIL_RESULT_1(T) degrees(T&& radians)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::degrees(std::forward<T>(radians));
        }   
        template <class T>
        SWIZZLE_DETAIL_RESULT_1(T) sin(T&& angle)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::sin(std::forward<T>(angle));
        }         
        template <class T>
        SWIZZLE_DETAIL_RESULT_1(T) cos(T&& angle)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::cos(std::forward<T>(angle));
        }         
        template <class T>
        SWIZZLE_DETAIL_RESULT_1(T) tan(T&& angle)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::tan(std::forward<T>(angle));
        }         
        template <class T>
        SWIZZLE_DETAIL_RESULT_1(T) asin(T&& x)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::asin(std::forward<T>(x));
        }            
        template <class T>
        SWIZZLE_DETAIL_RESULT_1(T) acos(T&& x)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::acos(std::forward<T>(x));
        }            
        template <class T, class U>
        SWIZZLE_DETAIL_RESULT_2(T, U) atan(T&& y, U&& x)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::atan(std::forward<T>(y), std::forward<T>(x));
        }  
        template <class T>
        SWIZZLE_DETAIL_RESULT_1(T) atan(T&& y_over_x)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::atan(std::forward<T>(y_over_x));
        } 

        // Exponential Functions 
        template <class T, class U>
        SWIZZLE_DETAIL_RESULT_2(T, U) pow(T&& x, U&& y)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::pow(std::forward<T>(x), std::forward<T>(y));
        }    

        template <class T>
        SWIZZLE_DETAIL_RESULT_1(T) exp(T&& x)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::exp(std::forward<T>(x));
        }             
        template <class T>
        SWIZZLE_DETAIL_RESULT_1(T) log(T&& x)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::log(std::forward<T>(x));
        }             
        template <class T>
        SWIZZLE_DETAIL_RESULT_1(T) exp2(T&& x)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::exp2(std::forward<T>(x));
        }            
        template <class T>
        SWIZZLE_DETAIL_RESULT_1(T) log2(T&& x)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::log2(std::forward<T>(x));
        }            
        template <class T>
        SWIZZLE_DETAIL_RESULT_1(T) sqrt(T&& x)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::sqrt(std::forward<T>(x));
        }            
        template <class T>
        SWIZZLE_DETAIL_RESULT_1(T) inversesqrt(T&& x)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::inversesqrt(std::forward<T>(x));
        }     

        template <class T>
        SWIZZLE_DETAIL_RESULT_1(T) abs(T&& x)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::abs(std::forward<T>(x));
        }                          
        template <class T>
        SWIZZLE_DETAIL_RESULT_1(T) sign(T&& x)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::sign(std::forward<T>(x));
        }                                                             
        template <class T>
        SWIZZLE_DETAIL_RESULT_1(T) floor(T&& x)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::floor(std::forward<T>(x));
        }                          
        template <class T>
        SWIZZLE_DETAIL_RESULT_1(T) ceil(T&& x)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::ceil(std::forward<T>(x));
        }                           
        template <class T>
        SWIZZLE_DETAIL_RESULT_1(T) fract(T&& x)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::fract(std::forward<T>(x));
        }                          
        template <class T>
        SWIZZLE_DETAIL_RESULT_1(T) mod(T&& x, SWIZZLE_DETAIL_SCALAR_PROXY(T) && y)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::mod(std::forward<T>(x), y);
        }                     
        template <class T, class U>
        SWIZZLE_DETAIL_RESULT_2(T, U) mod(T&& x, U&& y)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::mod(std::forward<T>(x), std::forward<T>(y));
        }                   
        template <class T, class U>
        SWIZZLE_DETAIL_RESULT_2(T, U) min(T&& x, U&& y)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::min(std::forward<T>(x), std::forward<T>(y));
        }                   
        template <class T>
        SWIZZLE_DETAIL_RESULT_1(T) min(T&& x, SWIZZLE_DETAIL_SCALAR_PROXY(T) y)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::min(std::forward<T>(x), std::forward<T>(y));
        }     
        template <class T, class U>
        SWIZZLE_DETAIL_RESULT_2(T, U) max(T&& x, U&& y)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::max(std::forward<T>(x), std::forward<T>(y));
        }                   
        template <class T>
        SWIZZLE_DETAIL_RESULT_1(T) max(T&& x, SWIZZLE_DETAIL_SCALAR_PROXY(T) y)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::max(std::forward<T>(x), std::forward<T>(y));
        }     

        template <class T, class U, class V>
        SWIZZLE_DETAIL_RESULT_3(T, U, V) clamp(T&& x, U&& minVal, V&& maxVal)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::clamp(x, minVal, maxVal);
        }            
        template <class T>
        SWIZZLE_DETAIL_RESULT_1(T) clamp(T&& x, SWIZZLE_DETAIL_SCALAR_PROXY(T) minVal, SWIZZLE_DETAIL_SCALAR_PROXY(T) maxVal)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::clamp(x, minVal, maxVal);
        }     
        template <class T, class U, class V>
        SWIZZLE_DETAIL_RESULT_3(T, U, V) mix(T&& x, U&& y, V&& a)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::mix(x, y, a);
        }       
        template <class T, class U>
        SWIZZLE_DETAIL_RESULT_2(T, U) mix(T&& x, U&& y, SWIZZLE_DETAIL_SCALAR_PROXY(T) a)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::mix(x, y, a);
        }     
        template <class T, class U>
        SWIZZLE_DETAIL_RESULT_2(T, U) step(T&& edge, U&& x)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::step(edge, x);
        }                 
        template <class T>
        SWIZZLE_DETAIL_RESULT_1(T) step(SWIZZLE_DETAIL_SCALAR_PROXY(T) edge, T&& x)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::step(edge, x);
        }     
        template <class T, class U, class V>
        SWIZZLE_DETAIL_RESULT_3(T, U, V) smoothstep(T&& edge0, U&& edge1, V&& x)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::smoothstep(edge0, edge1, x);
        }       
        template <class T>
        SWIZZLE_DETAIL_RESULT_1(T) step(SWIZZLE_DETAIL_SCALAR_PROXY(T) edge0, SWIZZLE_DETAIL_SCALAR_PROXY(T) edge1, T&& x)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::smoothstep(edge0, edge1, x);
        }     
        template <class T, class U>
        SWIZZLE_DETAIL_RESULT_2(T, U) reflect(T&& I, U&& N)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::reflect(I, N);
        }     

        // General functions
        template <class T>
        SWIZZLE_DETAIL_SCALAR_PROXY(T) length(T&& x)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::length(x);
        }
        template <class T, class U>
        typename std::enable_if< SWIZZLE_DETAIL_ARE_SAME(T, U), SWIZZLE_DETAIL_SCALAR_PROXY(T) >::type distance(T&& p0, U&& p1)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::distance(p0, p1);
        }       
        template <class T, class U>
        typename std::enable_if< SWIZZLE_DETAIL_ARE_SAME(T, U), SWIZZLE_DETAIL_SCALAR_PROXY(T) >::type dot(T&& p0, U&& p1)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::dot(p0, p1);
        }       
            
        // vec3 cross (vec3 x , vec3 y )                      
        template <class T>
        SWIZZLE_DETAIL_RESULT_1(T) normalize(T&& x)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::normalize(x);
        }                     
        // vec4 ftransform()             

        template <class T, class U, class V>
        SWIZZLE_DETAIL_RESULT_3(T, U, V) faceforward(T&& x, U&& y, V&& a)
        {
            return SWIZZLE_DETAIL_VECTOR(T)::faceforward(x, y, a);
        }       


        
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
