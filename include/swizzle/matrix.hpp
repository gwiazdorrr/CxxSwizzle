// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <array>
#include <swizzle/detail/utils.h>
#include <swizzle/detail/common_binary_operators.h> 
#include <utility>
#include <swizzle/vector.hpp>

namespace swizzle
{
    template <class TScalar, size_t TNumRows, size_t... TColumns >
    struct matrix_;

    namespace detail
    {
        template <class TScalar, size_t TNumRows, size_t... TColumns> 
        matrix_<TScalar, TNumRows, TColumns...> make_matrix_type_helper(std::index_sequence<TColumns...>);
    }

    //! A naive matrix implementation.
    //! Stores data as an array of vertices.
    template <class TScalar, size_t TNumRows, size_t... TColumns >
    struct matrix_ : detail::common_binary_operators<matrix_<TScalar, TNumRows, TColumns...>, TScalar>
    {
    public:
        static constexpr size_t num_rows = TNumRows;
        static constexpr size_t num_columns = sizeof...(TColumns);
        static constexpr size_t num_cells = num_rows * num_columns;

        typedef matrix_ matrix_type;
        typedef vector<TScalar, num_columns> row_type;
        typedef vector<TScalar, num_rows> column_type;
        typedef TScalar scalar_type;

        using this_type = matrix_;
        using rows_sequence = std::make_index_sequence<num_rows>;
        using cells_sequence = std::make_index_sequence<num_cells>;


    // CONSTRUCTION
    public:

        //! Decays the vector. For Size==1 this is going to return a scalar, for all other sizes - same vector
        inline matrix_type decay() const
        {
            return *this;
        }


        //! Default zeroing constructor.
        matrix_()
        {}

        //! Copying constructor
        matrix_(const matrix_& other)
        {
            data = other.data;
        }

        template <size_t TOtherNumRows, size_t... TOtherColumns>
        matrix_(const matrix_<scalar_type, TOtherNumRows, TOtherColumns...>& other) : matrix_(scalar_type(1))
        {
            ((TOtherColumns < num_columns ? blend_column(column(TOtherColumns), other.column(TOtherColumns)) : void(0)), ...);
        }

        //! Init with s diagonally
        explicit matrix_(const scalar_type& s)
        {
            ((TColumns < num_rows ? data[TColumns] = make_identity_column<TColumns>(s) : column_type(0)), ...);
        }

        //! Only 2x2 matrix can be initialised with a single vector
        explicit matrix_(const detail::only_if<num_cells == 4, vector<scalar_type, 4>>& vec)
        {
            detail::static_for<0, num_cells>([&](size_t i) -> void { cell(i % num_rows, i / num_rows) = vec[i]; });
        }

        template <class T0, class T1, class... T,
            class = typename std::enable_if< 
                !(num_cells <= detail::get_total_component_count_v<T0, T1, T...> - detail::get_total_component_count_v<detail::last_t<T0, T1, T...>>) &&
                 (num_cells <= detail::get_total_component_count_v<T0, T1, T...>),
                void>::type 
            >
        explicit matrix_(T0&& t0, T1&& t1, T&&... ts)
        {
            construct<0>(std::forward<T0>(t0), std::forward<T1>(t1), std::forward<T>(ts)...);
        }

        
        template <typename TOtherMatrix>
        static matrix_type make_transposed(TOtherMatrix& other)
        {
            return matrix_type(other.row(TColumns)...);
        }



    // OPERATORS
    public:

        //! Row accessor.
        column_type& operator[](size_t i)
        {
            return data[i >= 0 && i < num_columns ? i : 0];
        }

        //! Row accessor.
        const column_type& operator[](size_t i) const
        {
            return data[i >= 0 && i < num_columns ? i : 0];
        }

        // Scalar operators

        matrix_type& operator+=(const scalar_type& v)
        {
            return ((column(TColumns) += v), ..., *this);
        }

        matrix_type& operator-=(const scalar_type& v)
        {
            return ((column(TColumns) -= v), ..., *this);
        }

        matrix_type& operator*=(const scalar_type& v)
        {
            return ((column(TColumns) *= v), ..., *this);
        }

        matrix_type& operator/=(const scalar_type& v)
        {
            return ((column(TColumns) /= v), ..., *this);
        }

        // Matrix operators

