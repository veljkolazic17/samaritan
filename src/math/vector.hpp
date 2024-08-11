#pragma once
#include <defines.hpp>

BEGIN_NAMESPACE


struct smVec4 
{
	smVec4(){}

	explicit smVec4(const mfloat32 x, const mfloat32 y, const mfloat32 z, const mfloat32 w)
		: m_X(x), m_Y(y), m_Z(z), m_W(w){}

	void Set(const mfloat32 x, const mfloat32 y, const mfloat32 z, const mfloat32 w);
	void Zero();

	mfloat32 operator[](const int index) const;
	mfloat32& operator[](const int index);
	smVec4 operator-() const;
	mfloat32 operator*(const smVec4& a) const;
	smVec4 operator*(const mfloat32 a) const;
	smVec4 operator/(const mfloat32 a) const;
	smVec4 operator+(const smVec4& a) const;
	smVec4 operator-(const smVec4& a) const;
	smVec4& operator+=(const smVec4& a);
	smVec4& operator-=(const smVec4& a);
	smVec4& operator/=(const smVec4& a);
	smVec4& operator/=(const mfloat32 a);
	smVec4& operator*=(const mfloat32 a);

	friend smVec4 operator*(const mfloat32 a, const smVec4 b);

	mbool Compare(const smVec4& a) const;									// exact compare, no epsilon
	mbool Compare(const smVec4& a, const mfloat32 epsilon) const;			// compare with epsilon
	mbool operator==(const smVec4& a) const;								// exact compare, no epsilon
	mbool operator!=(const smVec4& a) const;								// exact compare, no epsilon

	mfloat32 Length() const;
	mfloat32 LengthSqr() const;
	mfloat32 Normalize();			// returns length
	mfloat32 NormalizeFast();		// returns length

	muint8 GetDimension() const;

	void Lerp(const smVec4& v1, const smVec4& v2, const mfloat32 l);

	mfloat32 m_X;
	mfloat32 m_Y;
	mfloat32 m_Z;
	mfloat32 m_W;
};

SM_INLINE void smVec4::Set(const mfloat32 x, const mfloat32 y, const mfloat32 z, const mfloat32 w)
{
	m_X = x;
	m_Y = y;
	m_Z = z;
	m_W = w;
}

SM_INLINE void smVec4::Zero() 
{
	m_X = m_Y = m_Z = m_W = 0.0f;
}

SM_INLINE mfloat32 smVec4::operator[](int index) const
{
	return (&m_X)[index];
}

SM_INLINE mfloat32& smVec4::operator[](int index)
{
	return (&m_X)[index];
}

SM_INLINE smVec4 smVec4::operator-() const 
{
	return smVec4(-m_X, -m_Y, -m_Z, -m_W);
}

SM_INLINE smVec4 smVec4::operator-(const smVec4& a) const 
{
	return smVec4(m_X - a.m_X, m_Y - a.m_Y, m_Z - a.m_Z, m_W - a.m_W);
}

SM_INLINE mfloat32 smVec4::operator*(const smVec4& a) const
{
	return m_X * a.m_X + m_Y * a.m_Y + m_Y * a.m_Z + m_W * a.m_W;
}

SM_INLINE smVec4 smVec4::operator*(const float a) const 
{
	return smVec4(m_X * a, m_Y * a, m_Z * a, m_W * a);
}

SM_INLINE smVec4 smVec4::operator/(const mfloat32 a) const
{
	mfloat32 inverted = 1.0f / a;
	return smVec4(m_X * inverted, m_Y * inverted, m_Z * inverted, m_W * inverted);
}

SM_INLINE smVec4 operator*(const mfloat32 a, const smVec4 b)
{
	return smVec4(b.m_X * a, b.m_Y * a, b.m_Z * a, b.m_W * a);
}

SM_INLINE smVec4 smVec4::operator+(const smVec4& a) const 
{
	return smVec4(m_X + a.m_X, m_Y + a.m_Y, m_Z + a.m_Z, m_W + a.m_W);
}

SM_INLINE smVec4& smVec4::operator+=(const smVec4& a) 
{
	m_X += a.m_X;
	m_Y += a.m_Y;
	m_Z += a.m_Z;
	m_W += a.m_W;

	return *this;
}

SM_INLINE smVec4& smVec4::operator/=(const smVec4& a) 
{
	m_X /= a.m_X;
	m_Y /= a.m_Y;
	m_Z /= a.m_Z;
	m_W /= a.m_W;

	return *this;
}

SM_INLINE smVec4& smVec4::operator/=(const mfloat32 a)
{
	mfloat32 inverted = 1.0f / a;
	m_X *= inverted;
	m_Y *= inverted;
	m_Z *= inverted;
	m_W *= inverted;

	return *this;
}

SM_INLINE smVec4& smVec4::operator-=(const smVec4& a) 
{
	m_X -= a.m_X;
	m_Y -= a.m_Y;
	m_Z -= a.m_Z;
	m_W -= a.m_W;

	return *this;
}

SM_INLINE smVec4& smVec4::operator*=(const mfloat32 a)
{
	m_X *= a;
	m_Y *= a;
	m_Z *= a;
	m_W *= a;

	return *this;
}

SM_INLINE mbool smVec4::Compare(const smVec4& a) const 
{
	return ((m_X == a.m_X) && (m_Y == a.m_Y) && (m_Z == a.m_Z) && m_W == a.m_W);
}

SM_INLINE mbool smVec4::operator==(const smVec4& a) const 
{
	return Compare(a);
}

SM_INLINE mbool smVec4::operator!=(const smVec4& a) const 
{
	return !Compare(a);
}

SM_INLINE muint8 smVec4::GetDimension(void) const 
{
	return 4;
}


END_NAMESPACE
