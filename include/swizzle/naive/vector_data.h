#ifndef HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_DATA
#define HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_DATA

#include "../detail/vector_binary_operators.h"
#include "../detail/writable_wrapper.h"
#include "vector_proxy.h"

namespace swizzle
{
    namespace naive
    {
        template <template <class> class TVector, class TTraits>
        struct naive_vector_data_helper
        {
            typedef typename TTraits::scalar_type scalar_type;
            typedef typename TTraits::tag_type tag_type;
            static const size_t num_of_components = TTraits::num_of_components;

            template <size_t x>
            struct proxy1_factory
            {
                typedef vector_proxy< TVector, TTraits, x> proxy_type;
                typedef typename std::conditional < proxy_type::is_writable, detail::writable_wrapper<proxy_type>, proxy_type>::type type;
            };
            template <size_t x, size_t y>
            struct proxy2_factory
            {
                typedef vector_proxy< TVector, TTraits, x, y> proxy_type;
                typedef typename std::conditional < proxy_type::is_writable, detail::writable_wrapper<proxy_type>, proxy_type>::type type;
            };
            template <size_t x, size_t y, size_t z>
            struct proxy3_factory
            {
                typedef vector_proxy< TVector, TTraits, x, y, z> proxy_type;
                typedef typename std::conditional < proxy_type::is_writable, detail::writable_wrapper<proxy_type>, proxy_type>::type type;
            };
            template <size_t x, size_t y, size_t z, size_t w>
            struct proxy4_factory
            {
                typedef vector_proxy< TVector, TTraits, x, y, z, w> proxy_type;
                typedef typename std::conditional < proxy_type::is_writable, detail::writable_wrapper<proxy_type>, proxy_type>::type type;
            };
        };

        template <template <class> class TVector, class TTraits, size_t NumOfComponents>
        class naive_vector_data;

        template <template <class> class TVector, class TTraits>
        class naive_vector_data< TVector, TTraits, 1 >
        {
            typedef naive_vector_data_helper<TVector, TTraits> helper_type;

        public:
            typedef typename helper_type::scalar_type scalar_type;
            static const size_t num_of_components = helper_type::num_of_components;

            union
            {
                scalar_type _components[num_of_components];

                struct
                {
                    scalar_type x;
                };

                typename helper_type::template proxy2_factory<0,0>::type xx;
                typename helper_type::template proxy3_factory<0,0,0>::type xxx;
                typename helper_type::template proxy4_factory<0,0,0,0>::type xxxx;
            };
        };

        template <template <class> class TVector, class TTraits>
        class naive_vector_data< TVector, TTraits, 2 >
        {
            typedef naive_vector_data_helper<TVector, TTraits> helper_type;

        public:
            typedef typename helper_type::scalar_type scalar_type;
            static const size_t num_of_components = helper_type::num_of_components;

            union
            {
                scalar_type _components[num_of_components];

                struct
                {
                    scalar_type x;
                    scalar_type y;
                };

                typename helper_type::template proxy2_factory<0,0>::type xx;
                typename helper_type::template proxy2_factory<0,1>::type xy;
                typename helper_type::template proxy2_factory<1,0>::type yx;
                typename helper_type::template proxy2_factory<1,1>::type yy;
                typename helper_type::template proxy3_factory<0,0,0>::type xxx;
                typename helper_type::template proxy3_factory<0,0,1>::type xxy;
                typename helper_type::template proxy3_factory<0,1,0>::type xyx;
                typename helper_type::template proxy3_factory<0,1,1>::type xyy;
                typename helper_type::template proxy3_factory<1,0,0>::type yxx;
                typename helper_type::template proxy3_factory<1,0,1>::type yxy;
                typename helper_type::template proxy3_factory<1,1,0>::type yyx;
                typename helper_type::template proxy3_factory<1,1,1>::type yyy;
                typename helper_type::template proxy4_factory<0,0,0,0>::type xxxx;
                typename helper_type::template proxy4_factory<0,0,0,1>::type xxxy;
                typename helper_type::template proxy4_factory<0,0,1,0>::type xxyx;
                typename helper_type::template proxy4_factory<0,0,1,1>::type xxyy;
                typename helper_type::template proxy4_factory<0,1,0,0>::type xyxx;
                typename helper_type::template proxy4_factory<0,1,0,1>::type xyxy;
                typename helper_type::template proxy4_factory<0,1,1,0>::type xyyx;
                typename helper_type::template proxy4_factory<0,1,1,1>::type xyyy;
                typename helper_type::template proxy4_factory<1,0,0,0>::type yxxx;
                typename helper_type::template proxy4_factory<1,0,0,1>::type yxxy;
                typename helper_type::template proxy4_factory<1,0,1,0>::type yxyx;
                typename helper_type::template proxy4_factory<1,0,1,1>::type yxyy;
                typename helper_type::template proxy4_factory<1,1,0,0>::type yyxx;
                typename helper_type::template proxy4_factory<1,1,0,1>::type yyxy;
                typename helper_type::template proxy4_factory<1,1,1,0>::type yyyx;
                typename helper_type::template proxy4_factory<1,1,1,1>::type yyyy;
            };
        };

        template <template <class> class TVector, class TTraits>
        class naive_vector_data< TVector, TTraits, 3 >
        {
            typedef naive_vector_data_helper<TVector, TTraits> helper_type;