        matrix_type& operator+=(const matrix_type& v)
        {
            return ((column(TColumns) += v.column(TColumns)), ..., *this);
        }

        matrix_type& operator-=(const matrix_type& v)
        {
            return ((column(TColumns) -= v.column(TColumns)), ..., *this);
        }

        matrix_type& operator*=(const matrix_type& v)
        {
            // Matrix multiplication is "special"
            return *this = mul<num_columns>(*this, v);
        }

        matrix_type& operator/=(const matrix_type& v)
        {
            return ((column(TColumns) /= v.column(TColumns)), ..., *this);
        }

        // Other operators

        matrix_type operator-() const
        {
            return matrix_type(-column(TColumns)...);
        }

        bool operator==(const matrix_type& o) const
        {
            return ((column(TColumns) == o.column(TColumns)) && ...);
        }

        bool operator!=(const matrix_type& o) const
        {
            return !(*this == o);
        }


        //! This needs to be a member rather than free function cause in latter case it would
        //! fail to be applied in cases of multiplying a proxy
        friend column_type operator*(const matrix_type& m, const row_type& row)
        {
            return mul(m, row);
        }

        //! As above.
        friend row_type operator*(const column_type& col, const matrix_type& m)
        {
            return mul(col, m);
        }

    // UTILITY FUNCTIONS
    public:

        //! \return Column
        const column_type& column(size_t i) const
        {
            return data[i];
        }

        //! \return Column
        column_type& column(size_t i)
        {
            return data[i];
        }

        row_type row(size_t i) const
        {
            return row_type(cell(i, TColumns)...);
        }

        scalar_type& cell(size_t row, size_t col)
        {
            return data[col][row];
        }

        const scalar_type& cell(size_t row, size_t col) const
        {
            return data[col][row];
        }

        inline friend matrix<TScalar, num_columns, num_rows> transpose(const matrix_type& m)
        {
            return matrix<TScalar, num_columns, num_rows>::make_transposed(m);
        }


        //! Matrix-vector multiplication.
        static row_type mul(const column_type& v, const matrix_type& m)
        {
            return row_type(v.call_dot(v, m.column(TColumns))...);
        }

        //! Matrix-vector multiplication.
        static column_type mul(const matrix_type& m, const row_type& v)
        {
            return m.mul_by_row(v, rows_sequence{});
            //return column_type(v.call_dot(v, m.row(TColumns))...);
        }

        template <size_t... TRows>
        column_type mul_by_row(const row_type& v, std::index_sequence<TRows...>) const
        {
            return column_type(v.call_dot(v, row(TRows))...);
        }

        //! Matrix-matrix multiplication.
        template <size_t TOtherNumRows>
        static matrix<TScalar, TOtherNumRows, num_columns> mul(const matrix<TScalar, TOtherNumRows, num_rows>& m1, const matrix_type& m2)
        {
            // TODO: questionable
            auto tr = transpose(m1);
            return matrix<TScalar, TOtherNumRows, num_columns>((m2.column(TColumns) * tr)...);
        }

        friend this_type matrixCompMult(const this_type& a, const this_type& b)
        {
            return this_type((a.column(TColumns) * b.column(TColumns))...);
        }


    private:

        template <size_t TOffset, class T0, class... Tail>
        void construct(T0&& t0, Tail&&... tail)
        {
            using raw_t = std::remove_reference_t<T0>;
            if constexpr (std::is_constructible_v<scalar_type, raw_t>) 
            {
                cell(TOffset % num_rows, TOffset / num_rows) = scalar_type(std::forward<T0>(t0));
            }
            else 
            {
                compose_vector<TOffset>(detail::decay(std::forward<T0>(t0)));
            }
             
            if constexpr (sizeof...(Tail) > 0)
            {
                construct<TOffset + detail::get_total_component_count_v<T0> >(std::forward<Tail>(tail)...);
            }
        }

        template <size_t... TOtherRows>
        void blend_column(column_type& column, const vector_<scalar_type, TOtherRows...>& other)
        {
            ((TOtherRows < num_rows ? column[TOtherRows] = other[TOtherRows], (void)0 : (void)0), ...);
        }

        template <size_t TIndex>
        column_type make_identity_column(TScalar value)
        {
            column_type result(0);
            result[TIndex] = value;
            return result;
        }

