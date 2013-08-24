#ifndef HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_ADAPTER
#define HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_ADAPTER

#include <type_traits>
#include <algorithm>
#include <iterator>
#include <functional>
#include <stdexcept>
#define _USE_MATH_DEFINES
#include <cmath>

#include "vector_traits.h"
#include "vector_adapter_traits.h"
#include "vector_proxy.h"
#include "../detail/utils.h"
#include "../detail/vector_data.h"

namespace swizzle
{
    namespace naive
    {
        template <template <class> class TVector, class TTraits>
        struct vector_adapter_helper
        {
            typedef TVector<typename TTraits::change_num_of_components<1>::type> vec1;
            typedef TVector<typename TTraits::change_num_of_components<2>::type> vec2;
            typedef TVector<typename TTraits::change_num_of_components<3>::type> vec3;
            typedef TVector<typename TTraits::change_num_of_components<4>::type> vec4;
            
            typedef typename TTraits::scalar_type scalar_type;
            typedef typename TTraits::tag_type tag_type;
            static const size_t num_of_components = TTraits::num_of_components;

            template <size_t x, size_t y, size_t z, size_t w>
            struct proxy_factory
            {
                typedef indexed_proxy< vec4, std::array<scalar_type, num_of_components>, tag_type, x, y, z, w> type;
            };
            template <size_t x>
            struct proxy_factory<x, -1, -1, -1>
            {
                typedef indexed_proxy< vec1, std::array<scalar_type, num_of_components>, tag_type, x > type;
            };
            template <size_t x, size_t y>
            struct proxy_factory<x, y, -1, -1>
            {
                typedef indexed_proxy< vec2, std::array<scalar_type, num_of_components>, tag_type, x, y > type;
            };
            template <size_t x, size_t y, size_t z>
            struct proxy_factory<x, y, z, -1>
            {
                typedef indexed_proxy< vec3, std::array<scalar_type, num_of_components>, tag_type, x, y, z > type;
            };

        };

