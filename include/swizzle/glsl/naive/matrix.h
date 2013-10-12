// CxxSwizzle
// Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <array>
#include <swizzle/detail/utils.h>

namespace swizzle
{
    namespace glsl
    {
        namespace naive
        {
            //! A naive matrix implementation.
            //! Stores data as an array of vertices.
            template <template <class, size_t> class TVector, class TScalar, size_t N, size_t M>
            class matrix
            {
            public:
                typedef matrix matrix_type;
                typedef TVector<TScalar, M> row_type;
                typedef TVector<TScalar, N> column_type;
                typedef TScalar scalar_type;
                static const size_t n_dimension = N;
                static const size_t m_dimension = M;

            // CONSTRUCTION HELPERS
            private:
                //! Not defined on purpose. It will be only linked against if an argument is missing,
                //! generating error.
                template <class T>
                static T too_few_args( std::true_type );
            
                //! Well... if the name of this type appears somewhere in the error log it should be quite obvious what's going on :)
                //! Important bit - this type is both private and not convertible, so using it explicitly in a constructor is doomed to fail
                struct invalid_number_of_parameters {};

                template <class T>
                static invalid_number_of_parameters too_few_args(std::false_type)
                {
                    return invalid_number_of_parameters();
                }

                //! Will return scalar_type for valid cell number (i.e. less than N*M), invalid_number_of_paramters othewise
                template <size_t Cell>
                static auto scalar_arg() -> decltype( too_few_args<scalar_type>( detail::is_greater<N*M, Cell>() ) )
                {
                    return too_few_args<scalar_type>( detail::is_greater<N*M, Cell>() );
                }

                //! Will return row_type for valid row number (i.e. less than N), invalid_number_of_paramters othewise
                template <size_t Row>
                static auto row_arg() -> decltype( too_few_args<row_type>( detail::is_greater<N, Row>() ) )
                {
                    return too_few_args<row_type>( detail::is_greater<N, Row>() );
                }

            // CONSTRUCTION
            public:

                //! Default zeroing constructor.
                matrix()
                {}

                //! Copying constructor
                matrix(const matrix& other)
                {
                    m_data = other.m_data;
                }

                //! Constructor for matrices smaller than current one
                template <size_t OtherM, size_t OtherN>
                matrix(const matrix<TVector, TScalar, OtherN, OtherM>& other, 
                    typename std::enable_if<(OtherM > M) && (OtherN > N), void>::type* = 0)
                {
                    detail::static_for<0, OtherN>([&](size_t row) -> void 
                    {
                        detail::static_for<0, OtherM>([&](size_t col) -> void
                        {
                            m_data[row][col] = other[row][col];
                        });
                    });
                    // fill rest with 1s
                    const size_t min_inner = OtherN > OtherM ? OtherM : OtherN;
                    const size_t min_outer = N > M ? M : N;
                    detail::static_for<min_inner, min_outer>([&](size_t i) -> void { m_data[i][i] = 1; });
                }

                //! Init with s diagonally
                matrix(scalar_type s)
                {
                    const size_t min_dim = N > M ? M : N;
                    detail::static_for<0, min_dim>([&](size_t i) -> void { m_data[i][i] = s; });
                }

                //! Construct matrix from rows.
                //! This patter may not work everywhere, but was a fun to invent. Basically I didn't want to write separate
                //! constructors/classes for each matrix with a different dimensions. The first argument is a vector,
                //! the rest will be either a vector or invalid_number_of_parameters each, depending on the dimensions.
                //! If you pass "too much" you'll get "can't convert to invalid_number_of_parameters" error. If to little,
                //! you'll get linker error.
                matrix(row_type row0,
                       decltype(row_arg<1>()) row1 = row_arg<1>(),
                       decltype(row_arg<2>()) row2 = row_arg<2>(),
                       decltype(row_arg<3>()) row3 = row_arg<3>())
                {
                    optional_init(0, row0);
                    optional_init(1, row1);
                    optional_init(2, row2);
                    optional_init(3, row3);
                }