        template <size_t index, typename TVectorScalar, size_t... TVectorIndices>
        constexpr void compose_vector(const vector_<TVectorScalar, TVectorIndices...> v)
        {
            constexpr size_t left_to_assign = num_cells - index;
            constexpr size_t limit = v.num_components > left_to_assign ? left_to_assign : v.num_components;
            ((TVectorIndices < limit ? cell((index + TVectorIndices) % num_rows, (index + TVectorIndices) / num_rows) = v[TVectorIndices] : cell(0, 0)), ...);
        }

    private:
        std::array< column_type, num_columns > data; 


    public:
        constexpr int length() const noexcept
        {
            return num_columns;
        }

        constexpr int _cxxswizzle_func_length() const noexcept
        {
            return length();
        }
    };

    template <class TScalar>
    struct matrix_<TScalar, 1, 0>;

    template <class TScalar>
    struct matrix_<TScalar, 0>;

    template <class TScalar, size_t... TColumns>
    struct matrix_<TScalar, 0, TColumns...>;

    template <class TScalar, size_t TNumRows>
    struct matrix_<TScalar, TNumRows>;

    namespace detail 
    {
        using m2f = matrix_<float_type, 2, 0, 1>;
        using m3f = matrix_<float_type, 3, 0, 1, 2>;
        using m4f = matrix_<float_type, 4, 0, 1, 2, 3>;
    }

    inline float_type determinant(const detail::m2f& m)
    {
        return m.cell(0, 0) * m.cell(1, 1) - m.cell(0, 1) * m.cell(1, 0);
    }

    inline float_type determinant(const detail::m3f& m)
    {
        return m.cell(0, 0) * (m.cell(1, 1) * m.cell(2, 2) - m.cell(2, 1) * m.cell(1, 2)) -
               m.cell(0, 1) * (m.cell(1, 0) * m.cell(2, 2) - m.cell(1, 2) * m.cell(2, 0)) +
               m.cell(0, 2) * (m.cell(1, 0) * m.cell(2, 1) - m.cell(1, 1) * m.cell(2, 0));
    }

    inline float_type determinant(const detail::m4f& m)
    {
        // https://stackoverflow.com/a/44446912/461632
        auto A2323 = m.cell(2, 2) * m.cell(3, 3) - m.cell(2, 3) * m.cell(3, 2);
        auto A1323 = m.cell(2, 1) * m.cell(3, 3) - m.cell(2, 3) * m.cell(3, 1);
        auto A1223 = m.cell(2, 1) * m.cell(3, 2) - m.cell(2, 2) * m.cell(3, 1);
        auto A0323 = m.cell(2, 0) * m.cell(3, 3) - m.cell(2, 3) * m.cell(3, 0);
        auto A0223 = m.cell(2, 0) * m.cell(3, 2) - m.cell(2, 2) * m.cell(3, 0);
        auto A0123 = m.cell(2, 0) * m.cell(3, 1) - m.cell(2, 1) * m.cell(3, 0);
        auto A2313 = m.cell(1, 2) * m.cell(3, 3) - m.cell(1, 3) * m.cell(3, 2);
        auto A1313 = m.cell(1, 1) * m.cell(3, 3) - m.cell(1, 3) * m.cell(3, 1);
        auto A1213 = m.cell(1, 1) * m.cell(3, 2) - m.cell(1, 2) * m.cell(3, 1);
        auto A2312 = m.cell(1, 2) * m.cell(2, 3) - m.cell(1, 3) * m.cell(2, 2);
        auto A1312 = m.cell(1, 1) * m.cell(2, 3) - m.cell(1, 3) * m.cell(2, 1);
        auto A1212 = m.cell(1, 1) * m.cell(2, 2) - m.cell(1, 2) * m.cell(2, 1);
        auto A0313 = m.cell(1, 0) * m.cell(3, 3) - m.cell(1, 3) * m.cell(3, 0);
        auto A0213 = m.cell(1, 0) * m.cell(3, 2) - m.cell(1, 2) * m.cell(3, 0);
        auto A0312 = m.cell(1, 0) * m.cell(2, 3) - m.cell(1, 3) * m.cell(2, 0);
        auto A0212 = m.cell(1, 0) * m.cell(2, 2) - m.cell(1, 2) * m.cell(2, 0);
        auto A0113 = m.cell(1, 0) * m.cell(3, 1) - m.cell(1, 1) * m.cell(3, 0);
        auto A0112 = m.cell(1, 0) * m.cell(2, 1) - m.cell(1, 1) * m.cell(2, 0);

        return m.cell(0, 0) * (m.cell(1, 1) * A2323 - m.cell(1, 2) * A1323 + m.cell(1, 3) * A1223)
             - m.cell(0, 1) * (m.cell(1, 0) * A2323 - m.cell(1, 2) * A0323 + m.cell(1, 3) * A0223)
             + m.cell(0, 2) * (m.cell(1, 0) * A1323 - m.cell(1, 1) * A0323 + m.cell(1, 3) * A0123)
             - m.cell(0, 3) * (m.cell(1, 0) * A1223 - m.cell(1, 1) * A0223 + m.cell(1, 2) * A0123);
    }

