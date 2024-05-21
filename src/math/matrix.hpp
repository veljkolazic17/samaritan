#pragma once
#include <defines.hpp>
#include <math/vector.hpp>
#include <math/math.hpp>
#include <utils/asserts/assert.hpp>
#include <engine/memory/memory.hpp>

#define MATRIX_INVERSE_EPSILON		1e-14
#define MATRIX_EPSILON				1e-6

BEGIN_NAMESPACE

//START Definition: smMat4
class smMat4 {
public:
	smMat4(void);
	explicit smMat4(const smVec4& x, const smVec4& y, const smVec4& z, const smVec4& w);
	explicit smMat4(const smfloat32 xx, const smfloat32 xy, const smfloat32 xz, const smfloat32 xw,
		const smfloat32 yx, const smfloat32 yy, const smfloat32 yz, const smfloat32 yw,
		const smfloat32 zx, const smfloat32 zy, const smfloat32 zz, const smfloat32 zw,
		const smfloat32 wx, const smfloat32 wy, const smfloat32 wz, const smfloat32 ww);
	//explicit smMat4(const smMat3& rotation, const smVec3& translation);
	explicit smMat4(const smfloat32 src[4][4]);

	const smVec4&	operator[](int index) const;
	smVec4&			operator[](int index);
	smMat4			operator*(const smfloat32 a) const;
	smVec4			operator*(const smVec4& vec) const;
	smVec3			operator*(const smVec3& vec) const;
	smMat4			operator*(const smMat4& a) const;
	smMat4			operator+(const smMat4& a) const;
	smMat4			operator-(const smMat4& a) const;
	smMat4&			operator*=(const smfloat32 a);
	smMat4&			operator*=(const smMat4& a);
	smMat4&			operator+=(const smMat4& a);
	smMat4&			operator-=(const smMat4& a);

	friend smMat4	operator*(const smfloat32 a, const smMat4& mat);
	friend smVec4	operator*(const smVec4& vec, const smMat4& mat);
	friend smVec3	operator*(const smVec3& vec, const smMat4& mat);
	friend smVec4&	operator*=(smVec4& vec, const smMat4& mat);
	friend smVec3&	operator*=(smVec3& vec, const smMat4& mat);

	smbool			Compare(const smMat4& a) const;						// exact compare, no epsilon
	smbool			Compare(const smMat4& a, const smfloat32 epsilon) const;	// compare with epsilon
	smbool			operator==(const smMat4& a) const;					// exact compare, no epsilon
	smbool			operator!=(const smMat4& a) const;					// exact compare, no epsilon

	void Zero(void);

	//void			smZero(void);
	void			Identity(void);
	smbool			IsIdentity(const smfloat32 epsilon = MATRIX_EPSILON) const;
	smbool			IsSymmetric(const smfloat32 epsilon = MATRIX_EPSILON) const;
	smbool			IsDiagonal(const smfloat32 epsilon = MATRIX_EPSILON) const;
	smbool			IsRotated(void) const;

	void			ProjectVector(const smVec4& src, smVec4& dst) const;
	void			UnprojectVector(const smVec4& src, smVec4& dst) const;

	smfloat32		Trace(void) const;
	smfloat32		Determinant(void) const;
	smMat4			Transpose(void) const;	// returns transpose
	smMat4&			TransposeSelf(void);
	smMat4			Inverse(void) const;		// returns the inverse ( m * m.Inverse() = identity )
	smbool			InverseSelf(void);		// returns false if determinant is zero
	smMat4			InverseFast(void) const;	// returns the inverse ( m * m.Inverse() = identity )
	smbool			InverseFastSelf(void);	// returns false if determinant is zero
	//smMat4			TransposeMultiply(const smMat4& b) const;

	smVec3			Up();
	smVec3			Down();
	smVec3			Left();
	smVec3			Right();
	smVec3			Forward();
	smVec3			Backward();

	static smMat4	Perspective(smfloat32 fovRadius, smfloat32 aspectRation, smfloat32 nearClip, smfloat32 farClip);
	static smMat4	RotationEulerX(smfloat32 angle);
	static smMat4	RotationEulerY(smfloat32 angle);
	static smMat4	RotationEulerZ(smfloat32 angle);
	static smMat4	RotationEuler(smfloat32 x, smfloat32 y, smfloat32 z);

	int				GetDimension(void) const;

	smcstring		ToString(int precision = 2) const;

private:
	smVec4			mat[4];
};

