#pragma once

#include <array>

namespace swizzle
{
    namespace detail
    {
        template <class TScalar, size_t Size, template <size_t, size_t, size_t, size_t> class TProxyFactory, class TData=std::array<TScalar,Size> >
        struct vector_data;

        template <class TScalar, template <size_t, size_t=-1, size_t=-1, size_t=-1> class TProxyFactory, class TData>
        struct vector_data<TScalar, 1, TProxyFactory, TData>
        {
            typedef TScalar scalar_type;
            typedef TData data_type;

            static const size_t num_of_components = 1;

            union
            {
                TData m_data;

                /*scalar_type x;*/

                typename TProxyFactory<0>::type x;
                typename TProxyFactory<0,0>::type xx;
                typename TProxyFactory<0,0,0>::type xxx;
                typename TProxyFactory<0,0,0,0>::type xxxx;
            };
        };

        template <class TScalar, template <size_t, size_t=-1, size_t=-1, size_t=-1> class TProxyFactory, class TData>
        struct vector_data<TScalar, 2, TProxyFactory, TData>
        {
            typedef TScalar scalar_type;
            typedef TData data_type;

            static const size_t num_of_components = 2;

            union
            {
                TData m_data;

                /*struct
                {
                    scalar_type x;
                    scalar_type y;
                };*/

                typename TProxyFactory<0>::type x;
                typename TProxyFactory<1>::type y;
                typename TProxyFactory<0,0>::type xx;
                typename TProxyFactory<0,1>::type xy;
                typename TProxyFactory<1,0>::type yx;
                typename TProxyFactory<1,1>::type yy;
                typename TProxyFactory<0,0,0>::type xxx;
                typename TProxyFactory<0,0,1>::type xxy;
                typename TProxyFactory<0,1,0>::type xyx;
                typename TProxyFactory<0,1,1>::type xyy;
                typename TProxyFactory<1,0,0>::type yxx;
                typename TProxyFactory<1,0,1>::type yxy;
                typename TProxyFactory<1,1,0>::type yyx;
                typename TProxyFactory<1,1,1>::type yyy;
                typename TProxyFactory<0,0,0,0>::type xxxx;
                typename TProxyFactory<0,0,0,1>::type xxxy;
                typename TProxyFactory<0,0,1,0>::type xxyx;
                typename TProxyFactory<0,0,1,1>::type xxyy;
                typename TProxyFactory<0,1,0,0>::type xyxx;
                typename TProxyFactory<0,1,0,1>::type xyxy;
                typename TProxyFactory<0,1,1,0>::type xyyx;
                typename TProxyFactory<0,1,1,1>::type xyyy;
                typename TProxyFactory<1,0,0,0>::type yxxx;
                typename TProxyFactory<1,0,0,1>::type yxxy;
                typename TProxyFactory<1,0,1,0>::type yxyx;
                typename TProxyFactory<1,0,1,1>::type yxyy;
                typename TProxyFactory<1,1,0,0>::type yyxx;
                typename TProxyFactory<1,1,0,1>::type yyxy;
                typename TProxyFactory<1,1,1,0>::type yyyx;
                typename TProxyFactory<1,1,1,1>::type yyyy;
            };
        };

        template <class TScalar, template <size_t, size_t=-1, size_t=-1, size_t=-1> class TProxyFactory, class TData>
        struct vector_data<TScalar, 3, TProxyFactory, TData>
        {
            typedef TScalar scalar_type;
            typedef TData data_type;

            static const size_t num_of_components = 3;

            union
            {
                TData m_data;

                /*struct
                {
                    scalar_type x;
                    scalar_type y;
                    scalar_type z;
                };*/

