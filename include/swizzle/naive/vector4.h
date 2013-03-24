#ifndef HEADER_GUARD_SWIZZLE_NAIVE_VECTOR4
#define HEADER_GUARD_SWIZZLE_NAIVE_VECTOR4

#include "proxies.h"
#include "vector_base.h"

namespace swizzle
{
    namespace naive
    {
        template <class T>
        class vector4 : public vector_base< vector4<T>, T >
        {
        public:
            //! Component type
            typedef T scalar_type;
            //! Type of this vector.
            typedef vector4<T> vector_type;
            //! Number of components of this vector.
            static const size_t num_of_components = 4;

        private:
            //! A factory defining types for 2-components proxies.
            template <size_t x, size_t y>
            struct proxy2_factory : ::swizzle::naive::proxy2_factory<vector2<scalar_type>, scalar_type, num_of_components, x, y> {};
            //! A factory defining types for 3-components proxies.
            template <size_t x, size_t y, size_t z>
            struct proxy3_factory : ::swizzle::naive::proxy3_factory<vector3<scalar_type>, scalar_type, num_of_components, x, y, z> {};
            //! A factory defining types for 4-components proxies.
            template <size_t x, size_t y, size_t z, size_t w>
            struct proxy4_factory : ::swizzle::naive::proxy4_factory<vector4<scalar_type>, scalar_type, num_of_components, x, y, z, w> {};

        public:
            union
            {
                struct
                {
                    scalar_type x;
                    scalar_type y;
                    scalar_type z;
                    scalar_type w;
                };

                scalar_type _components[num_of_components];

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


        public:

            vector_type() : x( scalar_type() ), y( scalar_type() ), z( scalar_type() ), w( scalar_type() )
            {}

            explicit vector_type(const scalar_type& src) : x(src), y(src), z(src), w(src)
            {}

            vector_type(const vector_type& src) : x(src.x), y(src.y), z(src.z), w(src.w)
            {}

            vector_type(const scalar_type& x, const scalar_type& y, const scalar_type& z, const scalar_type& w) : x(x), y(y), z(z), w(w)
            {}

            // unary vector operators

            vector_type& operator=(const vector_type& o)
            {
                x = o.x;
                y = o.y;
                z = o.z;
                w = o.w;
                return *this;
            }

            vector_type& operator+=(const vector_type& o)
            {
                x += o.x;
                y += o.y;
                z += o.z;
                w += o.w;
                return *this;
            }

            vector_type& operator-=(const vector_type& o)
            {
                x -= o.x;
                y -= o.y;
                z -= o.z;
                w -= o.w;
                return *this;
            }

            vector_type& operator*=(const vector_type& o)
            {
                x *= o.x;
                y *= o.y;
                z *= o.z;
                w *= o.w;
                return *this;
            }

            vector_type& operator/=(const vector_type& o)
            {
                x /= o.x;
                y /= o.y;
                z /= o.z;
                w /= o.w;
                return *this;
            }
        };
    }
}



#endif  HEADER_GUARD_SWIZZLE_NAIVE_VECTOR4