#define smMat4_zero		smMat4(smVec4(0, 0, 0, 0), smVec4(0, 0, 0, 0), smVec4(0, 0, 0, 0), smVec4(0, 0, 0, 0))
#define smMat4_identity	smMat4(smVec4(1, 0, 0, 0), smVec4(0, 1, 0, 0), smVec4(0, 0, 1, 0), smVec4(0, 0, 0, 1))

SM_INLINE smMat4 smMat4Translation(smVec3 position) 
{
	smMat4 outMatrix = smMat4_identity;
	outMatrix[3][0] = position.m_X;
	outMatrix[3][1] = position.m_Y;
	outMatrix[3][2] = position.m_Z;
	return outMatrix;
}

SM_INLINE smMat4::smMat4(void) {
}

SM_INLINE smMat4::smMat4(const smVec4& x, const smVec4& y, const smVec4& z, const smVec4& w) {
	mat[0] = x;
	mat[1] = y;
	mat[2] = z;
	mat[3] = w;
}

SM_INLINE smMat4::smMat4(const smfloat32 xx, const smfloat32 xy, const smfloat32 xz, const smfloat32 xw,
	const smfloat32 yx, const smfloat32 yy, const smfloat32 yz, const smfloat32 yw,
	const smfloat32 zx, const smfloat32 zy, const smfloat32 zz, const smfloat32 zw,
	const smfloat32 wx, const smfloat32 wy, const smfloat32 wz, const smfloat32 ww) {
	mat[0][0] = xx; mat[0][1] = xy; mat[0][2] = xz; mat[0][3] = xw;
	mat[1][0] = yx; mat[1][1] = yy; mat[1][2] = yz; mat[1][3] = yw;
	mat[2][0] = zx; mat[2][1] = zy; mat[2][2] = zz; mat[2][3] = zw;
	mat[3][0] = wx; mat[3][1] = wy; mat[3][2] = wz; mat[3][3] = ww;
}

//SM_INLINE smMat4::smMat4(const smMat3& rotation, const smVec3& translation) {
//	mat[0][0] = rotation[0][0];
//	mat[0][1] = rotation[1][0];
//	mat[0][2] = rotation[2][0];
//	mat[0][3] = translation[0];
//	mat[1][0] = rotation[0][1];
//	mat[1][1] = rotation[1][1];
//	mat[1][2] = rotation[2][1];
//	mat[1][3] = translation[1];
//	mat[2][0] = rotation[0][2];
//	mat[2][1] = rotation[1][2];
//	mat[2][2] = rotation[2][2];
//	mat[2][3] = translation[2];
//	mat[3][0] = 0.0f;
//	mat[3][1] = 0.0f;
//	mat[3][2] = 0.0f;
//	mat[3][3] = 1.0f;
//}

SM_INLINE smMat4::smMat4(const smfloat32 src[4][4])
{
	smCopy(mat, (void*)src, 4 * 4 * sizeof(smfloat32));
}

SM_INLINE const smVec4& smMat4::operator[](int index) const {
	return mat[index];
}

SM_INLINE smVec4& smMat4::operator[](int index) {
	return mat[index];
}

SM_INLINE smMat4 smMat4::operator*(const smfloat32 a) const {
	return smMat4(
		mat[0].m_X * a, mat[0].m_Y * a, mat[0].m_Z * a, mat[0].m_W * a,
		mat[1].m_X * a, mat[1].m_Y * a, mat[1].m_Z * a, mat[1].m_W * a,
		mat[2].m_X * a, mat[2].m_Y * a, mat[2].m_Z * a, mat[2].m_W * a,
		mat[3].m_X * a, mat[3].m_Y * a, mat[3].m_Z * a, mat[3].m_W * a);
}

SM_INLINE smVec4 smMat4::operator*(const smVec4& vec) const {
	return smVec4(
		mat[0].m_X * vec.m_X + mat[0].m_Y * vec.m_Y + mat[0].m_Z * vec.m_Z + mat[0].m_W * vec.m_W,
		mat[1].m_X * vec.m_X + mat[1].m_Y * vec.m_Y + mat[1].m_Z * vec.m_Z + mat[1].m_W * vec.m_W,
		mat[2].m_X * vec.m_X + mat[2].m_Y * vec.m_Y + mat[2].m_Z * vec.m_Z + mat[2].m_W * vec.m_W,
		mat[3].m_X * vec.m_X + mat[3].m_Y * vec.m_Y + mat[3].m_Z * vec.m_Z + mat[3].m_W * vec.m_W);
}

