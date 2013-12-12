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

                template <class T0, class... T,
                    class = typename std::enable_if< 
                        !(N*M <= detail::get_total_size<T0, T...>::value - detail::get_total_size<typename detail::last<T0, T...>::type >::value) &&
                        (N*M <= detail::get_total_size<T0, T...>::value),
                        void>::type 
                    >
                explicit matrix(T0&& t0, T&&... ts)
                {
                    construct<0>(std::forward<T0>(t0), std::forward<T>(ts)..., detail::nothing{});
                }

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

                template <size_t offset, class T0, class... Tail>
                void construct(T0&& t0, Tail&&... tail)
                {
                    // the pyramid of MSVC shame
                    compose<offset>(detail::decay(std::forward<T0>(t0)));
                    construct<offset + detail::get_total_size<T0>::value>(std::forward<Tail>(tail)...);
                }

                template <size_t>
                void construct(detail::nothing)
                {}

                //! Optimised setter used when setting whole column
                template <size_t CellIdx>
                typename std::enable_if<CellIdx % N == 0, void>::type compose(const column_type& v)
                {
                    column( CellIdx / N ) = v;
                }

                //! Vector fallback setter, when CellIdx is not align
                template <size_t CellIdx, class VectorScalarType, size_t VectorSize>
                typename std::enable_if<CellIdx % N != 0 || VectorSize != N, void>::type compose(const VectorType<VectorScalarType, VectorSize>& v)
                {
                    // do not go over the matrix size!
                    const size_t c_limit = (CellIdx + VectorSize > N * M) ? (N * M) : (CellIdx + VectorSize);
                    detail::static_for<CellIdx, c_limit>([&](size_t i) -> void { cell(i % N, i / N) = v[i - CellIdx]; });
                }

                template <size_t CellIdx>
                void compose(scalar_type s)
                {
                    cell( CellIdx % N, CellIdx / N ) = s;
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