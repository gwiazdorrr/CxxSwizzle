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
            typedef TScalar scalar_type;
            static const size_t n_dimension = N;
            static const size_t m_dimension = M;

        private:
            std::array< vector_type, N > m_data; 

            struct invalid_number_of_parameters {};

            void set_row(size_t i, vector_type row)
            {
                m_data[i] = std::move(row);
            }
            void set_row(size_t i, invalid_number_of_parameters)
            {}

            void set_cell(size_t i, scalar_type cell)
            {
                auto row = i / m_dimension;
                auto col = i % m_dimension;
                m_data[row][col] = std::move(cell);
            }
            void set_cell(size_t i, invalid_number_of_parameters)
            {}



            static vector_type row_impl( std::true_type );
            static invalid_number_of_parameters row_impl(std::false_type)
            {
                return invalid_number_of_parameters();
            }

            template <size_t Row>
            static auto row() -> decltype( row_impl( detail::is_greater<N, Row>() ) )
            {
                return row_impl( detail::is_greater<N, Row>() );
            }

            static scalar_type scalar_impl( std::true_type );
            static invalid_number_of_parameters scalar_impl(std::false_type)
            {
                return invalid_number_of_parameters();
            }

            template <size_t Cell>
            static auto scalar() -> decltype( scalar_impl( detail::is_greater<N*M, Cell>() ) )
            {
                return scalar_impl( detail::is_greater<N*M, Cell>() );
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
                   decltype(row<1>()) row1 = row<1>(),
                   decltype(row<2>()) row2 = row<2>(),
                   decltype(row<3>()) row3 = row<3>())
            {
                set_row(0, row0);
                set_row(1, row1);
                set_row(2, row2);
                set_row(3, row3);
            }

            matrix(scalar_type s0,
                decltype(scalar<1>()) s1 = scalar<1>(),
                decltype(scalar<2>()) s2 = scalar<2>(),
                decltype(scalar<3>()) s3 = scalar<3>(),
                decltype(scalar<4>()) s4 = scalar<4>(),
                decltype(scalar<5>()) s5 = scalar<5>(),
                decltype(scalar<6>()) s6 = scalar<6>(),
                decltype(scalar<7>()) s7 = scalar<7>(),
                decltype(scalar<8>()) s8 = scalar<8>(),
                decltype(scalar<9>()) s9 = scalar<9>(),
                decltype(scalar<10>()) s10 = scalar<10>(),
                decltype(scalar<11>()) s11 = scalar<11>(),
                decltype(scalar<12>()) s12 = scalar<12>(),
                decltype(scalar<13>()) s13 = scalar<13>(),
                decltype(scalar<14>()) s14 = scalar<14>(),
                decltype(scalar<15>()) s15 = scalar<15>())
            {
                set_cell(0, s0);
                set_cell(1, s1);
                set_cell(2, s2);
                set_cell(3, s3);
                set_cell(4, s4);
                set_cell(5, s5);
                set_cell(6, s6);
                set_cell(7, s7);
                set_cell(8, s8);
                set_cell(9, s9);
                set_cell(10, s10);
                set_cell(11, s11);
                set_cell(12, s12);
                set_cell(13, s13);
                set_cell(14, s14);
                set_cell(15, s15);
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
        };

        template <template <class, size_t> class TVector, class TScalar, size_t M, size_t N>
        auto operator*(const matrix<TVector, TScalar, M, N>& m, const TVector<TScalar, M>& v ) -> decltype( m.mul(m, v) )
        {
            return m.mul(m, v);
        }
        

    }
}