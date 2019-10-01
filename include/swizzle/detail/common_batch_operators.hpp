// unary

CXXSWIZZLE_FORCE_INLINE this_type& operator=(this_arg other) &
{
    assign(other);
    return *this;
}

CXXSWIZZLE_FORCE_INLINE this_type& operator=(this_arg other) &&
{
    assign_fast(other);
    return *this;
}

CXXSWIZZLE_FORCE_INLINE this_type operator-() const 
{ 
    return this_type(construct_tag{}, -at<Index>()...); 
}
CXXSWIZZLE_FORCE_INLINE this_type& operator+=(this_arg other) & {  return (assign_at<Index>(at<Index>() + other.at<Index>()), ..., *this); }
CXXSWIZZLE_FORCE_INLINE this_type& operator-=(this_arg other) & {  return (assign_at<Index>(at<Index>() - other.at<Index>()), ..., *this); }
CXXSWIZZLE_FORCE_INLINE this_type& operator*=(this_arg other) & {  return (assign_at<Index>(at<Index>() * other.at<Index>()), ..., *this); }
CXXSWIZZLE_FORCE_INLINE this_type& operator/=(this_arg other) & {  return (assign_at<Index>(at<Index>() / other.at<Index>()), ..., *this); }

CXXSWIZZLE_FORCE_INLINE this_type& operator+=(this_arg other) && { return (assign_at_fast<Index>(at<Index>() + other.at<Index>()), ..., *this); } 
CXXSWIZZLE_FORCE_INLINE this_type& operator-=(this_arg other) && { return (assign_at_fast<Index>(at<Index>() - other.at<Index>()), ..., *this); }
CXXSWIZZLE_FORCE_INLINE this_type& operator*=(this_arg other) && { return (assign_at_fast<Index>(at<Index>() * other.at<Index>()), ..., *this); }
CXXSWIZZLE_FORCE_INLINE this_type& operator/=(this_arg other) && { return (assign_at_fast<Index>(at<Index>() / other.at<Index>()), ..., *this); }


CXXSWIZZLE_FORCE_INLINE this_type& operator++()
{
    const auto one = batch_scalar_cast(primitive_type(1));
    return (assign_at<Index>(at<Index>() + one), ..., *this);
}

CXXSWIZZLE_FORCE_INLINE this_type operator++(int)
{
    this_type result(*this);
    ++(*this);
    return result;
}

CXXSWIZZLE_FORCE_INLINE this_type& operator--()
{
    const auto one = batch_scalar_cast(primitive_type(1));
    return (assign_at<Index>(at<Index>() - one), ..., *this);
}

CXXSWIZZLE_FORCE_INLINE this_type operator--(int)
{
    this_type result(*this);
    --(*this);
    return result;
}

// binary

