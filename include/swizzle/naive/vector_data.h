#ifndef HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_DATA
#define HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_DATA

#include "../detail/vector_binary_operators.h"
#include "../detail/writable_wrapper.h"
#include "vector_proxy.h"

namespace swizzle
{
    namespace naive
    {
        template <template <class, size_t> class TVector, class TScalar, size_t NumComponents>
        struct naive_vector_data;

        template <template <class, size_t> class TVector, class TScalar, size_t NumComponents>
        struct naive_vector_data_base
        {
            typedef TScalar scalar_type;
            static const size_t num_of_components = NumComponents;

            template <size_t x>
            struct proxy1_factory
            {
                typedef vector_proxy< TVector<scalar_type, 1>, scalar_type, num_of_components, x> proxy_type;
                typedef typename std::conditional < proxy_type::is_writable, detail::writable_wrapper<proxy_type>, proxy_type>::type type;
            };
            template <size_t x, size_t y>
            struct proxy2_factory
            {
                typedef vector_proxy< TVector<scalar_type, 2>, scalar_type, num_of_components, x, y> proxy_type;
                typedef typename std::conditional < proxy_type::is_writable, detail::writable_wrapper<proxy_type>, proxy_type>::type type;
            };
            template <size_t x, size_t y, size_t z>
            struct proxy3_factory
            {
                typedef vector_proxy< TVector<scalar_type, 3>, scalar_type, num_of_components, x, y, z> proxy_type;
                typedef typename std::conditional < proxy_type::is_writable, detail::writable_wrapper<proxy_type>, proxy_type>::type type;
            };
            template <size_t x, size_t y, size_t z, size_t w>
            struct proxy4_factory
            {
                typedef vector_proxy< TVector<scalar_type, 4>, scalar_type, num_of_components, x, y, z, w> proxy_type;
                typedef typename std::conditional < proxy_type::is_writable, detail::writable_wrapper<proxy_type>, proxy_type>::type type;
            };
        };

        template <template <class, size_t> class TVector, class TScalar>
        struct naive_vector_data< TVector, TScalar, 1 > : naive_vector_data_base<TVector, TScalar, 1>
        {
            typedef naive_vector_data_base<TVector, TScalar, 1> base_type;
            using base_type::scalar_type;
            using base_type::num_of_components;
            using base_type::proxy2_factory;
            using base_type::proxy3_factory;
            using base_type::proxy4_factory;

            union
            {
                scalar_type _components[num_of_components];

                struct 
                {
                    scalar_type _x;
                };

                typename proxy1_factory<0>::type x;
                typename proxy2_factory<0,0>::type xx;
                typename proxy3_factory<0,0,0>::type xxx;
                typename proxy4_factory<0,0,0,0>::type xxxx;
            };
        };

        template <template <class, size_t> class TVector, class TScalar>
        struct naive_vector_data< TVector, TScalar, 2 > : naive_vector_data_base<TVector, TScalar, 2>
        {
            typedef naive_vector_data_base<TVector, TScalar, 2> base_type;
            using base_type::scalar_type;
            using base_type::num_of_components;
            using base_type::proxy2_factory;
            using base_type::proxy3_factory;
            using base_type::proxy4_factory;

            union
            {
                scalar_type _components[num_of_components];

                struct 
                {
                    scalar_type _x;
                    scalar_type _y;
                };

                struct 
                {
                    scalar_type x;
                    scalar_type y;
                };

                /*typename proxy1_factory<0>::type x;
                typename proxy1_factory<1>::type y;*/
                typename proxy2_factory<0,0>::type xx;
                typename proxy2_factory<0,1>::type xy;
                typename proxy2_factory<1,0>::type yx;
                typename proxy2_factory<1,1>::type yy;
                typename proxy3_factory<0,0,0>::type xxx;
                typename proxy3_factory<0,0,1>::type xxy;
                typename proxy3_factory<0,1,0>::type xyx;
                typename proxy3_factory<0,1,1>::type xyy;
                typename proxy3_factory<1,0,0>::type yxx;
                typename proxy3_factory<1,0,1>::type yxy;
                typename proxy3_factory<1,1,0>::type yyx;
                typename proxy3_factory<1,1,1>::type yyy;
                typename proxy4_factory<0,0,0,0>::type xxxx;
                typename proxy4_factory<0,0,0,1>::type xxxy;
                typename proxy4_factory<0,0,1,0>::type xxyx;
                typename proxy4_factory<0,0,1,1>::type xxyy;
                typename proxy4_factory<0,1,0,0>::type xyxx;
                typename proxy4_factory<0,1,0,1>::type xyxy;
                typename proxy4_factory<0,1,1,0>::type xyyx;
                typename proxy4_factory<0,1,1,1>::type xyyy;
                typename proxy4_factory<1,0,0,0>::type yxxx;
                typename proxy4_factory<1,0,0,1>::type yxxy;
                typename proxy4_factory<1,0,1,0>::type yxyx;
                typename proxy4_factory<1,0,1,1>::type yxyy;
                typename proxy4_factory<1,1,0,0>::type yyxx;
                typename proxy4_factory<1,1,0,1>::type yyxy;
                typename proxy4_factory<1,1,1,0>::type yyyx;
                typename proxy4_factory<1,1,1,1>::type yyyy;
            };
        };

