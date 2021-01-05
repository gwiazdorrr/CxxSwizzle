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

        typedef matrix_ matrix_type;
        typedef vector<TScalar, num_columns> row_type;
        typedef vector<TScalar, num_rows> column_type;
        typedef TScalar scalar_type;

    // CONSTRUCTION
    public:

        //! Default zeroing constructor.
        matrix_()
        {}

        //! Copying constructor
        matrix_(const matrix_& other)
        {
            data = other.data;
        }

        template <size_t TIndex>
        column_type set_return(column_type src, TScalar value)
        {
            src[TIndex] = value;
            return src;
        }

        template <size_t TNumOfComponents, typename TSomeVector>
        column_type pad_with_zeros(const TSomeVector& src)
        {
            return column_type((TColumns < TNumOfComponents ? src[TColumns] : 0)...);
        }

        // This should work but doesn't (I'm tired)
        ////! Constructor for matrices smaller than current one
        //template <size_t OtherNumColumns, size_t TOtherNumRows>
        //matrix_(const matrix<TVector, TScalar, TOtherNumRows, OtherNumColumns>& other)
        //{
        //    static_assert(OtherNumColumns <= num_columns);
        //    static_assert(TOtherNumRows <= num_rows);

        //    // copy columns
        //    ((TColumns < OtherNumColumns ? data[TColumns] = pad_with_zeros<TOtherNumRows>(other.data[TColumns]) : set_return<TColumns>(zero, 1)), ...);
        //}

        //! Init with s diagonally
        matrix_(const scalar_type& s)
        {
            column_type zero(0);
            ((TColumns < num_rows ? data[TColumns] = set_return<TColumns>(zero, s) : zero), ...);
        }

        template <class T0, class T1, class... T,
            class = typename std::enable_if< 
                !(num_rows*num_columns <= detail::get_total_component_count_v<T0, T1, T...> - detail::get_total_component_count_v<typename detail::last<T0, T1, T...>::type >) &&
                (num_rows*num_columns <= detail::get_total_component_count_v<T0, T1, T...>),
                void>::type 
            >
        explicit matrix_(T0&& t0, T1&& t1, T&&... ts)
        {
            construct<0>(std::forward<T0>(t0), std::forward<T1>(t1), std::forward<T>(ts)..., detail::nothing{});
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
            return data[i];
        }

        //! Row accessor.
        const column_type& operator[](size_t i) const
        {
            return data[i];
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
        static column_type mul(const matrix_type& m, const row_type& v)
        {
            return mul(v, transpose(m));
        }

        //! Matrix-matrix multiplication.
        template <size_t TOtherNumRows>
        static matrix<TScalar, TOtherNumRows, num_columns> mul(const matrix<TScalar, TOtherNumRows, num_rows>& m1, const matrix_type& m2)
        {
            // TODO: questionable
            auto tr = transpose(m1);
            return matrix<TScalar, TOtherNumRows, num_columns>((m2.column(TColumns) * tr)...);
        }

        //! Matrix-vector multiplication.
        static row_type mul(const column_type& v, const matrix_type& m)
        {
            return row_type(v.call_dot(v, m.column(TColumns))...);
        }

    private:

        template <size_t TOffset, class T0, class... Tail>
        void construct(T0&& t0, Tail&&... tail)
        {
            // the pyramid of MSVC shame
            compose<TOffset>(detail::decay(std::forward<T0>(t0)));
            construct<TOffset + detail::get_total_component_count_v<T0> >(std::forward<Tail>(tail)...);
        }

        template <size_t>
        void construct(detail::nothing)
        {}

        //! Optimised setter used when setting whole column
        template <size_t TCellIdx>
        typename std::enable_if<TCellIdx % num_rows == 0, void>::type compose(const column_type& v)
        {
            detail::nonmasked(column( TCellIdx / num_rows )) = v;
        }

        //! Vector fallback setter, when TCellIdx is not aligned
        template <size_t TCellIdx, class TVectorScalar, size_t TVectorSize>
        typename std::enable_if<TCellIdx % num_rows != 0 || TVectorSize != num_rows, void>::type compose(const vector<TVectorScalar, TVectorSize>& v)
        {
            // do not go over the matrix size!
            const size_t c_limit = (TCellIdx + TVectorSize > num_rows * num_columns) ? (num_rows * num_columns) : (TCellIdx + TVectorSize);
            detail::static_for<TCellIdx, c_limit>([&](size_t i) -> void { cell(i % num_rows, i / num_rows) = v[i - TCellIdx]; });
        }

        template <size_t TCellIdx, typename TSomeScalar>
        void compose(TSomeScalar&& scalar, std::enable_if_t<std::is_constructible_v<scalar_type, TSomeScalar>>* = nullptr)
        {
            cell(TCellIdx % num_rows, TCellIdx / num_rows) = scalar_type(std::forward<TSomeScalar>(scalar));
        }

    private:
        std::array< column_type, num_columns > data; 


    public:
        int length() const
        {
            return num_columns;
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

    //template <class TScalar, size_t M, size_t TNumRows, size_t OtherNumColumns>
    //matrix<TVector, TScalar, TNumRows, OtherNumColumns> operator*(const matrix<TVector, TScalar, N, M>& m1, const matrix<TVector, TScalar, M, OtherNumColumns>& m2 )
    //{
    //    return m1.mul(m1, m2);
    //}
}