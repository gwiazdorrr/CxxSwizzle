#ifndef HEADER_GUARD_SWIZZLE_NAIVE_MATH
#define HEADER_GUARD_SWIZZLE_NAIVE_MATH

#include <type_traits>
#define _USE_MATH_DEFINES
#include <cmath>
#include "vector_adapter.h"
static_assert(0, "wtf");
namespace swizzle
{
    namespace naive
    {
        #define SWIZZLE_NAIVE_ENABLE_IS_SAME(T, U) typename std::enable_if<std::is_same<typename vector_traits<T>::vector_type, typename vector_traits<U>::vector_type>::value, typename vector_traits<T>::vector_type>::type
        #define SWIZZLE_NAIVE_MATH_VECTOR(T) typename vector_traits<T>::vector_type
        #define SWIZZLE_NAIVE_MATH_SCALAR(T) typename vector_traits<T>::scalar_type
        #define SWIZZLE_NAIVE_MATH_NUM(T) vector_traits<T>::num_of_components

        #define genTypeVector SWIZZLE_NAIVE_MATH_VECTOR(genType)
        #define genTypeScalar SWIZZLE_NAIVE_MATH_SCALAR(genType)
        #define genTypeCommonVector(x, y) SWIZZLE_NAIVE_ENABLE_IS_SAME(x, y)


        #pragma region Angle and Trigonometry Functions

        template <class genType>
        genTypeVector sin(const genType& t)
        {
            return genTypeVector(t).transform_func( std::sin );
        }

        template <class genType>
        genTypeVector cos(const genType& t)
        {
            return genTypeVector(t).transform_func( std::cos );
        }

        template <class genType>
        genTypeVector tan(const genType& t)
        {
            return genTypeVector(t).transform_func( std::tan );
        }

        template <class genType>
        genTypeVector asin(const genType& t)
        {
            return genTypeVector(t).transform_func( std::asin );
        }

        template <class genType>
        genTypeVector acos(const genType& t)
        {
            return genTypeVector(t).transform_func( std::acos );
        }

        template <class genType>
        genTypeVector atan(const genType& t)
        {
            return genTypeVector(t).transform_func( std::atan );
        }

        template <class genType, class genType2>
        genTypeCommonVector(genType, genType2) atan(const genType& t, const genType2& u)
        {
            return atan( t / u );
        }

        template <class genType>
        genTypeVector radians(const genType& t)
        {
            typedef genTypeScalar scalar_type;
            return genTypeVector(t).transform( [](scalar_type x) -> scalar_type { return 180 * x / scalar_type(M_PI) } );
        }

        template <class genType>
        genTypeVector degrees(const genType& t)
        {
            typedef genTypeScalar scalar_type;
            return genTypeVector(t).transform( [](scalar_type x) -> scalar_type { return scalar_type(M_PI) * x / 180 } );
        }

        #pragma endregion

        #pragma region Exponential Functions

        template <class genType, class genType2>
        genTypeCommonVector(genType, genType2) pow(const genType& t, const genType2& u)
        {
            return genTypeVector(t).transform(u, [](genTypeScalar x, genTypeScalar y) -> genTypeScalar { return std::pow(x, y); } );
        }

        template <class genType>
        genTypeVector exp(const genType& t)
        {
            return genTypeVector(t).transform_func( std::exp );
        }

        template <class genType>
        genTypeVector log(const genType& t)
        {
            return genTypeVector(t).transform_func( std::log );
        }

        template <class genType>
        genTypeVector exp2(const genType& t)
        {
            return genTypeVector(t).transform( [](genTypeScalar x) -> genTypeScalar { return std::pow(2, x); } );
        }

        template <class genType>
        genTypeVector log2(const genType& t)
        {
            static const genTypeScalar c_log2 = std::log( genTypeScalar(2) );
            return genTypeVector(t).transform( [](genTypeScalar x) -> genTypeScalar { return std::log(x) / c_log2; } );
        }

        template <class genType>
        genTypeVector sqrt(const genType& t)
        {
            // TODO: this is extremely inefficient
            return genTypeVector(t).transform_func( std::sqrt );
        }

        template <class genType>
        genTypeVector inversesqrt(const genType& t)
        {
            // TODO: this is extremely inefficient
            return 1 / sqrt(t);
        }

        #pragma endregion

        #pragma region Common functions

        template <class genType >
        genTypeVector abs( const genType& x )
        {
            return genTypeVector(x).transform_func( std::abs );
        }

        template <class genType >
        genTypeVector sign( const genType& x )
        {
            return genTypeVector(x).transform( [](genTypeScalar s) -> genTypeScalar
            {
                if ( s > 0 )
                {
                    return genTypeScalar(1);
                }
                else if ( s < 0 )
                {
                    return genTypeScalar(-1);
                }
                return genTypeScalar(0);
            } );
        }

        template <class genType >
        genTypeVector floor( const genType& x )
        {
            return genTypeVector(x).transform_func( std::floor );
        }

        template <class genType >
        genTypeVector ceil( const genType& x )
        {
            return genTypeVector(x).transform_func( std::ceil );
        }

        template <class genType >
        genTypeVector fract( const genType& x )
        {
            return genTypeVector(x).transform_func( std::fract );
        }

        template <class genType >
        genTypeVector mod( const genType& x, double y )
        {
            typedef genTypeScalar scalar_type;
            return genTypeVector(x).transform( [=](genTypeScalar s) -> genTypeScalar { return std::fmod(s, scalar_type(y)); });
        }

        template <class genType, class genType2>
        genTypeCommonVector(genType, genType2) mod(const genType& t, const genType2& u)
        {
            return genTypeVector(t).transform(u, [](genTypeScalar x, genTypeScalar y) -> genTypeScalar { return std::fmod(x, y); } );
        }

        /*
        genType clamp( genType, genType, genType )
        genType clamp( genType, float, float )
        genType floor( genType )
        genType fract( genType )
        genType max( genType, genType )
        genType max( genType, float )
        genType min( genType, genType )
        genType min( genType, float )
        genType mix( genType, genType, genType )
        genType mix( genType, genType, float )
        genType mod( genType, genType )
        genType mod( genType, float )
        genType sign( genType )
        genType smoothstep( genType, genType, genType )
        genType smoothstep( float, float, genType )
        genType step( genType, genType )
        genType step( float, genType ) */

        #pragma endregion
    }
}

#endif HEADER_GUARD_SWIZZLE_NAIVE_MATH