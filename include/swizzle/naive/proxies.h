#ifndef HEADER_GUARD_SWIZZLE_NAIVE_PROXIES
#define HEADER_GUARD_SWIZZLE_NAIVE_PROXIES

#include "../detail/read_only_wrapper.h"
#include "../detail/writable_wrapper.h"

namespace swizzle
{
    namespace naive
    {
        template <class T> class vector2;
        template <class T> class vector3;
        template <class T> class vector4;


        template <class TVector, class TScalar, size_t size, size_t x, size_t y>
        struct proxy2
        {
            typedef TVector vector_type;
            typedef TScalar scalar_type;

            operator TVector() const
            {
                return TVector(data[x], data[y]);
            }

        protected:
            TScalar data[size];
        };

        template <class TVector, class TScalar, size_t size, size_t x, size_t y>
        struct proxy2_writable : public proxy2<TVector, TScalar, size, x, y>
        {
            proxy2_writable& operator=(const TVector& vec) 
            { 
                data[x] = vec.x; 
                data[y] = vec.y; 
                return *this; 
            }
        };




        template <class TVector, class TScalar, size_t size, size_t x, size_t y, size_t z>
        struct proxy3
        {
            typedef TVector vector_type;
            typedef TScalar scalar_type;

            operator TVector() const
            {
                return TVector(data[x], data[y], data[z]);
            }

        protected:
            TScalar data[size];
        };

        template <class TVector, class TScalar, size_t size, size_t x, size_t y, size_t z>
        struct proxy3_writable : public proxy3<TVector, TScalar, size, x, y, z>
        {
            proxy3_writable& operator=(const TVector& vec) 
            { 
                data[x] = vec.x; 
                data[y] = vec.y; 
                data[z] = vec.z; 
                return *this; 
            }
        };

        template <class TVector, class TScalar, size_t size, size_t x, size_t y, size_t z, size_t w>
        struct proxy4
        {
            typedef TVector vector_type;
            typedef TScalar scalar_type;

            operator TVector() const
            {
                return TVector(data[x], data[y], data[z], data[w]);
            }

        protected:
            TScalar data[size];
        };

        template <class TVector, class TScalar, size_t size, size_t x, size_t y, size_t z, size_t w>
        struct proxy4_writable : public proxy4<TVector, TScalar, size, x, y, z, w>
        {
            proxy4_writable& operator=(const TVector& vec) 
            { 
                data[x] = vec.x; 
                data[y] = vec.y; 
                data[z] = vec.z; 
                return *this; 
            }
        };

        template <class TVector, class TScalar, size_t components, size_t x, size_t y>
        struct proxy2_factory
        {
            typedef typename std::conditional< x == y, 
                detail::read_only_wrapper< proxy2<TVector, TScalar, components, x, y> >,
                detail::writable_wrapper< proxy2_writable<TVector, TScalar, components, x, y> > >::type type;
        };

        template <class TVector, class TScalar, size_t components, size_t x, size_t y, size_t z>
        struct proxy3_factory
        {
            typedef typename std::conditional< x == y || x == z || y == z,
                detail::read_only_wrapper< proxy3<TVector, TScalar, components, x, y, z> >,
                detail::writable_wrapper< proxy3_writable<TVector, TScalar, components, x, y, z> > >::type type;
        };

        template <class TVector, class TScalar, size_t components, size_t x, size_t y, size_t z, size_t w>
        struct proxy4_factory
        {
            typedef typename std::conditional< x == y || x == z || x == w || y == z || y == w || z == w, 
                detail::read_only_wrapper< proxy4<TVector, TScalar, components, x, y, z, w> >,
                detail::writable_wrapper< proxy4_writable<TVector, TScalar, components, x, y, z, w> > >::type type;
        };
    }
}


#endif  HEADER_GUARD_SWIZZLE_NAIVE_PROXIES
