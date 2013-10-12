// CxxSwizzle
// Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr.github@gmail.com>
#pragma once

#include <type_traits>
#include <iostream>

#include <swizzle/detail/utils.h>
#include <swizzle/detail/vector_binary_operators.h>
#include <swizzle/detail/vector_base.h>
#include <swizzle/detail/indexed_vector_iterator.h>
#include <swizzle/glsl/naive/vector_functions_adapter.h>
#include <swizzle/glsl/naive/vector_traits_impl.h>
#include <swizzle/glsl/naive/vector_helper.h>

namespace swizzle
{
    namespace glsl
    {
        namespace naive
        {
            //! A version with Size == 1 is a special case that should not be created explicitly; it only lives in function proxies and
            //! to create a "common" type for different set of parameters. It is implicitly constructible from a scalar and has to-scalar
            //! conversion operator - this combination is a short path to ambiguity errors when combined with operator overloading.
            //! Why std::conditional shenanegance and not template specialisation? Well, the former needs less code in this case.
            template < class ScalarType, size_t Size >
            class vector : 
                // let the helper decide which base class to choose
                public vector_helper<ScalarType, Size>::base_type,
                // add static glsl functions
                public vector_functions_adapter<
                    // enable binary operators; if there's only one component do not do it, because it would
                    // cause ambiguity due to conversion to scalar type operator
                    typename std::conditional< 
                        Size == 1,
                        detail::nothing,
                        detail::vector_binary_operators<vector<ScalarType, Size>, ScalarType>
                    >::type,
                    vector,
                    ScalarType,
                    Size
                >
            {
                static_assert(Size >= 1, "Size must be >= 1");

                //! A convenient mnemonic for base type
                typedef typename vector_helper<ScalarType, Size>::base_type base_type;
                //! "Hide" m_data from outside and make it locally visible
                using base_type::m_data;

            // TYPEDEFS
            public:
                //! Number of components of this vector.
                static const size_t num_of_components = Size;
                //! This type.
                typedef vector vector_type;
                //! Scalar type.
                typedef ScalarType scalar_type;
                //! Type static functions return; for single-component they decay to a scalar
                typedef typename std::conditional<Size==1, scalar_type, vector>::type decay_type;
                //! Sanity checks
                static_assert( sizeof(base_type) == sizeof(scalar_type) * Size, "Size of the base class is not equal to size of its components, most likely empty base class optimisation failed");

            // CONSTRUCTION
            public:

                //! Default constructor.
                vector()
                {
                    iterate( [&](size_t i) -> void { m_data[i] = 0; } );
                }

                //! Copy constructor
                vector( const vector& o )
                {
                    iterate( [&](size_t i) -> void { m_data[i] = o[i]; } );
                }

                //! Implicit constructor from scalar-convertible only for one-component vector
                vector( typename std::conditional<Size==1, scalar_type, detail::operation_not_available>::type s )
                {
                    iterate( [&](size_t i) -> void { m_data[i] = s; } );
                }

                //! For vectors bigger than 1 conversion from scalar should be explicit.
                explicit vector( typename std::conditional<Size!=1, scalar_type, detail::operation_not_available>::type s )
                {
                    iterate( [&](size_t i) -> void { m_data[i] = s; } );
                }

                //! A composite constructor variant with 1 argument
                //! Note that for types convertibles to scalar type the instantiation will fail effectively falling back to one of previous two constructors
                template <class T1>
                explicit vector( const T1& v1,
                    typename std::enable_if< !std::is_convertible<T1, scalar_type>::value && detail::are_sizes_greater_or_equal<Size, detail::get_vector_size<T1>::value>::value, void>::type* = 0 )
                {
                    compose<0>(detail::decay(v1));
                }

                //! A composite constructor variant with 2 arguments
                template <class T1, class T2>
                vector( const T1& v1, const T2& v2,
                    typename std::enable_if< detail::are_sizes_greater_or_equal<Size, detail::get_vector_size<T1>::value, detail::get_vector_size<T2>::value>::value, void>::type* = 0 )
                {
                    compose<0>(detail::decay(v1));
                    compose<detail::get_vector_size<T1>::value>(detail::decay(v2));
                }

                //! A composite constructor variant with 3 arguments
                template <class T1, class T2, class T3>
                vector( const T1& v1, const T2& v2, const T3& v3,
                    typename std::enable_if< detail::are_sizes_greater_or_equal<Size, detail::get_vector_size<T1>::value, detail::get_vector_size<T2>::value, detail::get_vector_size<T3>::value>::value, void>::type* = 0 )
                {
                    compose<0>(detail::decay(v1));
                    compose<detail::get_vector_size<T1>::value>(detail::decay(v2));
                    compose<detail::get_vector_size<T1>::value + detail::get_vector_size<T2>::value>(detail::decay(v3));
                }

                //! A composite constructor variant with 4 arguments
                template <class T1, class T2, class T3, class T4>
                vector( const T1& v1, const T2& v2, const T3& v3, const T4& v4,
                    typename std::enable_if< detail::are_sizes_greater_or_equal<Size, detail::get_vector_size<T1>::value, detail::get_vector_size<T2>::value, detail::get_vector_size<T3>::value, detail::get_vector_size<T4>::value>::value, void>::type* = 0 )
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
                    return m_data[i];
                }
                const scalar_type& operator[](size_t i) const
                {
                    return m_data[i];
                }