SM_INLINE smVec3 smMat4::operator*(const smVec3& vec) const {
	smfloat32 s = mat[3].m_X * vec.m_X + mat[3].m_Y * vec.m_Y + mat[3].m_Z * vec.m_Z + mat[3].m_W;
	if (s == 0.0f) {
		return smVec3(0.0f, 0.0f, 0.0f);
	}
	if (s == 1.0f) {
		return smVec3(
			mat[0].m_X * vec.m_X + mat[0].m_Y * vec.m_Y + mat[0].m_Z * vec.m_Z + mat[0].m_W,
			mat[1].m_X * vec.m_X + mat[1].m_Y * vec.m_Y + mat[1].m_Z * vec.m_Z + mat[1].m_W,
			mat[2].m_X * vec.m_X + mat[2].m_Y * vec.m_Y + mat[2].m_Z * vec.m_Z + mat[2].m_W);
	}
	else {
		smfloat32 invS = 1.0f / s;
		return smVec3(
			(mat[0].m_X * vec.m_X + mat[0].m_Y * vec.m_Y + mat[0].m_Z * vec.m_Z + mat[0].m_W) * invS,
			(mat[1].m_X * vec.m_X + mat[1].m_Y * vec.m_Y + mat[1].m_Z * vec.m_Z + mat[1].m_W) * invS,
			(mat[2].m_X * vec.m_X + mat[2].m_Y * vec.m_Y + mat[2].m_Z * vec.m_Z + mat[2].m_W) * invS);
	}
}

SM_INLINE smMat4 smMat4::operator*(const smMat4& a) const {
	int i, j;
	const smfloat32* m1Ptr, * m2Ptr;
	smfloat32* dstPtr;
	smMat4 dst;

	m1Ptr = reinterpret_cast<const smfloat32*>(this);
	m2Ptr = reinterpret_cast<const smfloat32*>(&a);
	dstPtr = reinterpret_cast<smfloat32*>(&dst);

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			*dstPtr = m1Ptr[0] * m2Ptr[0 * 4 + j]
				+ m1Ptr[1] * m2Ptr[1 * 4 + j]
				+ m1Ptr[2] * m2Ptr[2 * 4 + j]
				+ m1Ptr[3] * m2Ptr[3 * 4 + j];
			dstPtr++;
		}
		m1Ptr += 4;
	}
	return dst;
}

SM_INLINE smMat4 smMat4::operator+(const smMat4& a) const {
	return smMat4(
		mat[0].m_X + a[0].m_X, mat[0].m_Y + a[0].m_Y, mat[0].m_Z + a[0].m_Z, mat[0].m_W + a[0].m_W,
		mat[1].m_X + a[1].m_X, mat[1].m_Y + a[1].m_Y, mat[1].m_Z + a[1].m_Z, mat[1].m_W + a[1].m_W,
		mat[2].m_X + a[2].m_X, mat[2].m_Y + a[2].m_Y, mat[2].m_Z + a[2].m_Z, mat[2].m_W + a[2].m_W,
		mat[3].m_X + a[3].m_X, mat[3].m_Y + a[3].m_Y, mat[3].m_Z + a[3].m_Z, mat[3].m_W + a[3].m_W);
}

SM_INLINE smMat4 smMat4::operator-(const smMat4& a) const {
	return smMat4(
		mat[0].m_X - a[0].m_X, mat[0].m_Y - a[0].m_Y, mat[0].m_Z - a[0].m_Z, mat[0].m_W - a[0].m_W,
		mat[1].m_X - a[1].m_X, mat[1].m_Y - a[1].m_Y, mat[1].m_Z - a[1].m_Z, mat[1].m_W - a[1].m_W,
		mat[2].m_X - a[2].m_X, mat[2].m_Y - a[2].m_Y, mat[2].m_Z - a[2].m_Z, mat[2].m_W - a[2].m_W,
		mat[3].m_X - a[3].m_X, mat[3].m_Y - a[3].m_Y, mat[3].m_Z - a[3].m_Z, mat[3].m_W - a[3].m_W);
}

SM_INLINE smMat4& smMat4::operator*=(const smfloat32 a) {
	mat[0].m_X *= a; mat[0].m_Y *= a; mat[0].m_Z *= a; mat[0].m_W *= a;
	mat[1].m_X *= a; mat[1].m_Y *= a; mat[1].m_Z *= a; mat[1].m_W *= a;
	mat[2].m_X *= a; mat[2].m_Y *= a; mat[2].m_Z *= a; mat[2].m_W *= a;
	mat[3].m_X *= a; mat[3].m_Y *= a; mat[3].m_Z *= a; mat[3].m_W *= a;
	return *this;
}

SM_INLINE smMat4& smMat4::operator*=(const smMat4& a) {
	*this = (*this) * a;
	return *this;
}

