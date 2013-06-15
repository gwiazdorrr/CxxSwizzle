#ifndef HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_ADAPTER
#define HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_ADAPTER

#include <type_traits>
#include "vector_traits.h"
#include "../detail/vector_binary_operators.h"
#include "../detail/vector_functions.h"
#include "vector_data.h"
#include "scalars.h"
#include <type_traits>
#include "../detail/utils.h"
#include "../detail/vector_traits.h"
#include <algorithm>
#include <iterator>

#define _USE_MATH_DEFINES
#include <cmath>

namespace swizzle
{
    namespace naive
    {
        template < class TScalar, size_t NumComponents > 
        class vector_adapter : 
            public naive_vector_data< vector_adapter, TScalar, NumComponents >, 
            public detail::default_vector_unary_operators_tag,
            public detail::default_functions_tag
        {
            typedef naive_vector_data< ::swizzle::naive::vector_adapter, TScalar, NumComponents > base_type;

        public:
            static const size_t num_of_components = base_type::num_of_components;
            using base_type::_components;

            typedef vector_adapter vector_type;
            typedef typename base_type::scalar_type scalar_type;
            typedef double fallback_scalar_type;

            typedef typename std::conditional<num_of_components==1, scalar_type, vector_adapter>::type result_type;
            typedef typename std::conditional<num_of_components==1, detail::scalar_proxy<scalar_type>, vector_adapter>::type result_type_proxy;

        public:
            typedef scalar_type array_type[num_of_components];
            typedef scalar_type* iterator;
            typedef const scalar_type* const_iterator;
            typedef std::reverse_iterator<iterator> reverse_iterator;
            typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

        public:
            //! Default constructor.
            vector_adapter()
            {
                for_each_component( [&](size_t i) -> void { at(i) = 0; } );
            }

            //! Copy constructor
            vector_adapter( const vector_adapter& o )
            {
                for_each_component( [&](size_t i) -> void { at(i) = o.at(i); } );
            }

            //! Constructor from scalar for 1-component vector should be implicit
            vector_adapter( typename std::conditional<num_of_components == 1, scalar_type, detail::not_available>::type o )
            {
                for_each_component( [&](size_t i) -> void { at(i) = o; } );
            }
            //! Constructor from scalar for 2 and more component vectors are explicit
            explicit vector_adapter( typename std::conditional<num_of_components == 1, detail::not_available, scalar_type>::type o )
            {
                for_each_component( [&](size_t i) -> void { at(i) = o; } );
            }

            //! Helper constructor from an array.
            explicit vector_adapter( const array_type& values )
            {
                for_each_component( [&](size_t i) -> void { at(i) = values[i]; } );
            }

            //! A composite constructor variant with 1 argument
            template <class T1>
            explicit vector_adapter( const T1& v1, 
                typename std::enable_if< detail::are_sizes_valid<num_of_components, detail::vector_traits<T1>::num_of_components, 0, 0, 0>::value, void>::type* = 0 )
            {
                compose<0>(v1);
            }

            //! A composite constructor variant with 2 arguments
            template <class T1, class T2>
            vector_adapter( const T1& v1, const T2& v2,
                typename std::enable_if< detail::are_sizes_valid<num_of_components, detail::vector_traits<T1>::num_of_components, detail::vector_traits<T2>::num_of_components, 0, 0>::value, void>::type* = 0 )
            {
                compose<0>(v1);
                compose<detail::vector_traits<T1>::num_of_components>(v2);
            }

            //! A composite constructor variant with 3 arguments
            template <class T1, class T2, class T3>
            vector_adapter( const T1& v1, const T2& v2, const T3& v3,
                typename std::enable_if< detail::are_sizes_valid<num_of_components, detail::vector_traits<T1>::num_of_components, detail::vector_traits<T2>::num_of_components, detail::vector_traits<T3>::num_of_components, 0>::value, void>::type* = 0 )
            {
                compose<0>(v1);
                compose<detail::vector_traits<T1>::num_of_components>(v2);
                compose<detail::vector_traits<T1>::num_of_components + detail::vector_traits<T2>::num_of_components>(v3);
            }

            //! A composite constructor variant with 4 arguments
            template <class T1, class T2, class T3, class T4>
            vector_adapter( const T1& v1, const T2& v2, const T3& v3, const T4& v4,
                typename std::enable_if< detail::are_sizes_valid<num_of_components, detail::vector_traits<T1>::num_of_components, detail::vector_traits<T2>::num_of_components, detail::vector_traits<T3>::num_of_components, detail::vector_traits<T4>::num_of_components>::value, void>::type* = 0 )
            {
                compose<0>(v1);
                compose<detail::vector_traits<T1>::num_of_components>(v2);
                compose<detail::vector_traits<T1>::num_of_components + detail::vector_traits<T2>::num_of_components>(v3);
                compose<detail::vector_traits<T1>::num_of_components + detail::vector_traits<T2>::num_of_components + detail::vector_traits<T3>::num_of_components>(v4);
            }

        public:
            iterator begin()
            {
                return _components;
            }
            const_iterator begin() const
            {
                return _components;
            }
            iterator end()
            {
                return _components + num_of_components;
            }
            const_iterator end() const
            {
                return _components + num_of_components;
            }
            reverse_iterator rbegin()
            {
                return reverse_iterator(end());
            }
            const_reverse_iterator rbegin() const
            {
                return const_reverse_iterator(end());
            }
            reverse_iterator rend()
            {
                return reverse_iterator(begin());
            }
            const_reverse_iterator rend() const
            {
                return reverse_iterator(begin());
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

            typename std::conditional<num_of_components==1, vector_adapter, detail::not_available>::type& operator++()
            {
                ++_components[0];
                return *this;
            }

            typename std::conditional<num_of_components==1, vector_adapter, detail::not_available>::type operator++(int)
            {
                auto copy = *this;
                ++(*this);
                return copy;
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

            template <class Func>
            const vector_type& for_each_component(Func func) const
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

            template <size_t NStart, size_t NEnd, class Func>
            const vector_type& for_each_component_custom(Func func, std::integral_constant<size_t, NStart>, std::integral_constant<size_t, NEnd>) const
            {
                static_assert( NStart >= 0 && NEnd <= num_of_components && NStart < NEnd, "Out of bounds" );
                for (size_t i = NStart; i < NEnd; ++i)
                {
                    func(i);
                }
                return *this;
            }


            template <class T>
            operator typename vector_adapter<T, num_of_components>() const
            {
                vector_adapter<T, num_of_components> result;
                return result.for_each_component( [&](size_t i) -> void { result.at(i) = at(i); } );
            }

        private:

            operator typename std::conditional<num_of_components==1, result_type_proxy, detail::not_available>::type()
            {
                //static_assert(num_of_components == 1);
                return _components[0];
            }

            template <size_t N>
            void compose(scalar_type v)
            {
                _components[N] = v;
            }

            template <size_t N, class TScalar, size_t NumOfComponents>
            void compose( const vector_adapter<TScalar, NumOfComponents>& v )
            {
                for_each_component_custom([&](size_t i) -> void { at(i) = v.at(i - N); },
                    std::integral_constant<size_t, N>(),
                    std::integral_constant<size_t, (N + NumOfComponents > num_of_components) ? num_of_components : (N + NumOfComponents)>()
                );
            }

            template <size_t N, class TVector, class TScalar, size_t NumOfComponents, size_t x, size_t y, size_t z, size_t w>
            void compose( const vector_proxy<TVector, TScalar, NumOfComponents, x, y, z, w>& v )
            {
                compose<N>( v.operator TVector() );
            }

        public:

            //\1template <class T>\n\1SWIZZLE_DETAIL_VECTOR(T) \2(T&& \3)\n\1{\n\1    return SWIZZLE_DETAIL_VECTOR(T)::\2(\3);\n\1}
            //\1static vector_adapter \2(vector_adapter \3)\n\1{\n\1    return \3.transform_func( std::\2 );\n\1}
            static result_type_proxy add(vector_adapter a, const vector_adapter& b)
            {
                return a += b;
            }
            static result_type_proxy sub(vector_adapter a, const vector_adapter& b)
            {
                return a -= b;
            }
            static result_type_proxy mul(vector_adapter a, const vector_adapter& b)
            {
                return a *= b;
            }
            static result_type_proxy div(vector_adapter a, const vector_adapter& b)
            {
                return a /= b;
            }

            static result_type_proxy radians(vector_adapter degrees)
            {
                return degrees.transform( [](scalar_type a) -> scalar_type { return scalar_type(M_PI * a / 180) } );
            }
            static result_type_proxy degrees(vector_adapter radians)
            {
                return radians.transform( [](scalar_type a) -> scalar_type { return scalar_type(180 * a / M_PI) } );
            }   
            static result_type_proxy sin(vector_adapter angle)
            {
                return angle.transform_func( std::sin );
            }         
            static result_type_proxy cos(vector_adapter angle)
            {
                return angle.transform_func( std::cos );
            }         
            static result_type_proxy tan(vector_adapter angle)
            {
                return angle.transform_func( std::tan );
            }         
            static result_type_proxy asin(vector_adapter x)
            {
                return x.transform_func( std::asin );
            }            
            static result_type_proxy acos(vector_adapter x)
            {
                return x.transform_func( std::acos );
            }            
            static result_type_proxy atan(const vector_adapter& y, const vector_adapter& x)
            {
                return atan(y / x);
            }
            static result_type_proxy atan(vector_adapter y_over_x)
            {
                return y_over_x.transform_func( std::atan );
            } 

            // Exponential Functions 
            static result_type_proxy pow(vector_adapter x, const vector_adapter& y)
            {
                return x.transform(y, [](scalar_type a, scalar_type b) -> scalar_type { return std::pow(a, b); } );
            }
            static result_type_proxy exp(vector_adapter x)
            {
                return x.transform_func( std::exp );
            }             
            static result_type_proxy log(vector_adapter x)
            {
                return x.transform_func( std::log );
            }             
            static result_type_proxy exp2(const vector_adapter& x)
            {
                return pow( vector_adapter(2), x);
            }            
            static result_type_proxy log2(vector_adapter x)
            {
                return x.transform( [](scalar_type a) -> scalar_type { return scalar_type( std::log(a) / M_LOG2E); } );
            }            
            static result_type_proxy sqrt(vector_adapter x)
            {
                return x.transform_func( std::sqrt );
            }            
            static result_type_proxy inversesqrt(vector_adapter x)
            {
                return 1 / sqrt(x);
            }
            

            
            
            static result_type_proxy abs(vector_adapter x)
            {
                return x.transform_func( std::abs );
            }                          
            static result_type_proxy sign(vector_adapter x)
            {
                return x.transform( [](scalar_type a) -> scalar_type { return a > 0 ? 1 : ( a < 0 ? -1 : 0 ); } );
            }                                    
            static result_type_proxy floor(vector_adapter x)
            {
                return x.transform_func( std::floor );
            }                          
            static result_type_proxy ceil(vector_adapter x)
            {
                return x.transform_func( std::ceil );
            }                           
            static result_type_proxy fract(const vector_adapter& x)
            {
                return x - floor(x);
            }                          
            static result_type_proxy mod(vector_adapter x, fallback_scalar_type y)
            {
                return x.transform( [=](scalar_type a) -> scalar_type { return std::fmod(static_cast<fallback_scalar_type>(a), y); } );
            }                   
            static result_type_proxy mod(vector_adapter x, const vector_adapter& y)
            {
                return x.transform(y, [](scalar_type a, scalar_type b) -> scalar_type { return std::fmod(a, b); } );
            }                 
            static result_type_proxy min(vector_adapter x, const vector_adapter& y)
            {
                return x.transform(y, [](scalar_type a, scalar_type b) -> scalar_type { return std::min(a, b); } );
            }                 
            static result_type_proxy min(vector_adapter x, fallback_scalar_type y)
            {
                return x.transform( [=](scalar_type a) -> scalar_type { return std::min<fallback_scalar_type>(a, y); } );
            }   
            static result_type_proxy max(vector_adapter x, const vector_adapter& y)
            {
                return x.transform(y, [](scalar_type a, scalar_type b) -> scalar_type { return std::max(a, b); } ); 
            }                 
            static result_type_proxy max(vector_adapter x, fallback_scalar_type y)
            {
                return x.transform( [=](scalar_type a) -> scalar_type { return std::max<fallback_scalar_type>(a, y); } );
            }   
            static result_type_proxy clamp(vector_adapter x, const vector_adapter& minVal, const vector_adapter& maxVal)
            {
                return max( result_type(min(x, maxVal)), minVal);
            }
            static result_type_proxy clamp(vector_adapter x, fallback_scalar_type minVal, fallback_scalar_type maxVal)
            {
                return max( result_type(min(x, maxVal)), minVal);
            }
            static result_type_proxy mix(vector_adapter x, const vector_adapter& y, const vector_adapter& a)
            {
                return x.for_each_component([&](size_t i) -> void { x.at(i) = x.at(i) + a.at(i) * (y.at(i) - x.at(i)); } );
            }
            static result_type_proxy mix(vector_adapter x, const vector_adapter& y, const typename vector_adapter<scalar_type, 1> a)
            {
                return x.for_each_component([&](size_t i) -> void { x.at(i) = x.at(i) + a * (y.at(i) - x.at(i)); } );
            }
            static result_type_proxy step(const vector_adapter& edge, vector_adapter x)
            {
                return x.transform(edge, [](scalar_type a, scalar_type b) -> scalar_type { return a > b ? 1 : 0; });
            }               
            static result_type_proxy step(fallback_scalar_type edge, vector_adapter x)
            {
                return x.transform([=](scalar_type a) -> scalar_type { return a > edge ? 1 : 0; });
            }               
            static result_type_proxy smoothstep(const vector_adapter& edge0, const vector_adapter& edge1, vector_adapter x)
            {
                auto t = clamp((x - edge0) / (edge1 - edge0), 0, 1);
                return t * t * (3 - 2 * t);
            }
            static result_type_proxy smoothstep(fallback_scalar_type edge0, fallback_scalar_type edge1, vector_adapter x)
            {
                auto t = clamp((x - edge0) / (edge1 - edge0), 0, 1);
                return t * t * (3.0f - 2.0f * t);
            }
            static result_type_proxy reflect(const vector_adapter& I, const vector_adapter& N)
            {
                return I - 2 * dot(I, N) * N;
            }       

            // Geometric functions
            static fallback_scalar_type length(const vector_adapter& x)
            {
                fallback_scalar_type result = 0;
                x.for_each_component([&](size_t i) -> void { result += x.at(i) * x.at(i); } );
                return std::sqrt(result);
            }
                
            fallback_scalar_type distance (const vector_adapter& p0 , const vector_adapter& p1 )
            {
                return length(p0 - p1);
            }

            static fallback_scalar_type dot(const vector_adapter& x, const vector_adapter& y)
            {
                fallback_scalar_type result = 0.0;
                x.for_each_component( [&](size_t i) -> void { result += x.at(i) * y.at(i); } );
                return result;
            }
            
            static result_type_proxy normalize(const vector_adapter& x)
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
