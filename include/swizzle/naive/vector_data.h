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

            template <size_t x, size_t y, size_t z = -1, size_t w = -1>
            struct proxy2_factory
            {
                typedef vector_proxy< TVector<scalar_type, 2>, scalar_type, num_of_components, x, y> proxy_type;
                typedef typename std::conditional < proxy_type::is_writable, detail::writable_wrapper<proxy_type>, proxy_type>::type type;
            };
            template <size_t x, size_t y, size_t z, size_t w = -1>
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
                    scalar_type x;
                };

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
                    scalar_type x;
                    scalar_type y;
                };

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
                    scalar_type x;
                    scalar_type y;
                    scalar_type z;
                    scalar_type w;
                };

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
                    scalar_type x;
                    scalar_type y;
                    scalar_type z;
                    scalar_type w;
                };

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

    }
}


#endif  HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_DATA
