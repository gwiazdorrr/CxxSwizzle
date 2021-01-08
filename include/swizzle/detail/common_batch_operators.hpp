// unary

CXXSWIZZLE_FORCE_INLINE this_type& operator=(this_arg other)&
{
    this->assign(other);
    return *this;
}

CXXSWIZZLE_FORCE_INLINE this_type& operator=(this_arg other)&&
{
    this->assign_fast(other);
    return *this;
}

CXXSWIZZLE_FORCE_INLINE this_type operator-() const
{
    return this_type(construct_tag{}, -at<TIndices>()...);
}
CXXSWIZZLE_FORCE_INLINE this_type& operator+=(this_arg other)& { return (this->template assign_at<TIndices>(at<TIndices>() + other.at<TIndices>()), ..., *this); }
CXXSWIZZLE_FORCE_INLINE this_type& operator-=(this_arg other)& { return (this->template assign_at<TIndices>(at<TIndices>() - other.at<TIndices>()), ..., *this); }
CXXSWIZZLE_FORCE_INLINE this_type& operator*=(this_arg other)& { return (this->template assign_at<TIndices>(at<TIndices>() * other.at<TIndices>()), ..., *this); }
CXXSWIZZLE_FORCE_INLINE this_type& operator/=(this_arg other)& { return (this->template assign_at<TIndices>(at<TIndices>() / other.at<TIndices>()), ..., *this); }

CXXSWIZZLE_FORCE_INLINE this_type&& operator+=(this_arg other)&& { return (this->template assign_at_fast<TIndices>(at<TIndices>() + other.at<TIndices>()), ..., std::move(*this)); }
CXXSWIZZLE_FORCE_INLINE this_type&& operator-=(this_arg other)&& { return (this->template assign_at_fast<TIndices>(at<TIndices>() - other.at<TIndices>()), ..., std::move(*this)); }
CXXSWIZZLE_FORCE_INLINE this_type&& operator*=(this_arg other)&& { return (this->template assign_at_fast<TIndices>(at<TIndices>() * other.at<TIndices>()), ..., std::move(*this)); }
CXXSWIZZLE_FORCE_INLINE this_type&& operator/=(this_arg other)&& { return (this->template assign_at_fast<TIndices>(at<TIndices>() / other.at<TIndices>()), ..., std::move(*this)); }


CXXSWIZZLE_FORCE_INLINE this_type& operator++()
{
    const auto one = batch_scalar_cast(primitive_type(1));
    return (this->template assign_at<TIndices>(at<TIndices>() + one), ..., *this);
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
    return (this->template assign_at<TIndices>(at<TIndices>() - one), ..., *this);
}

CXXSWIZZLE_FORCE_INLINE this_type operator--(int)
{
    this_type result(*this);
    --(*this);
    return result;
}

// binary

