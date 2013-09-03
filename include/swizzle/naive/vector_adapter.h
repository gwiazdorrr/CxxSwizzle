#pragma once

#include <type_traits>
#include <algorithm>
#include <iterator>
#include <functional>
#include <stdexcept>
#include <cmath>
#include <iosfwd>

#include <swizzle/detail/utils.h>
#include <swizzle/detail/vector_binary_operators.h>
#include <swizzle/detail/vector_data.h>
#include <swizzle/naive/vector_traits_impl.h>
#include <swizzle/naive/indexed_proxy.h>

namespace swizzle
{
    namespace naive
    {
        template <template <class, size_t> class TVector, class TScalar, size_t Size>
        struct vector_adapter_helper
        {
            typedef TVector<TScalar, 1> vec1;
            typedef TVector<TScalar, 2> vec2;
            typedef TVector<TScalar, 3> vec3;
            typedef TVector<TScalar, 4> vec4;
            
            typedef TScalar scalar_type;
            typedef detail::binary_operators::tag tag_type;
            static const size_t num_of_components = Size;

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

        template < class TScalar, size_t Size >
        class vector_adapter : 
            private detail::binary_operators::tag,
            public detail::vector_data< TScalar, Size, vector_adapter_helper<vector_adapter, TScalar, Size>::template proxy_factory>
        {
            typedef vector_adapter_helper< ::swizzle::naive::vector_adapter, TScalar, Size> helper_type;

            //! A convenient mnemonic for base type
            typedef detail::vector_data< TScalar, Size, helper_type::template proxy_factory > base_type;

            //! "Hide" _data
            using base_type::m_data;
            
            template <class TOtherScalar, size_t OtherSize>
            friend std::ostream& operator<<(std::ostream&, const ::swizzle::naive::vector_adapter<TOtherScalar, OtherSize>&);
            template <class TOtherScalar, size_t OtherSize>
            friend std::istream& operator>>(std::istream&, ::swizzle::naive::vector_adapter<TOtherScalar, OtherSize>&);

        public:
            //! Number of components of this vector.
            static const size_t num_of_components = base_type::num_of_components;
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

            //! The reason why this is here is that the enable_if in implicit constructor seemed to confuse g++ (4.8.1) with message "sorry, unimplemented: mangling error_mark".
            //! Sneaking the enable if in a static function works, strangely.
            template <class T>
            static void* enable_if_workaround( typename std::enable_if<num_of_components==1 && std::is_convertible<T, scalar_type>::value, void>::type* = 0 );

            //! Implicit constructor from scalar-convertible only for one-component vector
            //! The reason why this is done with a template is that if a type decays to a scalar it can not be 
            //! passed directly to a function taking vector - implicit conversion to scalar then implicit construction
            //! just won't happen.
            template <class T>
            vector_adapter( const T& t, decltype( enable_if_workaround<T>() ) = 0 )
            {
                iterate( [&](size_t i) -> void { at(i) = t; } );
            }

            //! For vectors bigger than 1 conversion from scalar should be explicit.
            explicit vector_adapter( typename std::conditional<num_of_components!=1, scalar_type, detail::operation_not_available>::type s )
            {
                iterate( [&](size_t i) -> void { at(i) = s; } );
            }

            //! A composite constructor variant with 1 argument
            //! Note that for types convertibles to scalar type the instantiation will fail effectively falling back to one of previous two constructors
            template <class T1>
            explicit vector_adapter( T1&& v1,
                typename std::enable_if< !std::is_convertible<T1, scalar_type>::value && detail::are_sizes_valid<num_of_components, detail::get_vector_size<T1>::value>::value, void>::type* = 0 )
            {
                compose<0>(std::forward<T1>(v1));
            }

            //! A composite constructor variant with 2 arguments
            template <class T1, class T2>
            vector_adapter( T1&& v1, T2&& v2,
                typename std::enable_if< detail::are_sizes_valid<num_of_components, detail::get_vector_size<T1>::value, detail::get_vector_size<T2>::value>::value, void>::type* = 0 )
            {
                compose<0>(std::forward<T1>(v1));
                compose<detail::get_vector_size<T1>::value>(std::forward<T2>(v2));
            }

            //! A composite constructor variant with 3 arguments
            template <class T1, class T2, class T3>
            vector_adapter( T1&& v1, T2&& v2, T3&& v3,
                typename std::enable_if< detail::are_sizes_valid<num_of_components, detail::get_vector_size<T1>::value, detail::get_vector_size<T2>::value, detail::get_vector_size<T3>::value>::value, void>::type* = 0 )
            {
                compose<0>(std::forward<T1>(v1));
                compose<detail::get_vector_size<T1>::value>(std::forward<T2>(v2));
                compose<detail::get_vector_size<T1>::value + detail::get_vector_size<T2>::value>(std::forward<T3>(v3));
            }

            //! A composite constructor variant with 4 arguments
            template <class T1, class T2, class T3, class T4>
            vector_adapter( T1&& v1, T2&& v2, T3&& v3, T4&& v4,
                typename std::enable_if< detail::are_sizes_valid<num_of_components, detail::get_vector_size<T1>::value, detail::get_vector_size<T2>::value, detail::get_vector_size<T3>::value, detail::get_vector_size<T4>::value>::value, void>::type* = 0 )
            {
                compose<0>(std::forward<T1>(v1));
                compose<detail::get_vector_size<T1>::value>(std::forward<T2>(v2));
                compose<detail::get_vector_size<T1>::value + detail::get_vector_size<T2>::value>(std::forward<T3>(v3));
                compose<detail::get_vector_size<T1>::value + detail::get_vector_size<T2>::value + detail::get_vector_size<T3>::value>(std::forward<T4>(v4));
            }

        public:
            auto begin() -> decltype( std::begin(m_data) )
            {
                return std::begin(m_data);
            }
            auto begin() const -> decltype( std::begin(*((const data_type*)0)) )
            {
                return std::begin(m_data);
            }
            auto end() -> decltype ( std::end(m_data) )
            {
                return std::end(m_data);
            }
            auto end() const -> decltype ( std::end(*((const data_type*)0)) )
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

            // vector operators

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

            // scalar operators

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

            //! Decays into scalar_type for a single component vector
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

            template <size_t N, class TOtherScalar, size_t OtherSize>
            void compose( const vector_adapter<TOtherScalar, OtherSize>& v )
            {
                const size_t limit = (N + OtherSize > num_of_components) ? num_of_components : (N + OtherSize);
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
                static const scalar_type pi = scalar_type(3.14159265358979323846);
                return transform_and_decay(degrees, [](scalar_type a) -> scalar_type { return (pi * a / 180); } );
            }
            static decay_type degrees(vector_adapter radians)
            {
                static const scalar_type pi = scalar_type(3.14159265358979323846);
                return transform_and_decay(radians, [](scalar_type a) -> scalar_type { return scalar_type(180 * a / pi); } );
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
            static decay_type atan(vector_adapter y, const vector_adapter& x)
            {
                return transform_and_decay(y, x, std::atan2);
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
                const scalar_type c_log2e = 1.44269504088896340736;
                return transform_and_decay(x, [=](scalar_type a) -> scalar_type { return scalar_type( std::log(a) / c_log2e); } );
            }
            static decay_type sqrt(vector_adapter x)
            {
                return transform_and_decay(x, std::sqrt );
            }
            static decay_type inversesqrt(vector_adapter x)
            {
                vector_adapter one( scalar_type(1) );
                return one /= sqrt(x);
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
                const scalar_type& (*func)(const scalar_type&, const scalar_type&) = std::min<scalar_type>;
                return transform_and_decay(x, y, func);
            }
            static decay_type min(vector_adapter x, amiguity_protected_scalar_type y)
            {
                return transform_and_decay(x, [=](scalar_type a) -> scalar_type { return std::min(a, y); } );
            }
            static decay_type max(vector_adapter x, const vector_adapter& y)
            {
                const scalar_type& (*func)(const scalar_type&, const scalar_type&) = std::max<scalar_type>;
                return transform_and_decay(x, y, func);
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


        template < class TScalar, size_t Size >
        std::ostream& operator<<(std::ostream& os, const vector_adapter<TScalar, Size>& vec)
        {
            vec.iterate( [&](size_t i) -> void { os << vec[i] << (i == vec.size() - 1 ? "" : ","); } );
            return os;
        }

        template < class TScalar, size_t Size >
        std::istream& operator>>(std::istream& is, vector_adapter<TScalar, Size>& vec)
        {
            vec.iterate( [&](size_t i) -> void 
            { 
                is >> vec[i];
                if ( i < vec.size() - 1 && is.peek() == ',')
                {
                    is.ignore(1);
                }
            });
            return is;
        }
    }
}
