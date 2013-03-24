#ifndef HEADER_GUARD_SWIZZLE_NAIVE_VECTOR2
#define HEADER_GUARD_SWIZZLE_NAIVE_VECTOR2

#include "proxies.h"
#include "vector_base.h"

namespace swizzle
{
    namespace naive
    {
        template <class T>
        class vector2 : public vector_base< vector2<T>, T >
        {
        public:
            //! Component type
            typedef T scalar_type;
            //! Type of this vector.
            typedef vector2<T> vector_type;
            //! Number of components of this vector.
            static const size_t num_of_components = 2;

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
                };

                scalar_type _components[num_of_components];

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


        public:

            vector_type() : x( scalar_type() ), y( scalar_type() )
            {}

            explicit vector_type(const scalar_type& src) : x(src), y(src)
            {}

            vector_type(const vector_type& src) : x(src.x), y(src.y)
            {}

            vector_type(const scalar_type& x, const scalar_type& y) : x(x), y(y)
            {}

            // unary vector operators

            vector_type& operator=(const vector_type& o)
            {
                x = o.x;
                y = o.y;
                return *this;
            }

            vector_type& operator+=(const vector_type& o)
            {
                x += o.x;
                y += o.y;
                return *this;
            }

            vector_type& operator-=(const vector_type& o)
            {
                x -= o.x;
                y -= o.y;
                return *this;
            }

            vector_type& operator*=(const vector_type& o)
            {
                x *= o.x;
                y *= o.y;
                return *this;
            }

            vector_type& operator/=(const vector_type& o)
            {
                x /= o.x;
                y /= o.y;
                return *this;
            }
        };
    }
}



#endif  HEADER_GUARD_SWIZZLE_NAIVE_VECTOR2
