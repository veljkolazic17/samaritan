#pragma once
#include <defines.hpp>

#include <math/math.hpp>

#include <string>
#include <cstdio>


BEGIN_NAMESPACE
	//START Definition: smVec4
struct smVec4 
{
	smVec4(){}

	explicit smVec4(const smfloat32 x, const smfloat32 y, const smfloat32 z, const smfloat32 w)
		: m_X(x), m_Y(y), m_Z(z), m_W(w){}

	void Set(const smfloat32 x, const smfloat32 y, const smfloat32 z, const smfloat32 w);
	void Zero();

	smfloat32 operator[](const int index) const;
	smfloat32& operator[](const int index);
	smVec4 operator-() const;
	smfloat32 operator*(const smVec4& a) const;
	smVec4 operator*(const smfloat32 a) const;
	smVec4 operator/(const smfloat32 a) const;
	smVec4 operator+(const smVec4& a) const;
	smVec4 operator-(const smVec4& a) const;
	smVec4& operator+=(const smVec4& a);
	smVec4& operator-=(const smVec4& a);
	smVec4& operator/=(const smVec4& a);
	smVec4& operator/=(const smfloat32 a);
	smVec4& operator*=(const smfloat32 a);

	friend smVec4 operator*(const smfloat32 a, const smVec4 b);

	smbool Compare(const smVec4& a) const;									// exact compare, no epsilon
	smbool Compare(const smVec4& a, const smfloat32 epsilon) const;			// compare with epsilon
	smbool operator==(const smVec4& a) const;								// exact compare, no epsilon
	smbool operator!=(const smVec4& a) const;								// exact compare, no epsilon

	smfloat32 Length() const;
	smfloat32 LengthSqr() const;
	smfloat32 Normalize();			// returns length
	smfloat32 NormalizeFast();		// returns length

	smuint8 GetDimension() const;

	void Lerp(const smVec4& v1, const smVec4& v2, const smfloat32 l);

	SM_INLINE static smVec4 StringToVec4(const std::string& str);

	smfloat32 m_X;
	smfloat32 m_Y;
	smfloat32 m_Z;
	smfloat32 m_W;
};

#define smVec4_zero	smVec4{0, 0, 0, 0}
#define smVec4_one	smVec4{1, 1, 1, 1}

SM_INLINE smVec4 smVec4::StringToVec4(const std::string& str)
{
	smfloat32 x, y, z, w;
	smint32 numParsed = std::sscanf(str.c_str(), "%f %f %f %f", &x, &y, &z, &w);
	if (numParsed == 4) 
	{
		return smVec4(x, y, z, w);
	}
	else 
	{
		return smVec4(0.0f, 0.0f, 0.0f, 0.0f);
	}
}

SM_INLINE void smVec4::Set(const smfloat32 x, const smfloat32 y, const smfloat32 z, const smfloat32 w)
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

SM_INLINE smfloat32 smVec4::operator[](int index) const
{
	return (&m_X)[index];
}

SM_INLINE smfloat32& smVec4::operator[](int index)
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

SM_INLINE smfloat32 smVec4::operator*(const smVec4& a) const
{
	return m_X * a.m_X + m_Y * a.m_Y + m_Y * a.m_Z + m_W * a.m_W;
}

SM_INLINE smVec4 smVec4::operator*(const float a) const 
{
	return smVec4(m_X * a, m_Y * a, m_Z * a, m_W * a);
}

SM_INLINE smVec4 smVec4::operator/(const smfloat32 a) const
{
	smfloat32 inverted = 1.0f / a;
	return smVec4(m_X * inverted, m_Y * inverted, m_Z * inverted, m_W * inverted);
}

SM_INLINE smVec4 operator*(const smfloat32 a, const smVec4 b)
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