                typename TProxyFactory<0>::type x;
                typename TProxyFactory<1>::type y;
                typename TProxyFactory<2>::type z;
                typename TProxyFactory<0,0>::type xx;
                typename TProxyFactory<0,1>::type xy;
                typename TProxyFactory<0,2>::type xz;
                typename TProxyFactory<1,0>::type yx;
                typename TProxyFactory<1,1>::type yy;
                typename TProxyFactory<1,2>::type yz;
                typename TProxyFactory<2,0>::type zx;
                typename TProxyFactory<2,1>::type zy;
                typename TProxyFactory<2,2>::type zz;
                typename TProxyFactory<0,0,0>::type xxx;
                typename TProxyFactory<0,0,1>::type xxy;
                typename TProxyFactory<0,0,2>::type xxz;
                typename TProxyFactory<0,1,0>::type xyx;
                typename TProxyFactory<0,1,1>::type xyy;
                typename TProxyFactory<0,1,2>::type xyz;
                typename TProxyFactory<0,2,0>::type xzx;
                typename TProxyFactory<0,2,1>::type xzy;
                typename TProxyFactory<0,2,2>::type xzz;
                typename TProxyFactory<1,0,0>::type yxx;
                typename TProxyFactory<1,0,1>::type yxy;
                typename TProxyFactory<1,0,2>::type yxz;
                typename TProxyFactory<1,1,0>::type yyx;
                typename TProxyFactory<1,1,1>::type yyy;
                typename TProxyFactory<1,1,2>::type yyz;
                typename TProxyFactory<1,2,0>::type yzx;
                typename TProxyFactory<1,2,1>::type yzy;
                typename TProxyFactory<1,2,2>::type yzz;
                typename TProxyFactory<2,0,0>::type zxx;
                typename TProxyFactory<2,0,1>::type zxy;
                typename TProxyFactory<2,0,2>::type zxz;
                typename TProxyFactory<2,1,0>::type zyx;
                typename TProxyFactory<2,1,1>::type zyy;
                typename TProxyFactory<2,1,2>::type zyz;
                typename TProxyFactory<2,2,0>::type zzx;
                typename TProxyFactory<2,2,1>::type zzy;
                typename TProxyFactory<2,2,2>::type zzz;
                typename TProxyFactory<0,0,0,0>::type xxxx;
                typename TProxyFactory<0,0,0,1>::type xxxy;
                typename TProxyFactory<0,0,0,2>::type xxxz;
                typename TProxyFactory<0,0,1,0>::type xxyx;
                typename TProxyFactory<0,0,1,1>::type xxyy;
                typename TProxyFactory<0,0,1,2>::type xxyz;
                typename TProxyFactory<0,0,2,0>::type xxzx;
                typename TProxyFactory<0,0,2,1>::type xxzy;
                typename TProxyFactory<0,0,2,2>::type xxzz;
                typename TProxyFactory<0,1,0,0>::type xyxx;
                typename TProxyFactory<0,1,0,1>::type xyxy;
                typename TProxyFactory<0,1,0,2>::type xyxz;
                typename TProxyFactory<0,1,1,0>::type xyyx;
                typename TProxyFactory<0,1,1,1>::type xyyy;
                typename TProxyFactory<0,1,1,2>::type xyyz;
                typename TProxyFactory<0,1,2,0>::type xyzx;
                typename TProxyFactory<0,1,2,1>::type xyzy;
                typename TProxyFactory<0,1,2,2>::type xyzz;
                typename TProxyFactory<0,2,0,0>::type xzxx;
                typename TProxyFactory<0,2,0,1>::type xzxy;
                typename TProxyFactory<0,2,0,2>::type xzxz;
                typename TProxyFactory<0,2,1,0>::type xzyx;
                typename TProxyFactory<0,2,1,1>::type xzyy;
                typename TProxyFactory<0,2,1,2>::type xzyz;
                typename TProxyFactory<0,2,2,0>::type xzzx;
                typename TProxyFactory<0,2,2,1>::type xzzy;
                typename TProxyFactory<0,2,2,2>::type xzzz;
                typename TProxyFactory<1,0,0,0>::type yxxx;
                typename TProxyFactory<1,0,0,1>::type yxxy;
                typename TProxyFactory<1,0,0,2>::type yxxz;
                typename TProxyFactory<1,0,1,0>::type yxyx;
                typename TProxyFactory<1,0,1,1>::type yxyy;
                typename TProxyFactory<1,0,1,2>::type yxyz;
                typename TProxyFactory<1,0,2,0>::type yxzx;
                typename TProxyFactory<1,0,2,1>::type yxzy;
                typename TProxyFactory<1,0,2,2>::type yxzz;
                typename TProxyFactory<1,1,0,0>::type yyxx;
                typename TProxyFactory<1,1,0,1>::type yyxy;
                typename TProxyFactory<1,1,0,2>::type yyxz;
                typename TProxyFactory<1,1,1,0>::type yyyx;
                typename TProxyFactory<1,1,1,1>::type yyyy;
                typename TProxyFactory<1,1,1,2>::type yyyz;
                typename TProxyFactory<1,1,2,0>::type yyzx;
                typename TProxyFactory<1,1,2,1>::type yyzy;
                typename TProxyFactory<1,1,2,2>::type yyzz;
                typename TProxyFactory<1,2,0,0>::type yzxx;
                typename TProxyFactory<1,2,0,1>::type yzxy;
                typename TProxyFactory<1,2,0,2>::type yzxz;
                typename TProxyFactory<1,2,1,0>::type yzyx;
                typename TProxyFactory<1,2,1,1>::type yzyy;
                typename TProxyFactory<1,2,1,2>::type yzyz;
                typename TProxyFactory<1,2,2,0>::type yzzx;
                typename TProxyFactory<1,2,2,1>::type yzzy;
                typename TProxyFactory<1,2,2,2>::type yzzz;
                typename TProxyFactory<2,0,0,0>::type zxxx;
                typename TProxyFactory<2,0,0,1>::type zxxy;
                typename TProxyFactory<2,0,0,2>::type zxxz;
                typename TProxyFactory<2,0,1,0>::type zxyx;
                typename TProxyFactory<2,0,1,1>::type zxyy;
                typename TProxyFactory<2,0,1,2>::type zxyz;
                typename TProxyFactory<2,0,2,0>::type zxzx;
                typename TProxyFactory<2,0,2,1>::type zxzy;
                typename TProxyFactory<2,0,2,2>::type zxzz;
                typename TProxyFactory<2,1,0,0>::type zyxx;
                typename TProxyFactory<2,1,0,1>::type zyxy;
                typename TProxyFactory<2,1,0,2>::type zyxz;
                typename TProxyFactory<2,1,1,0>::type zyyx;
                typename TProxyFactory<2,1,1,1>::type zyyy;
                typename TProxyFactory<2,1,1,2>::type zyyz;
                typename TProxyFactory<2,1,2,0>::type zyzx;
                typename TProxyFactory<2,1,2,1>::type zyzy;
                typename TProxyFactory<2,1,2,2>::type zyzz;
                typename TProxyFactory<2,2,0,0>::type zzxx;
                typename TProxyFactory<2,2,0,1>::type zzxy;
                typename TProxyFactory<2,2,0,2>::type zzxz;
                typename TProxyFactory<2,2,1,0>::type zzyx;
                typename TProxyFactory<2,2,1,1>::type zzyy;
                typename TProxyFactory<2,2,1,2>::type zzyz;
                typename TProxyFactory<2,2,2,0>::type zzzx;
                typename TProxyFactory<2,2,2,1>::type zzzy;
                typename TProxyFactory<2,2,2,2>::type zzzz;
            };
        };

