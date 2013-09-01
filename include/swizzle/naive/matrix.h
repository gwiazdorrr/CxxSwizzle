#pragma once

#include <array>
#include "../detail/utils.h"
#include "vector_adapter.h"

namespace swizzle
{
    namespace naive
    {
        template <template <class, size_t> class TVector, class TScalar, size_t M, size_t N>
        class matrix
        {
        public:
            typedef matrix matrix_type;
            typedef TVector<TScalar, M> vector_type;
            typedef TVector<TScalar, N> column_type;
            typedef TScalar scalar_type;
            static const size_t n_dimension = N;
            static const size_t m_dimension = M;

        private:
            std::array< vector_type, N > m_data; 

            struct invalid_number_of_parameters {};

            void init_row(size_t i, vector_type row)
            {
                m_data[i] = std::move(row);
            }
            void init_row(size_t i, invalid_number_of_parameters)
            {}

            void init_cell(size_t i, scalar_type cell)
            {
                auto row = i / m_dimension;
                auto col = i % m_dimension;
                m_data[row][col] = std::move(cell);
            }
            void init_cell(size_t i, invalid_number_of_parameters)
            {}



            static vector_type row_arg_impl( std::true_type );
            static invalid_number_of_parameters row_arg_impl(std::false_type)
            {
                return invalid_number_of_parameters();
            }

            template <size_t Row>
            static auto row_arg() -> decltype( row_arg_impl( detail::is_greater<N, Row>() ) )
            {
                return row_arg_impl( detail::is_greater<N, Row>() );
            }

            static scalar_type scalar_arg_impl( std::true_type );
            static invalid_number_of_parameters scalar_arg_impl(std::false_type)
            {
                return invalid_number_of_parameters();
            }

            template <size_t Cell>
            static auto scalar_arg() -> decltype( scalar_arg_impl( detail::is_greater<N*M, Cell>() ) )
            {
                return scalar_arg_impl( detail::is_greater<N*M, Cell>() );
            }

            column_type column(size_t i) const
            {
                column_type result;
                detail::compile_time_for<0, n_dimension>([&](size_t row) -> void
                {
                    result[row] = m_data[row][i];
                });
                return result;
            }


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
            matrix(const ::swizzle::naive::matrix<TVector, TScalar, OtherM, OtherN>& other, 
                typename std::enable_if< !detail::is_greater<OtherM, M>::value && !detail::is_greater<OtherN, N>::value, void>::type* = 0)
            {
                detail::compile_time_for<0, OtherN>([&](size_t row) -> void 
                {
                    detail::compile_time_for<0, OtherM>([&](size_t col) -> void
                    {
                        m_data[row][col] = other[row][col];
                    });
                });
            }

            matrix(vector_type row0,
                   decltype(row_arg<1>()) row1 = row_arg<1>(),
                   decltype(row_arg<2>()) row2 = row_arg<2>(),
                   decltype(row_arg<3>()) row3 = row_arg<3>())
            {
                init_row(0, row0);
                init_row(1, row1);
                init_row(2, row2);
                init_row(3, row3);
            }

            matrix(scalar_type s0,
                decltype(scalar_arg<1>()) s1 = scalar_arg<1>(),
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
                init_cell(0, s0);
                init_cell(1, s1);
                init_cell(2, s2);
                init_cell(3, s3);
                init_cell(4, s4);
                init_cell(5, s5);
                init_cell(6, s6);
                init_cell(7, s7);
                init_cell(8, s8);
                init_cell(9, s9);
                init_cell(10, s10);
                init_cell(11, s11);
                init_cell(12, s12);
                init_cell(13, s13);
                init_cell(14, s14);
                init_cell(15, s15);
            }

            //! Row accessor.
            vector_type& operator[](size_t i)
            {
                return m_data[i];
            }
            //! Row accessor.
            const vector_type& operator[](size_t i) const
            {
                return m_data[i];
            }


        public:

            static typename std::enable_if<M == N, vector_type>::type mul(const matrix_type& m, const vector_type& v)
            {
                vector_type result;
                detail::compile_time_for<0, M>([&](size_t col) -> void
                {
                    scalar_type s = 0;
                    detail::compile_time_for<0, N>([&](size_t row) -> void 
                    {
                        s += m[row][col] * v[row];
                    });
                    result[col] = s;
                });
                return result;
            }

            template <size_t OtherM>
            static matrix<TVector, TScalar, OtherM, N> mul(const matrix_type& m1, const matrix<TVector, TScalar, OtherM, M>& m2)
            {
                matrix<TVector, TScalar, OtherM, N> result;
                typedef TVector<TScalar, OtherM> result_row;


                detail::compile_time_for<0, N>([&](size_t row) -> void
                {
                    auto row_data = m1[row];
                    result_row resultRow;
                    detail::compile_time_for<0, OtherM>([&](size_t col) -> void
                    {
                        resultRow[col] = row_data.dot(row_data, m2.column(col));
                    });
                    result[row] = resultRow;
                });
                return result;
            }

            
            vector_type operator*(const vector_type& v) const
            {
                return mul(*this, v);
            }


        };


        template <template <class, size_t> class TVector, class TScalar, size_t M, size_t N, size_t OtherM>
        auto operator*(const matrix<TVector, TScalar, M, N>& m1, const matrix<TVector, TScalar, OtherM, M>& m2 ) -> decltype( m1.mul(m1, m2) )
        {
            return m1.mul(m1, m2);
        }
        

    }
}