    //template <typename TScalar 
    //inline friend matrix_type outerProduct(const detail::only_if<num_columns == num_rows, column_type>& c, column_type r)
    //{
    //    return {};
    //}

    inline detail::m2f inverse(const detail::m2f& m)
    {
        return detail::m2f(m.cell(1, 1), -m.cell(1, 0), -m.cell(0, 1), m.cell(0, 0)) / determinant(m);
    }

    inline detail::m3f inverse(const detail::m3f& m)
    {
        auto invdet = 1 / determinant(m);
        detail::m3f result;
        result.cell(0, 0) = (m.cell(1, 1) * m.cell(2, 2) - m.cell(2, 1) * m.cell(1, 2)) * invdet;
        result.cell(0, 1) = (m.cell(0, 2) * m.cell(2, 1) - m.cell(0, 1) * m.cell(2, 2)) * invdet;
        result.cell(0, 2) = (m.cell(0, 1) * m.cell(1, 2) - m.cell(0, 2) * m.cell(1, 1)) * invdet;
        result.cell(1, 0) = (m.cell(1, 2) * m.cell(2, 0) - m.cell(1, 0) * m.cell(2, 2)) * invdet;
        result.cell(1, 1) = (m.cell(0, 0) * m.cell(2, 2) - m.cell(0, 2) * m.cell(2, 0)) * invdet;
        result.cell(1, 2) = (m.cell(1, 0) * m.cell(0, 2) - m.cell(0, 0) * m.cell(1, 2)) * invdet;
        result.cell(2, 0) = (m.cell(1, 0) * m.cell(2, 1) - m.cell(2, 0) * m.cell(1, 1)) * invdet;
        result.cell(2, 1) = (m.cell(2, 0) * m.cell(0, 1) - m.cell(0, 0) * m.cell(2, 1)) * invdet;
        result.cell(2, 2) = (m.cell(0, 0) * m.cell(1, 1) - m.cell(1, 0) * m.cell(0, 1)) * invdet;
        return result;
    }

