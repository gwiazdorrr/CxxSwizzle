// CxxSwizzle
// Copyright (c) 2013-2015, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>
#pragma once


namespace swizzle
{
    namespace detail
    {
        //! Inspired by boost/operators, this class defines arithmetic operators for a vector
        //! and a scalar as friend inline functions (only accessible with ADL). The reason for that,
        //! contrary to having global template operators is there's less typing and types decaying
        //! to a vector/scalar (proxies!) can use these operators too.
        //! Note that because VC++ is unable to align SIMD types when passing by value,
        //! functions defined here use C++03 pass-by-reference style.
        template <typename TVector, typename TScalar, typename TVectorArg = const TVector&, typename TScalarArg = const TScalar&>
        struct common_binary_operators
        {
            friend TVector operator+(TVectorArg v, TScalarArg s)
            {
                return TVector(v) += s;
            }
            friend TVector operator+(TScalarArg s, TVectorArg v)
            {
                return v + s;
            }
            friend TVector operator+(TVectorArg v1, TVectorArg v2)
            {
                return TVector(v1) += v2;
            }

            friend TVector operator*(TVectorArg v, TScalarArg s)
            {
                return TVector(v) *= s;
            }
            friend TVector operator*(TScalarArg s, TVectorArg v)
            {
                return v * s;
            }
            friend TVector operator*(TVectorArg v1, TVectorArg v2)
            {
                return TVector(v1) *= v2;
            }

            friend TVector operator-(TVectorArg v, TScalarArg s)
            {
                return TVector(v) -= s;
            }
            friend TVector operator-(TScalarArg s, TVectorArg v)
            {
                return TVector(s) -= v;
            }
            friend TVector operator-(TVectorArg v1, TVectorArg v2)
            {
                return TVector(v1) -= v2;
            }

            friend TVector operator/(TVectorArg v, TScalarArg s)
            {
                return TVector(v) /= s;
            }
            friend TVector operator/(TScalarArg s, TVectorArg v)
            {
                return TVector(s) /= v;
            }
            inline friend TVector operator/(TVectorArg v1, TVectorArg v2)
            {
                return TVector(v1) /= v2;
            }
        };
    }
}