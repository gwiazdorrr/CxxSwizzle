#ifndef HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_ADAPTER
#define HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_ADAPTER

#include <type_traits>
#include <algorithm>
#include <iterator>
#include <functional>
#define _USE_MATH_DEFINES
#include <cmath>

#include "vector_traits.h"
#include "vector_data.h"
#include "vector_adapter_traits.h"
#include "../detail/utils.h"

namespace swizzle
{
    namespace naive
    {
        template < class TTraits > 
        class vector_adapter : 
            public naive_vector_data< vector_adapter, TTraits, TTraits::num_of_components >, 
            public TTraits::tag_type
        {
            typedef naive_vector_data< ::swizzle::naive::vector_adapter, TTraits, TTraits::num_of_components > base_type;

        public:
            static const size_t num_of_components = base_type::num_of_components;
            using base_type::_components;

            typedef vector_adapter vector_type;
            typedef typename base_type::scalar_type scalar_type;
            typedef double fallback_scalar_type;

            typedef typename std::conditional<num_of_components==1, scalar_type, vector_adapter>::type result_type;

        public:
            typedef scalar_type* iterator;
            typedef const scalar_type* const_iterator;
            typedef std::reverse_iterator<iterator> reverse_iterator;
            typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

        private:
            template <class T>
            struct get_num_of_components
            {
                static const size_t value = detail::get_vector_type<T>::type::num_of_components;
            };

            class not_available
            {
                not_available() {}
            };

        public:
            //! Default constructor.
            vector_adapter()
            {
                iterate( [&](size_t i) -> void { at(i) = 0; } );
            }

            //! Copy constructor
            vector_adapter( const vector_adapter& o )
            {
                std::copy(o._components, o._components + num_of_components, _components);
            }

            //! Move constructor
            vector_adapter( vector_adapter&& o )
            {
                std::copy(o._components, o._components + num_of_components, _components);
            }

            //! Constructor from scalar for 1-component vector should be implicit
            vector_adapter( typename std::conditional<num_of_components == 1, scalar_type, not_available>::type o )
            {
                iterate( [&](size_t i) -> void { at(i) = o; } );
            }
            //! Constructor from scalar for 2 and more component vectors are explicit
            explicit vector_adapter( typename std::conditional<num_of_components == 1, not_available, scalar_type>::type o )
            {
                iterate( [&](size_t i) -> void { at(i) = o; } );
            }

            //! A composite constructor variant with 1 argument
            template <class T1>
            explicit vector_adapter( const T1& v1, 
                typename std::enable_if< detail::are_sizes_valid<num_of_components, get_num_of_components<T1>::value, 0, 0, 0>::value, void>::type* = 0 )
            {
                compose<0>(v1);
            }

            //! A composite constructor variant with 2 arguments
            template <class T1, class T2>
            vector_adapter( const T1& v1, const T2& v2,
                typename std::enable_if< detail::are_sizes_valid<num_of_components, get_num_of_components<T1>::value, get_num_of_components<T2>::value, 0, 0>::value, void>::type* = 0 )
            {
                compose<0>(v1);
                compose<get_num_of_components<T1>::value>(v2);
            }

            //! A composite constructor variant with 3 arguments
            template <class T1, class T2, class T3>
            vector_adapter( const T1& v1, const T2& v2, const T3& v3,
                typename std::enable_if< detail::are_sizes_valid<num_of_components, get_num_of_components<T1>::value, get_num_of_components<T2>::value, get_num_of_components<T3>::value, 0>::value, void>::type* = 0 )
            {
                compose<0>(v1);
                compose<get_num_of_components<T1>::value>(v2);
                compose<get_num_of_components<T1>::value + get_num_of_components<T2>::value>(v3);
            }

            //! A composite constructor variant with 4 arguments
            template <class T1, class T2, class T3, class T4>
            vector_adapter( const T1& v1, const T2& v2, const T3& v3, const T4& v4,
                typename std::enable_if< detail::are_sizes_valid<num_of_components, get_num_of_components<T1>::value, get_num_of_components<T2>::value, get_num_of_components<T3>::value, get_num_of_components<T4>::value>::value, void>::type* = 0 )
            {
                compose<0>(v1);
                compose<get_num_of_components<T1>::value>(v2);
                compose<get_num_of_components<T1>::value + get_num_of_components<T2>::value>(v3);
                compose<get_num_of_components<T1>::value + get_num_of_components<T2>::value + get_num_of_components<T3>::value>(v4);
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
                iterate( [&](size_t i) -> void { at(i) = o.at(i); } );
                return *this;
            }

            vector_type operator-() const
            {
                vector_type result;
                iterate([&](size_t i) -> void { result.at(i) = -at(i); });
                return result;
            }

            vector_type& operator+=(const vector_type& o)
            {
                iterate( [&](size_t i) -> void { at(i) += o.at(i); } );
                return *this;
            }
            vector_type& operator-=(const vector_type& o)
            {
                iterate( [&](size_t i) -> void { at(i) -= o.at(i); } );
                return *this;
            }
            vector_type& operator*=(const vector_type& o)
            {
                iterate( [&](size_t i) -> void { at(i) *= o.at(i); } );
                return *this;
            }
            vector_type& operator/=(const vector_type& o)
            {
                iterate( [&](size_t i) -> void { at(i) /= o.at(i); } );
                return *this;
            }

            vector_type& operator+=(const scalar_type& o)
            {
                iterate( [&](size_t i) -> void { at(i) += o; } );
                return *this;
            }
            vector_type& operator-=(const scalar_type& o)
            {
                iterate( [&](size_t i) -> void { at(i) -= o; } );
                return *this;
            }
            vector_type& operator*=(const scalar_type& o)
            {
                iterate( [&](size_t i) -> void { at(i) *= o; } );
                return *this;
            }
            vector_type& operator/=(const scalar_type& o)
            {
                iterate( [&](size_t i) -> void { at(i) /= o; } );
                return *this;
            }

            typename std::conditional<num_of_components==1, vector_adapter, not_available>::type& operator++()
            {
                ++_components[0];
                return *this;
            }

            typename std::conditional<num_of_components==1, vector_adapter, not_available>::type operator++(int)
            {
                auto copy = *this;
                ++(*this);
                return copy;
            }

            //template <class TTraits>
            //operator typename vector_adapter<TTag, T, num_of_components>() const
            //{
            //    vector_adapter<TTag, T, num_of_components> result;
            //    result.iterate( [&](size_t i) -> void { result.at(i) = at(i); } );
            //    return result;
            //}

            scalar_type& at(size_t i)
            {
                return _components[i];
            }

            const scalar_type& at(size_t i) const
            {
                return _components[i];
            }

        private:
            template <class Func>
            void iterate(Func func) const
            {
                iterate<0, num_of_components>(func);
            }

            template <size_t NStart, size_t NEnd, class Func>
            void iterate(Func func, std::integral_constant<size_t, NStart> = std::integral_constant<size_t, NStart>(), std::integral_constant<size_t, NEnd> = std::integral_constant<size_t, NEnd>()) const
            {
                static_assert( NStart >= 0 && NEnd <= num_of_components && NStart < NEnd, "Out of bounds" );
                for (size_t i = NStart; i < NEnd; ++i)
                {
                    func(i);
                }
            }



        private:



            template <size_t N>
            void compose(scalar_type v)
            {
                _components[N] = v;
            }

            template <size_t N, class TTraits>
            void compose( const vector_adapter<TTraits>& v )
            {
                const size_t limit = (N + TTraits::num_of_components > num_of_components) ? num_of_components : (N + TTraits::num_of_components);
                iterate<N, limit>([&](size_t i) -> void { at(i) = v.at(i - N); });
            }

            template <size_t N, template <class> class TVector, class TTraits, size_t x, size_t y, size_t z, size_t w>
            void compose( const vector_proxy<TVector, TTraits, x, y, z, w>& v )
            {
                compose<N>( v.operator TVector<TTraits>() );
            }

        private:
            static vector_adapter& as_result_inner(vector_adapter& x, std::false_type)
            {
                return x;
            }

            static scalar_type as_result_inner(vector_adapter& x, std::true_type)
            {
                return x._components[0];
            }

            static auto as_result(vector_adapter& x)
                -> decltype( as_result_inner(x, typename std::conditional<num_of_components==1, std::true_type, std::false_type>::type()) )
            {
                return as_result_inner(x, typename std::conditional<num_of_components==1, std::true_type, std::false_type>::type());
            }

            static result_type transform(vector_type& x, scalar_type (*func)(scalar_type))
            {
                return transform<scalar_type (*)(scalar_type)>(x, func);
            }

            static result_type transform(vector_type& x, const vector_type& y, scalar_type (*func)(scalar_type, scalar_type))
            {
                return transform<scalar_type (*)(scalar_type, scalar_type)>(x, y, func);
            }

            template <class Func>
            static result_type transform(vector_type& x, Func func)
            {
                x.iterate( [&](size_t i) -> void { x.at(i) = func(x.at(i)); } );
                return as_result(x);
            }

            template <class Func>
            static result_type transform(vector_type& x, const vector_type& y, Func func)
            {
                x.iterate( [&](size_t i) -> void { x.at(i) = func(x.at(i), y.at(i)); } );
                return as_result(x);
            }

        public:

            //\1template <class T>\n\1SWIZZLE_DETAIL_VECTOR(T) \2(T&& \3)\n\1{\n\1    return SWIZZLE_DETAIL_VECTOR(T)::\2(\3);\n\1}
            //\1static vector_adapter \2(vector_adapter \3)\n\1{\n\1    return \3.transform( std::\2 );\n\1}
            static result_type add(vector_adapter a, const vector_adapter& b)
            {
                return as_result(a += b);
            }
            static result_type sub(vector_adapter a, const vector_adapter& b)
            {
                return as_result(a -= b);
            }
            static result_type mul(vector_adapter a, const vector_adapter& b)
            {
                return as_result(a *= b);
            }
            static result_type div(vector_adapter a, const vector_adapter& b)
            {
                return as_result(a /= b);
            }

            static result_type add(vector_adapter a, scalar_type b)
            {
                return as_result(a += b);
            }
            static result_type sub(vector_adapter a, scalar_type b)
            {
                return as_result(a -= b);
            }
            static result_type mul(vector_adapter a, scalar_type b)
            {
                return as_result(a *= b);
            }
            static result_type div(vector_adapter a, scalar_type b)
            {
                return as_result(a /= b);
            }

            static result_type radians(vector_adapter degrees)
            {
                return transform(degrees, [](scalar_type a) -> scalar_type { return scalar_type(M_PI * a / 180) } );
            }
            static result_type degrees(vector_adapter radians)
            {
                return transform(radians, [](scalar_type a) -> scalar_type { return scalar_type(180 * a / M_PI) } );
            }   
            static result_type sin(vector_adapter x)
            {
                return transform(x, std::sin);
            }         
            static result_type cos(vector_adapter x)
            {
                return transform(x, std::cos);
            }         
            static result_type tan(vector_adapter x)
            {
                return transform(x, std::tan);
            }         
            static result_type asin(vector_adapter x)
            {
                return transform(x, std::asin);
            }            
            static result_type acos(vector_adapter x)
            {
                return transform(x, std::acos);
            }            
            static result_type atan(const vector_adapter& y, const vector_adapter& x)
            {
                return atan(y / x);
            }
            static result_type atan(vector_adapter y_over_x)
            {
                return transform(y_over_x, std::atan);
            } 

            // Exponential Functions 
            static result_type pow(vector_adapter x, const vector_adapter& y)
            {
                return transform(x, y, std::pow );
            }
            static result_type exp(vector_adapter x)
            {
                return transform(x, std::exp );
            }             
            static result_type log(vector_adapter x)
            {
                return transform(x, std::log );
            }             
            static result_type exp2(const vector_adapter& x)
            {
                return pow( vector_adapter(2), x);
            }            
            static result_type log2(vector_adapter x)
            {
                return transform(x, [](scalar_type a) -> scalar_type { return scalar_type( std::log(a) / M_LOG2E); } );
            }            
            static result_type sqrt(vector_adapter x)
            {
                return transform(x, std::sqrt );
            }            
            static result_type inversesqrt(vector_adapter x)
            {
                return 1 / sqrt(x);
            }
            

            static result_type abs(vector_adapter x)
            {
                return transform(x, std::abs );
            }                          
            static result_type sign(vector_adapter x)
            {
                return transform(x,  [](scalar_type a) -> scalar_type { return ( scalar_type(0) < a ) - ( a < scalar_type(0) ); } );
            }                                    
            static result_type floor(vector_adapter x)
            {
                return transform(x, std::floor );
            }                          
            static result_type ceil(vector_adapter x)
            {
                return transform(x, std::ceil );
            }                           
            static result_type fract(const vector_adapter& x)
            {
                return as_result(x - floor(x));
            }                          
            static result_type mod(vector_adapter x, scalar_type y)
            {
                return mod(x, vector_adapter(y));
            }                   
            static result_type mod(vector_adapter x, const vector_adapter& y)
            {
                return transform(x, y, std::fmod);
            }                 
            static result_type min(vector_adapter x, const vector_adapter& y)
            {
                return transform(x, y, std::min<scalar_type>);
            }                 
            static result_type min(vector_adapter x, scalar_type y)
            {
                return min(x, vector_adapter(y));
            }   
            static result_type max(vector_adapter x, const vector_adapter& y)
            {
                return transform(x, y, std::max<scalar_type>);
            }                 
            static result_type max(vector_adapter x, scalar_type y)
            {
                return max(x, vector_adapter(y));
            }   
            static result_type clamp(vector_adapter x, const vector_adapter& minVal, const vector_adapter& maxVal)
            {
                return max( result_type(min(x, maxVal)), minVal);
            }
            static result_type clamp(vector_adapter x, scalar_type minVal, scalar_type maxVal)
            {
                return max( result_type(min(x, maxVal)), minVal);
            }
            static result_type mix(vector_adapter x, const vector_adapter& y, const vector_adapter& a)
            {
                x.iterate([&](size_t i) -> void { x.at(i) = x.at(i) + a.at(i) * (y.at(i) - x.at(i)); } );
                return as_result(x);
            }
            static result_type mix(vector_adapter x, const vector_adapter& y, scalar_type a)
            {
                x.iterate([&](size_t i) -> void { x.at(i) = x.at(i) + a * (y.at(i) - x.at(i)); } );
                return as_result(x);
            }
            static result_type step(const vector_adapter& edge, vector_adapter x)
            {
                return transform(x, edge, [](scalar_type a, scalar_type b) -> scalar_type { return a > b ? 1 : 0; });
            }               
            static result_type step(scalar_type edge, vector_adapter x)
            {
                return step(vector_adapter(edge), x);
            }               
            static result_type smoothstep(const vector_adapter& edge0, const vector_adapter& edge1, vector_adapter x)
            {
                auto t = clamp((x - edge0) / (edge1 - edge0), 0, 1);
                return as_result(t * t * (3 - 2 * t));
            }
            static result_type smoothstep(scalar_type edge0, scalar_type edge1, vector_adapter x)
            {
                auto t = clamp((x - edge0) / (edge1 - edge0), 0, 1);
                return as_result(t * t * (3.0 - 2.0 * t));
            }
            static result_type reflect(const vector_adapter& I, const vector_adapter& N)
            {
                return as_result(I - 2 * dot(I, N) * N);
            }       

            // Geometric functions
            static fallback_scalar_type length(const vector_adapter& x)
            {
                fallback_scalar_type result = 0;
                x.iterate([&](size_t i) -> void { result += x.at(i) * x.at(i); } );
                return std::sqrt(result);
            }
                
            fallback_scalar_type distance (vector_adapter p0 , const vector_adapter& p1 )
            {
                return length(p0 -= p1);
            }

            static fallback_scalar_type dot(const vector_adapter& x, const vector_adapter& y)
            {
                fallback_scalar_type result = 0.0;
                x.iterate( [&](size_t i) -> void { result += x.at(i) * y.at(i); } );
                return result;
            }
            
            static result_type normalize(vector_adapter x)
            {
                x /= length(x);
                return as_result(x);
            }  

                // vec3 cross (vec3 x , vec3 y )                      
                                       
                // vec4 ftransform()                                  
                // genType faceforward(genType N, genType I, genType Nref )


        };


       


    }
}


#endif  HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_ADAPTER