    inline detail::m4f inverse(const detail::m4f& m)
    {
        auto A2323 = m.cell(2, 2) * m.cell(3, 3) - m.cell(2, 3) * m.cell(3, 2) ;
        auto A1323 = m.cell(2, 1) * m.cell(3, 3) - m.cell(2, 3) * m.cell(3, 1) ;
        auto A1223 = m.cell(2, 1) * m.cell(3, 2) - m.cell(2, 2) * m.cell(3, 1) ;
        auto A0323 = m.cell(2, 0) * m.cell(3, 3) - m.cell(2, 3) * m.cell(3, 0) ;
        auto A0223 = m.cell(2, 0) * m.cell(3, 2) - m.cell(2, 2) * m.cell(3, 0) ;
        auto A0123 = m.cell(2, 0) * m.cell(3, 1) - m.cell(2, 1) * m.cell(3, 0) ;
        auto A2313 = m.cell(1, 2) * m.cell(3, 3) - m.cell(1, 3) * m.cell(3, 2) ;
        auto A1313 = m.cell(1, 1) * m.cell(3, 3) - m.cell(1, 3) * m.cell(3, 1) ;
        auto A1213 = m.cell(1, 1) * m.cell(3, 2) - m.cell(1, 2) * m.cell(3, 1) ;
        auto A2312 = m.cell(1, 2) * m.cell(2, 3) - m.cell(1, 3) * m.cell(2, 2) ;
        auto A1312 = m.cell(1, 1) * m.cell(2, 3) - m.cell(1, 3) * m.cell(2, 1) ;
        auto A1212 = m.cell(1, 1) * m.cell(2, 2) - m.cell(1, 2) * m.cell(2, 1) ;
        auto A0313 = m.cell(1, 0) * m.cell(3, 3) - m.cell(1, 3) * m.cell(3, 0) ;
        auto A0213 = m.cell(1, 0) * m.cell(3, 2) - m.cell(1, 2) * m.cell(3, 0) ;
        auto A0312 = m.cell(1, 0) * m.cell(2, 3) - m.cell(1, 3) * m.cell(2, 0) ;
        auto A0212 = m.cell(1, 0) * m.cell(2, 2) - m.cell(1, 2) * m.cell(2, 0) ;
        auto A0113 = m.cell(1, 0) * m.cell(3, 1) - m.cell(1, 1) * m.cell(3, 0) ;
        auto A0112 = m.cell(1, 0) * m.cell(2, 1) - m.cell(1, 1) * m.cell(2, 0) ;
         
        auto det = 
              m.cell(0, 0) * ( m.cell(1, 1) * A2323 - m.cell(1, 2) * A1323 + m.cell(1, 3) * A1223 ) 
            - m.cell(0, 1) * ( m.cell(1, 0) * A2323 - m.cell(1, 2) * A0323 + m.cell(1, 3) * A0223 ) 
            + m.cell(0, 2) * ( m.cell(1, 0) * A1323 - m.cell(1, 1) * A0323 + m.cell(1, 3) * A0123 ) 
            - m.cell(0, 3) * ( m.cell(1, 0) * A1223 - m.cell(1, 1) * A0223 + m.cell(1, 2) * A0123 ) ;

        det = 1 / det;

        detail::m4f r;
        r.cell(0, 0) = det *   ( m.cell(1, 1) * A2323 - m.cell(1, 2) * A1323 + m.cell(1, 3) * A1223 );
        r.cell(0, 1) = det * - ( m.cell(0, 1) * A2323 - m.cell(0, 2) * A1323 + m.cell(0, 3) * A1223 );
        r.cell(0, 2) = det *   ( m.cell(0, 1) * A2313 - m.cell(0, 2) * A1313 + m.cell(0, 3) * A1213 );
        r.cell(0, 3) = det * - ( m.cell(0, 1) * A2312 - m.cell(0, 2) * A1312 + m.cell(0, 3) * A1212 );
        r.cell(1, 0) = det * - ( m.cell(1, 0) * A2323 - m.cell(1, 2) * A0323 + m.cell(1, 3) * A0223 );
        r.cell(1, 1) = det *   ( m.cell(0, 0) * A2323 - m.cell(0, 2) * A0323 + m.cell(0, 3) * A0223 );
        r.cell(1, 2) = det * - ( m.cell(0, 0) * A2313 - m.cell(0, 2) * A0313 + m.cell(0, 3) * A0213 );
        r.cell(1, 3) = det *   ( m.cell(0, 0) * A2312 - m.cell(0, 2) * A0312 + m.cell(0, 3) * A0212 );
        r.cell(2, 0) = det *   ( m.cell(1, 0) * A1323 - m.cell(1, 1) * A0323 + m.cell(1, 3) * A0123 );
        r.cell(2, 1) = det * - ( m.cell(0, 0) * A1323 - m.cell(0, 1) * A0323 + m.cell(0, 3) * A0123 );
        r.cell(2, 2) = det *   ( m.cell(0, 0) * A1313 - m.cell(0, 1) * A0313 + m.cell(0, 3) * A0113 );
        r.cell(2, 3) = det * - ( m.cell(0, 0) * A1312 - m.cell(0, 1) * A0312 + m.cell(0, 3) * A0112 );
        r.cell(3, 0) = det * - ( m.cell(1, 0) * A1223 - m.cell(1, 1) * A0223 + m.cell(1, 2) * A0123 );
        r.cell(3, 1) = det *   ( m.cell(0, 0) * A1223 - m.cell(0, 1) * A0223 + m.cell(0, 2) * A0123 );
        r.cell(3, 2) = det * - ( m.cell(0, 0) * A1213 - m.cell(0, 1) * A0213 + m.cell(0, 2) * A0113 );
        r.cell(3, 3) = det *   ( m.cell(0, 0) * A1212 - m.cell(0, 1) * A0212 + m.cell(0, 2) * A0112 );
        return r;
    }
}