        template <template <class, size_t> class TVector, class TScalar>
        struct naive_vector_data< TVector, TScalar, 3 > : naive_vector_data_base<TVector, TScalar, 3>
        {
            typedef naive_vector_data_base<TVector, TScalar, 3> base_type;
            using base_type::scalar_type;
            using base_type::num_of_components;
            using base_type::proxy2_factory;
            using base_type::proxy3_factory;
            using base_type::proxy4_factory;

            union
            {
                scalar_type _components[num_of_components];

                struct 
                {
                    scalar_type _x;
                    scalar_type _y;
                    scalar_type _z;
                };

                struct 
                {
                    scalar_type x;
                    scalar_type y;
                    scalar_type z;
                  
                };

                /*typename proxy1_factory<0>::type x;
                typename proxy1_factory<1>::type y;
                typename proxy1_factory<2>::type z;*/

                typename proxy2_factory<0,0>::type xx;
                typename proxy2_factory<0,1>::type xy;
                typename proxy2_factory<0,2>::type xz;
                typename proxy2_factory<1,0>::type yx;
                typename proxy2_factory<1,1>::type yy;
                typename proxy2_factory<1,2>::type yz;
                typename proxy2_factory<2,0>::type zx;
                typename proxy2_factory<2,1>::type zy;
                typename proxy2_factory<2,2>::type zz;
                typename proxy3_factory<0,0,0>::type xxx;
                typename proxy3_factory<0,0,1>::type xxy;
                typename proxy3_factory<0,0,2>::type xxz;
                typename proxy3_factory<0,1,0>::type xyx;
                typename proxy3_factory<0,1,1>::type xyy;
                typename proxy3_factory<0,1,2>::type xyz;
                typename proxy3_factory<0,2,0>::type xzx;
                typename proxy3_factory<0,2,1>::type xzy;
                typename proxy3_factory<0,2,2>::type xzz;
                typename proxy3_factory<1,0,0>::type yxx;
                typename proxy3_factory<1,0,1>::type yxy;
                typename proxy3_factory<1,0,2>::type yxz;
                typename proxy3_factory<1,1,0>::type yyx;
                typename proxy3_factory<1,1,1>::type yyy;
                typename proxy3_factory<1,1,2>::type yyz;
                typename proxy3_factory<1,2,0>::type yzx;
                typename proxy3_factory<1,2,1>::type yzy;
                typename proxy3_factory<1,2,2>::type yzz;
                typename proxy3_factory<2,0,0>::type zxx;
                typename proxy3_factory<2,0,1>::type zxy;
                typename proxy3_factory<2,0,2>::type zxz;
                typename proxy3_factory<2,1,0>::type zyx;
                typename proxy3_factory<2,1,1>::type zyy;
                typename proxy3_factory<2,1,2>::type zyz;
                typename proxy3_factory<2,2,0>::type zzx;
                typename proxy3_factory<2,2,1>::type zzy;
                typename proxy3_factory<2,2,2>::type zzz;
                typename proxy4_factory<0,0,0,0>::type xxxx;
                typename proxy4_factory<0,0,0,1>::type xxxy;
                typename proxy4_factory<0,0,0,2>::type xxxz;
                typename proxy4_factory<0,0,1,0>::type xxyx;
                typename proxy4_factory<0,0,1,1>::type xxyy;
                typename proxy4_factory<0,0,1,2>::type xxyz;
                typename proxy4_factory<0,0,2,0>::type xxzx;
                typename proxy4_factory<0,0,2,1>::type xxzy;
                typename proxy4_factory<0,0,2,2>::type xxzz;
                typename proxy4_factory<0,1,0,0>::type xyxx;
                typename proxy4_factory<0,1,0,1>::type xyxy;
                typename proxy4_factory<0,1,0,2>::type xyxz;
                typename proxy4_factory<0,1,1,0>::type xyyx;
                typename proxy4_factory<0,1,1,1>::type xyyy;
                typename proxy4_factory<0,1,1,2>::type xyyz;
                typename proxy4_factory<0,1,2,0>::type xyzx;
                typename proxy4_factory<0,1,2,1>::type xyzy;
                typename proxy4_factory<0,1,2,2>::type xyzz;
                typename proxy4_factory<0,2,0,0>::type xzxx;
                typename proxy4_factory<0,2,0,1>::type xzxy;
                typename proxy4_factory<0,2,0,2>::type xzxz;
                typename proxy4_factory<0,2,1,0>::type xzyx;
                typename proxy4_factory<0,2,1,1>::type xzyy;
                typename proxy4_factory<0,2,1,2>::type xzyz;
                typename proxy4_factory<0,2,2,0>::type xzzx;
                typename proxy4_factory<0,2,2,1>::type xzzy;
                typename proxy4_factory<0,2,2,2>::type xzzz;
                typename proxy4_factory<1,0,0,0>::type yxxx;
                typename proxy4_factory<1,0,0,1>::type yxxy;
                typename proxy4_factory<1,0,0,2>::type yxxz;
                typename proxy4_factory<1,0,1,0>::type yxyx;
                typename proxy4_factory<1,0,1,1>::type yxyy;
                typename proxy4_factory<1,0,1,2>::type yxyz;
                typename proxy4_factory<1,0,2,0>::type yxzx;
                typename proxy4_factory<1,0,2,1>::type yxzy;
                typename proxy4_factory<1,0,2,2>::type yxzz;
                typename proxy4_factory<1,1,0,0>::type yyxx;
                typename proxy4_factory<1,1,0,1>::type yyxy;
                typename proxy4_factory<1,1,0,2>::type yyxz;
                typename proxy4_factory<1,1,1,0>::type yyyx;
                typename proxy4_factory<1,1,1,1>::type yyyy;
                typename proxy4_factory<1,1,1,2>::type yyyz;
                typename proxy4_factory<1,1,2,0>::type yyzx;
                typename proxy4_factory<1,1,2,1>::type yyzy;
                typename proxy4_factory<1,1,2,2>::type yyzz;
                typename proxy4_factory<1,2,0,0>::type yzxx;
                typename proxy4_factory<1,2,0,1>::type yzxy;
                typename proxy4_factory<1,2,0,2>::type yzxz;
                typename proxy4_factory<1,2,1,0>::type yzyx;
                typename proxy4_factory<1,2,1,1>::type yzyy;
                typename proxy4_factory<1,2,1,2>::type yzyz;
                typename proxy4_factory<1,2,2,0>::type yzzx;
                typename proxy4_factory<1,2,2,1>::type yzzy;
                typename proxy4_factory<1,2,2,2>::type yzzz;
                typename proxy4_factory<2,0,0,0>::type zxxx;
                typename proxy4_factory<2,0,0,1>::type zxxy;
                typename proxy4_factory<2,0,0,2>::type zxxz;
                typename proxy4_factory<2,0,1,0>::type zxyx;
                typename proxy4_factory<2,0,1,1>::type zxyy;
                typename proxy4_factory<2,0,1,2>::type zxyz;
                typename proxy4_factory<2,0,2,0>::type zxzx;
                typename proxy4_factory<2,0,2,1>::type zxzy;
                typename proxy4_factory<2,0,2,2>::type zxzz;
                typename proxy4_factory<2,1,0,0>::type zyxx;
                typename proxy4_factory<2,1,0,1>::type zyxy;
                typename proxy4_factory<2,1,0,2>::type zyxz;
                typename proxy4_factory<2,1,1,0>::type zyyx;
                typename proxy4_factory<2,1,1,1>::type zyyy;
                typename proxy4_factory<2,1,1,2>::type zyyz;
                typename proxy4_factory<2,1,2,0>::type zyzx;
                typename proxy4_factory<2,1,2,1>::type zyzy;
                typename proxy4_factory<2,1,2,2>::type zyzz;
                typename proxy4_factory<2,2,0,0>::type zzxx;
                typename proxy4_factory<2,2,0,1>::type zzxy;
                typename proxy4_factory<2,2,0,2>::type zzxz;
                typename proxy4_factory<2,2,1,0>::type zzyx;
                typename proxy4_factory<2,2,1,1>::type zzyy;
                typename proxy4_factory<2,2,1,2>::type zzyz;
                typename proxy4_factory<2,2,2,0>::type zzzx;
                typename proxy4_factory<2,2,2,1>::type zzzy;
                typename proxy4_factory<2,2,2,2>::type zzzz;
            };
        };