        public:
            typedef typename helper_type::scalar_type scalar_type;
            static const size_t num_of_components = helper_type::num_of_components;

            union
            {
                scalar_type _components[num_of_components];

                struct
                {
                    scalar_type x;
                    scalar_type y;
                    scalar_type z;
                };

                typename helper_type::template proxy2_factory<0,0>::type xx;
                typename helper_type::template proxy2_factory<0,1>::type xy;
                typename helper_type::template proxy2_factory<0,2>::type xz;
                typename helper_type::template proxy2_factory<1,0>::type yx;
                typename helper_type::template proxy2_factory<1,1>::type yy;
                typename helper_type::template proxy2_factory<1,2>::type yz;
                typename helper_type::template proxy2_factory<2,0>::type zx;
                typename helper_type::template proxy2_factory<2,1>::type zy;
                typename helper_type::template proxy2_factory<2,2>::type zz;
                typename helper_type::template proxy3_factory<0,0,0>::type xxx;
                typename helper_type::template proxy3_factory<0,0,1>::type xxy;
                typename helper_type::template proxy3_factory<0,0,2>::type xxz;
                typename helper_type::template proxy3_factory<0,1,0>::type xyx;
                typename helper_type::template proxy3_factory<0,1,1>::type xyy;
                typename helper_type::template proxy3_factory<0,1,2>::type xyz;
                typename helper_type::template proxy3_factory<0,2,0>::type xzx;
                typename helper_type::template proxy3_factory<0,2,1>::type xzy;
                typename helper_type::template proxy3_factory<0,2,2>::type xzz;
                typename helper_type::template proxy3_factory<1,0,0>::type yxx;
                typename helper_type::template proxy3_factory<1,0,1>::type yxy;
                typename helper_type::template proxy3_factory<1,0,2>::type yxz;
                typename helper_type::template proxy3_factory<1,1,0>::type yyx;
                typename helper_type::template proxy3_factory<1,1,1>::type yyy;
                typename helper_type::template proxy3_factory<1,1,2>::type yyz;
                typename helper_type::template proxy3_factory<1,2,0>::type yzx;
                typename helper_type::template proxy3_factory<1,2,1>::type yzy;
                typename helper_type::template proxy3_factory<1,2,2>::type yzz;
                typename helper_type::template proxy3_factory<2,0,0>::type zxx;
                typename helper_type::template proxy3_factory<2,0,1>::type zxy;
                typename helper_type::template proxy3_factory<2,0,2>::type zxz;
                typename helper_type::template proxy3_factory<2,1,0>::type zyx;
                typename helper_type::template proxy3_factory<2,1,1>::type zyy;
                typename helper_type::template proxy3_factory<2,1,2>::type zyz;
                typename helper_type::template proxy3_factory<2,2,0>::type zzx;
                typename helper_type::template proxy3_factory<2,2,1>::type zzy;
                typename helper_type::template proxy3_factory<2,2,2>::type zzz;
                typename helper_type::template proxy4_factory<0,0,0,0>::type xxxx;
                typename helper_type::template proxy4_factory<0,0,0,1>::type xxxy;
                typename helper_type::template proxy4_factory<0,0,0,2>::type xxxz;
                typename helper_type::template proxy4_factory<0,0,1,0>::type xxyx;
                typename helper_type::template proxy4_factory<0,0,1,1>::type xxyy;
                typename helper_type::template proxy4_factory<0,0,1,2>::type xxyz;
                typename helper_type::template proxy4_factory<0,0,2,0>::type xxzx;
                typename helper_type::template proxy4_factory<0,0,2,1>::type xxzy;
                typename helper_type::template proxy4_factory<0,0,2,2>::type xxzz;
                typename helper_type::template proxy4_factory<0,1,0,0>::type xyxx;
                typename helper_type::template proxy4_factory<0,1,0,1>::type xyxy;
                typename helper_type::template proxy4_factory<0,1,0,2>::type xyxz;
                typename helper_type::template proxy4_factory<0,1,1,0>::type xyyx;
                typename helper_type::template proxy4_factory<0,1,1,1>::type xyyy;
                typename helper_type::template proxy4_factory<0,1,1,2>::type xyyz;
                typename helper_type::template proxy4_factory<0,1,2,0>::type xyzx;
                typename helper_type::template proxy4_factory<0,1,2,1>::type xyzy;
                typename helper_type::template proxy4_factory<0,1,2,2>::type xyzz;
                typename helper_type::template proxy4_factory<0,2,0,0>::type xzxx;
                typename helper_type::template proxy4_factory<0,2,0,1>::type xzxy;
                typename helper_type::template proxy4_factory<0,2,0,2>::type xzxz;
                typename helper_type::template proxy4_factory<0,2,1,0>::type xzyx;
                typename helper_type::template proxy4_factory<0,2,1,1>::type xzyy;
                typename helper_type::template proxy4_factory<0,2,1,2>::type xzyz;
                typename helper_type::template proxy4_factory<0,2,2,0>::type xzzx;
                typename helper_type::template proxy4_factory<0,2,2,1>::type xzzy;
                typename helper_type::template proxy4_factory<0,2,2,2>::type xzzz;
                typename helper_type::template proxy4_factory<1,0,0,0>::type yxxx;
                typename helper_type::template proxy4_factory<1,0,0,1>::type yxxy;
                typename helper_type::template proxy4_factory<1,0,0,2>::type yxxz;
                typename helper_type::template proxy4_factory<1,0,1,0>::type yxyx;
                typename helper_type::template proxy4_factory<1,0,1,1>::type yxyy;
                typename helper_type::template proxy4_factory<1,0,1,2>::type yxyz;
                typename helper_type::template proxy4_factory<1,0,2,0>::type yxzx;
                typename helper_type::template proxy4_factory<1,0,2,1>::type yxzy;
                typename helper_type::template proxy4_factory<1,0,2,2>::type yxzz;
                typename helper_type::template proxy4_factory<1,1,0,0>::type yyxx;
                typename helper_type::template proxy4_factory<1,1,0,1>::type yyxy;
                typename helper_type::template proxy4_factory<1,1,0,2>::type yyxz;
                typename helper_type::template proxy4_factory<1,1,1,0>::type yyyx;
                typename helper_type::template proxy4_factory<1,1,1,1>::type yyyy;
                typename helper_type::template proxy4_factory<1,1,1,2>::type yyyz;
                typename helper_type::template proxy4_factory<1,1,2,0>::type yyzx;
                typename helper_type::template proxy4_factory<1,1,2,1>::type yyzy;
                typename helper_type::template proxy4_factory<1,1,2,2>::type yyzz;
                typename helper_type::template proxy4_factory<1,2,0,0>::type yzxx;
                typename helper_type::template proxy4_factory<1,2,0,1>::type yzxy;
                typename helper_type::template proxy4_factory<1,2,0,2>::type yzxz;
                typename helper_type::template proxy4_factory<1,2,1,0>::type yzyx;
                typename helper_type::template proxy4_factory<1,2,1,1>::type yzyy;
                typename helper_type::template proxy4_factory<1,2,1,2>::type yzyz;
                typename helper_type::template proxy4_factory<1,2,2,0>::type yzzx;
                typename helper_type::template proxy4_factory<1,2,2,1>::type yzzy;
                typename helper_type::template proxy4_factory<1,2,2,2>::type yzzz;
                typename helper_type::template proxy4_factory<2,0,0,0>::type zxxx;
                typename helper_type::template proxy4_factory<2,0,0,1>::type zxxy;
                typename helper_type::template proxy4_factory<2,0,0,2>::type zxxz;
                typename helper_type::template proxy4_factory<2,0,1,0>::type zxyx;
                typename helper_type::template proxy4_factory<2,0,1,1>::type zxyy;
                typename helper_type::template proxy4_factory<2,0,1,2>::type zxyz;
                typename helper_type::template proxy4_factory<2,0,2,0>::type zxzx;
                typename helper_type::template proxy4_factory<2,0,2,1>::type zxzy;
                typename helper_type::template proxy4_factory<2,0,2,2>::type zxzz;
                typename helper_type::template proxy4_factory<2,1,0,0>::type zyxx;
                typename helper_type::template proxy4_factory<2,1,0,1>::type zyxy;
                typename helper_type::template proxy4_factory<2,1,0,2>::type zyxz;
                typename helper_type::template proxy4_factory<2,1,1,0>::type zyyx;
                typename helper_type::template proxy4_factory<2,1,1,1>::type zyyy;
                typename helper_type::template proxy4_factory<2,1,1,2>::type zyyz;
                typename helper_type::template proxy4_factory<2,1,2,0>::type zyzx;
                typename helper_type::template proxy4_factory<2,1,2,1>::type zyzy;
                typename helper_type::template proxy4_factory<2,1,2,2>::type zyzz;
                typename helper_type::template proxy4_factory<2,2,0,0>::type zzxx;
                typename helper_type::template proxy4_factory<2,2,0,1>::type zzxy;
                typename helper_type::template proxy4_factory<2,2,0,2>::type zzxz;
                typename helper_type::template proxy4_factory<2,2,1,0>::type zzyx;
                typename helper_type::template proxy4_factory<2,2,1,1>::type zzyy;
                typename helper_type::template proxy4_factory<2,2,1,2>::type zzyz;
                typename helper_type::template proxy4_factory<2,2,2,0>::type zzzx;
                typename helper_type::template proxy4_factory<2,2,2,1>::type zzzy;
                typename helper_type::template proxy4_factory<2,2,2,2>::type zzzz;
            };
        };