CXXSWIZZLE_FORCE_INLINE friend this_type operator+(this_arg a, this_arg b) { return this_type(construct_tag{}, a.at<Index>() + b.at<Index>()...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator-(this_arg a, this_arg b) { return this_type(construct_tag{}, a.at<Index>() - b.at<Index>()...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator*(this_arg a, this_arg b) { return this_type(construct_tag{}, a.at<Index>() * b.at<Index>()...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator/(this_arg a, this_arg b) { return this_type(construct_tag{}, a.at<Index>() / b.at<Index>()...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator%(this_arg a, this_arg b) { return this_type(construct_tag{}, a.at<Index>() % b.at<Index>()...); }

CXXSWIZZLE_FORCE_INLINE friend this_type operator+(this_arg a, int _b) { auto b = data_type(_b); return this_type(construct_tag{}, a.at<Index>() + b...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator-(this_arg a, int _b) { auto b = data_type(_b); return this_type(construct_tag{}, a.at<Index>() - b...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator*(this_arg a, int _b) { auto b = data_type(_b); return this_type(construct_tag{}, a.at<Index>() * b...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator/(this_arg a, int _b) { auto b = data_type(_b); return this_type(construct_tag{}, a.at<Index>() / b...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator%(this_arg a, int _b) { auto b = data_type(_b); return this_type(construct_tag{}, a.at<Index>() % b...); }
                                                                                            
CXXSWIZZLE_FORCE_INLINE friend this_type operator+(int _a, this_arg b) { auto a = data_type(_a); return this_type(construct_tag{}, a + b.at<Index>()...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator-(int _a, this_arg b) { auto a = data_type(_a); return this_type(construct_tag{}, a - b.at<Index>()...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator*(int _a, this_arg b) { auto a = data_type(_a); return this_type(construct_tag{}, a * b.at<Index>()...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator/(int _a, this_arg b) { auto a = data_type(_a); return this_type(construct_tag{}, a / b.at<Index>()...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator%(int _a, this_arg b) { auto a = data_type(_a); return this_type(construct_tag{}, a % b.at<Index>()...); }

CXXSWIZZLE_FORCE_INLINE friend this_type operator+(this_arg a, double _b) { auto b = data_type(_b); return this_type(construct_tag{}, a.at<Index>() + b...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator-(this_arg a, double _b) { auto b = data_type(_b); return this_type(construct_tag{}, a.at<Index>() - b...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator*(this_arg a, double _b) { auto b = data_type(_b); return this_type(construct_tag{}, a.at<Index>() * b...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator/(this_arg a, double _b) { auto b = data_type(_b); return this_type(construct_tag{}, a.at<Index>() / b...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator%(this_arg a, double _b) { auto b = data_type(_b); return this_type(construct_tag{}, a.at<Index>() % b...); }

CXXSWIZZLE_FORCE_INLINE friend this_type operator+(double _a, this_arg b) { auto a = data_type(_a); return this_type(construct_tag{}, a + b.at<Index>()...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator-(double _a, this_arg b) { auto a = data_type(_a); return this_type(construct_tag{}, a - b.at<Index>()...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator*(double _a, this_arg b) { auto a = data_type(_a); return this_type(construct_tag{}, a * b.at<Index>()...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator/(double _a, this_arg b) { auto a = data_type(_a); return this_type(construct_tag{}, a / b.at<Index>()...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator%(double _a, this_arg b) { auto a = data_type(_a); return this_type(construct_tag{}, a % b.at<Index>()...); }


CXXSWIZZLE_FORCE_INLINE friend this_type operator&(this_arg a, this_arg b)  { return this_type(construct_tag{}, a.at<Index>() & b.at<Index>()...); }

CXXSWIZZLE_FORCE_INLINE friend this_type operator>>(this_arg a, this_arg b) { return this_type(construct_tag{}, a.at<Index>() >> b.at<Index>()...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator<<(this_arg a, this_arg b) { return this_type(construct_tag{}, a.at<Index>() << b.at<Index>()...); }

CXXSWIZZLE_FORCE_INLINE friend bool_type operator>(this_arg a, this_arg b)  { return bool_type(construct_tag{}, static_cast<bool_type::data_type>(a.at<Index>() > b.at<Index>())...); }
CXXSWIZZLE_FORCE_INLINE friend bool_type operator>=(this_arg a, this_arg b) { return bool_type(construct_tag{}, static_cast<bool_type::data_type>(a.at<Index>() >= b.at<Index>())...); }
CXXSWIZZLE_FORCE_INLINE friend bool_type operator<(this_arg a, this_arg b)  { return bool_type(construct_tag{}, static_cast<bool_type::data_type>(a.at<Index>() < b.at<Index>())...); }
CXXSWIZZLE_FORCE_INLINE friend bool_type operator<=(this_arg a, this_arg b) { return bool_type(construct_tag{}, static_cast<bool_type::data_type>(a.at<Index>() <= b.at<Index>())...); }
CXXSWIZZLE_FORCE_INLINE friend bool_type operator==(this_arg a, this_arg b) { return bool_type(construct_tag{}, static_cast<bool_type::data_type>(a.at<Index>() == b.at<Index>())...); }
CXXSWIZZLE_FORCE_INLINE friend bool_type operator!=(this_arg a, this_arg b) { return bool_type(construct_tag{}, static_cast<bool_type::data_type>(a.at<Index>() != b.at<Index>())...); }