SM_INLINE smMat4& smMat4::operator+=(const smMat4& a) {
	mat[0].m_X += a[0].m_X; mat[0].m_Y += a[0].m_Y; mat[0].m_Z += a[0].m_Z; mat[0].m_W += a[0].m_W;
	mat[1].m_X += a[1].m_X; mat[1].m_Y += a[1].m_Y; mat[1].m_Z += a[1].m_Z; mat[1].m_W += a[1].m_W;
	mat[2].m_X += a[2].m_X; mat[2].m_Y += a[2].m_Y; mat[2].m_Z += a[2].m_Z; mat[2].m_W += a[2].m_W;
	mat[3].m_X += a[3].m_X; mat[3].m_Y += a[3].m_Y; mat[3].m_Z += a[3].m_Z; mat[3].m_W += a[3].m_W;
	return *this;
}

SM_INLINE smMat4& smMat4::operator-=(const smMat4& a) {
	mat[0].m_X -= a[0].m_X; mat[0].m_Y -= a[0].m_Y; mat[0].m_Z -= a[0].m_Z; mat[0].m_W -= a[0].m_W;
	mat[1].m_X -= a[1].m_X; mat[1].m_Y -= a[1].m_Y; mat[1].m_Z -= a[1].m_Z; mat[1].m_W -= a[1].m_W;
	mat[2].m_X -= a[2].m_X; mat[2].m_Y -= a[2].m_Y; mat[2].m_Z -= a[2].m_Z; mat[2].m_W -= a[2].m_W;
	mat[3].m_X -= a[3].m_X; mat[3].m_Y -= a[3].m_Y; mat[3].m_Z -= a[3].m_Z; mat[3].m_W -= a[3].m_W;
	return *this;
}

SM_INLINE smMat4 operator*(const smfloat32 a, const smMat4& mat) {
	return mat * a;
}

SM_INLINE smVec4 operator*(const smVec4& vec, const smMat4& mat) {
	return mat * vec;
}

SM_INLINE smVec3 operator*(const smVec3& vec, const smMat4& mat) {
	return mat * vec;
}

SM_INLINE smVec4& operator*=(smVec4& vec, const smMat4& mat) {
	vec = mat * vec;
	return vec;
}

SM_INLINE smVec3& operator*=(smVec3& vec, const smMat4& mat) {
	vec = mat * vec;
	return vec;
}

SM_INLINE smbool smMat4::Compare(const smMat4& a) const {
	smuint32 i;
	const smfloat32* ptr1, * ptr2;

	ptr1 = reinterpret_cast<const smfloat32*>(mat);
	ptr2 = reinterpret_cast<const smfloat32*>(a.mat);
	for (i = 0; i < 4 * 4; i++) {
		if (ptr1[i] != ptr2[i]) {
			return false;
		}
	}
	return true;
}

SM_INLINE smbool smMat4::Compare(const smMat4& a, const smfloat32 epsilon) const {
	smuint32 i;
	const smfloat32* ptr1, * ptr2;

	ptr1 = reinterpret_cast<const smfloat32*>(mat);
	ptr2 = reinterpret_cast<const smfloat32*>(a.mat);
	for (i = 0; i < 4 * 4; i++) {
		if (Math::Fabs(ptr1[i] - ptr2[i]) > epsilon) {
			return false;
		}
	}
	return true;
}

SM_INLINE smbool smMat4::operator==(const smMat4& a) const
{
	return Compare(a);
}

SM_INLINE smbool smMat4::operator!=(const smMat4& a) const
{
	return !Compare(a);
}

SM_INLINE void smMat4::Zero(void)
{
	smZero(mat, 0, sizeof(smMat4));
}

SM_INLINE void smMat4::Identity(void)
{
	*this = smMat4_identity;
}

SM_INLINE smbool smMat4::IsIdentity(const smfloat32 epsilon) const {
	return Compare(smMat4_identity, epsilon);
}

SM_INLINE smbool smMat4::IsSymmetric(const smfloat32 epsilon) const {
	for (int i = 1; i < 4; i++) {
		for (int j = 0; j < i; j++) {
			if (Math::Fabs(mat[i][j] - mat[j][i]) > epsilon) {
				return false;
			}
		}
	}
	return true;
}

SM_INLINE smbool smMat4::IsDiagonal(const smfloat32 epsilon) const {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (i != j && Math::Fabs(mat[i][j]) > epsilon) {
				return false;
			}
		}
	}
	return true;
}

SM_INLINE smbool smMat4::IsRotated(void) const {
	if (!mat[0][1] && !mat[0][2] &&
		!mat[1][0] && !mat[1][2] &&
		!mat[2][0] && !mat[2][1]) {
		return false;
	}
	return true;
}