SM_INLINE smVec4& smVec4::operator/=(const smfloat32 a)
{
	smfloat32 inverted = 1.0f / a;
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

SM_INLINE smVec4& smVec4::operator*=(const smfloat32 a)
{
	m_X *= a;
	m_Y *= a;
	m_Z *= a;
	m_W *= a;

	return *this;
}

SM_INLINE smbool smVec4::Compare(const smVec4& a) const 
{
	return ((m_X == a.m_X) && (m_Y == a.m_Y) && (m_Z == a.m_Z) && m_W == a.m_W);
}

SM_INLINE smbool smVec4::operator==(const smVec4& a) const 
{
	return Compare(a);
}

SM_INLINE smbool smVec4::operator!=(const smVec4& a) const 
{
	return !Compare(a);
}

SM_INLINE smuint8 smVec4::GetDimension(void) const 
{
	return 4;
}
//END Definition smVec4

//START Definition smVec3
struct smVec3
{
	smVec3() {}

	explicit smVec3(const smfloat32 x, const smfloat32 y, const smfloat32 z)
		: m_X(x), m_Y(y), m_Z(z) {}

	void Set(const smfloat32 x, const smfloat32 y, const smfloat32 z);
	void Zero();

	smfloat32 operator[](const int index) const;
	smfloat32& operator[](const int index);
	smVec3 operator-() const;
	smfloat32 operator*(const smVec3& a) const;
	smVec3 operator*(const smfloat32 a) const;
	smVec3 operator/(const smfloat32 a) const;
	smVec3 operator+(const smVec3& a) const;
	smVec3 operator-(const smVec3& a) const;
	smVec3& operator+=(const smVec3& a);
	smVec3& operator-=(const smVec3& a);
	smVec3& operator/=(const smVec3& a);
	smVec3& operator/=(const smfloat32 a);
	smVec3& operator*=(const smfloat32 a);

	friend smVec3 operator*(const smfloat32 a, const smVec3 b);

	smbool Compare(const smVec3& a) const;									// exact compare, no epsilon
	smbool Compare(const smVec3& a, const smfloat32 epsilon) const;			// compare with epsilon
	smbool operator==(const smVec3& a) const;								// exact compare, no epsilon
	smbool operator!=(const smVec3& a) const;								// exact compare, no epsilon

	smfloat32 Length() const;
	smfloat32 LengthSqr() const;
	smfloat32 Normalize();			// returns length

	smuint8 GetDimension() const;

	void Lerp(const smVec3& v1, const smVec3& v2, const smfloat32 l);

	smfloat32 m_X;
	smfloat32 m_Y;
	smfloat32 m_Z;
};

#define smVec3_zero	smVec3{0, 0, 0}
#define smVec3_one	smVec3{1, 1, 1}

SM_INLINE void smVec3::Set(const smfloat32 x, const smfloat32 y, const smfloat32 z)
{
	m_X = x;
	m_Y = y;
	m_Z = z;
}

SM_INLINE void smVec3::Zero()
{
	m_X = m_Y = m_Z = 0.0f;
}

SM_INLINE smfloat32 smVec3::operator[](int index) const
{
	return (&m_X)[index];
}

SM_INLINE smfloat32& smVec3::operator[](int index)
{
	return (&m_X)[index];
}

SM_INLINE smVec3 smVec3::operator-() const
{
	return smVec3(-m_X, -m_Y, -m_Z);
}

SM_INLINE smVec3 smVec3::operator-(const smVec3& a) const
{
	return smVec3(m_X - a.m_X, m_Y - a.m_Y, m_Z - a.m_Z);
}

SM_INLINE smfloat32 smVec3::operator*(const smVec3& a) const
{
	return m_X * a.m_X + m_Y * a.m_Y + m_Y * a.m_Z;
}

SM_INLINE smVec3 smVec3::operator*(const float a) const
{
	return smVec3(m_X * a, m_Y * a, m_Z * a);
}

SM_INLINE smVec3 smVec3::operator/(const smfloat32 a) const
{
	smfloat32 inverted = 1.0f / a;
	return smVec3(m_X * inverted, m_Y * inverted, m_Z * inverted);
}

SM_INLINE smVec3 operator*(const smfloat32 a, const smVec3 b)
{
	return smVec3(b.m_X * a, b.m_Y * a, b.m_Z * a);
}

SM_INLINE smVec3 smVec3::operator+(const smVec3& a) const
{
	return smVec3(m_X + a.m_X, m_Y + a.m_Y, m_Z + a.m_Z);
}

SM_INLINE smVec3& smVec3::operator+=(const smVec3& a)
{
	m_X += a.m_X;
	m_Y += a.m_Y;
	m_Z += a.m_Z;

	return *this;
}

SM_INLINE smVec3& smVec3::operator/=(const smVec3& a)
{
	m_X /= a.m_X;
	m_Y /= a.m_Y;
	m_Z /= a.m_Z;

	return *this;
}

SM_INLINE smVec3& smVec3::operator/=(const smfloat32 a)
{
	smfloat32 inverted = 1.0f / a;
	m_X *= inverted;
	m_Y *= inverted;
	m_Z *= inverted;

	return *this;
}

SM_INLINE smVec3& smVec3::operator-=(const smVec3& a)
{
	m_X -= a.m_X;
	m_Y -= a.m_Y;
	m_Z -= a.m_Z;

	return *this;
}

SM_INLINE smVec3& smVec3::operator*=(const smfloat32 a)
{
	m_X *= a;
	m_Y *= a;
	m_Z *= a;

	return *this;
}

SM_INLINE smbool smVec3::Compare(const smVec3& a) const
{
	return ((m_X == a.m_X) && (m_Y == a.m_Y) && (m_Z == a.m_Z));
}

SM_INLINE smbool smVec3::operator==(const smVec3& a) const
{
	return Compare(a);
}

SM_INLINE smbool smVec3::operator!=(const smVec3& a) const
{
	return !Compare(a);
}

SM_INLINE smuint8 smVec3::GetDimension(void) const
{
	return 3;
}

SM_INLINE smfloat32 smVec3::Normalize()
{
	smfloat32 sqrLength, invLength;

	sqrLength = m_X * m_X + m_Y * m_Y + m_Z * m_Z;
	invLength = Math::DOOM_InvSqrt(sqrLength);
	m_X *= invLength;
	m_Y *= invLength;
	m_Z *= invLength;
	return invLength * sqrLength;
}


//END Definition smVec3

//START Definition smVec2

struct smVec2
{
	smVec2() {}

	explicit smVec2(const smfloat32 x, const smfloat32 y)
		: m_X(x), m_Y(y) {}

	void Set(const smfloat32 x, const smfloat32 y);
	void Zero();

	smfloat32 operator[](const int index) const;
	smfloat32& operator[](const int index);
	smVec2 operator-() const;
	smfloat32 operator*(const smVec2& a) const;
	smVec2 operator*(const smfloat32 a) const;
	smVec2 operator/(const smfloat32 a) const;
	smVec2 operator+(const smVec2& a) const;
	smVec2 operator-(const smVec2& a) const;
	smVec2& operator+=(const smVec2& a);
	smVec2& operator-=(const smVec2& a);
	smVec2& operator/=(const smVec2& a);
	smVec2& operator/=(const smfloat32 a);
	smVec2& operator*=(const smfloat32 a);

	friend smVec2 operator*(const smfloat32 a, const smVec2 b);

	smbool Compare(const smVec2& a) const;									// exact compare, no epsilon
	smbool Compare(const smVec2& a, const smfloat32 epsilon) const;			// compare with epsilon
	smbool operator==(const smVec2& a) const;								// exact compare, no epsilon
	smbool operator!=(const smVec2& a) const;								// exact compare, no epsilon

	smfloat32 Length() const;
	smfloat32 LengthSqr() const;
	smfloat32 Normalize();			// returns length

	smuint8 GetDimension() const;

	void Lerp(const smVec2& v1, const smVec2& v2, const smfloat32 l);

	smfloat32 m_X;
	smfloat32 m_Y;
};

#define smVec2_zero	smVec2{0, 0}
#define smVec2_one	smVec2{1, 1}

SM_INLINE void smVec2::Set(const smfloat32 x, const smfloat32 y)
{
	m_X = x;
	m_Y = y;
}

SM_INLINE void smVec2::Zero()
{
	m_X = m_Y = 0.0f;
}

SM_INLINE smfloat32 smVec2::operator[](int index) const
{
	return (&m_X)[index];
}

SM_INLINE smfloat32& smVec2::operator[](int index)
{
	return (&m_X)[index];
}

SM_INLINE smVec2 smVec2::operator-() const
{
	return smVec2(-m_X, -m_Y);
}

SM_INLINE smVec2 smVec2::operator-(const smVec2& a) const
{
	return smVec2(m_X - a.m_X, m_Y - a.m_Y);
}

SM_INLINE smfloat32 smVec2::operator*(const smVec2& a) const
{
	return m_X * a.m_X + m_Y * a.m_Y;
}

SM_INLINE smVec2 smVec2::operator*(const float a) const
{
	return smVec2(m_X * a, m_Y * a);
}

SM_INLINE smVec2 smVec2::operator/(const smfloat32 a) const
{
	smfloat32 inverted = 1.0f / a;
	return smVec2(m_X * inverted, m_Y * inverted);
}

SM_INLINE smVec2 operator*(const smfloat32 a, const smVec2 b)
{
	return smVec2(b.m_X * a, b.m_Y * a);
}

SM_INLINE smVec2 smVec2::operator+(const smVec2& a) const
{
	return smVec2(m_X + a.m_X, m_Y + a.m_Y);
}

SM_INLINE smVec2& smVec2::operator+=(const smVec2& a)
{
	m_X += a.m_X;
	m_Y += a.m_Y;

	return *this;
}

SM_INLINE smVec2& smVec2::operator/=(const smVec2& a)
{
	m_X /= a.m_X;
	m_Y /= a.m_Y;

	return *this;
}

SM_INLINE smVec2& smVec2::operator/=(const smfloat32 a)
{
	smfloat32 inverted = 1.0f / a;
	m_X *= inverted;
	m_Y *= inverted;

	return *this;
}

SM_INLINE smVec2& smVec2::operator-=(const smVec2& a)
{
	m_X -= a.m_X;
	m_Y -= a.m_Y;

	return *this;
}

SM_INLINE smVec2& smVec2::operator*=(const smfloat32 a)
{
	m_X *= a;
	m_Y *= a;

	return *this;
}

SM_INLINE smbool smVec2::Compare(const smVec2& a) const
{
	return ((m_X == a.m_X) && (m_Y == a.m_Y));
}

SM_INLINE smbool smVec2::operator==(const smVec2& a) const
{
	return Compare(a);
}

SM_INLINE smbool smVec2::operator!=(const smVec2& a) const
{
	return !Compare(a);
}

SM_INLINE smuint8 smVec2::GetDimension(void) const
{
	return 2;
}

SM_INLINE smfloat32 smVec2::Normalize()
{
	smfloat32 sqrLength, invLength;

	sqrLength = m_X * m_X + m_Y * m_Y;
	invLength = Math::DOOM_InvSqrt(sqrLength);
	m_X *= invLength;
	m_Y *= invLength;
	return invLength * sqrLength;
}

//END Definition smVec2


//START Definition smVert3D
struct smVert3D
{
	smVec3 m_Position;
	smVec2 m_TextureCoordinates;
};
//END Definition smVert3D

END_NAMESPACE