        template <class TScalar, template <size_t, size_t=-1, size_t=-1, size_t=-1> class TProxyFactory, class TData>
        struct vector_data<TScalar, 4, TProxyFactory, TData>
        {
            typedef TScalar scalar_type;
            typedef TData data_type;

            static const size_t num_of_components = 4;

            union
            {
                TData m_data;

                /*struct
                {
                    scalar_type x;
                    scalar_type y;
                    scalar_type z;
                    scalar_type w;
                };*/

                typename TProxyFactory<0>::type x;
                typename TProxyFactory<1>::type y;
                typename TProxyFactory<2>::type z;
                typename TProxyFactory<3>::type w;
                typename TProxyFactory<0,0>::type xx;
                typename TProxyFactory<0,1>::type xy;
                typename TProxyFactory<0,2>::type xz;
                typename TProxyFactory<0,3>::type xw;
                typename TProxyFactory<1,0>::type yx;
                typename TProxyFactory<1,1>::type yy;
                typename TProxyFactory<1,2>::type yz;
                typename TProxyFactory<1,3>::type yw;
                typename TProxyFactory<2,0>::type zx;
                typename TProxyFactory<2,1>::type zy;
                typename TProxyFactory<2,2>::type zz;
                typename TProxyFactory<2,3>::type zw;
                typename TProxyFactory<3,0>::type wx;
                typename TProxyFactory<3,1>::type wy;
                typename TProxyFactory<3,2>::type wz;
                typename TProxyFactory<3,3>::type ww;
                typename TProxyFactory<0,0,0>::type xxx;
                typename TProxyFactory<0,0,1>::type xxy;
                typename TProxyFactory<0,0,2>::type xxz;
                typename TProxyFactory<0,0,3>::type xxw;
                typename TProxyFactory<0,1,0>::type xyx;
                typename TProxyFactory<0,1,1>::type xyy;
                typename TProxyFactory<0,1,2>::type xyz;
                typename TProxyFactory<0,1,3>::type xyw;
                typename TProxyFactory<0,2,0>::type xzx;
                typename TProxyFactory<0,2,1>::type xzy;
                typename TProxyFactory<0,2,2>::type xzz;
                typename TProxyFactory<0,2,3>::type xzw;
                typename TProxyFactory<0,3,0>::type xwx;
                typename TProxyFactory<0,3,1>::type xwy;
                typename TProxyFactory<0,3,2>::type xwz;
                typename TProxyFactory<0,3,3>::type xww;
                typename TProxyFactory<1,0,0>::type yxx;
                typename TProxyFactory<1,0,1>::type yxy;
                typename TProxyFactory<1,0,2>::type yxz;
                typename TProxyFactory<1,0,3>::type yxw;
                typename TProxyFactory<1,1,0>::type yyx;
                typename TProxyFactory<1,1,1>::type yyy;
                typename TProxyFactory<1,1,2>::type yyz;
                typename TProxyFactory<1,1,3>::type yyw;
                typename TProxyFactory<1,2,0>::type yzx;
                typename TProxyFactory<1,2,1>::type yzy;
                typename TProxyFactory<1,2,2>::type yzz;
                typename TProxyFactory<1,2,3>::type yzw;
                typename TProxyFactory<1,3,0>::type ywx;
                typename TProxyFactory<1,3,1>::type ywy;
                typename TProxyFactory<1,3,2>::type ywz;
                typename TProxyFactory<1,3,3>::type yww;
                typename TProxyFactory<2,0,0>::type zxx;
                typename TProxyFactory<2,0,1>::type zxy;
                typename TProxyFactory<2,0,2>::type zxz;
                typename TProxyFactory<2,0,3>::type zxw;
                typename TProxyFactory<2,1,0>::type zyx;
                typename TProxyFactory<2,1,1>::type zyy;
                typename TProxyFactory<2,1,2>::type zyz;
                typename TProxyFactory<2,1,3>::type zyw;
                typename TProxyFactory<2,2,0>::type zzx;
                typename TProxyFactory<2,2,1>::type zzy;
                typename TProxyFactory<2,2,2>::type zzz;
                typename TProxyFactory<2,2,3>::type zzw;
                typename TProxyFactory<2,3,0>::type zwx;
                typename TProxyFactory<2,3,1>::type zwy;
                typename TProxyFactory<2,3,2>::type zwz;
                typename TProxyFactory<2,3,3>::type zww;
                typename TProxyFactory<3,0,0>::type wxx;
                typename TProxyFactory<3,0,1>::type wxy;
                typename TProxyFactory<3,0,2>::type wxz;
                typename TProxyFactory<3,0,3>::type wxw;
                typename TProxyFactory<3,1,0>::type wyx;
                typename TProxyFactory<3,1,1>::type wyy;
                typename TProxyFactory<3,1,2>::type wyz;
                typename TProxyFactory<3,1,3>::type wyw;
                typename TProxyFactory<3,2,0>::type wzx;
                typename TProxyFactory<3,2,1>::type wzy;
                typename TProxyFactory<3,2,2>::type wzz;
                typename TProxyFactory<3,2,3>::type wzw;
                typename TProxyFactory<3,3,0>::type wwx;
                typename TProxyFactory<3,3,1>::type wwy;
                typename TProxyFactory<3,3,2>::type wwz;
                typename TProxyFactory<3,3,3>::type www;
                typename TProxyFactory<0,0,0,0>::type xxxx;
                typename TProxyFactory<0,0,0,1>::type xxxy;
                typename TProxyFactory<0,0,0,2>::type xxxz;
                typename TProxyFactory<0,0,0,3>::type xxxw;
                typename TProxyFactory<0,0,1,0>::type xxyx;
                typename TProxyFactory<0,0,1,1>::type xxyy;
                typename TProxyFactory<0,0,1,2>::type xxyz;
                typename TProxyFactory<0,0,1,3>::type xxyw;
                typename TProxyFactory<0,0,2,0>::type xxzx;
                typename TProxyFactory<0,0,2,1>::type xxzy;
                typename TProxyFactory<0,0,2,2>::type xxzz;
                typename TProxyFactory<0,0,2,3>::type xxzw;
                typename TProxyFactory<0,0,3,0>::type xxwx;
                typename TProxyFactory<0,0,3,1>::type xxwy;
                typename TProxyFactory<0,0,3,2>::type xxwz;
                typename TProxyFactory<0,0,3,3>::type xxww;
                typename TProxyFactory<0,1,0,0>::type xyxx;
                typename TProxyFactory<0,1,0,1>::type xyxy;
                typename TProxyFactory<0,1,0,2>::type xyxz;
                typename TProxyFactory<0,1,0,3>::type xyxw;
                typename TProxyFactory<0,1,1,0>::type xyyx;
                typename TProxyFactory<0,1,1,1>::type xyyy;
                typename TProxyFactory<0,1,1,2>::type xyyz;
                typename TProxyFactory<0,1,1,3>::type xyyw;
                typename TProxyFactory<0,1,2,0>::type xyzx;
                typename TProxyFactory<0,1,2,1>::type xyzy;
                typename TProxyFactory<0,1,2,2>::type xyzz;
                typename TProxyFactory<0,1,2,3>::type xyzw;
                typename TProxyFactory<0,1,3,0>::type xywx;
                typename TProxyFactory<0,1,3,1>::type xywy;
                typename TProxyFactory<0,1,3,2>::type xywz;
                typename TProxyFactory<0,1,3,3>::type xyww;
                typename TProxyFactory<0,2,0,0>::type xzxx;
                typename TProxyFactory<0,2,0,1>::type xzxy;
                typename TProxyFactory<0,2,0,2>::type xzxz;
                typename TProxyFactory<0,2,0,3>::type xzxw;
                typename TProxyFactory<0,2,1,0>::type xzyx;
                typename TProxyFactory<0,2,1,1>::type xzyy;
                typename TProxyFactory<0,2,1,2>::type xzyz;
                typename TProxyFactory<0,2,1,3>::type xzyw;
                typename TProxyFactory<0,2,2,0>::type xzzx;
                typename TProxyFactory<0,2,2,1>::type xzzy;
                typename TProxyFactory<0,2,2,2>::type xzzz;
                typename TProxyFactory<0,2,2,3>::type xzzw;
                typename TProxyFactory<0,2,3,0>::type xzwx;
                typename TProxyFactory<0,2,3,1>::type xzwy;
                typename TProxyFactory<0,2,3,2>::type xzwz;
                typename TProxyFactory<0,2,3,3>::type xzww;
                typename TProxyFactory<0,3,0,0>::type xwxx;
                typename TProxyFactory<0,3,0,1>::type xwxy;
                typename TProxyFactory<0,3,0,2>::type xwxz;
                typename TProxyFactory<0,3,0,3>::type xwxw;
                typename TProxyFactory<0,3,1,0>::type xwyx;
                typename TProxyFactory<0,3,1,1>::type xwyy;
                typename TProxyFactory<0,3,1,2>::type xwyz;
                typename TProxyFactory<0,3,1,3>::type xwyw;
                typename TProxyFactory<0,3,2,0>::type xwzx;
                typename TProxyFactory<0,3,2,1>::type xwzy;
                typename TProxyFactory<0,3,2,2>::type xwzz;
                typename TProxyFactory<0,3,2,3>::type xwzw;
                typename TProxyFactory<0,3,3,0>::type xwwx;
                typename TProxyFactory<0,3,3,1>::type xwwy;
                typename TProxyFactory<0,3,3,2>::type xwwz;
                typename TProxyFactory<0,3,3,3>::type xwww;
                typename TProxyFactory<1,0,0,0>::type yxxx;
                typename TProxyFactory<1,0,0,1>::type yxxy;
                typename TProxyFactory<1,0,0,2>::type yxxz;
                typename TProxyFactory<1,0,0,3>::type yxxw;
                typename TProxyFactory<1,0,1,0>::type yxyx;
                typename TProxyFactory<1,0,1,1>::type yxyy;
                typename TProxyFactory<1,0,1,2>::type yxyz;
                typename TProxyFactory<1,0,1,3>::type yxyw;
                typename TProxyFactory<1,0,2,0>::type yxzx;
                typename TProxyFactory<1,0,2,1>::type yxzy;
                typename TProxyFactory<1,0,2,2>::type yxzz;
                typename TProxyFactory<1,0,2,3>::type yxzw;
                typename TProxyFactory<1,0,3,0>::type yxwx;
                typename TProxyFactory<1,0,3,1>::type yxwy;
                typename TProxyFactory<1,0,3,2>::type yxwz;
                typename TProxyFactory<1,0,3,3>::type yxww;
                typename TProxyFactory<1,1,0,0>::type yyxx;
                typename TProxyFactory<1,1,0,1>::type yyxy;
                typename TProxyFactory<1,1,0,2>::type yyxz;
                typename TProxyFactory<1,1,0,3>::type yyxw;
                typename TProxyFactory<1,1,1,0>::type yyyx;
                typename TProxyFactory<1,1,1,1>::type yyyy;
                typename TProxyFactory<1,1,1,2>::type yyyz;
                typename TProxyFactory<1,1,1,3>::type yyyw;
                typename TProxyFactory<1,1,2,0>::type yyzx;
                typename TProxyFactory<1,1,2,1>::type yyzy;
                typename TProxyFactory<1,1,2,2>::type yyzz;
                typename TProxyFactory<1,1,2,3>::type yyzw;
                typename TProxyFactory<1,1,3,0>::type yywx;
                typename TProxyFactory<1,1,3,1>::type yywy;
                typename TProxyFactory<1,1,3,2>::type yywz;
                typename TProxyFactory<1,1,3,3>::type yyww;
                typename TProxyFactory<1,2,0,0>::type yzxx;
                typename TProxyFactory<1,2,0,1>::type yzxy;
                typename TProxyFactory<1,2,0,2>::type yzxz;
                typename TProxyFactory<1,2,0,3>::type yzxw;
                typename TProxyFactory<1,2,1,0>::type yzyx;
                typename TProxyFactory<1,2,1,1>::type yzyy;
                typename TProxyFactory<1,2,1,2>::type yzyz;
                typename TProxyFactory<1,2,1,3>::type yzyw;
                typename TProxyFactory<1,2,2,0>::type yzzx;
                typename TProxyFactory<1,2,2,1>::type yzzy;
                typename TProxyFactory<1,2,2,2>::type yzzz;
                typename TProxyFactory<1,2,2,3>::type yzzw;
                typename TProxyFactory<1,2,3,0>::type yzwx;
                typename TProxyFactory<1,2,3,1>::type yzwy;
                typename TProxyFactory<1,2,3,2>::type yzwz;
                typename TProxyFactory<1,2,3,3>::type yzww;
                typename TProxyFactory<1,3,0,0>::type ywxx;
                typename TProxyFactory<1,3,0,1>::type ywxy;
                typename TProxyFactory<1,3,0,2>::type ywxz;
                typename TProxyFactory<1,3,0,3>::type ywxw;
                typename TProxyFactory<1,3,1,0>::type ywyx;
                typename TProxyFactory<1,3,1,1>::type ywyy;
                typename TProxyFactory<1,3,1,2>::type ywyz;
                typename TProxyFactory<1,3,1,3>::type ywyw;
                typename TProxyFactory<1,3,2,0>::type ywzx;
                typename TProxyFactory<1,3,2,1>::type ywzy;
                typename TProxyFactory<1,3,2,2>::type ywzz;
                typename TProxyFactory<1,3,2,3>::type ywzw;
                typename TProxyFactory<1,3,3,0>::type ywwx;
                typename TProxyFactory<1,3,3,1>::type ywwy;
                typename TProxyFactory<1,3,3,2>::type ywwz;
                typename TProxyFactory<1,3,3,3>::type ywww;
                typename TProxyFactory<2,0,0,0>::type zxxx;
                typename TProxyFactory<2,0,0,1>::type zxxy;
                typename TProxyFactory<2,0,0,2>::type zxxz;
                typename TProxyFactory<2,0,0,3>::type zxxw;
                typename TProxyFactory<2,0,1,0>::type zxyx;
                typename TProxyFactory<2,0,1,1>::type zxyy;
                typename TProxyFactory<2,0,1,2>::type zxyz;
                typename TProxyFactory<2,0,1,3>::type zxyw;
                typename TProxyFactory<2,0,2,0>::type zxzx;
                typename TProxyFactory<2,0,2,1>::type zxzy;
                typename TProxyFactory<2,0,2,2>::type zxzz;
                typename TProxyFactory<2,0,2,3>::type zxzw;
                typename TProxyFactory<2,0,3,0>::type zxwx;
                typename TProxyFactory<2,0,3,1>::type zxwy;
                typename TProxyFactory<2,0,3,2>::type zxwz;
                typename TProxyFactory<2,0,3,3>::type zxww;
                typename TProxyFactory<2,1,0,0>::type zyxx;
                typename TProxyFactory<2,1,0,1>::type zyxy;
                typename TProxyFactory<2,1,0,2>::type zyxz;
                typename TProxyFactory<2,1,0,3>::type zyxw;
                typename TProxyFactory<2,1,1,0>::type zyyx;
                typename TProxyFactory<2,1,1,1>::type zyyy;
                typename TProxyFactory<2,1,1,2>::type zyyz;
                typename TProxyFactory<2,1,1,3>::type zyyw;
                typename TProxyFactory<2,1,2,0>::type zyzx;
                typename TProxyFactory<2,1,2,1>::type zyzy;
                typename TProxyFactory<2,1,2,2>::type zyzz;
                typename TProxyFactory<2,1,2,3>::type zyzw;
                typename TProxyFactory<2,1,3,0>::type zywx;
                typename TProxyFactory<2,1,3,1>::type zywy;
                typename TProxyFactory<2,1,3,2>::type zywz;
                typename TProxyFactory<2,1,3,3>::type zyww;
                typename TProxyFactory<2,2,0,0>::type zzxx;
                typename TProxyFactory<2,2,0,1>::type zzxy;
                typename TProxyFactory<2,2,0,2>::type zzxz;
                typename TProxyFactory<2,2,0,3>::type zzxw;
                typename TProxyFactory<2,2,1,0>::type zzyx;
                typename TProxyFactory<2,2,1,1>::type zzyy;
                typename TProxyFactory<2,2,1,2>::type zzyz;
                typename TProxyFactory<2,2,1,3>::type zzyw;
                typename TProxyFactory<2,2,2,0>::type zzzx;
                typename TProxyFactory<2,2,2,1>::type zzzy;
                typename TProxyFactory<2,2,2,2>::type zzzz;
                typename TProxyFactory<2,2,2,3>::type zzzw;
                typename TProxyFactory<2,2,3,0>::type zzwx;
                typename TProxyFactory<2,2,3,1>::type zzwy;
                typename TProxyFactory<2,2,3,2>::type zzwz;
                typename TProxyFactory<2,2,3,3>::type zzww;
                typename TProxyFactory<2,3,0,0>::type zwxx;
                typename TProxyFactory<2,3,0,1>::type zwxy;
                typename TProxyFactory<2,3,0,2>::type zwxz;
                typename TProxyFactory<2,3,0,3>::type zwxw;
                typename TProxyFactory<2,3,1,0>::type zwyx;
                typename TProxyFactory<2,3,1,1>::type zwyy;
                typename TProxyFactory<2,3,1,2>::type zwyz;
                typename TProxyFactory<2,3,1,3>::type zwyw;
                typename TProxyFactory<2,3,2,0>::type zwzx;
                typename TProxyFactory<2,3,2,1>::type zwzy;
                typename TProxyFactory<2,3,2,2>::type zwzz;
                typename TProxyFactory<2,3,2,3>::type zwzw;
                typename TProxyFactory<2,3,3,0>::type zwwx;
                typename TProxyFactory<2,3,3,1>::type zwwy;
                typename TProxyFactory<2,3,3,2>::type zwwz;
                typename TProxyFactory<2,3,3,3>::type zwww;
                typename TProxyFactory<3,0,0,0>::type wxxx;
                typename TProxyFactory<3,0,0,1>::type wxxy;
                typename TProxyFactory<3,0,0,2>::type wxxz;
                typename TProxyFactory<3,0,0,3>::type wxxw;
                typename TProxyFactory<3,0,1,0>::type wxyx;
                typename TProxyFactory<3,0,1,1>::type wxyy;
                typename TProxyFactory<3,0,1,2>::type wxyz;
                typename TProxyFactory<3,0,1,3>::type wxyw;
                typename TProxyFactory<3,0,2,0>::type wxzx;
                typename TProxyFactory<3,0,2,1>::type wxzy;
                typename TProxyFactory<3,0,2,2>::type wxzz;
                typename TProxyFactory<3,0,2,3>::type wxzw;
                typename TProxyFactory<3,0,3,0>::type wxwx;
                typename TProxyFactory<3,0,3,1>::type wxwy;
                typename TProxyFactory<3,0,3,2>::type wxwz;
                typename TProxyFactory<3,0,3,3>::type wxww;
                typename TProxyFactory<3,1,0,0>::type wyxx;
                typename TProxyFactory<3,1,0,1>::type wyxy;
                typename TProxyFactory<3,1,0,2>::type wyxz;
                typename TProxyFactory<3,1,0,3>::type wyxw;
                typename TProxyFactory<3,1,1,0>::type wyyx;
                typename TProxyFactory<3,1,1,1>::type wyyy;
                typename TProxyFactory<3,1,1,2>::type wyyz;
                typename TProxyFactory<3,1,1,3>::type wyyw;
                typename TProxyFactory<3,1,2,0>::type wyzx;
                typename TProxyFactory<3,1,2,1>::type wyzy;
                typename TProxyFactory<3,1,2,2>::type wyzz;
                typename TProxyFactory<3,1,2,3>::type wyzw;
                typename TProxyFactory<3,1,3,0>::type wywx;
                typename TProxyFactory<3,1,3,1>::type wywy;
                typename TProxyFactory<3,1,3,2>::type wywz;
                typename TProxyFactory<3,1,3,3>::type wyww;
                typename TProxyFactory<3,2,0,0>::type wzxx;
                typename TProxyFactory<3,2,0,1>::type wzxy;
                typename TProxyFactory<3,2,0,2>::type wzxz;
                typename TProxyFactory<3,2,0,3>::type wzxw;
                typename TProxyFactory<3,2,1,0>::type wzyx;
                typename TProxyFactory<3,2,1,1>::type wzyy;
                typename TProxyFactory<3,2,1,2>::type wzyz;
                typename TProxyFactory<3,2,1,3>::type wzyw;
                typename TProxyFactory<3,2,2,0>::type wzzx;
                typename TProxyFactory<3,2,2,1>::type wzzy;
                typename TProxyFactory<3,2,2,2>::type wzzz;
                typename TProxyFactory<3,2,2,3>::type wzzw;
                typename TProxyFactory<3,2,3,0>::type wzwx;
                typename TProxyFactory<3,2,3,1>::type wzwy;
                typename TProxyFactory<3,2,3,2>::type wzwz;
                typename TProxyFactory<3,2,3,3>::type wzww;
                typename TProxyFactory<3,3,0,0>::type wwxx;
                typename TProxyFactory<3,3,0,1>::type wwxy;
                typename TProxyFactory<3,3,0,2>::type wwxz;
                typename TProxyFactory<3,3,0,3>::type wwxw;
                typename TProxyFactory<3,3,1,0>::type wwyx;
                typename TProxyFactory<3,3,1,1>::type wwyy;
                typename TProxyFactory<3,3,1,2>::type wwyz;
                typename TProxyFactory<3,3,1,3>::type wwyw;
                typename TProxyFactory<3,3,2,0>::type wwzx;
                typename TProxyFactory<3,3,2,1>::type wwzy;
                typename TProxyFactory<3,3,2,2>::type wwzz;
                typename TProxyFactory<3,3,2,3>::type wwzw;
                typename TProxyFactory<3,3,3,0>::type wwwx;
                typename TProxyFactory<3,3,3,1>::type wwwy;
                typename TProxyFactory<3,3,3,2>::type wwwz;
                typename TProxyFactory<3,3,3,3>::type wwww;
            };
        };
    }
}