        template < class TTraits >
        class vector_adapter : public detail::vector_data<
            typename TTraits::scalar_type,
            TTraits::num_of_components,
            vector_adapter_helper<vector_adapter, TTraits>::proxy_factory>
        {
            typedef vector_adapter_helper< ::swizzle::naive::vector_adapter, TTraits> helper_type;

            //! A convenient mnemonic for base type
            typedef detail::vector_data< typename TTraits::scalar_type, TTraits::num_of_components, helper_type::proxy_factory > base_type;

            //! "Hide" _data
            using base_type::m_data;
            

        public:
            //! Number of components of this vector.
            static const size_t num_of_components = base_type::num_of_components;
            typedef TTraits traits_type;
            //! This type.
            typedef vector_adapter vector_type;
            //! Scalar type.
            typedef typename base_type::data_type data_type;
            //! Scalar type.
            typedef typename base_type::scalar_type scalar_type;
            //! Type static functions return; for single-component they decay to a scalar
            typedef typename std::conditional<num_of_components==1, scalar_type, vector_adapter>::type decay_type;

            //! Sanity checks
            static_assert( sizeof(base_type) == sizeof(scalar_type) * num_of_components, "Size of the base class is not equal to size of its components, most likely empty base class optimisation failed");

            //! Because this type is implicitly constructed of anything convertible to scalar_type if it has one component
            //! this type needs to be used in overloaded functions to avoid ambiguity
            typedef typename std::conditional<num_of_components==1, detail::operation_not_available, scalar_type>::type amiguity_protected_scalar_type;


        public:
            //! Default constructor.
            vector_adapter()
            {
                iterate( [&](size_t i) -> void { at(i) = 0; } );
            }

            //! Copy constructor
            vector_adapter( const vector_adapter& o )
            {
                iterate( [&](size_t i) -> void { at(i) = o[i]; } );
            }

            //! Implicit constructor from scalar-convertible only for one-component vector
            //! The reason why this is done with a template is that if a type decays to a scalar it can not be 
            //! passed directly to a function taking vector - implicit conversion to scalar then implicit construction
            //! just won't happen.
            template <class T>
            vector_adapter( const T& t, typename std::enable_if<num_of_components == 1 && std::is_convertible<T, scalar_type>::value, void>::type* = 0)
            {
                iterate( [&](size_t i) -> void { at(i) = t; } );
            }

            //! For vectors bigger than 1 conversion from scalar should be explicit.
            template <class T>
            explicit vector_adapter( const T& t, typename std::enable_if<num_of_components != 1 && std::is_convertible<T, scalar_type>::value, void>::type* = 0 )
            {
                iterate( [&](size_t i) -> void { at(i) = t; } );
            }

            //! A composite constructor variant with 1 argument
            //! Note that for types convertibles to scalar type the instantiation will fail effectively falling back to one of previous two constructors
            template <class T1>
            explicit vector_adapter( T1&& v1,
                typename std::enable_if< !std::is_convertible<T1, scalar_type>::value && detail::are_sizes_valid<num_of_components, detail::get_num_of_components<T1>::value>::value, void>::type* = 0 )
            {
                compose<0>(std::forward<T1>(v1));
            }

            //! A composite constructor variant with 2 arguments
            template <class T1, class T2>
            vector_adapter( T1&& v1, T2&& v2,
                typename std::enable_if< detail::are_sizes_valid<num_of_components, detail::get_num_of_components<T1>::value, detail::get_num_of_components<T2>::value>::value, void>::type* = 0 )
            {
                compose<0>(std::forward<T1>(v1));
                compose<detail::get_num_of_components<T1>::value>(std::forward<T2>(v2));
            }

            //! A composite constructor variant with 3 arguments
            template <class T1, class T2, class T3>
            vector_adapter( T1&& v1, T2&& v2, T3&& v3,
                typename std::enable_if< detail::are_sizes_valid<num_of_components, detail::get_num_of_components<T1>::value, detail::get_num_of_components<T2>::value, detail::get_num_of_components<T3>::value>::value, void>::type* = 0 )
            {
                compose<0>(std::forward<T1>(v1));
                compose<detail::get_num_of_components<T1>::value>(std::forward<T2>(v2));
                compose<detail::get_num_of_components<T1>::value + detail::get_num_of_components<T2>::value>(std::forward<T3>(v3));
            }

            //! A composite constructor variant with 4 arguments
            template <class T1, class T2, class T3, class T4>
            vector_adapter( T1&& v1, T2&& v2, T3&& v3, T4&& v4,
                typename std::enable_if< detail::are_sizes_valid<num_of_components, detail::get_num_of_components<T1>::value, detail::get_num_of_components<T2>::value, detail::get_num_of_components<T3>::value, detail::get_num_of_components<T4>::value>::value, void>::type* = 0 )
            {
                compose<0>(std::forward<T1>(v1));
                compose<detail::get_num_of_components<T1>::value>(std::forward<T2>(v2));
                compose<detail::get_num_of_components<T1>::value + detail::get_num_of_components<T2>::value>(std::forward<T3>(v3));
                compose<detail::get_num_of_components<T1>::value + detail::get_num_of_components<T2>::value + detail::get_num_of_components<T3>::value>(std::forward<T4>(v4));
            }

        public:
            auto begin() -> decltype( std::begin(m_data) )
            {
                return std::begin(m_data);
            }
            auto begin() const -> decltype( std::begin(m_data) )
            {
                return std::begin(m_data);
            }
            auto end() -> decltype ( std::end(m_data) )
            {
                return std::end(m_data);
            }
            auto end() const -> decltype ( std::end(m_data) )
            {
                return std::end(m_data);
            }

            scalar_type& operator[](size_t i)
            {
                return at(i);
            }
            const scalar_type& operator[](size_t i) const
            {
                return at(i);
            }
            size_t size() const
            {
                return num_of_components;
            }

        public:
            vector_type& operator=(const vector_type& o)
            {
                iterate( [&](size_t i) -> void { at(i) = o[i]; } );
                return *this;
            }

            vector_type operator-() const
            {
                vector_type result;
                iterate([&](size_t i) -> void { result[i] = -at(i); });
                return result;
            }

            vector_type& operator+=(const vector_adapter& o)
            {
                iterate( [&](size_t i) -> void { at(i) += o[i]; } );
                return *this;
            }
            vector_type& operator-=(const vector_adapter& o)
            {
                iterate( [&](size_t i) -> void { at(i) -= o[i]; } );
                return *this;
            }
            vector_type& operator*=(const vector_adapter& o)
            {
                iterate( [&](size_t i) -> void { at(i) *= o[i]; } );
                return *this;
            }
            vector_type& operator/=(const vector_adapter& o)
            {
                iterate( [&](size_t i) -> void { at(i) /= o[i]; } );
                return *this;
            }

            vector_type& operator+=(amiguity_protected_scalar_type o)
            {
                iterate( [&](size_t i) -> void { at(i) += o; } );
                return *this;
            }
            vector_type& operator-=(amiguity_protected_scalar_type o)
            {
                iterate( [&](size_t i) -> void { at(i) -= o; } );
                return *this;
            }
            vector_type& operator*=(amiguity_protected_scalar_type o)
            {
                iterate( [&](size_t i) -> void { at(i) *= o; } );
                return *this;
            }
            vector_type& operator/=(amiguity_protected_scalar_type o)
            {
                iterate( [&](size_t i) -> void { at(i) /= o; } );
                return *this;
            }

            typename std::conditional<num_of_components==1, vector_adapter, swizzle::detail::operation_not_available>::type& operator++()
            {
                ++at(0);
                return *this;
            }

            typename std::conditional<num_of_components==1, vector_adapter, swizzle::detail::operation_not_available>::type operator++(int)
            {
                auto copy = *this;
                ++(*this);
                return copy;
            }

            typename std::conditional<num_of_components==1, vector_adapter, swizzle::detail::operation_not_available>::type& operator--()
            {
                --at(0);
                return *this;
            }

            typename std::conditional<num_of_components==1, vector_adapter, swizzle::detail::operation_not_available>::type operator--(int)
            {
                auto copy = *this;
                --(*this);
                return copy;
            }

            operator typename std::conditional<num_of_components==1, scalar_type, swizzle::detail::operation_not_available>::type()
            {
                return at(0);
            }

        private:

            const scalar_type& at(size_t i) const
            {
                return m_data[i];
            }
            scalar_type& at(size_t i)
            {
                return m_data[i];
            }


            template <class Func>
            void iterate(Func func) const
            {
                iterate<0, num_of_components>(func);
            }

            template <size_t NStart, size_t NEnd, class Func>
            void iterate(Func func, std::integral_constant<size_t, NStart> = std::integral_constant<size_t, NStart>(), std::integral_constant<size_t, NEnd> = std::integral_constant<size_t, NEnd>()) const
            {
                static_assert( NStart >= 0 && NEnd <= num_of_components && NStart < NEnd, "Out of bounds" );
                detail::compile_time_for<NStart, NEnd>(func);
            }

            template <size_t N>
            void compose(scalar_type v)
            {
                at(N) = v;
            }

            template <size_t N, class TOtherTraits>
            void compose( const vector_adapter<TOtherTraits>& v )
            {
                const size_t limit = (N + TOtherTraits::num_of_components > num_of_components) ? num_of_components : (N + TOtherTraits::num_of_components);
                iterate<N, limit>([&](size_t i) -> void { at(i) = v[i - N]; });
            }

            template <size_t N, class TVector, class TData, class TTag, size_t x, size_t y, size_t z, size_t w>
            void compose( const indexed_proxy<TVector, TData, TTag, x, y, z, w>& v )
            {
                compose<N>(v.decay());
            }

        private:

            const decay_type& decay() const
            {
                struct selector
                {
                    const vector_adapter& operator()(const vector_adapter& x, vector_adapter*)
                    {
                        return x;
                    }
                    const scalar_type& operator()(const vector_adapter& x, scalar_type*)
                    {
                        return x.at(0);
                    }
                };
                return selector()( *this, static_cast<decay_type*>(nullptr) );
            }

            static decay_type transform_and_decay(vector_type& x, scalar_type (*func)(scalar_type))
            {
                return transform_and_decay<scalar_type (*)(scalar_type)>(x, func);
            }

            static decay_type transform_and_decay(vector_type& x, const vector_type& y, scalar_type (*func)(scalar_type, scalar_type))
            {
                return transform_and_decay<scalar_type (*)(scalar_type, scalar_type)>(x, y, func);
            }

            template <class Func>
            static decay_type transform_and_decay(vector_type& x, Func func)
            {
                x.iterate( [&](size_t i) -> void { x[i] = func(x[i]); } );
                return x.decay();
            }

            template <class Func>
            static decay_type transform_and_decay(vector_type& x, const vector_type& y, Func func)
            {
                x.iterate( [&](size_t i) -> void { x[i] = func(x[i], y[i]); } );
                return x.decay();
            }

        public:

            static decay_type radians(vector_adapter degrees)
            {
                return transform_and_decay(degrees, [](scalar_type a) -> scalar_type { return scalar_type(M_PI * a / 180); } );
            }
            static decay_type degrees(vector_adapter radians)
            {
                return transform_and_decay(radians, [](scalar_type a) -> scalar_type { return scalar_type(180 * a / M_PI); } );
            }
            static decay_type sin(vector_adapter x)
            {
                return transform_and_decay(x, std::sin);
            }
            static decay_type cos(vector_adapter x)
            {
                return transform_and_decay(x, std::cos);
            }
            static decay_type tan(vector_adapter x)
            {
                return transform_and_decay(x, std::tan);
            }
            static decay_type asin(vector_adapter x)
            {
                return transform_and_decay(x, std::asin);
            }
            static decay_type acos(vector_adapter x)
            {
                return transform_and_decay(x, std::acos);
            }
            static decay_type atan(const vector_adapter& y, const vector_adapter& x)
            {
                return atan(y / x);
            }
            static decay_type atan(vector_adapter y_over_x)
            {
                return transform_and_decay(y_over_x, std::atan);
            }

            // Exponential Functions
            static decay_type pow(vector_adapter x, const vector_adapter& y)
            {
                return transform_and_decay(x, y, std::pow );
            }
            static decay_type exp(vector_adapter x)
            {
                return transform_and_decay(x, std::exp );
            }
            static decay_type log(vector_adapter x)
            {
                return transform_and_decay(x, std::log );
            }
            static decay_type exp2(const vector_adapter& x)
            {
                return pow( vector_adapter(2), x);
            }
            static decay_type log2(vector_adapter x)
            {
                return transform_and_decay(x, [](scalar_type a) -> scalar_type { return scalar_type( std::log(a) / M_LOG2E); } );
            }
            static decay_type sqrt(vector_adapter x)
            {
                return transform_and_decay(x, std::sqrt );
            }
            static decay_type inversesqrt(vector_adapter x)
            {
                return 1 / sqrt(x);
            }


            static decay_type abs(vector_adapter x)
            {
                return transform_and_decay(x, std::abs );
            }
            static decay_type sign(vector_adapter x)
            {
                return transform_and_decay(x,  [](scalar_type a) -> scalar_type { return ( scalar_type(0) < a ) - ( a < scalar_type(0) ); } );
            }
            static decay_type floor(vector_adapter x)
            {
                return transform_and_decay(x, std::floor );
            }
            static decay_type ceil(vector_adapter x)
            {
                return transform_and_decay(x, std::ceil );
            }
            static decay_type fract(vector_adapter x)
            {
                return transform_and_decay(x, [=](scalar_type a) -> scalar_type { return a - std::floor(a); } );
            }
            static decay_type mod(vector_adapter x, amiguity_protected_scalar_type y)
            {
                return mod(x, vector_adapter(y));
            }
            static decay_type mod(vector_adapter x, vector_adapter y)
            {
                auto x_div_y = x;
                x_div_y /= y;
                y *= floor(x_div_y);
                x -= y;
                return x.decay();
            }
            static decay_type min(vector_adapter x, const vector_adapter& y)
            {
                return transform_and_decay(x, y, std::min<scalar_type>);
            }
            static decay_type min(vector_adapter x, amiguity_protected_scalar_type y)
            {
                return transform_and_decay(x, [=](scalar_type a) -> scalar_type { return std::min(a, y); } );
            }
            static decay_type max(vector_adapter x, const vector_adapter& y)
            {
                return transform_and_decay(x, y, std::max<scalar_type>);
            }
            static decay_type max(vector_adapter x, amiguity_protected_scalar_type y)
            {
                return transform_and_decay(x, [=](scalar_type a) -> scalar_type { return std::max(a, y); } );
            }
            static decay_type clamp(vector_adapter x, const vector_adapter& minVal, const vector_adapter& maxVal)
            {
                return max( min(x, maxVal), minVal);
            }
            static decay_type clamp(vector_adapter x, amiguity_protected_scalar_type minVal, amiguity_protected_scalar_type maxVal)
            {
                return max( min(x, maxVal), minVal);
            }
            static decay_type mix(vector_adapter x, const vector_adapter& y, const vector_adapter& a)
            {
                x.iterate([&](size_t i) -> void { x[i] = x[i] + a[i] * (y[i] - x[i]); } );
                return x.decay();
            }
            static decay_type mix(vector_adapter x, const vector_adapter& y, amiguity_protected_scalar_type a)
            {
                x.iterate([&](size_t i) -> void { x[i] = x[i] + a * (y[i] - x[i]); } );
                return x.decay();
            }
            static decay_type step(const vector_adapter& edge, vector_adapter x)
            {
                return transform_and_decay(x, edge, [](scalar_type a, scalar_type b) -> scalar_type { return a > b ? 1 : 0; });
            }
            static decay_type step(scalar_type edge, vector_adapter x)
            {
                return step(vector_adapter(edge), x);
            }
            static decay_type smoothstep(const vector_adapter& edge0, const vector_adapter& edge1, vector_adapter x)
            {
                auto a = x;
                a -= edge0;

                auto b = edge1;
                b -= edge0;

                auto t = clamp( a /= b, 0, 1 );
                
                a = vector_adapter( scalar_type(3) );
                a -= t;
                a -= t;
                
                a *= t;
                a *= t;

                return a.decay();
                //auto t = clamp((x - edge0) / (edge1 - edge0), 0, 1);
                //return as_result(t * t * (3 - 2 * t));
            }
            static decay_type smoothstep(amiguity_protected_scalar_type edge0, amiguity_protected_scalar_type edge1, vector_adapter x)
            {
                x -= edge0;
                x /= (edge1 - edge0);
                auto t = clamp(x, 0, 1);
                return t * t * (3.0 - 2.0 * t);
            }
            static decay_type reflect(const vector_adapter& I, const vector_adapter& N)
            {
                return (I - 2 * dot(I, N) * N).decay();
            }

            // Geometric functions
            static scalar_type length(const vector_adapter& x)
            {
                scalar_type result = 0;
                x.iterate([&](size_t i) -> void { result += x[i] * x[i]; } );
                return std::sqrt(result);
            }

            static scalar_type distance (vector_adapter p0 , const vector_adapter& p1 )
            {
                return length(p0 -= p1);
            }

            static scalar_type dot(const vector_adapter& x, const vector_adapter& y)
            {
                scalar_type result = 0;
                x.iterate( [&](size_t i) -> void { result += x[i] * y[i]; } );
                return result;
            }

            static decay_type normalize(vector_adapter x)
            {
                x /= length(x);
                return x.decay();
            }


            static typename std::conditional<num_of_components==3, decay_type, swizzle::detail::operation_not_available>::type cross(const vector_adapter& x, const vector_adapter& y)
            {
                auto rx = x.y*y.z - x.z*y.y;
                auto ry = x.z*y.x - x.x*y.z;
                auto rz = x.x*y.y - x.y*y.x;
                return vector_adapter(rx, ry, rz).decay();
            }
                // vec4 ftransform()
                // genType faceforward(genType N, genType I, genType Nref )
        };





    }
}


#endif // HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_ADAPTER
