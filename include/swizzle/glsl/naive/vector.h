// CxxSwizzle
// Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <type_traits>
#include <iostream>

#include <swizzle/detail/utils.h>
#include <swizzle/detail/common_binary_operators.h>
#include <swizzle/detail/vector_base.h>
#include <swizzle/detail/indexed_vector_iterator.h>
#include <swizzle/glsl/naive/vector_functions_adapter.h>
#include <swizzle/glsl/naive/vector_traits_impl.h>
#include <swizzle/glsl/naive/vector_helper.h>
#include <swizzle/glsl/naive/matrix.h>

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
                        detail::common_binary_operators<vector<ScalarType, Size>, ScalarType>
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

                // Block of generic proxy-constructos calling construct member function. Compiler
                // will likely optimise this.
                CXXSWIZZLE_CONSTRUCTOR_GREATER_OR_EQUAL_COMPONENTS(vector, Size, 1, 3)
                CXXSWIZZLE_CONSTRUCTOR_GREATER_OR_EQUAL_COMPONENTS(vector, Size, 2, 2)
                CXXSWIZZLE_CONSTRUCTOR_GREATER_OR_EQUAL_COMPONENTS(vector, Size, 3, 1)
                CXXSWIZZLE_CONSTRUCTOR_GREATER_OR_EQUAL_COMPONENTS(vector, Size, 4, 0)
        
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

                // Matrix multiply operation
                vector& operator*=(const matrix<::swizzle::glsl::naive::vector, ScalarType, Size, Size>& m)
                {
                    return *this = *this * m;
                }

                // Others

                vector& operator=(const vector& o)
                {
                    iterate( [&](size_t i) -> void { m_data[i] = o[i]; } );
                    return *this;
                }

                bool operator==(const vector& o) const
                {
                    bool are_equal = true;
                    iterate( [&](size_t i) -> void { are_equal &= ( m_data[i] == o[i] ); });
                    return are_equal;
                }

                bool operator!=(const vector& o) const
                {
                    return !(*this == o);
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

                template <class T1, class T2, class T3, class T4>
                void construct(T1&& t1, T2&& t2, T3&& t3, T4&& t4)
                {
                    // the pyramid of MSVC shame
                    compose<T1, 0>(t1);
                    compose<T2, detail::get_total_size<T1>::value>(t2);
                    compose<T3, detail::get_total_size<T1, T2>::value>(t3);
                    compose<T4, detail::get_total_size<T1, T2, T3>::value>(t4);
                }

                template <class T, size_t CellIdx>
                void compose(typename std::remove_reference<T>::type& t)
                {
                    compose_impl<CellIdx>( detail::decay( std::forward<T>(t) ) );
                }

                template <size_t N>
                void compose_impl(detail::nothing)
                {
                    // noop
                }

                //! Puts scalar at given position. Used only during construction.
                template <size_t N>
                void compose_impl(scalar_type v)
                {
                    operator[](N) = v;
                }

                //! Puts a vector (or it's part) at given position. Used only during construction.
                template <size_t N, class TOtherScalar, size_t OtherSize>
                void compose_impl( const vector<TOtherScalar, OtherSize>& v )
                {
                    const size_t limit = (N + OtherSize > Size) ? Size : (N + OtherSize);
                    detail::static_for<N, limit>([&](size_t i) -> void { m_data[i] = v[i - N]; });
                }

                //! Iterates over the vector, firing Func for each index
                template <class Func>
                static void iterate(Func func)
                {
                    detail::static_for<0, Size>(func);
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