                //! See comment for constructor out of rows.
                matrix(scalar_type s0,
                       scalar_type s1,
                       decltype(scalar_arg<2>()) s2 = scalar_arg<2>(),
                       decltype(scalar_arg<3>()) s3 = scalar_arg<3>(),
                       decltype(scalar_arg<4>()) s4 = scalar_arg<4>(),
                       decltype(scalar_arg<5>()) s5 = scalar_arg<5>(),
                       decltype(scalar_arg<6>()) s6 = scalar_arg<6>(),
                       decltype(scalar_arg<7>()) s7 = scalar_arg<7>(),
                       decltype(scalar_arg<8>()) s8 = scalar_arg<8>(),
                       decltype(scalar_arg<9>()) s9 = scalar_arg<9>(),
                       decltype(scalar_arg<10>()) s10 = scalar_arg<10>(),
                       decltype(scalar_arg<11>()) s11 = scalar_arg<11>(),
                       decltype(scalar_arg<12>()) s12 = scalar_arg<12>(),
                       decltype(scalar_arg<13>()) s13 = scalar_arg<13>(),
                       decltype(scalar_arg<14>()) s14 = scalar_arg<14>(),
                       decltype(scalar_arg<15>()) s15 = scalar_arg<15>())
                {
                    optional_init(0, s0);
                    optional_init(1, s1);
                    optional_init(2, s2);
                    optional_init(3, s3);
                    optional_init(4, s4);
                    optional_init(5, s5);
                    optional_init(6, s6);
                    optional_init(7, s7);
                    optional_init(8, s8);
                    optional_init(9, s9);
                    optional_init(10, s10);
                    optional_init(11, s11);
                    optional_init(12, s12);
                    optional_init(13, s13);
                    optional_init(14, s14);
                    optional_init(15, s15);
                }

            // OPERATORS
            public:

                //! Row accessor.
                row_type& operator[](size_t i)
                {
                    return m_data[i];
                }
                //! Row accessor.
                const row_type& operator[](size_t i) const
                {
                    return m_data[i];
                }

                //! This needs to be a member rather than free function cause in latter case it would
                //! fail to be applied in cases of multiplying a proxy
                row_type operator*(const row_type& v) const
                {
                    return mul(*this, v);
                }

                //! Multiplies entire matrix by a scalar.
                matrix_type& operator*=(scalar_type v)
                {
                    detail::static_for<0, N>([&](size_t row) -> void { m_data[row] *= v; } );
                    return *this;
                }

            // UTILITY FUNCTIONS
            public:

                //! Matrix-vector multiplication.
                static row_type mul(const matrix_type& m, const typename std::conditional<M==N, row_type, detail::operation_not_available>::type& v)
                {
                    row_type result;
                    detail::static_for<0, M>([&](size_t col) -> void
                    {
                        scalar_type s = 0;
                        detail::static_for<0, N>([&](size_t row) -> void 
                        {
                            s += m[row][col] * v[row];
                        });
                        result[col] = s;
                    });
                    return result;
                }

                //! Matrix-matrix multiplication.
                template <size_t OtherM>
                static matrix<TVector, TScalar, N, OtherM> mul(const matrix_type& m1, const matrix<TVector, TScalar, M, OtherM>& m2)
                {
                    typedef matrix<TVector, TScalar, N, OtherM> result_type;
                
                    result_type result;

                    // iterate over rows and columns and store a dot product for each m1.row-m2.column pair
                    detail::static_for<0, N>([&](size_t row) -> void
                    {
                        row_type row_data = m1[row];
                        typename result_type::row_type result_row;
                        detail::static_for<0, OtherM>([&](size_t col) -> void
                        {
                            result_row[col] = row_data.dot(row_data, m2.column(col));
                        });
                        result[row] = result_row;
                    });

                    return result;
                }

            private:
                std::array< row_type, N > m_data; 

                //! Set i-th row
                void optional_init(size_t i, row_type row)
                {
                    m_data[i] = std::move(row);
                }

                //! Set i-th cell
                void optional_init(size_t i, scalar_type cell)
                {
                    auto row = i / M;
                    auto col = i % M;
                    m_data[row][col] = std::move(cell);
                }

                //! Intentionally here, does nothing. Being called for not needed parameters.
                void optional_init(size_t i, invalid_number_of_parameters)
                {}

                //! \return Column
                column_type column(size_t i) const
                {
                    column_type result;
                    detail::static_for<0, N>([&](size_t row) -> void
                    {
                        result[row] = m_data[row][i];
                    });
                    return result;
                }
            };

            template <template <class, size_t> class TVector, class TScalar, size_t M, size_t N, size_t OtherM>
            auto operator*(const matrix<TVector, TScalar, N, M>& m1, const matrix<TVector, TScalar, M, OtherM>& m2 ) -> decltype( m1.mul(m1, m2) )
            {
                return m1.mul(m1, m2);
            }
        }
    }
}