        template <template <class, size_t> class TVector, class TScalar>
        struct naive_vector_data< TVector, TScalar, 4 > : naive_vector_data_base<TVector, TScalar, 4>
        {
            typedef naive_vector_data_base<TVector, TScalar, 4> base_type;
            using base_type::scalar_type;
            using base_type::num_of_components;
            using base_type::proxy2_factory;
            using base_type::proxy3_factory;
            using base_type::proxy4_factory;

            union
            {
                scalar_type _components[num_of_components];

                struct 
                {
                    scalar_type _x;
                    scalar_type _y;
                    scalar_type _z;
                    scalar_type _w;
                };

                struct 
                {
                    scalar_type x;
                    scalar_type y;
                    scalar_type z;
                    scalar_type w;
                };


//                 typename proxy1_factory<0>::type x;
//                 typename proxy1_factory<1>::type y;
//                 typename proxy1_factory<2>::type z;
//                 typename proxy1_factory<3>::type w;

                typename proxy2_factory<0,0>::type xx;
                typename proxy2_factory<0,1>::type xy;
                typename proxy2_factory<0,2>::type xz;
                typename proxy2_factory<0,3>::type xw;
                typename proxy2_factory<1,0>::type yx;
                typename proxy2_factory<1,1>::type yy;
                typename proxy2_factory<1,2>::type yz;
                typename proxy2_factory<1,3>::type yw;
                typename proxy2_factory<2,0>::type zx;
                typename proxy2_factory<2,1>::type zy;
                typename proxy2_factory<2,2>::type zz;
                typename proxy2_factory<2,3>::type zw;
                typename proxy2_factory<3,0>::type wx;
                typename proxy2_factory<3,1>::type wy;
                typename proxy2_factory<3,2>::type wz;
                typename proxy2_factory<3,3>::type ww;
                typename proxy3_factory<0,0,0>::type xxx;
                typename proxy3_factory<0,0,1>::type xxy;
                typename proxy3_factory<0,0,2>::type xxz;
                typename proxy3_factory<0,0,3>::type xxw;
                typename proxy3_factory<0,1,0>::type xyx;
                typename proxy3_factory<0,1,1>::type xyy;
                typename proxy3_factory<0,1,2>::type xyz;
                typename proxy3_factory<0,1,3>::type xyw;
                typename proxy3_factory<0,2,0>::type xzx;
                typename proxy3_factory<0,2,1>::type xzy;
                typename proxy3_factory<0,2,2>::type xzz;
                typename proxy3_factory<0,2,3>::type xzw;
                typename proxy3_factory<0,3,0>::type xwx;
                typename proxy3_factory<0,3,1>::type xwy;
                typename proxy3_factory<0,3,2>::type xwz;
                typename proxy3_factory<0,3,3>::type xww;
                typename proxy3_factory<1,0,0>::type yxx;
                typename proxy3_factory<1,0,1>::type yxy;
                typename proxy3_factory<1,0,2>::type yxz;
                typename proxy3_factory<1,0,3>::type yxw;
                typename proxy3_factory<1,1,0>::type yyx;
                typename proxy3_factory<1,1,1>::type yyy;
                typename proxy3_factory<1,1,2>::type yyz;
                typename proxy3_factory<1,1,3>::type yyw;
                typename proxy3_factory<1,2,0>::type yzx;
                typename proxy3_factory<1,2,1>::type yzy;
                typename proxy3_factory<1,2,2>::type yzz;
                typename proxy3_factory<1,2,3>::type yzw;
                typename proxy3_factory<1,3,0>::type ywx;
                typename proxy3_factory<1,3,1>::type ywy;
                typename proxy3_factory<1,3,2>::type ywz;
                typename proxy3_factory<1,3,3>::type yww;
                typename proxy3_factory<2,0,0>::type zxx;
                typename proxy3_factory<2,0,1>::type zxy;
                typename proxy3_factory<2,0,2>::type zxz;
                typename proxy3_factory<2,0,3>::type zxw;
                typename proxy3_factory<2,1,0>::type zyx;
                typename proxy3_factory<2,1,1>::type zyy;
                typename proxy3_factory<2,1,2>::type zyz;
                typename proxy3_factory<2,1,3>::type zyw;
                typename proxy3_factory<2,2,0>::type zzx;
                typename proxy3_factory<2,2,1>::type zzy;
                typename proxy3_factory<2,2,2>::type zzz;
                typename proxy3_factory<2,2,3>::type zzw;
                typename proxy3_factory<2,3,0>::type zwx;
                typename proxy3_factory<2,3,1>::type zwy;
                typename proxy3_factory<2,3,2>::type zwz;
                typename proxy3_factory<2,3,3>::type zww;
                typename proxy3_factory<3,0,0>::type wxx;
                typename proxy3_factory<3,0,1>::type wxy;
                typename proxy3_factory<3,0,2>::type wxz;
                typename proxy3_factory<3,0,3>::type wxw;
                typename proxy3_factory<3,1,0>::type wyx;
                typename proxy3_factory<3,1,1>::type wyy;
                typename proxy3_factory<3,1,2>::type wyz;
                typename proxy3_factory<3,1,3>::type wyw;
                typename proxy3_factory<3,2,0>::type wzx;
                typename proxy3_factory<3,2,1>::type wzy;
                typename proxy3_factory<3,2,2>::type wzz;
                typename proxy3_factory<3,2,3>::type wzw;
                typename proxy3_factory<3,3,0>::type wwx;
                typename proxy3_factory<3,3,1>::type wwy;
                typename proxy3_factory<3,3,2>::type wwz;
                typename proxy3_factory<3,3,3>::type www;
                typename proxy4_factory<0,0,0,0>::type xxxx;
                typename proxy4_factory<0,0,0,1>::type xxxy;
                typename proxy4_factory<0,0,0,2>::type xxxz;
                typename proxy4_factory<0,0,0,3>::type xxxw;
                typename proxy4_factory<0,0,1,0>::type xxyx;
                typename proxy4_factory<0,0,1,1>::type xxyy;
                typename proxy4_factory<0,0,1,2>::type xxyz;
                typename proxy4_factory<0,0,1,3>::type xxyw;
                typename proxy4_factory<0,0,2,0>::type xxzx;
                typename proxy4_factory<0,0,2,1>::type xxzy;
                typename proxy4_factory<0,0,2,2>::type xxzz;
                typename proxy4_factory<0,0,2,3>::type xxzw;
                typename proxy4_factory<0,0,3,0>::type xxwx;
                typename proxy4_factory<0,0,3,1>::type xxwy;
                typename proxy4_factory<0,0,3,2>::type xxwz;
                typename proxy4_factory<0,0,3,3>::type xxww;
                typename proxy4_factory<0,1,0,0>::type xyxx;
                typename proxy4_factory<0,1,0,1>::type xyxy;
                typename proxy4_factory<0,1,0,2>::type xyxz;
                typename proxy4_factory<0,1,0,3>::type xyxw;
                typename proxy4_factory<0,1,1,0>::type xyyx;
                typename proxy4_factory<0,1,1,1>::type xyyy;
                typename proxy4_factory<0,1,1,2>::type xyyz;
                typename proxy4_factory<0,1,1,3>::type xyyw;
                typename proxy4_factory<0,1,2,0>::type xyzx;
                typename proxy4_factory<0,1,2,1>::type xyzy;
                typename proxy4_factory<0,1,2,2>::type xyzz;
                typename proxy4_factory<0,1,2,3>::type xyzw;
                typename proxy4_factory<0,1,3,0>::type xywx;
                typename proxy4_factory<0,1,3,1>::type xywy;
                typename proxy4_factory<0,1,3,2>::type xywz;
                typename proxy4_factory<0,1,3,3>::type xyww;
                typename proxy4_factory<0,2,0,0>::type xzxx;
                typename proxy4_factory<0,2,0,1>::type xzxy;
                typename proxy4_factory<0,2,0,2>::type xzxz;
                typename proxy4_factory<0,2,0,3>::type xzxw;
                typename proxy4_factory<0,2,1,0>::type xzyx;
                typename proxy4_factory<0,2,1,1>::type xzyy;
                typename proxy4_factory<0,2,1,2>::type xzyz;
                typename proxy4_factory<0,2,1,3>::type xzyw;
                typename proxy4_factory<0,2,2,0>::type xzzx;
                typename proxy4_factory<0,2,2,1>::type xzzy;
                typename proxy4_factory<0,2,2,2>::type xzzz;
                typename proxy4_factory<0,2,2,3>::type xzzw;
                typename proxy4_factory<0,2,3,0>::type xzwx;
                typename proxy4_factory<0,2,3,1>::type xzwy;
                typename proxy4_factory<0,2,3,2>::type xzwz;
                typename proxy4_factory<0,2,3,3>::type xzww;
                typename proxy4_factory<0,3,0,0>::type xwxx;
                typename proxy4_factory<0,3,0,1>::type xwxy;
                typename proxy4_factory<0,3,0,2>::type xwxz;
                typename proxy4_factory<0,3,0,3>::type xwxw;
                typename proxy4_factory<0,3,1,0>::type xwyx;
                typename proxy4_factory<0,3,1,1>::type xwyy;
                typename proxy4_factory<0,3,1,2>::type xwyz;
                typename proxy4_factory<0,3,1,3>::type xwyw;
                typename proxy4_factory<0,3,2,0>::type xwzx;
                typename proxy4_factory<0,3,2,1>::type xwzy;
                typename proxy4_factory<0,3,2,2>::type xwzz;
                typename proxy4_factory<0,3,2,3>::type xwzw;
                typename proxy4_factory<0,3,3,0>::type xwwx;
                typename proxy4_factory<0,3,3,1>::type xwwy;
                typename proxy4_factory<0,3,3,2>::type xwwz;
                typename proxy4_factory<0,3,3,3>::type xwww;
                typename proxy4_factory<1,0,0,0>::type yxxx;
                typename proxy4_factory<1,0,0,1>::type yxxy;
                typename proxy4_factory<1,0,0,2>::type yxxz;
                typename proxy4_factory<1,0,0,3>::type yxxw;
                typename proxy4_factory<1,0,1,0>::type yxyx;
                typename proxy4_factory<1,0,1,1>::type yxyy;
                typename proxy4_factory<1,0,1,2>::type yxyz;
                typename proxy4_factory<1,0,1,3>::type yxyw;
                typename proxy4_factory<1,0,2,0>::type yxzx;
                typename proxy4_factory<1,0,2,1>::type yxzy;
                typename proxy4_factory<1,0,2,2>::type yxzz;
                typename proxy4_factory<1,0,2,3>::type yxzw;
                typename proxy4_factory<1,0,3,0>::type yxwx;
                typename proxy4_factory<1,0,3,1>::type yxwy;
                typename proxy4_factory<1,0,3,2>::type yxwz;
                typename proxy4_factory<1,0,3,3>::type yxww;
                typename proxy4_factory<1,1,0,0>::type yyxx;
                typename proxy4_factory<1,1,0,1>::type yyxy;
                typename proxy4_factory<1,1,0,2>::type yyxz;
                typename proxy4_factory<1,1,0,3>::type yyxw;
                typename proxy4_factory<1,1,1,0>::type yyyx;
                typename proxy4_factory<1,1,1,1>::type yyyy;
                typename proxy4_factory<1,1,1,2>::type yyyz;
                typename proxy4_factory<1,1,1,3>::type yyyw;
                typename proxy4_factory<1,1,2,0>::type yyzx;
                typename proxy4_factory<1,1,2,1>::type yyzy;
                typename proxy4_factory<1,1,2,2>::type yyzz;
                typename proxy4_factory<1,1,2,3>::type yyzw;
                typename proxy4_factory<1,1,3,0>::type yywx;
                typename proxy4_factory<1,1,3,1>::type yywy;
                typename proxy4_factory<1,1,3,2>::type yywz;
                typename proxy4_factory<1,1,3,3>::type yyww;
                typename proxy4_factory<1,2,0,0>::type yzxx;
                typename proxy4_factory<1,2,0,1>::type yzxy;
                typename proxy4_factory<1,2,0,2>::type yzxz;
                typename proxy4_factory<1,2,0,3>::type yzxw;
                typename proxy4_factory<1,2,1,0>::type yzyx;
                typename proxy4_factory<1,2,1,1>::type yzyy;
                typename proxy4_factory<1,2,1,2>::type yzyz;
                typename proxy4_factory<1,2,1,3>::type yzyw;
                typename proxy4_factory<1,2,2,0>::type yzzx;
                typename proxy4_factory<1,2,2,1>::type yzzy;
                typename proxy4_factory<1,2,2,2>::type yzzz;
                typename proxy4_factory<1,2,2,3>::type yzzw;
                typename proxy4_factory<1,2,3,0>::type yzwx;
                typename proxy4_factory<1,2,3,1>::type yzwy;
                typename proxy4_factory<1,2,3,2>::type yzwz;
                typename proxy4_factory<1,2,3,3>::type yzww;
                typename proxy4_factory<1,3,0,0>::type ywxx;
                typename proxy4_factory<1,3,0,1>::type ywxy;
                typename proxy4_factory<1,3,0,2>::type ywxz;
                typename proxy4_factory<1,3,0,3>::type ywxw;
                typename proxy4_factory<1,3,1,0>::type ywyx;
                typename proxy4_factory<1,3,1,1>::type ywyy;
                typename proxy4_factory<1,3,1,2>::type ywyz;
                typename proxy4_factory<1,3,1,3>::type ywyw;
                typename proxy4_factory<1,3,2,0>::type ywzx;
                typename proxy4_factory<1,3,2,1>::type ywzy;
                typename proxy4_factory<1,3,2,2>::type ywzz;
                typename proxy4_factory<1,3,2,3>::type ywzw;
                typename proxy4_factory<1,3,3,0>::type ywwx;
                typename proxy4_factory<1,3,3,1>::type ywwy;
                typename proxy4_factory<1,3,3,2>::type ywwz;
                typename proxy4_factory<1,3,3,3>::type ywww;
                typename proxy4_factory<2,0,0,0>::type zxxx;
                typename proxy4_factory<2,0,0,1>::type zxxy;
                typename proxy4_factory<2,0,0,2>::type zxxz;
                typename proxy4_factory<2,0,0,3>::type zxxw;
                typename proxy4_factory<2,0,1,0>::type zxyx;
                typename proxy4_factory<2,0,1,1>::type zxyy;
                typename proxy4_factory<2,0,1,2>::type zxyz;
                typename proxy4_factory<2,0,1,3>::type zxyw;
                typename proxy4_factory<2,0,2,0>::type zxzx;
                typename proxy4_factory<2,0,2,1>::type zxzy;
                typename proxy4_factory<2,0,2,2>::type zxzz;
                typename proxy4_factory<2,0,2,3>::type zxzw;
                typename proxy4_factory<2,0,3,0>::type zxwx;
                typename proxy4_factory<2,0,3,1>::type zxwy;
                typename proxy4_factory<2,0,3,2>::type zxwz;
                typename proxy4_factory<2,0,3,3>::type zxww;
                typename proxy4_factory<2,1,0,0>::type zyxx;
                typename proxy4_factory<2,1,0,1>::type zyxy;
                typename proxy4_factory<2,1,0,2>::type zyxz;
                typename proxy4_factory<2,1,0,3>::type zyxw;
                typename proxy4_factory<2,1,1,0>::type zyyx;
                typename proxy4_factory<2,1,1,1>::type zyyy;
                typename proxy4_factory<2,1,1,2>::type zyyz;
                typename proxy4_factory<2,1,1,3>::type zyyw;
                typename proxy4_factory<2,1,2,0>::type zyzx;
                typename proxy4_factory<2,1,2,1>::type zyzy;
                typename proxy4_factory<2,1,2,2>::type zyzz;
                typename proxy4_factory<2,1,2,3>::type zyzw;
                typename proxy4_factory<2,1,3,0>::type zywx;
                typename proxy4_factory<2,1,3,1>::type zywy;
                typename proxy4_factory<2,1,3,2>::type zywz;
                typename proxy4_factory<2,1,3,3>::type zyww;
                typename proxy4_factory<2,2,0,0>::type zzxx;
                typename proxy4_factory<2,2,0,1>::type zzxy;
                typename proxy4_factory<2,2,0,2>::type zzxz;
                typename proxy4_factory<2,2,0,3>::type zzxw;
                typename proxy4_factory<2,2,1,0>::type zzyx;
                typename proxy4_factory<2,2,1,1>::type zzyy;
                typename proxy4_factory<2,2,1,2>::type zzyz;
                typename proxy4_factory<2,2,1,3>::type zzyw;
                typename proxy4_factory<2,2,2,0>::type zzzx;
                typename proxy4_factory<2,2,2,1>::type zzzy;
                typename proxy4_factory<2,2,2,2>::type zzzz;
                typename proxy4_factory<2,2,2,3>::type zzzw;
                typename proxy4_factory<2,2,3,0>::type zzwx;
                typename proxy4_factory<2,2,3,1>::type zzwy;
                typename proxy4_factory<2,2,3,2>::type zzwz;
                typename proxy4_factory<2,2,3,3>::type zzww;
                typename proxy4_factory<2,3,0,0>::type zwxx;
                typename proxy4_factory<2,3,0,1>::type zwxy;
                typename proxy4_factory<2,3,0,2>::type zwxz;
                typename proxy4_factory<2,3,0,3>::type zwxw;
                typename proxy4_factory<2,3,1,0>::type zwyx;
                typename proxy4_factory<2,3,1,1>::type zwyy;
                typename proxy4_factory<2,3,1,2>::type zwyz;
                typename proxy4_factory<2,3,1,3>::type zwyw;
                typename proxy4_factory<2,3,2,0>::type zwzx;
                typename proxy4_factory<2,3,2,1>::type zwzy;
                typename proxy4_factory<2,3,2,2>::type zwzz;
                typename proxy4_factory<2,3,2,3>::type zwzw;
                typename proxy4_factory<2,3,3,0>::type zwwx;
                typename proxy4_factory<2,3,3,1>::type zwwy;
                typename proxy4_factory<2,3,3,2>::type zwwz;
                typename proxy4_factory<2,3,3,3>::type zwww;
                typename proxy4_factory<3,0,0,0>::type wxxx;
                typename proxy4_factory<3,0,0,1>::type wxxy;
                typename proxy4_factory<3,0,0,2>::type wxxz;
                typename proxy4_factory<3,0,0,3>::type wxxw;
                typename proxy4_factory<3,0,1,0>::type wxyx;
                typename proxy4_factory<3,0,1,1>::type wxyy;
                typename proxy4_factory<3,0,1,2>::type wxyz;
                typename proxy4_factory<3,0,1,3>::type wxyw;
                typename proxy4_factory<3,0,2,0>::type wxzx;
                typename proxy4_factory<3,0,2,1>::type wxzy;
                typename proxy4_factory<3,0,2,2>::type wxzz;
                typename proxy4_factory<3,0,2,3>::type wxzw;
                typename proxy4_factory<3,0,3,0>::type wxwx;
                typename proxy4_factory<3,0,3,1>::type wxwy;
                typename proxy4_factory<3,0,3,2>::type wxwz;
                typename proxy4_factory<3,0,3,3>::type wxww;
                typename proxy4_factory<3,1,0,0>::type wyxx;
                typename proxy4_factory<3,1,0,1>::type wyxy;
                typename proxy4_factory<3,1,0,2>::type wyxz;
                typename proxy4_factory<3,1,0,3>::type wyxw;
                typename proxy4_factory<3,1,1,0>::type wyyx;
                typename proxy4_factory<3,1,1,1>::type wyyy;
                typename proxy4_factory<3,1,1,2>::type wyyz;
                typename proxy4_factory<3,1,1,3>::type wyyw;
                typename proxy4_factory<3,1,2,0>::type wyzx;
                typename proxy4_factory<3,1,2,1>::type wyzy;
                typename proxy4_factory<3,1,2,2>::type wyzz;
                typename proxy4_factory<3,1,2,3>::type wyzw;
                typename proxy4_factory<3,1,3,0>::type wywx;
                typename proxy4_factory<3,1,3,1>::type wywy;
                typename proxy4_factory<3,1,3,2>::type wywz;
                typename proxy4_factory<3,1,3,3>::type wyww;
                typename proxy4_factory<3,2,0,0>::type wzxx;
                typename proxy4_factory<3,2,0,1>::type wzxy;
                typename proxy4_factory<3,2,0,2>::type wzxz;
                typename proxy4_factory<3,2,0,3>::type wzxw;
                typename proxy4_factory<3,2,1,0>::type wzyx;
                typename proxy4_factory<3,2,1,1>::type wzyy;
                typename proxy4_factory<3,2,1,2>::type wzyz;
                typename proxy4_factory<3,2,1,3>::type wzyw;
                typename proxy4_factory<3,2,2,0>::type wzzx;
                typename proxy4_factory<3,2,2,1>::type wzzy;
                typename proxy4_factory<3,2,2,2>::type wzzz;
                typename proxy4_factory<3,2,2,3>::type wzzw;
                typename proxy4_factory<3,2,3,0>::type wzwx;
                typename proxy4_factory<3,2,3,1>::type wzwy;
                typename proxy4_factory<3,2,3,2>::type wzwz;
                typename proxy4_factory<3,2,3,3>::type wzww;
                typename proxy4_factory<3,3,0,0>::type wwxx;
                typename proxy4_factory<3,3,0,1>::type wwxy;
                typename proxy4_factory<3,3,0,2>::type wwxz;
                typename proxy4_factory<3,3,0,3>::type wwxw;
                typename proxy4_factory<3,3,1,0>::type wwyx;
                typename proxy4_factory<3,3,1,1>::type wwyy;
                typename proxy4_factory<3,3,1,2>::type wwyz;
                typename proxy4_factory<3,3,1,3>::type wwyw;
                typename proxy4_factory<3,3,2,0>::type wwzx;
                typename proxy4_factory<3,3,2,1>::type wwzy;
                typename proxy4_factory<3,3,2,2>::type wwzz;
                typename proxy4_factory<3,3,2,3>::type wwzw;
                typename proxy4_factory<3,3,3,0>::type wwwx;
                typename proxy4_factory<3,3,3,1>::type wwwy;
                typename proxy4_factory<3,3,3,2>::type wwwz;
                typename proxy4_factory<3,3,3,3>::type wwww;
            };
        };

    }
}


#endif  HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_DATA
