// CxxSwizzle
// Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <array>
#include <swizzle/detail/utils.h>
#include <swizzle/detail/constructor_generator.h>
#include <swizzle/detail/common_binary_operators.h>
#include <utility>

namespace swizzle
{
    namespace glsl
    {
        namespace naive
        {
            template <template <class, size_t> class VectorType, class ScalarType, size_t N, size_t M, class = std::true_type>
            class matrix;

            //! A naive matrix implementation.
            //! Stores data as an array of vertices.
            template <template <class, size_t> class VectorType, class ScalarType, size_t N, size_t M >
            class matrix< VectorType, ScalarType, N, M, std::integral_constant<bool, (N > 1 && M > 1)> > :
                detail::common_binary_operators<matrix<VectorType, ScalarType, N, M>, ScalarType>
            {
            public:
                typedef matrix matrix_type;
                typedef VectorType<ScalarType, M> row_type;
                typedef VectorType<ScalarType, N> column_type;
                typedef ScalarType scalar_type;
                static const size_t n_dimension = N;
                static const size_t m_dimension = M;

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
                matrix(const matrix<VectorType, ScalarType, OtherN, OtherM>& other)
                {
                    const size_t min_m = OtherM > M ? M : OtherM;
                    const size_t min_n = OtherN > N ? N : OtherN;

                    detail::static_for<0, min_n>([&](size_t row) -> void 
                    {
                        // static_for kills compilers over here...
                        // detail::static_for<0, min_m>([&](size_t col) -> void 
                        for (size_t col = 0; col < min_m; ++col)
                        {
                            cell(row, col) = other.cell(row, col);
                        }
                    });

                    // fill rest with 1s
                    const size_t min_inner = min_n > min_m ? min_m : min_n;
                    const size_t min_outer = N > M ? M : N;
                    detail::static_for<min_inner, min_outer>([&](size_t i) -> void { m_data[i][i] = 1; });
                }

                //! Init with s diagonally
                matrix(scalar_type s)
                {
                    const size_t min_dim = N > M ? M : N;
                    detail::static_for<0, min_dim>([&](size_t i) -> void { m_data[i][i] = s; });
                }

                CXXSWIZZLE_CONSTRUCTOR_GREATER_OR_EQUAL_COMPONENTS(matrix, N*M, 1, 15)
                CXXSWIZZLE_CONSTRUCTOR_GREATER_OR_EQUAL_COMPONENTS(matrix, N*M, 2, 14)
                CXXSWIZZLE_CONSTRUCTOR_GREATER_OR_EQUAL_COMPONENTS(matrix, N*M, 3, 13)
                CXXSWIZZLE_CONSTRUCTOR_GREATER_OR_EQUAL_COMPONENTS(matrix, N*M, 4, 12)
                CXXSWIZZLE_CONSTRUCTOR_GREATER_OR_EQUAL_COMPONENTS(matrix, N*M, 5, 11)
                CXXSWIZZLE_CONSTRUCTOR_GREATER_OR_EQUAL_COMPONENTS(matrix, N*M, 6, 10)
                CXXSWIZZLE_CONSTRUCTOR_GREATER_OR_EQUAL_COMPONENTS(matrix, N*M, 7, 9)
                CXXSWIZZLE_CONSTRUCTOR_GREATER_OR_EQUAL_COMPONENTS(matrix, N*M, 8, 8)
                CXXSWIZZLE_CONSTRUCTOR_GREATER_OR_EQUAL_COMPONENTS(matrix, N*M, 9, 7)
                CXXSWIZZLE_CONSTRUCTOR_GREATER_OR_EQUAL_COMPONENTS(matrix, N*M, 10, 6)
                CXXSWIZZLE_CONSTRUCTOR_GREATER_OR_EQUAL_COMPONENTS(matrix, N*M, 11, 5)
                CXXSWIZZLE_CONSTRUCTOR_GREATER_OR_EQUAL_COMPONENTS(matrix, N*M, 12, 4)
                CXXSWIZZLE_CONSTRUCTOR_GREATER_OR_EQUAL_COMPONENTS(matrix, N*M, 13, 3)
                CXXSWIZZLE_CONSTRUCTOR_GREATER_OR_EQUAL_COMPONENTS(matrix, N*M, 14, 2)
                CXXSWIZZLE_CONSTRUCTOR_GREATER_OR_EQUAL_COMPONENTS(matrix, N*M, 15, 1)
                CXXSWIZZLE_CONSTRUCTOR_GREATER_OR_EQUAL_COMPONENTS(matrix, N*M, 16, 0)

            // OPERATORS
            public:

                //! Row accessor.
                column_type& operator[](size_t i)
                {
                    return m_data[i];
                }

                //! Row accessor.
                const column_type& operator[](size_t i) const
                {
                    return m_data[i];
                }

                // Scalar operators

                matrix_type& operator+=(scalar_type v)
                {
                    detail::static_for<0, M>([&](size_t col) -> void { m_data[col] += v; } );
                    return *this;
                }

                matrix_type& operator-=(scalar_type v)
                {
                    detail::static_for<0, M>([&](size_t col) -> void { m_data[col] -= v; } );
                    return *this;
                }

                matrix_type& operator*=(scalar_type v)
                {
                    detail::static_for<0, M>([&](size_t col) -> void { m_data[col] *= v; } );
                    return *this;
                }

                matrix_type& operator/=(scalar_type v)
                {
                    detail::static_for<0, M>([&](size_t col) -> void { m_data[col] /= v; } );
                    return *this;
                }

                // Matrix operators

                matrix_type& operator+=(const matrix_type& v)
                {
                    detail::static_for<0, M>([&](size_t col) -> void { m_data[col] += v[col]; } );
                    return *this;
                }

                matrix_type& operator-=(const matrix_type& v)
                {
                    detail::static_for<0, M>([&](size_t col) -> void { m_data[col] -= v[col]; } );
                    return *this;
                }

                matrix_type& operator*=(const matrix_type& v)
                {
                    // Matrix multiplication is "special"
                    return *this = mul(*this, v);
                }

                matrix_type& operator/=(const matrix_type& v)
                {
                    detail::static_for<0, M>([&](size_t col) -> void { m_data[col] /= v[col]; } );
                    return *this;
                }

                // Other operators

                bool operator==(const matrix_type& o) const
                {
                    bool are_equal = true;
                    detail::static_for<0, M>( [&](size_t i) -> void { are_equal &= ( m_data[i] == o[i] ); });
                    return are_equal;
                }

                bool operator!=(const matrix_type& o) const
                {
                    return !(*this == o);
                }


                //! This needs to be a member rather than free function cause in latter case it would
                //! fail to be applied in cases of multiplying a proxy
                friend column_type operator*(const matrix& m, const row_type& row)
                {
                    return mul(m, row);
                }

                //! As above.
                friend row_type operator*(const column_type& col, const matrix& m)
                {
                    return mul(col, m);
                }

            // UTILITY FUNCTIONS
            public:

                //! \return Column
                const column_type& column(size_t i) const
                {
                    return m_data[i];
                }

                //! \return Column
                column_type& column(size_t i)
                {
                    return m_data[i];
                }

                row_type row(size_t i) const
                {
                    row_type result;
                    detail::static_for<0, M>([&](size_t col) -> void
                    {
                        result[col] = cell(i, col);
                    });
                    return result;
                }

                scalar_type& cell(size_t row, size_t col)
                {
                    return m_data[col][row];
                }

                const scalar_type& cell(size_t row, size_t col) const
                {
                    return m_data[col][row];
                }

                //! Matrix-vector multiplication.
                static column_type mul(const matrix_type& m, const row_type& v)
                {
                    column_type result;

                    detail::static_for<0, N>([&](size_t row) -> void
                    {
                        result[row] = v.dot(v, m.row(row));
                    });

                    return result;
                }

                //! Matrix-matrix multiplication.
                template <size_t OtherM>
                static matrix<VectorType, ScalarType, N, OtherM> mul(const matrix_type& m1, const matrix<VectorType, ScalarType, M, OtherM>& m2)
                {
                    matrix<VectorType, ScalarType, N, OtherM> result;

                    detail::static_for<0, OtherM>([&](size_t col) -> void
                    {
                        result.column(col) = m1 * m2.column(col);
                    });

                    return result;
                }

                //! Matrix-vector multiplication.
                static row_type mul(const column_type& v, const matrix& m)
                {
                    row_type result;
                    
                    detail::static_for<0, M>([&](size_t col) -> void
                    {
                        result[col] = v.dot(v, m.column(col));
                    });

                    return result;
                }

            private:

                template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, 
                          class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16>
                void construct(T1&& t1, T2&& t2, T3&& t3, T4&& t4, T5&& t5, T6&& t6, T7&& t7, T8&& t8,
                               T9&& t9, T10&& t10, T11&& t11, T12&& t12, T13&& t13, T14&& t14, T15&& t15, T16&& t16)
                {
                    // the pyramid of MSVC shame (where are my variadic templates?!)
                    compose<T1, 0>(t1);
                    compose<T2, detail::get_total_size<T1>::value>(t2);
                    compose<T3, detail::get_total_size<T1, T2>::value>(t3);
                    compose<T4, detail::get_total_size<T1, T2, T3>::value>(t4);
                    compose<T5, detail::get_total_size<T1, T2, T3, T4>::value>(t5);
                    compose<T6, detail::get_total_size<T1, T2, T3, T4, T5>::value>(t6);
                    compose<T7, detail::get_total_size<T1, T2, T3, T4, T5, T6>::value>(t7);
                    compose<T8, detail::get_total_size<T1, T2, T3, T4, T5, T6, T7>::value>(t8);
                    compose<T9, detail::get_total_size<T1, T2, T3, T4, T5, T6, T7, T8>::value>(t9);
                    compose<T10, detail::get_total_size<T1, T2, T3, T4, T5, T6, T7, T8, T9>::value>(t10);
                    compose<T11, detail::get_total_size<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>::value>(t11);
                    compose<T12, detail::get_total_size<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>::value>(t12);
                    compose<T13, detail::get_total_size<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12>::value>(t13);
                    compose<T14, detail::get_total_size<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>::value>(t14);
                    compose<T15, detail::get_total_size<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14>::value>(t15);
                    compose<T16, detail::get_total_size<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15>::value>(t16);
                }

                template <class T, size_t CellIdx>
                void compose(typename std::remove_reference<T>::type& t)
                {
                    compose_impl<CellIdx>( detail::decay( std::forward<T>(t) ) );
                }

                //! Optimised setter used when setting whole column
                template <size_t CellIdx>
                typename std::enable_if<CellIdx % N == 0, void>::type compose_impl( const column_type& v )
                {
                    column( CellIdx / N ) = v;
                }

                //! Vector fallback setter, when CellIdx is not align
                template <size_t CellIdx, class VectorScalarType, size_t VectorSize>
                typename std::enable_if<CellIdx % N != 0 || VectorSize != N, void>::type compose_impl( const VectorType<VectorScalarType, VectorSize>& v )
                {
                    // do not go over the matrix size!
                    const size_t c_limit = (CellIdx + VectorSize > N * M) ? (N * M) : (CellIdx + VectorSize);
                    detail::static_for<CellIdx, c_limit>([&](size_t i) -> void { cell(i % N, i / N) = v[i - CellIdx]; });
                }

                template <size_t CellIdx>
                void compose_impl(scalar_type s)
                {
                    cell( CellIdx % N, CellIdx / N ) = s;
                }

                template <size_t CellIdx>
                void compose_impl(detail::nothing)
                {
                    // noop
                }

            private:
                std::array< column_type, M > m_data; 
            };

            template <template <class, size_t> class VectorType, class ScalarType, size_t M, size_t N, size_t OtherM>
            matrix<VectorType, ScalarType, N, OtherM> operator*(const matrix<VectorType, ScalarType, N, M>& m1, const matrix<VectorType, ScalarType, M, OtherM>& m2 )
            {
                return m1.mul(m1, m2);
            }
        }
    }
}