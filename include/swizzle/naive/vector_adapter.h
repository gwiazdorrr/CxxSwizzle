#ifndef HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_ADAPTER
#define HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_ADAPTER

#include "../detail/vector_binary_operators.h"
#include "../detail/vector_functions.h"
#include "vector_data.h"
#include <type_traits>
#include "../detail/utils.h"
#include "../detail/vector_traits.h"

#define _USE_MATH_DEFINES
#include <cmath>

namespace swizzle
{
    namespace naive
    {
        template <class TScalar, size_t Num>
        class vector_adapter;
    }

    namespace detail
    {
        struct not_available
        {
        private:
            not_available();
        };

        struct not_available2
        {
        private:
            not_available2();
        };

        template <class T>
        struct vector_traits
        {
            static_assert( std::is_arithmetic<T>::value, "Type must be arythmetic");
            static const size_t num_of_components = 1;
            typedef naive::vector_adapter< T, 1 > vector_type;
            typedef T scalar_type;
        };

        template <class TScalar, size_t Num>
        struct vector_traits< naive::vector_adapter<TScalar, Num> >
        {
            static const size_t num_of_components = Num;
            typedef naive::vector_adapter<TScalar, Num> vector_type;
            typedef TScalar scalar_type;
        };

        template <class TVector, class TScalar, size_t NumOfComponents, size_t x, size_t y, size_t z, size_t w>
        struct vector_traits< naive::vector_proxy<TVector, TScalar, NumOfComponents, x, y, z, w> >
        {
            static const size_t num_of_components = TVector::num_of_components;
            typedef TVector vector_type;
            typedef TScalar scalar_type;
        };

        template <class TBase>
        struct vector_traits< detail::writable_wrapper<TBase> >
        {
            static const size_t num_of_components = TBase::num_of_components;
            typedef typename TBase::vector_type vector_type;
            typedef typename TBase::scalar_type scalar_type;
        };
    }

    namespace naive
    {





       /* template <size_t Size, class T1>
        struct are_sizes_valid_1
        {
            static const bool value = detail::are_sizes_valid<
                Size,
                vector_type_getter<T1>::type::num_of_components,
                0, 0, 0
            >::value;
        };

        template <size_t Size, class T1, class T2>
        struct are_sizes_valid_2
        {
            static const bool value = detail::are_sizes_valid<
                Size,
                vector_type_getter<T1>::type::num_of_components,
                vector_type_getter<T2>::type::num_of_components,
                0, 0
            >::value;
        };*/

        /*
        template <size_t Size, class T1, class T2, class T3, class T4>
        struct are_sizes_valid_4
        {
            static const bool value = detail::are_sizes_valid<
                Size,
                vector_type_getter<T1>::type::num_of_components,
                vector_type_getter<T2>::type::num_of_components,
                vector_type_getter<T3>::type::num_of_components,
                vector_type_getter<T4>::type::num_of_components
            >::value;
        };
*/

