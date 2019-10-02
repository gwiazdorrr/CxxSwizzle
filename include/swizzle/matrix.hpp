// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once

#include <array>
#include <swizzle/detail/utils.h>
#include <swizzle/detail/common_binary_operators.h> 
#include <utility>

namespace swizzle
{
    template <template <class, size_t> class VectorType, class ScalarType, size_t NumRows, size_t... Columns >
    struct matrix_;

    namespace detail
    {
        template <template <class, size_t> class VectorType, class ScalarType, size_t NumRows, size_t... Columns> 
        matrix_<VectorType, ScalarType, NumRows, Columns...> make_matrix_type_helper(std::index_sequence<Columns...>);
    }

    template <template <class, size_t> class VectorType, class ScalarType, size_t NumRows, size_t NumColumns>
    using matrix = decltype(detail::make_matrix_type_helper<VectorType, ScalarType, NumRows>(std::make_index_sequence<NumColumns>{}));

    //! A naive matrix implementation.
    //! Stores data as an array of vertices.
    template <template <class, size_t> class VectorType, class ScalarType, size_t NumRows, size_t... Columns >
    struct matrix_ : detail::common_binary_operators<matrix_<VectorType, ScalarType, NumRows, Columns...>, ScalarType>
    {
    public:
        static const size_t num_rows = NumRows;
        static const size_t num_columns = sizeof...(Columns);

        static_assert(num_rows > 1 && num_columns > 1, "1");

        typedef matrix_ matrix_type;
        typedef VectorType<ScalarType, num_columns> row_type;
        typedef VectorType<ScalarType, num_rows> column_type;
        typedef ScalarType scalar_type;

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

        template <size_t Index>
        column_type set_return(column_type src, ScalarType value)
        {
            src[Index] = value;
            return src;
        }

        template <size_t NumOfComponents, typename SomeVectorType>
        column_type pad_with_zeros(const SomeVectorType& src)
        {
            return column_type((Columns < NumOfComponents ? src[Columns] : 0)...);
        }

        // This should work but doesn't (I'm tired)
        ////! Constructor for matrices smaller than current one
        //template <size_t OtherNumColumns, size_t OtherNumRows>
        //matrix_(const matrix<VectorType, ScalarType, OtherNumRows, OtherNumColumns>& other)
        //{
        //    static_assert(OtherNumColumns <= num_columns);
        //    static_assert(OtherNumRows <= num_rows);

        //    // copy columns
        //    ((Columns < OtherNumColumns ? data[Columns] = pad_with_zeros<OtherNumRows>(other.data[Columns]) : set_return<Columns>(zero, 1)), ...);
        //}

        //! Init with s diagonally
        matrix_(const scalar_type& s)
        {
            column_type zero((Columns, 0)...);
            ((Columns < num_rows ? data[Columns] = set_return<Columns>(zero, s) : zero), ...);
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

        
        template <typename OtherMatrixType>
        static matrix_type make_transposed(OtherMatrixType& other)
        {
            return matrix_type(other.row(Columns)...);
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
            return ((column(Columns) += v), ..., *this);
        }

        matrix_type& operator-=(const scalar_type& v)
        {
            return ((column(Columns) -= v), ..., *this);
        }

        matrix_type& operator*=(const scalar_type& v)
        {
            return ((column(Columns) *= v), ..., *this);
        }

        matrix_type& operator/=(const scalar_type& v)
        {
            return ((column(Columns) /= v), ..., *this);
        }

        // Matrix operators

        matrix_type& operator+=(const matrix_type& v)
        {
            return ((column(Columns) += v.column(Columns)), ..., *this);
        }

        matrix_type& operator-=(const matrix_type& v)
        {
            return ((column(Columns) -= v.column(Columns)), ..., *this);
        }

        matrix_type& operator*=(const matrix_type& v)
        {
            // Matrix multiplication is "special"
            return *this = mul<num_columns>(*this, v);
        }

        matrix_type& operator/=(const matrix_type& v)
        {
            return ((column(Columns) /= v.column(Columns)), ..., *this);
        }

        // Other operators

        bool operator==(const matrix_type& o) const
        {
            return ((column(Columns) == other.column(Columns)) && ...);
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
            return row_type(cell(i, Columns)...);
        }

        scalar_type& cell(size_t row, size_t col)
        {
            return data[col][row];
        }

        const scalar_type& cell(size_t row, size_t col) const
        {
            return data[col][row];
        }

        inline friend matrix<VectorType, ScalarType, num_columns, num_rows> transpose(const matrix_type& m)
        {
            return matrix<VectorType, ScalarType, num_columns, num_rows>::make_transposed(m);
        }

        //! Matrix-vector multiplication.
        static column_type mul(const matrix_type& m, const row_type& v)
        {
            return mul(v, transpose(m));
        }

        //! Matrix-matrix multiplication.
        template <size_t OtherNumRows>
        static matrix<VectorType, ScalarType, OtherNumRows, num_columns> mul(const matrix<VectorType, ScalarType, OtherNumRows, num_rows>& m1, const matrix_type& m2)
        {
            // TODO: questionable
            auto tr = transpose(m1);
            return matrix<VectorType, ScalarType, OtherNumRows, num_columns>((m2.column(Columns) * tr)...);
        }

        //! Matrix-vector multiplication.
        static row_type mul(const column_type& v, const matrix_type& m)
        {
            return row_type(v.call_dot(v, m.column(Columns))...);
        }

    private:

        template <size_t offset, class T0, class... Tail>
        void construct(T0&& t0, Tail&&... tail)
        {
            // the pyramid of MSVC shame
            compose<offset>(detail::decay(std::forward<T0>(t0)));
            construct<offset + detail::get_total_component_count_v<T0> >(std::forward<Tail>(tail)...);
        }

        template <size_t>
        void construct(detail::nothing)
        {}

        //! Optimised setter used when setting whole column
        template <size_t CellIdx>
        typename std::enable_if<CellIdx % num_rows == 0, void>::type compose(const column_type& v)
        {
            column( CellIdx / num_rows ) = v;
        }

        //! Vector fallback setter, when CellIdx is not aligned
        template <size_t CellIdx, class VectorScalarType, size_t VectorSize>
        typename std::enable_if<CellIdx % num_rows != 0 || VectorSize != num_rows, void>::type compose(const VectorType<VectorScalarType, VectorSize>& v)
        {
            // do not go over the matrix size!
            const size_t c_limit = (CellIdx + VectorSize > num_rows * num_columns) ? (num_rows * num_columns) : (CellIdx + VectorSize);
            detail::static_for<CellIdx, c_limit>([&](size_t i) -> void { cell(i % num_rows, i / num_rows) = v[i - CellIdx]; });
        }

        template <size_t CellIdx>
        void compose(const scalar_type& s)
        {
            cell( CellIdx % num_rows, CellIdx / num_rows ) = s;
        }

    private:
        std::array< column_type, num_columns > data; 
    };

    //template <template <class, size_t> class VectorType, class ScalarType, size_t M, size_t NumRows, size_t OtherNumColumns>
    //matrix<VectorType, ScalarType, NumRows, OtherNumColumns> operator*(const matrix<VectorType, ScalarType, N, M>& m1, const matrix<VectorType, ScalarType, M, OtherNumColumns>& m2 )
    //{
    //    return m1.mul(m1, m2);
    //}
}