SM_INLINE void smMat4::ProjectVector(const smVec4& src, smVec4& dst) const {
	dst.m_X = src * mat[0];
	dst.m_Y = src * mat[1];
	dst.m_Z = src * mat[2];
	dst.m_W = src * mat[3];
}

SM_INLINE void smMat4::UnprojectVector(const smVec4& src, smVec4& dst) const {
	dst = mat[0] * src.m_X + mat[1] * src.m_Y + mat[2] * src.m_Z + mat[3] * src.m_W;
}

SM_INLINE smfloat32 smMat4::Trace(void) const {
	return (mat[0][0] + mat[1][1] + mat[2][2] + mat[3][3]);
}

SM_INLINE smMat4 smMat4::Inverse(void) const {
	smMat4 invMat;

	invMat = *this;
	int r = invMat.InverseSelf();
	softAssert(r, "smMat4::InverseSelf");
	return invMat;
}

SM_INLINE smMat4 smMat4::InverseFast(void) const {
	smMat4 invMat;

	invMat = *this;
	int r = invMat.InverseFastSelf();
	softAssert(r, "smMat4::InverseSelf");
	return invMat;
}

//SM_INLINE smMat4 idMat3::ToMat4(void) const {
//	// NOTE: smMat3 is transposed because it is column-major
//	return smMat4(mat[0][0], mat[1][0], mat[2][0], 0.0f,
//		mat[0][1], mat[1][1], mat[2][1], 0.0f,
//		mat[0][2], mat[1][2], mat[2][2], 0.0f,
//		0.0f, 0.0f, 0.0f, 1.0f);
//}

SM_INLINE int smMat4::GetDimension(void) const {
	return 16;
}

SM_INLINE smMat4 smMat4::RotationEulerX(smfloat32 angle)
{
	smMat4 out = smMat4_identity;
	smfloat32 c = Math::Cos(angle);
	smfloat32 s = Math::Sin(angle);
	out[1][1] = c;
	out[1][2] = s;
	out[2][1] = -s;
	out[2][2] = c;
	return out;
}

SM_INLINE smMat4 smMat4::RotationEulerY(smfloat32 angle)
{
	smMat4 out = smMat4_identity;
	smfloat32 c = Math::Cos(angle);
	smfloat32 s = Math::Sin(angle);
	out[0][0] = c;
	out[0][2] = s;
	out[2][0] = -s;
	out[2][2] = c;
	return out;
}

SM_INLINE smMat4 smMat4::RotationEulerZ(smfloat32 angle)
{
	smMat4 out = smMat4_identity;
	smfloat32 c = Math::Cos(angle);
	smfloat32 s = Math::Sin(angle);
	out[0][0] = c;
	out[0][1] = s;
	out[1][0] = -s;
	out[1][1] = c;
	return out;
}

SM_INLINE smMat4 smMat4::RotationEuler(smfloat32 x, smfloat32 y, smfloat32 z)
{
	smMat4 rotationX = smMat4::RotationEulerX(x);
	smMat4 rotationY = smMat4::RotationEulerY(y);
	smMat4 rotationZ = smMat4::RotationEulerZ(z);
	smMat4 out = rotationX * rotationY;
	out *= rotationZ;
	return out;
}

SM_INLINE smVec3 smMat4::Up()
{
	smVec3 retVec = smVec3{ mat[0][1], mat[1][1], mat[2][1] };
	retVec.Normalize();
	return retVec;
}

SM_INLINE smVec3 smMat4::Down()
{
	smVec3 retVec = smVec3{ -mat[0][1], -mat[1][1], -mat[2][1] };
	retVec.Normalize();
	return retVec;
}

SM_INLINE smVec3 smMat4::Left()
{
	smVec3 retVec = smVec3{ -mat[0][0], -mat[1][0], -mat[2][0] };
	retVec.Normalize();
	return retVec;
}

SM_INLINE smVec3 smMat4::Right()
{
	smVec3 retVec = smVec3{ mat[0][0], mat[1][0], mat[2][0] };
	retVec.Normalize();
	return retVec;
}

SM_INLINE smVec3 smMat4::Forward()
{
	smVec3 retVec = smVec3{ -mat[0][2], -mat[1][2], -mat[2][2] };
	retVec.Normalize();
	return retVec;
}

SM_INLINE smVec3 smMat4::Backward()
{
	smVec3 retVec =  smVec3{ mat[0][2], mat[1][2], mat[2][2] };
	retVec.Normalize();
	return retVec;
}



//END Definition: smMat4

END_NAMESPACE