#ifndef HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_BASE
#define HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_BASE

namespace swizzle
{
    namespace naive
    {

        /** A base class for naive vectors defining all but unary-vector operators (I am really lazy).
            It defines:
            - binary scalar operators as cloning *this and then calling unary equivalent
            - unary scalar operators as converting self to ref to a vector and adding a vector 
              instance constructed from the scalar
            - binary vector operators as cloning *this and calling unary equivalent
            - global binary scalar operators where scalar is the first operand; they basically construct 
              vector from scalar and then call proper member operator
        */
        template <class TVector, class TScalar>
        class vector_base
        {
        public:
            typedef TVector vector_type;
            typedef TScalar scalar_type;

        private:
            //! Casts itself to a vector reference.
            vector_type& as_ref()
            {
                return static_cast<vector_type&>(*this);
            }
            //! Casts itself to a vector reference.
            const vector_type& as_ref() const
            {
                return static_cast<const vector_type&>(*this);
            }

        public:
            vector_type operator+(const scalar_type& o) const
            {
                return static_cast<vector_base&>(vector_type(as_ref())) += o;
            }
            vector_type operator-(const scalar_type& o) const
            {
                return static_cast<vector_base&>(vector_type(as_ref())) -= o;
            }
            vector_type operator*(const scalar_type& o) const
            {
                return static_cast<vector_base&>(vector_type(as_ref())) *= o;
            }
            vector_type operator/(const scalar_type& o) const
            {
                return static_cast<vector_base&>(vector_type(as_ref())) /= o;
            }

            vector_type& operator+=(const scalar_type& o)
            {
                return as_ref() += vector_type(o);
            }
            vector_type& operator-=(const scalar_type& o)
            {
                return as_ref() -= vector_type(o);
            }
            vector_type& operator*=(const scalar_type& o)
            {
                return as_ref() *= vector_type(o);
            }
            vector_type& operator/=(const scalar_type& o)
            {
                return as_ref() /= vector_type(o);
            }

            vector_type operator+(vector_type o) const
            {
                return o += as_ref();
            }
            vector_type operator-(const vector_type& o) const
            {
                return vector_type(as_ref()) -= o;
            }
            vector_type operator*(vector_type o) const
            {
                return o *= as_ref();
            }
            vector_type operator/(const vector_type& o) const
            {
                return vector_type(as_ref()) /= o;
            }
        };

        template <class TVector, class TScalar>
        TVector operator+(const TScalar& v, const vector_base< TVector, TScalar >& vec)
        {
            return TVector(v) + static_cast<const TVector&>(vec);
        }

        template <class TVector, class TScalar>
        TVector operator-(const TScalar& v, const vector_base< TVector, TScalar >& vec)
        {
            return TVector(v) - static_cast<const TVector&>(vec);
        }

        template <class TVector, class TScalar>
        TVector operator*(const TScalar& v, const vector_base< TVector, TScalar >& vec)
        {
            return TVector(v) * static_cast<const TVector&>(vec);
        }

        template <class TVector, class TScalar>
        TVector operator/(const TScalar& v, const vector_base< TVector, TScalar >& vec)
        {
            return TVector(v) / static_cast<const TVector&>(vec);
        }
    }
}

#endif  HEADER_GUARD_SWIZZLE_NAIVE_VECTOR_BASE