        template <template <class> class TVector, class TTraits>
        class naive_vector_data< TVector, TTraits, 4 >
        {
            typedef naive_vector_data_helper<TVector, TTraits> helper_type;

        public:
            typedef typename helper_type::scalar_type scalar_type;
            static const size_t num_of_components = helper_type::num_of_components;

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


//                 typename helper_type::template proxy1_factory<0>::type x;
//                 typename helper_type::template proxy1_factory<1>::type y;
//                 typename helper_type::template proxy1_factory<2>::type z;
//                 typename helper_type::template proxy1_factory<3>::type w;

                typename helper_type::template proxy2_factory<0,0>::type xx;
                typename helper_type::template proxy2_factory<0,1>::type xy;
                typename helper_type::template proxy2_factory<0,2>::type xz;
                typename helper_type::template proxy2_factory<0,3>::type xw;
                typename helper_type::template proxy2_factory<1,0>::type yx;
                typename helper_type::template proxy2_factory<1,1>::type yy;
                typename helper_type::template proxy2_factory<1,2>::type yz;
                typename helper_type::template proxy2_factory<1,3>::type yw;
                typename helper_type::template proxy2_factory<2,0>::type zx;
                typename helper_type::template proxy2_factory<2,1>::type zy;
                typename helper_type::template proxy2_factory<2,2>::type zz;
                typename helper_type::template proxy2_factory<2,3>::type zw;
                typename helper_type::template proxy2_factory<3,0>::type wx;
                typename helper_type::template proxy2_factory<3,1>::type wy;
                typename helper_type::template proxy2_factory<3,2>::type wz;
                typename helper_type::template proxy2_factory<3,3>::type ww;
                typename helper_type::template proxy3_factory<0,0,0>::type xxx;
                typename helper_type::template proxy3_factory<0,0,1>::type xxy;
                typename helper_type::template proxy3_factory<0,0,2>::type xxz;
                typename helper_type::template proxy3_factory<0,0,3>::type xxw;
                typename helper_type::template proxy3_factory<0,1,0>::type xyx;
                typename helper_type::template proxy3_factory<0,1,1>::type xyy;
                typename helper_type::template proxy3_factory<0,1,2>::type xyz;
                typename helper_type::template proxy3_factory<0,1,3>::type xyw;
                typename helper_type::template proxy3_factory<0,2,0>::type xzx;
                typename helper_type::template proxy3_factory<0,2,1>::type xzy;
                typename helper_type::template proxy3_factory<0,2,2>::type xzz;
                typename helper_type::template proxy3_factory<0,2,3>::type xzw;
                typename helper_type::template proxy3_factory<0,3,0>::type xwx;
                typename helper_type::template proxy3_factory<0,3,1>::type xwy;
                typename helper_type::template proxy3_factory<0,3,2>::type xwz;
                typename helper_type::template proxy3_factory<0,3,3>::type xww;
                typename helper_type::template proxy3_factory<1,0,0>::type yxx;
                typename helper_type::template proxy3_factory<1,0,1>::type yxy;
                typename helper_type::template proxy3_factory<1,0,2>::type yxz;
                typename helper_type::template proxy3_factory<1,0,3>::type yxw;
                typename helper_type::template proxy3_factory<1,1,0>::type yyx;
                typename helper_type::template proxy3_factory<1,1,1>::type yyy;
                typename helper_type::template proxy3_factory<1,1,2>::type yyz;
                typename helper_type::template proxy3_factory<1,1,3>::type yyw;
                typename helper_type::template proxy3_factory<1,2,0>::type yzx;
                typename helper_type::template proxy3_factory<1,2,1>::type yzy;
                typename helper_type::template proxy3_factory<1,2,2>::type yzz;
                typename helper_type::template proxy3_factory<1,2,3>::type yzw;
                typename helper_type::template proxy3_factory<1,3,0>::type ywx;
                typename helper_type::template proxy3_factory<1,3,1>::type ywy;
                typename helper_type::template proxy3_factory<1,3,2>::type ywz;
                typename helper_type::template proxy3_factory<1,3,3>::type yww;
                typename helper_type::template proxy3_factory<2,0,0>::type zxx;
                typename helper_type::template proxy3_factory<2,0,1>::type zxy;
                typename helper_type::template proxy3_factory<2,0,2>::type zxz;
                typename helper_type::template proxy3_factory<2,0,3>::type zxw;
                typename helper_type::template proxy3_factory<2,1,0>::type zyx;
                typename helper_type::template proxy3_factory<2,1,1>::type zyy;
                typename helper_type::template proxy3_factory<2,1,2>::type zyz;
                typename helper_type::template proxy3_factory<2,1,3>::type zyw;
                typename helper_type::template proxy3_factory<2,2,0>::type zzx;
                typename helper_type::template proxy3_factory<2,2,1>::type zzy;
                typename helper_type::template proxy3_factory<2,2,2>::type zzz;
                typename helper_type::template proxy3_factory<2,2,3>::type zzw;
                typename helper_type::template proxy3_factory<2,3,0>::type zwx;
                typename helper_type::template proxy3_factory<2,3,1>::type zwy;
                typename helper_type::template proxy3_factory<2,3,2>::type zwz;
                typename helper_type::template proxy3_factory<2,3,3>::type zww;
                typename helper_type::template proxy3_factory<3,0,0>::type wxx;
                typename helper_type::template proxy3_factory<3,0,1>::type wxy;
                typename helper_type::template proxy3_factory<3,0,2>::type wxz;
                typename helper_type::template proxy3_factory<3,0,3>::type wxw;
                typename helper_type::template proxy3_factory<3,1,0>::type wyx;
                typename helper_type::template proxy3_factory<3,1,1>::type wyy;
                typename helper_type::template proxy3_factory<3,1,2>::type wyz;
                typename helper_type::template proxy3_factory<3,1,3>::type wyw;
                typename helper_type::template proxy3_factory<3,2,0>::type wzx;
                typename helper_type::template proxy3_factory<3,2,1>::type wzy;
                typename helper_type::template proxy3_factory<3,2,2>::type wzz;
                typename helper_type::template proxy3_factory<3,2,3>::type wzw;
                typename helper_type::template proxy3_factory<3,3,0>::type wwx;
                typename helper_type::template proxy3_factory<3,3,1>::type wwy;
                typename helper_type::template proxy3_factory<3,3,2>::type wwz;
                typename helper_type::template proxy3_factory<3,3,3>::type www;
                typename helper_type::template proxy4_factory<0,0,0,0>::type xxxx;
                typename helper_type::template proxy4_factory<0,0,0,1>::type xxxy;
                typename helper_type::template proxy4_factory<0,0,0,2>::type xxxz;
                typename helper_type::template proxy4_factory<0,0,0,3>::type xxxw;
                typename helper_type::template proxy4_factory<0,0,1,0>::type xxyx;
                typename helper_type::template proxy4_factory<0,0,1,1>::type xxyy;
                typename helper_type::template proxy4_factory<0,0,1,2>::type xxyz;
                typename helper_type::template proxy4_factory<0,0,1,3>::type xxyw;
                typename helper_type::template proxy4_factory<0,0,2,0>::type xxzx;
                typename helper_type::template proxy4_factory<0,0,2,1>::type xxzy;
                typename helper_type::template proxy4_factory<0,0,2,2>::type xxzz;
                typename helper_type::template proxy4_factory<0,0,2,3>::type xxzw;
                typename helper_type::template proxy4_factory<0,0,3,0>::type xxwx;
                typename helper_type::template proxy4_factory<0,0,3,1>::type xxwy;
                typename helper_type::template proxy4_factory<0,0,3,2>::type xxwz;
                typename helper_type::template proxy4_factory<0,0,3,3>::type xxww;
                typename helper_type::template proxy4_factory<0,1,0,0>::type xyxx;
                typename helper_type::template proxy4_factory<0,1,0,1>::type xyxy;
                typename helper_type::template proxy4_factory<0,1,0,2>::type xyxz;
                typename helper_type::template proxy4_factory<0,1,0,3>::type xyxw;
                typename helper_type::template proxy4_factory<0,1,1,0>::type xyyx;
                typename helper_type::template proxy4_factory<0,1,1,1>::type xyyy;
                typename helper_type::template proxy4_factory<0,1,1,2>::type xyyz;
                typename helper_type::template proxy4_factory<0,1,1,3>::type xyyw;
                typename helper_type::template proxy4_factory<0,1,2,0>::type xyzx;
                typename helper_type::template proxy4_factory<0,1,2,1>::type xyzy;
                typename helper_type::template proxy4_factory<0,1,2,2>::type xyzz;
                typename helper_type::template proxy4_factory<0,1,2,3>::type xyzw;
                typename helper_type::template proxy4_factory<0,1,3,0>::type xywx;
                typename helper_type::template proxy4_factory<0,1,3,1>::type xywy;
                typename helper_type::template proxy4_factory<0,1,3,2>::type xywz;
                typename helper_type::template proxy4_factory<0,1,3,3>::type xyww;
                typename helper_type::template proxy4_factory<0,2,0,0>::type xzxx;
                typename helper_type::template proxy4_factory<0,2,0,1>::type xzxy;
                typename helper_type::template proxy4_factory<0,2,0,2>::type xzxz;
                typename helper_type::template proxy4_factory<0,2,0,3>::type xzxw;
                typename helper_type::template proxy4_factory<0,2,1,0>::type xzyx;
                typename helper_type::template proxy4_factory<0,2,1,1>::type xzyy;
                typename helper_type::template proxy4_factory<0,2,1,2>::type xzyz;
                typename helper_type::template proxy4_factory<0,2,1,3>::type xzyw;
                typename helper_type::template proxy4_factory<0,2,2,0>::type xzzx;
                typename helper_type::template proxy4_factory<0,2,2,1>::type xzzy;
                typename helper_type::template proxy4_factory<0,2,2,2>::type xzzz;
                typename helper_type::template proxy4_factory<0,2,2,3>::type xzzw;
                typename helper_type::template proxy4_factory<0,2,3,0>::type xzwx;
                typename helper_type::template proxy4_factory<0,2,3,1>::type xzwy;
                typename helper_type::template proxy4_factory<0,2,3,2>::type xzwz;
                typename helper_type::template proxy4_factory<0,2,3,3>::type xzww;
                typename helper_type::template proxy4_factory<0,3,0,0>::type xwxx;
                typename helper_type::template proxy4_factory<0,3,0,1>::type xwxy;
                typename helper_type::template proxy4_factory<0,3,0,2>::type xwxz;
                typename helper_type::template proxy4_factory<0,3,0,3>::type xwxw;
                typename helper_type::template proxy4_factory<0,3,1,0>::type xwyx;
                typename helper_type::template proxy4_factory<0,3,1,1>::type xwyy;
                typename helper_type::template proxy4_factory<0,3,1,2>::type xwyz;
                typename helper_type::template proxy4_factory<0,3,1,3>::type xwyw;
                typename helper_type::template proxy4_factory<0,3,2,0>::type xwzx;
                typename helper_type::template proxy4_factory<0,3,2,1>::type xwzy;
                typename helper_type::template proxy4_factory<0,3,2,2>::type xwzz;
                typename helper_type::template proxy4_factory<0,3,2,3>::type xwzw;
                typename helper_type::template proxy4_factory<0,3,3,0>::type xwwx;
                typename helper_type::template proxy4_factory<0,3,3,1>::type xwwy;
                typename helper_type::template proxy4_factory<0,3,3,2>::type xwwz;
                typename helper_type::template proxy4_factory<0,3,3,3>::type xwww;
                typename helper_type::template proxy4_factory<1,0,0,0>::type yxxx;
                typename helper_type::template proxy4_factory<1,0,0,1>::type yxxy;
                typename helper_type::template proxy4_factory<1,0,0,2>::type yxxz;
                typename helper_type::template proxy4_factory<1,0,0,3>::type yxxw;
                typename helper_type::template proxy4_factory<1,0,1,0>::type yxyx;
                typename helper_type::template proxy4_factory<1,0,1,1>::type yxyy;
                typename helper_type::template proxy4_factory<1,0,1,2>::type yxyz;
                typename helper_type::template proxy4_factory<1,0,1,3>::type yxyw;
                typename helper_type::template proxy4_factory<1,0,2,0>::type yxzx;
                typename helper_type::template proxy4_factory<1,0,2,1>::type yxzy;
                typename helper_type::template proxy4_factory<1,0,2,2>::type yxzz;
                typename helper_type::template proxy4_factory<1,0,2,3>::type yxzw;
                typename helper_type::template proxy4_factory<1,0,3,0>::type yxwx;
                typename helper_type::template proxy4_factory<1,0,3,1>::type yxwy;
                typename helper_type::template proxy4_factory<1,0,3,2>::type yxwz;
                typename helper_type::template proxy4_factory<1,0,3,3>::type yxww;
                typename helper_type::template proxy4_factory<1,1,0,0>::type yyxx;
                typename helper_type::template proxy4_factory<1,1,0,1>::type yyxy;
                typename helper_type::template proxy4_factory<1,1,0,2>::type yyxz;
                typename helper_type::template proxy4_factory<1,1,0,3>::type yyxw;
                typename helper_type::template proxy4_factory<1,1,1,0>::type yyyx;
                typename helper_type::template proxy4_factory<1,1,1,1>::type yyyy;
                typename helper_type::template proxy4_factory<1,1,1,2>::type yyyz;
                typename helper_type::template proxy4_factory<1,1,1,3>::type yyyw;
                typename helper_type::template proxy4_factory<1,1,2,0>::type yyzx;
                typename helper_type::template proxy4_factory<1,1,2,1>::type yyzy;
                typename helper_type::template proxy4_factory<1,1,2,2>::type yyzz;
                typename helper_type::template proxy4_factory<1,1,2,3>::type yyzw;
                typename helper_type::template proxy4_factory<1,1,3,0>::type yywx;
                typename helper_type::template proxy4_factory<1,1,3,1>::type yywy;
                typename helper_type::template proxy4_factory<1,1,3,2>::type yywz;
                typename helper_type::template proxy4_factory<1,1,3,3>::type yyww;
                typename helper_type::template proxy4_factory<1,2,0,0>::type yzxx;
                typename helper_type::template proxy4_factory<1,2,0,1>::type yzxy;
                typename helper_type::template proxy4_factory<1,2,0,2>::type yzxz;
                typename helper_type::template proxy4_factory<1,2,0,3>::type yzxw;
                typename helper_type::template proxy4_factory<1,2,1,0>::type yzyx;
                typename helper_type::template proxy4_factory<1,2,1,1>::type yzyy;
                typename helper_type::template proxy4_factory<1,2,1,2>::type yzyz;
                typename helper_type::template proxy4_factory<1,2,1,3>::type yzyw;
                typename helper_type::template proxy4_factory<1,2,2,0>::type yzzx;
                typename helper_type::template proxy4_factory<1,2,2,1>::type yzzy;
                typename helper_type::template proxy4_factory<1,2,2,2>::type yzzz;
                typename helper_type::template proxy4_factory<1,2,2,3>::type yzzw;
                typename helper_type::template proxy4_factory<1,2,3,0>::type yzwx;
                typename helper_type::template proxy4_factory<1,2,3,1>::type yzwy;
                typename helper_type::template proxy4_factory<1,2,3,2>::type yzwz;
                typename helper_type::template proxy4_factory<1,2,3,3>::type yzww;
                typename helper_type::template proxy4_factory<1,3,0,0>::type ywxx;
                typename helper_type::template proxy4_factory<1,3,0,1>::type ywxy;
                typename helper_type::template proxy4_factory<1,3,0,2>::type ywxz;
                typename helper_type::template proxy4_factory<1,3,0,3>::type ywxw;
                typename helper_type::template proxy4_factory<1,3,1,0>::type ywyx;
                typename helper_type::template proxy4_factory<1,3,1,1>::type ywyy;
                typename helper_type::template proxy4_factory<1,3,1,2>::type ywyz;
                typename helper_type::template proxy4_factory<1,3,1,3>::type ywyw;
                typename helper_type::template proxy4_factory<1,3,2,0>::type ywzx;
                typename helper_type::template proxy4_factory<1,3,2,1>::type ywzy;
                typename helper_type::template proxy4_factory<1,3,2,2>::type ywzz;
                typename helper_type::template proxy4_factory<1,3,2,3>::type ywzw;
                typename helper_type::template proxy4_factory<1,3,3,0>::type ywwx;
                typename helper_type::template proxy4_factory<1,3,3,1>::type ywwy;
                typename helper_type::template proxy4_factory<1,3,3,2>::type ywwz;
                typename helper_type::template proxy4_factory<1,3,3,3>::type ywww;
                typename helper_type::template proxy4_factory<2,0,0,0>::type zxxx;
                typename helper_type::template proxy4_factory<2,0,0,1>::type zxxy;
                typename helper_type::template proxy4_factory<2,0,0,2>::type zxxz;
                typename helper_type::template proxy4_factory<2,0,0,3>::type zxxw;
                typename helper_type::template proxy4_factory<2,0,1,0>::type zxyx;
                typename helper_type::template proxy4_factory<2,0,1,1>::type zxyy;
                typename helper_type::template proxy4_factory<2,0,1,2>::type zxyz;
                typename helper_type::template proxy4_factory<2,0,1,3>::type zxyw;
                typename helper_type::template proxy4_factory<2,0,2,0>::type zxzx;
                typename helper_type::template proxy4_factory<2,0,2,1>::type zxzy;
                typename helper_type::template proxy4_factory<2,0,2,2>::type zxzz;
                typename helper_type::template proxy4_factory<2,0,2,3>::type zxzw;
                typename helper_type::template proxy4_factory<2,0,3,0>::type zxwx;
                typename helper_type::template proxy4_factory<2,0,3,1>::type zxwy;
                typename helper_type::template proxy4_factory<2,0,3,2>::type zxwz;
                typename helper_type::template proxy4_factory<2,0,3,3>::type zxww;
                typename helper_type::template proxy4_factory<2,1,0,0>::type zyxx;
                typename helper_type::template proxy4_factory<2,1,0,1>::type zyxy;
                typename helper_type::template proxy4_factory<2,1,0,2>::type zyxz;
                typename helper_type::template proxy4_factory<2,1,0,3>::type zyxw;
                typename helper_type::template proxy4_factory<2,1,1,0>::type zyyx;
                typename helper_type::template proxy4_factory<2,1,1,1>::type zyyy;
                typename helper_type::template proxy4_factory<2,1,1,2>::type zyyz;
                typename helper_type::template proxy4_factory<2,1,1,3>::type zyyw;
                typename helper_type::template proxy4_factory<2,1,2,0>::type zyzx;
                typename helper_type::template proxy4_factory<2,1,2,1>::type zyzy;
                typename helper_type::template proxy4_factory<2,1,2,2>::type zyzz;
                typename helper_type::template proxy4_factory<2,1,2,3>::type zyzw;
                typename helper_type::template proxy4_factory<2,1,3,0>::type zywx;
                typename helper_type::template proxy4_factory<2,1,3,1>::type zywy;
                typename helper_type::template proxy4_factory<2,1,3,2>::type zywz;
                typename helper_type::template proxy4_factory<2,1,3,3>::type zyww;
                typename helper_type::template proxy4_factory<2,2,0,0>::type zzxx;
                typename helper_type::template proxy4_factory<2,2,0,1>::type zzxy;
                typename helper_type::template proxy4_factory<2,2,0,2>::type zzxz;
                typename helper_type::template proxy4_factory<2,2,0,3>::type zzxw;
                typename helper_type::template proxy4_factory<2,2,1,0>::type zzyx;
                typename helper_type::template proxy4_factory<2,2,1,1>::type zzyy;
                typename helper_type::template proxy4_factory<2,2,1,2>::type zzyz;
                typename helper_type::template proxy4_factory<2,2,1,3>::type zzyw;
                typename helper_type::template proxy4_factory<2,2,2,0>::type zzzx;
                typename helper_type::template proxy4_factory<2,2,2,1>::type zzzy;
                typename helper_type::template proxy4_factory<2,2,2,2>::type zzzz;
                typename helper_type::template proxy4_factory<2,2,2,3>::type zzzw;
                typename helper_type::template proxy4_factory<2,2,3,0>::type zzwx;
                typename helper_type::template proxy4_factory<2,2,3,1>::type zzwy;
                typename helper_type::template proxy4_factory<2,2,3,2>::type zzwz;
                typename helper_type::template proxy4_factory<2,2,3,3>::type zzww;
                typename helper_type::template proxy4_factory<2,3,0,0>::type zwxx;
                typename helper_type::template proxy4_factory<2,3,0,1>::type zwxy;
                typename helper_type::template proxy4_factory<2,3,0,2>::type zwxz;
                typename helper_type::template proxy4_factory<2,3,0,3>::type zwxw;
                typename helper_type::template proxy4_factory<2,3,1,0>::type zwyx;
                typename helper_type::template proxy4_factory<2,3,1,1>::type zwyy;
                typename helper_type::template proxy4_factory<2,3,1,2>::type zwyz;
                typename helper_type::template proxy4_factory<2,3,1,3>::type zwyw;
                typename helper_type::template proxy4_factory<2,3,2,0>::type zwzx;
                typename helper_type::template proxy4_factory<2,3,2,1>::type zwzy;
                typename helper_type::template proxy4_factory<2,3,2,2>::type zwzz;
                typename helper_type::template proxy4_factory<2,3,2,3>::type zwzw;
                typename helper_type::template proxy4_factory<2,3,3,0>::type zwwx;
                typename helper_type::template proxy4_factory<2,3,3,1>::type zwwy;
                typename helper_type::template proxy4_factory<2,3,3,2>::type zwwz;
                typename helper_type::template proxy4_factory<2,3,3,3>::type zwww;
                typename helper_type::template proxy4_factory<3,0,0,0>::type wxxx;
                typename helper_type::template proxy4_factory<3,0,0,1>::type wxxy;
                typename helper_type::template proxy4_factory<3,0,0,2>::type wxxz;
                typename helper_type::template proxy4_factory<3,0,0,3>::type wxxw;
                typename helper_type::template proxy4_factory<3,0,1,0>::type wxyx;
                typename helper_type::template proxy4_factory<3,0,1,1>::type wxyy;
                typename helper_type::template proxy4_factory<3,0,1,2>::type wxyz;
                typename helper_type::template proxy4_factory<3,0,1,3>::type wxyw;
                typename helper_type::template proxy4_factory<3,0,2,0>::type wxzx;
                typename helper_type::template proxy4_factory<3,0,2,1>::type wxzy;
                typename helper_type::template proxy4_factory<3,0,2,2>::type wxzz;
                typename helper_type::template proxy4_factory<3,0,2,3>::type wxzw;
                typename helper_type::template proxy4_factory<3,0,3,0>::type wxwx;
                typename helper_type::template proxy4_factory<3,0,3,1>::type wxwy;
                typename helper_type::template proxy4_factory<3,0,3,2>::type wxwz;
                typename helper_type::template proxy4_factory<3,0,3,3>::type wxww;
                typename helper_type::template proxy4_factory<3,1,0,0>::type wyxx;
                typename helper_type::template proxy4_factory<3,1,0,1>::type wyxy;
                typename helper_type::template proxy4_factory<3,1,0,2>::type wyxz;
                typename helper_type::template proxy4_factory<3,1,0,3>::type wyxw;
                typename helper_type::template proxy4_factory<3,1,1,0>::type wyyx;
                typename helper_type::template proxy4_factory<3,1,1,1>::type wyyy;
                typename helper_type::template proxy4_factory<3,1,1,2>::type wyyz;
                typename helper_type::template proxy4_factory<3,1,1,3>::type wyyw;
                typename helper_type::template proxy4_factory<3,1,2,0>::type wyzx;
                typename helper_type::template proxy4_factory<3,1,2,1>::type wyzy;
                typename helper_type::template proxy4_factory<3,1,2,2>::type wyzz;
                typename helper_type::template proxy4_factory<3,1,2,3>::type wyzw;
                typename helper_type::template proxy4_factory<3,1,3,0>::type wywx;
                typename helper_type::template proxy4_factory<3,1,3,1>::type wywy;
                typename helper_type::template proxy4_factory<3,1,3,2>::type wywz;
                typename helper_type::template proxy4_factory<3,1,3,3>::type wyww;
                typename helper_type::template proxy4_factory<3,2,0,0>::type wzxx;
                typename helper_type::template proxy4_factory<3,2,0,1>::type wzxy;
                typename helper_type::template proxy4_factory<3,2,0,2>::type wzxz;
                typename helper_type::template proxy4_factory<3,2,0,3>::type wzxw;
                typename helper_type::template proxy4_factory<3,2,1,0>::type wzyx;
                typename helper_type::template proxy4_factory<3,2,1,1>::type wzyy;
                typename helper_type::template proxy4_factory<3,2,1,2>::type wzyz;
                typename helper_type::template proxy4_factory<3,2,1,3>::type wzyw;
                typename helper_type::template proxy4_factory<3,2,2,0>::type wzzx;
                typename helper_type::template proxy4_factory<3,2,2,1>::type wzzy;
                typename helper_type::template proxy4_factory<3,2,2,2>::type wzzz;
                typename helper_type::template proxy4_factory<3,2,2,3>::type wzzw;
                typename helper_type::template proxy4_factory<3,2,3,0>::type wzwx;
                typename helper_type::template proxy4_factory<3,2,3,1>::type wzwy;
                typename helper_type::template proxy4_factory<3,2,3,2>::type wzwz;
                typename helper_type::template proxy4_factory<3,2,3,3>::type wzww;
                typename helper_type::template proxy4_factory<3,3,0,0>::type wwxx;
                typename helper_type::template proxy4_factory<3,3,0,1>::type wwxy;
                typename helper_type::template proxy4_factory<3,3,0,2>::type wwxz;
                typename helper_type::template proxy4_factory<3,3,0,3>::type wwxw;
                typename helper_type::template proxy4_factory<3,3,1,0>::type wwyx;
                typename helper_type::template proxy4_factory<3,3,1,1>::type wwyy;
                typename helper_type::template proxy4_factory<3,3,1,2>::type wwyz;
                typename helper_type::template proxy4_factory<3,3,1,3>::type wwyw;
                typename helper_type::template proxy4_factory<3,3,2,0>::type wwzx;
                typename helper_type::template proxy4_factory<3,3,2,1>::type wwzy;
                typename helper_type::template proxy4_factory<3,3,2,2>::type wwzz;
                typename helper_type::template proxy4_factory<3,3,2,3>::type wwzw;
                typename helper_type::template proxy4_factory<3,3,3,0>::type wwwx;
                typename helper_type::template proxy4_factory<3,3,3,1>::type wwwy;
                typename helper_type::template proxy4_factory<3,3,3,2>::type wwwz;
                typename helper_type::template proxy4_factory<3,3,3,3>::type wwww;
            };
        };

    }
}


#endif // HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_DATA
