//  CxxSwizzle
//  Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr.os@gmail.com>
#pragma once

#include <type_traits>
#include <algorithm>
#include <iterator>
#include <functional>
#include <stdexcept>
#include <cmath>
#include <iostream>
#include <array>

#include <swizzle/glsl/naive_functions_adapter.h>
#include <swizzle/detail/utils.h>
#include <swizzle/detail/vector_binary_operators.h>
#include <swizzle/detail/vector_base.h>
#include <swizzle/naive/vector_traits_impl.h>
#include <swizzle/naive/vector_adapter_helper.h>

#include <boost/operators.hpp>

namespace swizzle
{
    namespace naive
    {
        template < class ScalarType, size_t Size >
        class vector_adapter : 
            // let the helper decide which base class to choose
            public vector_adapter_helper<ScalarType, Size>::base_type,
            // add static glsl functions
            public swizzle::glsl::naive_functions_adapter<
                // enable binary operators; if only one component "disable" scalar operators
                detail::binary_operators< 
                    vector_adapter<ScalarType, Size>, 
                    ScalarType,
                    Size!=1
                >,
                vector_adapter,
                ScalarType,
                Size
            >
        {
            //! A convenient mnemonic for base type
            typedef typename vector_adapter_helper<ScalarType, Size>::base_type base_type;
            //! "Hide" m_data from outside and make it locally visible
            using base_type::m_data;

        public:
            //! Number of components of this vector.
            static const size_t num_of_components = Size;
            //! This type.
            typedef vector_adapter vector_type;
            //! Scalar type.
            typedef std::array<ScalarType, Size> data_type;
            //! Scalar type.
            typedef ScalarType scalar_type;
            //! Type static functions return; for single-component they decay to a scalar
            typedef typename std::conditional<num_of_components==1, scalar_type, vector_adapter>::type decay_type;
            //! Because this type is implicitly constructed of anything convertible to scalar_type if it has one component
            //! this type needs to be used in overloaded functions to avoid ambiguity
            typedef typename std::conditional<num_of_components==1, detail::operation_not_available, scalar_type>::type amiguity_protected_scalar_type;
            //! Sanity checks
            static_assert( sizeof(base_type) == sizeof(scalar_type) * num_of_components, "Size of the base class is not equal to size of its components, most likely empty base class optimisation failed");

        // CONSTRUCTION
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
            vector_adapter( typename std::conditional<num_of_components==1, scalar_type, detail::operation_not_available>::type s )
            {
                iterate( [&](size_t i) -> void { at(i) = s; } );
            }

            //! For vectors bigger than 1 conversion from scalar should be explicit.
            explicit vector_adapter( typename std::conditional<num_of_components!=1, scalar_type, detail::operation_not_available>::type s )
            {
                iterate( [&](size_t i) -> void { at(i) = s; } );
            }

            //! A composite constructor variant with 1 argument
            //! Note that for types convertibles to scalar type the instantiation will fail effectively falling back to one of previous two constructors
            template <class T1>
            explicit vector_adapter( const T1& v1,
                typename std::enable_if< !std::is_convertible<T1, scalar_type>::value && detail::are_sizes_valid<num_of_components, detail::get_vector_size<T1>::value>::value, void>::type* = 0 )
            {
                compose<0>(detail::decay(v1));
            }

            //! A composite constructor variant with 2 arguments
            template <class T1, class T2>
            vector_adapter( const T1& v1, const T2& v2,
                typename std::enable_if< detail::are_sizes_valid<num_of_components, detail::get_vector_size<T1>::value, detail::get_vector_size<T2>::value>::value, void>::type* = 0 )
            {
                compose<0>(detail::decay(v1));
                compose<detail::get_vector_size<T1>::value>(detail::decay(v2));
            }

            //! A composite constructor variant with 3 arguments
            template <class T1, class T2, class T3>
            vector_adapter( const T1& v1, const T2& v2, const T3& v3,
                typename std::enable_if< detail::are_sizes_valid<num_of_components, detail::get_vector_size<T1>::value, detail::get_vector_size<T2>::value, detail::get_vector_size<T3>::value>::value, void>::type* = 0 )
            {
                compose<0>(detail::decay(v1));
                compose<detail::get_vector_size<T1>::value>(detail::decay(v2));
                compose<detail::get_vector_size<T1>::value + detail::get_vector_size<T2>::value>(detail::decay(v3));
            }

            //! A composite constructor variant with 4 arguments
            template <class T1, class T2, class T3, class T4>
            vector_adapter( const T1& v1, const T2& v2, const T3& v3, const T4& v4,
                typename std::enable_if< detail::are_sizes_valid<num_of_components, detail::get_vector_size<T1>::value, detail::get_vector_size<T2>::value, detail::get_vector_size<T3>::value, detail::get_vector_size<T4>::value>::value, void>::type* = 0 )
            {
                compose<0>(detail::decay(v1));
                compose<detail::get_vector_size<T1>::value>(detail::decay(v2));
                compose<detail::get_vector_size<T1>::value + detail::get_vector_size<T2>::value>(detail::decay(v3));
                compose<detail::get_vector_size<T1>::value + detail::get_vector_size<T2>::value + detail::get_vector_size<T3>::value>(detail::decay(v4));
            }

        
        // OPERATORS
        public:

            // Indexing

            scalar_type& operator[](size_t i)
            {
                return at(i);
            }
            const scalar_type& operator[](size_t i) const
            {
                return at(i);
            }

            // Assignment-operation with vector argument

            vector_adapter& operator+=(const vector_adapter& o)
            {
                iterate( [&](size_t i) -> void { at(i) += o[i]; } );
                return *this;
            }
            vector_adapter& operator-=(const vector_adapter& o)
            {
                iterate( [&](size_t i) -> void { at(i) -= o[i]; } );
                return *this;
            }
            vector_adapter& operator*=(const vector_adapter& o)
            {
                iterate( [&](size_t i) -> void { at(i) *= o[i]; } );
                return *this;
            }
            vector_adapter& operator/=(const vector_adapter& o)
            {
                iterate( [&](size_t i) -> void { at(i) /= o[i]; } );
                return *this;
            }

            // Assignment-operation with scalar argument (only for Size > 1)

            vector_adapter& operator+=(amiguity_protected_scalar_type o)
            {
                iterate( [&](size_t i) -> void { at(i) += o; } );
                return *this;
            }
            vector_adapter& operator-=(amiguity_protected_scalar_type o)
            {
                iterate( [&](size_t i) -> void { at(i) -= o; } );
                return *this;
            }
            vector_adapter& operator*=(amiguity_protected_scalar_type o)
            {
                iterate( [&](size_t i) -> void { at(i) *= o; } );
                return *this;
            }
            vector_adapter& operator/=(amiguity_protected_scalar_type o)
            {
                iterate( [&](size_t i) -> void { at(i) /= o; } );
                return *this;
            }

            // Others

            vector_adapter& operator=(const vector_adapter& o)
            {
                iterate( [&](size_t i) -> void { at(i) = o[i]; } );
                return *this;
            }

            vector_adapter operator-() const
            {
                vector_adapter result;
                iterate([&](size_t i) -> void { result[i] = -at(i); });
                return result;
            }


        

        public:
            auto begin() -> decltype( std::begin(m_data) )
            {
                return std::begin(m_data);
            }
            auto begin() const -> decltype(std::begin( detail::declval<const data_type>() ))
            {
                return std::begin(m_data);
            }
            auto end() -> decltype ( std::end(m_data) )
            {
                return std::end(m_data);
            }
            auto end() const -> decltype (std::end( detail::declval<const data_type>() ))
            {
                return std::end(m_data);
            }

            size_t size() const
            {
                return num_of_components;
            }

        private:

            //! Helper accessor
            const scalar_type& at(size_t i) const
            {
                return m_data[i];
            }
            //! Helper accessor
            scalar_type& at(size_t i)
            {
                return m_data[i];
            }

            //! Puts scalar at given position. Used only during construction.
            template <size_t N>
            void compose(scalar_type v)
            {
                at(N) = v;
            }

            //! Puts a vector (or it's part) at given position. Used only during construction.
            template <size_t N, class TOtherScalar, size_t OtherSize>
            void compose( const vector_adapter<TOtherScalar, OtherSize>& v )
            {
                const size_t limit = (N + OtherSize > Size) ? Size : (N + OtherSize);
                iterate<N, limit>([&](size_t i) -> void { at(i) = v[i - N]; });
            }

            //! Iterates over the vector, firing Func for each index
            template <class Func>
            static void iterate(Func func)
            {
                iterate<0, num_of_components>(func);
            }

            //! Iterates over the vector, firing Func for each index
            template <size_t NStart, size_t NEnd, class Func>
            static void iterate(Func func, std::integral_constant<size_t, NStart> = std::integral_constant<size_t, NStart>(), std::integral_constant<size_t, NEnd> = std::integral_constant<size_t, NEnd>())
            {
                static_assert( NStart >= 0 && NEnd <= num_of_components && NStart < NEnd, "Out of bounds" );
                detail::compile_time_for<NStart, NEnd>(func);
            }

        public:
            //! Decays the vector. For Size==1 this is going to return a scalar, for all other sizes - same vector
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

            //! As an inline friend function, because thanks to that all convertibles will use same function.
            friend std::ostream& operator<<(std::ostream& os, const vector_adapter& vec)
            {
                vec.iterate( [&](size_t i) -> void { os << vec[i] << (i == vec.size() - 1 ? "" : ","); } );
                return os;
            }

            //! As an inline friend function, because thanks to that all convertibles will use same function.
            friend std::istream& operator>>(std::istream& is, vector_adapter& vec)
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

        };
    }
}