        template < class TScalar, size_t NumComponents > 
        class vector_adapter : 
            public naive_vector_data< vector_adapter, TScalar, NumComponents >, 
            public detail::default_vector_unary_operators_tag,
            public detail::default_functions_tag
        {
            typedef naive_vector_data< ::swizzle::naive::vector_adapter, TScalar, NumComponents > base_type;

        public:
            //using base_type::scalar_type;
            static const size_t num_of_components = base_type::num_of_components;
            typedef vector_adapter vector_type;
            typedef typename base_type::scalar_type scalar_type;
            typedef double fallback_scalar_type;
            typedef ::swizzle::naive::vector_adapter< double, 1 > scalar_proxy;


        private:
            using base_type::_components;

        public:

            //! Default constructor.
            vector_adapter()
            {
                for_each_component( [&](size_t i) -> void { at(i) = 0; } );
            }

            //! Constructors from privitives

            vector_adapter& _set(typename std::conditional<num_of_components==1, scalar_type, detail::not_available>::type v1)
            {
                _components[0] = v1;
                return *this;
            }

            vector_adapter& _set(typename std::conditional<num_of_components==2, scalar_type, detail::not_available>::type v1, scalar_type v2)
            {
                _components[0] = v1;
                _components[1] = v2;
                return *this;
            }

            vector_adapter& _set(typename std::conditional<num_of_components==3, scalar_type, detail::not_available>::type v1, scalar_type v2, scalar_type v3)
            {
                _components[0] = v1;
                _components[1] = v2;
                _components[2] = v3;
                return *this;
            }

            vector_adapter& _set(typename std::conditional<num_of_components==4, scalar_type, detail::not_available>::type v1, scalar_type v2, scalar_type v3, scalar_type v4)
            {
                _components[0] = v1;
                _components[1] = v2;
                _components[2] = v3;
                _components[3] = v4;
                return *this;
            }

            //! 1 argument constructors

            static vector_adapter from_scalar(scalar_type o)
            {
                auto result = vector_adapter();
                return result.for_each_component( [&](size_t i) -> void { result.at(i) = o; } );
            }
            static vector_adapter from_scalar(scalar_proxy o)
            {
                return from_scalar(o.x);
            }



            vector_adapter( typename std::conditional<num_of_components == 1, scalar_type, detail::not_available>::type o )
            {
                for_each_component( [&](size_t i) -> void { at(i) = o; } );
            }
            vector_adapter( typename std::conditional<num_of_components == 1 && !std::is_same<vector_adapter, scalar_proxy>::value, scalar_proxy, detail::not_available2>::type o )
            {
                for_each_component( [&](size_t i) -> void { at(i) = o.x; } );
            }

            explicit vector_adapter( typename std::conditional<num_of_components == 1, detail::not_available, scalar_type>::type o )
            {
                for_each_component( [&](size_t i) -> void { at(i) = o; } );
            }


            vector_adapter( const vector_adapter& o )
            {
                for_each_component( [&](size_t i) -> void { at(i) = o.at(i); } );
            }

            template <class T1>
            explicit vector_adapter( const T1& v1, 
                typename std::enable_if< detail::are_sizes_valid<num_of_components, detail::vector_traits<T1>::num_of_components, 0, 0, 0>::value, void>::type* = 0 )
            {
                assign<0>(v1);
            }

            template <class T1, class T2>
            vector_adapter( const T1& v1, const T2& v2,
                typename std::enable_if< detail::are_sizes_valid<num_of_components, detail::vector_traits<T1>::num_of_components, detail::vector_traits<T2>::num_of_components, 0, 0>::value, void>::type* = 0 )
            {
                assign<0>(v1);
                assign<detail::vector_traits<T1>::num_of_components>(v2);
            }

            template <class T1, class T2, class T3>
            vector_adapter( const T1& v1, const T2& v2, const T3& v3,
                typename std::enable_if< detail::are_sizes_valid<num_of_components, detail::vector_traits<T1>::num_of_components, detail::vector_traits<T2>::num_of_components, detail::vector_traits<T3>::num_of_components, 0>::value, void>::type* = 0 )
            {
                assign<0>(v1);
                assign<detail::vector_traits<T1>::num_of_components>(v2);
                assign<detail::vector_traits<T1>::num_of_components + detail::vector_traits<T2>::num_of_components>(v3);
            }

            template <class T1, class T2, class T3, class T4>
            vector_adapter( const T1& v1, const T2& v2, const T3& v3, const T4& v4,
                typename std::enable_if< detail::are_sizes_valid<num_of_components, detail::vector_traits<T1>::num_of_components, detail::vector_traits<T2>::num_of_components, detail::vector_traits<T3>::num_of_components, detail::vector_traits<T4>::num_of_components>::value, void>::type* = 0 )
            {
                assign<0>(v1);
                assign<detail::vector_traits<T1>::num_of_components>(v2);
                assign<detail::vector_traits<T1>::num_of_components + detail::vector_traits<T2>::num_of_components>(v3);
                assign<detail::vector_traits<T1>::num_of_components + detail::vector_traits<T2>::num_of_components + detail::vector_traits<T3>::num_of_components>(v4);
            }

        public:

            vector_type& operator=(const vector_type& o)
            {
                return for_each_component( [&](size_t i) -> void { at(i) = o.at(i); } );
            }

            vector_type operator-() const
            {
                vector_type result;
                result.for_each_component([&](size_t i) -> void { result.at(i) = -at(i); });
                return result;
            }

            vector_type& operator+=(const vector_type& o)
            {
                return for_each_component( [&](size_t i) -> void { at(i) += o.at(i); } );
            }
            vector_type& operator-=(const vector_type& o)
            {
                return for_each_component( [&](size_t i) -> void { at(i) -= o.at(i); } );
            }
            vector_type& operator*=(const vector_type& o)
            {
                return for_each_component( [&](size_t i) -> void { at(i) *= o.at(i); } );
            }
            vector_type& operator/=(const vector_type& o)
            {
                return for_each_component( [&](size_t i) -> void { at(i) /= o.at(i); } );
            }

            vector_type& operator+=(const scalar_type& o)
            {
                return for_each_component( [&](size_t i) -> void { at(i) += o; } );
            }
            vector_type& operator-=(const scalar_type& o)
            {
                return for_each_component( [&](size_t i) -> void { at(i) -= o; } );
            }
            vector_type& operator*=(const scalar_type& o)
            {
                return for_each_component( [&](size_t i) -> void { at(i) *= o; } );
            }
            vector_type& operator/=(const scalar_type& o)
            {
                return for_each_component( [&](size_t i) -> void { at(i) /= o; } );
            }

        public:

            template <class Func>
            vector_type& transform(const vector_adapter& arg, Func func)
            {
                return for_each_component( [&](size_t i) -> void { at(i) = func(at(i), arg.at(i)); } );
            }

            vector_type& transform_func(scalar_type (*func)(scalar_type))
            {
                return transform(func);
            }

            template <class Func>
            vector_type& transform(Func func)
            {
                return for_each_component( [&](size_t i) -> void { at(i) = func(at(i)); } );
            }

            scalar_type& at(size_t i)
            {
                return _components[i];
            }

            const scalar_type& at(size_t i) const
            {
                return _components[i];
            }

            template <class Func>
            vector_type& for_each_component(Func func)
            {
                return for_each_component_custom(func, std::integral_constant<size_t, 0>(), std::integral_constant<size_t, num_of_components>());
            }

            template <size_t NStart, size_t NEnd, class Func>
            vector_type& for_each_component_custom(Func func, std::integral_constant<size_t, NStart>, std::integral_constant<size_t, NEnd>)
            {
                static_assert( NStart >= 0 && NEnd <= num_of_components && NStart < NEnd, "Out of bounds" );
                for (size_t i = NStart; i < NEnd; ++i)
                {
                    func(i);
                }
                return *this;
            }

//             operator scalar_type()
//             {
//                 //static_assert(num_of_components == 1);
//                 return _components[0];
//             }

        private:

            template <size_t N>
            void assign(scalar_type v)
            {
                _components[N] = v;
            }

            template <size_t N, class TScalar, size_t NumOfComponents>
            void assign( const vector_adapter<TScalar, NumOfComponents>& v )
            {
                for_each_component_custom([&](size_t i) -> void { at(i) = v.at(i - N); },
                    std::integral_constant<size_t, N>(),
                    std::integral_constant<size_t, (N + NumOfComponents > num_of_components) ? num_of_components : (N + NumOfComponents)>()
                );
            }

            template <size_t N, class TVector, class TScalar, size_t NumOfComponents, size_t x, size_t y, size_t z, size_t w>
            void assign( const vector_proxy<TVector, TScalar, NumOfComponents, x, y, z, w>& v )
            {
                assign<N>( v.operator TVector() );
            }

        public:

            


            //\1template <class T>\n\1SWIZZLE_DETAIL_VECTOR(T) \2(T&& \3)\n\1{\n\1    return SWIZZLE_DETAIL_VECTOR(T)::\2(\3);\n\1}
            //\1static vector_adapter \2(vector_adapter \3)\n\1{\n\1    return \3.transform_func( std::\2 );\n\1}


            static vector_adapter radians(vector_adapter degrees)
            {
                return degrees.transform( [](scalar_type a) -> scalar_type { return scalar_type(M_PI * a / 180) } );
            }
            static vector_adapter degrees(vector_adapter radians)
            {
                return radians.transform( [](scalar_type a) -> scalar_type { return scalar_type(180 * a / M_PI) } );
            }   
            static vector_adapter sin(vector_adapter angle)
            {
                return angle.transform_func( std::sin );
            }         
            static vector_adapter cos(vector_adapter angle)
            {
                return angle.transform_func( std::cos );
            }         
            static vector_adapter tan(vector_adapter angle)
            {
                return angle.transform_func( std::tan );
            }         
            static vector_adapter asin(vector_adapter x)
            {
                return x.transform_func( std::asin );
            }            
            static vector_adapter acos(vector_adapter x)
            {
                return x.transform_func( std::acos );
            }            
            static vector_adapter atan(const vector_adapter& y, const vector_adapter& x)
            {
                return atan(y / x);
            }
            static vector_adapter atan(vector_adapter y_over_x)
            {
                return y_over_x.transform_func( std::atan );
            } 

            // Exponential Functions 
            static vector_adapter pow(vector_adapter x, const vector_adapter& y)
            {
                return x.transform(u, [](scalar_type a, scalar_type b) -> scalar_type { return std::pow(a, b); } );
            }
            static vector_adapter exp(vector_adapter x)
            {
                return x.transform_func( std::exp );
            }             
            static vector_adapter log(vector_adapter x)
            {
                return x.transform_func( std::log );
            }             
            static vector_adapter exp2(const vector_adapter& x)
            {
                return pow(from_scalar(2), x);
            }            
            static vector_adapter log2(vector_adapter x)
            {
                return x.transform( [](scalar_type a) -> scalar_type { return scalar_type( std::log(a) / M_LOG2E); } );
            }            
            static vector_adapter sqrt(vector_adapter x)
            {
                return x.transform_func( std::sqrt );
            }            
            static vector_adapter inversesqrt(vector_adapter x)
            {
                return 1 / sqrt(x);
            }
            

            
            
            static vector_adapter abs(vector_adapter x)
            {
                return x.transform_func( std::abs );
            }                          
            static vector_adapter sign(vector_adapter x)
            {
                return x.transform( [](scalar_type a) -> scalar_type { return a > 0 ? 1 : ( a < 0 ? -1 : 0 ); } );
            }                                    
            static vector_adapter floor(vector_adapter x)
            {
                return x.transform_func( std::floor );
            }                          
            static vector_adapter ceil(vector_adapter x)
            {
                return x.transform_func( std::ceil );
            }                           
            static vector_adapter fract(const vector_adapter& x)
            {
                return x - floor(x);
            }                          
            static vector_adapter mod(vector_adapter x, fallback_scalar_type y)
            {
                return x.transform( [=](scalar_type a) -> scalar_type { return std::fmod<fallback_scalar_type>(x, y); } );
            }                   
            static vector_adapter mod(vector_adapter x, const vector_adapter& y)
            {
                return x.transform( [](scalar_type a, scalar_type b) -> scalar_type { return std::fmod(x, b); } );
            }                 
            static vector_adapter min(vector_adapter x, const vector_adapter& y)
            {
                return x.transform( [](scalar_type a, scalar_type b) -> scalar_type { return std::min(x, b); } );
            }                 
            static vector_adapter min(vector_adapter x, fallback_scalar_type y)
            {
                return x.transform( [=](scalar_type a) -> scalar_type { return std::min<fallback_scalar_type>(x, y); } );
            }   
            static vector_adapter max(vector_adapter x, const vector_adapter& y)
            {
                return x.transform( [](scalar_type a, scalar_type b) -> scalar_type { return std::max(x, b); } ); 
            }                 
            static vector_adapter max(vector_adapter x, fallback_scalar_type y)
            {
                return x.transform( [=](scalar_type a) -> scalar_type { return std::max<fallback_scalar_type>(x, y); } );
            }   
            static vector_adapter clamp(vector_adapter x, const vector_adapter& minVal, const vector_adapter& maxVal)
            {
                return max(min(x, maxVal), minVal);
            }
            static vector_adapter clamp(vector_adapter x, fallback_scalar_type minVal, fallback_scalar_type maxVal)
            {
                return max(min(x, maxVal), minVal);
            }
            static vector_adapter mix(vector_adapter x, const vector_adapter& y, const vector_adapter& a)
            {
                return x.for_each_component([&](size_t i) -> void { x.at(i) = x.at(i) + a.at(i) * (y.at(i) - x.at(i)); } );
            }
            static vector_adapter mix(vector_adapter x, const vector_adapter& y, const typename vector_adapter<scalar_type, 1> a)
            {
                return x.for_each_component([&](size_t i) -> void { x.at(i) = x.at(i) + a * (y.at(i) - x.at(i)); } );
            }
            static vector_adapter step(const vector_adapter& edge, vector_adapter x)
            {
                return x.transform(edge, [](scalar_type a, scalar_type b) -> scalar_type { return a > b ? 1 : 0; });
            }               
            static vector_adapter step(fallback_scalar_type edge, vector_adapter x)
            {
                return x.transform([=](scalar_type a) -> scalar_type { return a > edge ? 1 : 0; });
            }               
            static vector_adapter smoothstep(const vector_adapter& edge0, const vector_adapter& edge1, vector_adapter x)
            {
                auto t = clamp((x - edge0) / (edge1 - edge0), 0, 1);
                return t * t * (3 - 2 * t);
            }
            static vector_adapter smoothstep(fallback_scalar_type edge0, fallback_scalar_type edge1, vector_adapter x)
            {
                auto t = clamp((x - edge0) / (edge1 - edge0), 0, 1);
                return t * t * (3 - 2 * t);
            }
            static vector_adapter reflect(const vector_adapter& I, const vector_adapter& N)
            {
                return I - 2 * dot(I, N) * N;
            }       

            // Geometric functions
            static fallback_scalar_type length(vector_adapter x)
            {
                fallback_scalar_type result = 0;
                x.for_each_component([&](size_t i) -> void { result += x.at(i); } );
                return std::sqrt(result);
            }
                
            fallback_scalar_type distance (const vector_adapter& p0 , const vector_adapter& p1 )
            {
                return length(p0 - p1);
            }

            static fallback_scalar_type dot(vector_adapter x, const vector_adapter& y)
            {
                fallback_scalar_type result = 0.0;
                x.for_each_component( [&](size_t i) -> void { result += x.at(i) * y.at(i); } );
                return result;
            }
            
            static vector_adapter normalize(const vector_adapter& x)
            {
                return x / length(x);
            }  

                // vec3 cross (vec3 x , vec3 y )                      
                                       
                // vec4 ftransform()                                  
                // genType faceforward(genType N, genType I, genType Nref )


        };


       


    }
}


#endif  HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_ADAPTER