CXXSWIZZLE_FORCE_INLINE friend this_type operator+(this_arg a, this_arg b) { return this_type(construct_tag{}, a.at<TIndices>() + b.at<TIndices>()...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator-(this_arg a, this_arg b) { return this_type(construct_tag{}, a.at<TIndices>() - b.at<TIndices>()...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator*(this_arg a, this_arg b) { return this_type(construct_tag{}, a.at<TIndices>() * b.at<TIndices>()...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator/(this_arg a, this_arg b) { return this_type(construct_tag{}, a.at<TIndices>() / b.at<TIndices>()...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator%(this_arg a, this_arg b) { return this_type(construct_tag{}, a.at<TIndices>() % b.at<TIndices>()...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator^(this_arg a, this_arg b) { return this_type(construct_tag{}, a.at<TIndices>() ^ b.at<TIndices>()...); }

CXXSWIZZLE_FORCE_INLINE friend this_type operator+(this_arg a, primitive_type _b) { auto b = typename base_type::data_type(_b); return this_type(construct_tag{}, a.at<TIndices>() + b...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator-(this_arg a, primitive_type _b) { auto b = typename base_type::data_type(_b); return this_type(construct_tag{}, a.at<TIndices>() - b...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator*(this_arg a, primitive_type _b) { auto b = typename base_type::data_type(_b); return this_type(construct_tag{}, a.at<TIndices>() * b...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator/(this_arg a, primitive_type _b) { auto b = typename base_type::data_type(_b); return this_type(construct_tag{}, a.at<TIndices>() / b...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator%(this_arg a, primitive_type _b) { auto b = typename base_type::data_type(_b); return this_type(construct_tag{}, a.at<TIndices>() % b...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator^(this_arg a, primitive_type _b) { auto b = typename base_type::data_type(_b); return this_type(construct_tag{}, a.at<TIndices>() ^ b...); }

CXXSWIZZLE_FORCE_INLINE friend this_type operator+(primitive_type _a, this_arg b) { return b + _a; }
CXXSWIZZLE_FORCE_INLINE friend this_type operator-(primitive_type _a, this_arg b) { auto a = typename base_type::data_type(_a); return this_type(construct_tag{}, a - b.at<TIndices>()...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator*(primitive_type _a, this_arg b) { return b * _a; }
CXXSWIZZLE_FORCE_INLINE friend this_type operator/(primitive_type _a, this_arg b) { auto a = typename base_type::data_type(_a); return this_type(construct_tag{}, a / b.at<TIndices>()...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator%(primitive_type _a, this_arg b) { auto a = typename base_type::data_type(_a); return this_type(construct_tag{}, a % b.at<TIndices>()...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator^(primitive_type _a, this_arg b) { auto a = typename base_type::data_type(_a); return this_type(construct_tag{}, a ^ b.at<TIndices>()...); }


CXXSWIZZLE_FORCE_INLINE friend this_type operator&(this_arg a, this_arg b) { return this_type(construct_tag{}, a.at<TIndices>() & b.at<TIndices>()...); }

CXXSWIZZLE_FORCE_INLINE friend this_type operator>>(this_arg a, this_arg b) { return this_type(construct_tag{}, a.at<TIndices>() >> b.at<TIndices>()...); }
CXXSWIZZLE_FORCE_INLINE friend this_type operator<<(this_arg a, this_arg b) { return this_type(construct_tag{}, a.at<TIndices>() << b.at<TIndices>()...); }

CXXSWIZZLE_FORCE_INLINE friend bool_type operator>(this_arg a, this_arg b) { return bool_type(construct_tag{}, static_cast<typename bool_type::data_type>(a.at<TIndices>() > b.at<TIndices>())...); }
CXXSWIZZLE_FORCE_INLINE friend bool_type operator>=(this_arg a, this_arg b) { return bool_type(construct_tag{}, static_cast<typename bool_type::data_type>(a.at<TIndices>() >= b.at<TIndices>())...); }
CXXSWIZZLE_FORCE_INLINE friend bool_type operator<(this_arg a, this_arg b) { return bool_type(construct_tag{}, static_cast<typename bool_type::data_type>(a.at<TIndices>() < b.at<TIndices>())...); }
CXXSWIZZLE_FORCE_INLINE friend bool_type operator<=(this_arg a, this_arg b) { return bool_type(construct_tag{}, static_cast<typename bool_type::data_type>(a.at<TIndices>() <= b.at<TIndices>())...); }
CXXSWIZZLE_FORCE_INLINE friend bool_type operator==(this_arg a, this_arg b) { return bool_type(construct_tag{}, static_cast<typename bool_type::data_type>(a.at<TIndices>() == b.at<TIndices>())...); }
CXXSWIZZLE_FORCE_INLINE friend bool_type operator!=(this_arg a, this_arg b) { return bool_type(construct_tag{}, static_cast<typename bool_type::data_type>(a.at<TIndices>() != b.at<TIndices>())...); }