                // Assignment-operation with vector argument

                vector& operator+=(const vector& o)
                {
                    iterate( [&](size_t i) -> void { m_data[i] += o[i]; } );
                    return *this;
                }
                vector& operator-=(const vector& o)
                {
                    iterate( [&](size_t i) -> void { m_data[i] -= o[i]; } );
                    return *this;
                }
                vector& operator*=(const vector& o)
                {
                    iterate( [&](size_t i) -> void { m_data[i] *= o[i]; } );
                    return *this;
                }
                vector& operator/=(const vector& o)
                {
                    iterate( [&](size_t i) -> void { m_data[i] /= o[i]; } );
                    return *this;
                }

                // Assignment-operation with scalar argument

                vector& operator+=(scalar_type o)
                {
                    iterate( [&](size_t i) -> void { m_data[i] += o; } );
                    return *this;
                }
                vector& operator-=(scalar_type o)
                {
                    iterate( [&](size_t i) -> void { m_data[i] -= o; } );
                    return *this;
                }
                vector& operator*=(scalar_type o)
                {
                    iterate( [&](size_t i) -> void { m_data[i] *= o; } );
                    return *this;
                }
                vector& operator/=(scalar_type o)
                {
                    iterate( [&](size_t i) -> void { m_data[i] /= o; } );
                    return *this;
                }

                // Others

                vector& operator=(const vector& o)
                {
                    iterate( [&](size_t i) -> void { m_data[i] = o[i]; } );
                    return *this;
                }

                vector operator-() const
                {
                    vector result;
                    iterate([&](size_t i) -> void { result[i] = -m_data[i]; });
                    return result;
                }

                // Conversion operator

                //! Auto-decay to scalar type only if this is a 1-sized vector
                operator typename std::conditional<Size == 1, scalar_type, detail::operation_not_available>::type() const
                {
                    return operator[](0);
                } 

            // AUXILIARY
            public:
                //! Decays the vector. For Size==1 this is going to return a scalar, for all other sizes - same vector
                decay_type decay() const
                {
                    struct selector
                    {
                        const vector& operator()(const vector& x, vector*)
                        {
                            return x;
                        }
                        const scalar_type& operator()(const vector& x, scalar_type*)
                        {
                            return x[0];
                        }
                    };
                    return selector()( *this, static_cast<decay_type*>(nullptr) );
                }
            
            private:
                //! Puts scalar at given position. Used only during construction.
                template <size_t N>
                void compose(scalar_type v)
                {
                    operator[](N) = v;
                }

                //! Puts a vector (or it's part) at given position. Used only during construction.
                template <size_t N, class TOtherScalar, size_t OtherSize>
                void compose( const vector<TOtherScalar, OtherSize>& v )
                {
                    const size_t limit = (N + OtherSize > Size) ? Size : (N + OtherSize);
                    iterate<N, limit>([&](size_t i) -> void { m_data[i] = v[i - N]; });
                }

                //! Iterates over the vector, firing Func for each index
                template <class Func>
                static void iterate(Func func)
                {
                    iterate<0, Size>(func);
                }

                //! Iterates over the vector, firing Func for each index
                template <size_t NStart, size_t NEnd, class Func>
                static void iterate(Func func, std::integral_constant<size_t, NStart> = std::integral_constant<size_t, NStart>(), std::integral_constant<size_t, NEnd> = std::integral_constant<size_t, NEnd>())
                {
                    static_assert( NStart >= 0 && NEnd <= Size && NStart < NEnd, "Out of bounds" );
                    detail::static_for<NStart, NEnd>(func);
                }
        
            // STL COMPABILITY (not needed, but useful for testing)
            public:
                //!
                typedef detail::indexed_vector_iterator<const vector> const_iterator;
                //!
                typedef detail::indexed_vector_iterator<vector> iterator;

                //! \return Immutable iterator.
                const_iterator begin() const
                {
                    return detail::make_indexed_vector_iterator(*this);
                }
                //! \return Immutable iterator.
                const_iterator end() const
                {
                    return detail::make_indexed_vector_iterator(*this, num_of_components);
                }
                //! \return Mutable iterator.
                iterator begin()
                {
                    return detail::make_indexed_vector_iterator(*this);
                }
                //! \return Mutable iterator.
                iterator end()
                {
                    return detail::make_indexed_vector_iterator(*this, num_of_components);
                }

                //! As an inline friend function, because thanks to that all convertibles will use same function.
                friend std::ostream& operator<<(std::ostream& os, const vector& vec)
                {
                    vec.iterate( [&](size_t i) -> void { os << vec[i] << (i == Size - 1 ? "" : ","); } );
                    return os;
                }

                //! As an inline friend function, because thanks to that all convertibles will use same function.
                friend std::istream& operator>>(std::istream& is, vector& vec)
                {
                    vec.iterate( [&](size_t i) -> void 
                    { 
                        is >> vec[i];
                        if ( i < Size - 1 && is.peek() == ',')
                        {
                            is.ignore(1);
                        }
                    });
                    return is;
                }
            };
        }
    }
}
