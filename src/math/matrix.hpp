#pragma once
#include <defines.hpp>
#include <math/vector.hpp>
#include <math/math.hpp>
#include <utils/asserts/assert.hpp>

#define MATRIX_INVERSE_EPSILON		1e-14
#define MATRIX_EPSILON				1e-6

BEGIN_NAMESPACE

//START Definition: smMat4
class smMat4 {
public:
	smMat4(void);
	explicit smMat4(const smVec4& x, const smVec4& y, const smVec4& z, const smVec4& w);
	explicit smMat4(const mfloat32 xx, const mfloat32 xy, const mfloat32 xz, const mfloat32 xw,
		const mfloat32 yx, const mfloat32 yy, const mfloat32 yz, const mfloat32 yw,
		const mfloat32 zx, const mfloat32 zy, const mfloat32 zz, const mfloat32 zw,
		const mfloat32 wx, const mfloat32 wy, const mfloat32 wz, const mfloat32 ww);
	//explicit smMat4(const smMat3& rotation, const smVec3& translation);
	explicit smMat4(const mfloat32 src[4][4]);

	const smVec4&	operator[](int index) const;
	smVec4&			operator[](int index);
	smMat4			operator*(const mfloat32 a) const;
	smVec4			operator*(const smVec4& vec) const;
	smVec3			operator*(const smVec3& vec) const;
	smMat4			operator*(const smMat4& a) const;
	smMat4			operator+(const smMat4& a) const;
	smMat4			operator-(const smMat4& a) const;
	smMat4&			operator*=(const mfloat32 a);
	smMat4&			operator*=(const smMat4& a);
	smMat4&			operator+=(const smMat4& a);
	smMat4&			operator-=(const smMat4& a);

	friend smMat4	operator*(const mfloat32 a, const smMat4& mat);
	friend smVec4	operator*(const smVec4& vec, const smMat4& mat);
	friend smVec3	operator*(const smVec3& vec, const smMat4& mat);
	friend smVec4&	operator*=(smVec4& vec, const smMat4& mat);
	friend smVec3&	operator*=(smVec3& vec, const smMat4& mat);

	mbool			Compare(const smMat4& a) const;						// exact compare, no epsilon
	mbool			Compare(const smMat4& a, const mfloat32 epsilon) const;	// compare with epsilon
	mbool			operator==(const smMat4& a) const;					// exact compare, no epsilon
	mbool			operator!=(const smMat4& a) const;					// exact compare, no epsilon

	void			Zero(void);
	void			Identity(void);
	mbool			IsIdentity(const mfloat32 epsilon = MATRIX_EPSILON) const;
	mbool			IsSymmetric(const mfloat32 epsilon = MATRIX_EPSILON) const;
	mbool			IsDiagonal(const mfloat32 epsilon = MATRIX_EPSILON) const;
	mbool			IsRotated(void) const;

	void			ProjectVector(const smVec4& src, smVec4& dst) const;
	void			UnprojectVector(const smVec4& src, smVec4& dst) const;

	mfloat32		Trace(void) const;
	mfloat32		Determinant(void) const;
	smMat4			Transpose(void) const;	// returns transpose
	smMat4&			TransposeSelf(void);
	smMat4			Inverse(void) const;		// returns the inverse ( m * m.Inverse() = identity )
	mbool			InverseSelf(void);		// returns false if determinant is zero
	smMat4			InverseFast(void) const;	// returns the inverse ( m * m.Inverse() = identity )
	mbool			InverseFastSelf(void);	// returns false if determinant is zero
	//smMat4			TransposeMultiply(const smMat4& b) const;

	static smMat4 Perspective(mfloat32 fovRadius, mfloat32 aspectRation, mfloat32 nearClip, mfloat32 farClip);

	int				GetDimension(void) const;

	mcstring		ToString(int precision = 2) const;

private:
	smVec4			mat[4];
};

#define smMat4_zero		smMat4(smVec4(0, 0, 0, 0), smVec4(0, 0, 0, 0), smVec4(0, 0, 0, 0), smVec4(0, 0, 0, 0))
#define smMat4_identity	smMat4(smVec4(1, 0, 0, 0), smVec4(0, 1, 0, 0), smVec4(0, 0, 1, 0), smVec4(0, 0, 0, 1))

SM_INLINE smMat4 smMat4Translation(smVec3 position) 
{
	smMat4 outMatrix = smMat4_identity;
	outMatrix[2][3] = position.m_X;	//12
	outMatrix[3][0] = position.m_Y;	//13
	outMatrix[3][1] = position.m_Z;	//14
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

SM_INLINE smMat4::smMat4(const mfloat32 xx, const mfloat32 xy, const mfloat32 xz, const mfloat32 xw,
	const mfloat32 yx, const mfloat32 yy, const mfloat32 yz, const mfloat32 yw,
	const mfloat32 zx, const mfloat32 zy, const mfloat32 zz, const mfloat32 zw,
	const mfloat32 wx, const mfloat32 wy, const mfloat32 wz, const mfloat32 ww) {
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

SM_INLINE smMat4::smMat4(const mfloat32 src[4][4]) {
	memcpy(mat, src, 4 * 4 * sizeof(mfloat32));
}

SM_INLINE const smVec4& smMat4::operator[](int index) const {
	return mat[index];
}

SM_INLINE smVec4& smMat4::operator[](int index) {
	return mat[index];
}

SM_INLINE smMat4 smMat4::operator*(const mfloat32 a) const {
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
	mfloat32 s = mat[3].m_X * vec.m_X + mat[3].m_Y * vec.m_Y + mat[3].m_Z * vec.m_Z + mat[3].m_W;
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
		mfloat32 invS = 1.0f / s;
		return smVec3(
			(mat[0].m_X * vec.m_X + mat[0].m_Y * vec.m_Y + mat[0].m_Z * vec.m_Z + mat[0].m_W) * invS,
			(mat[1].m_X * vec.m_X + mat[1].m_Y * vec.m_Y + mat[1].m_Z * vec.m_Z + mat[1].m_W) * invS,
			(mat[2].m_X * vec.m_X + mat[2].m_Y * vec.m_Y + mat[2].m_Z * vec.m_Z + mat[2].m_W) * invS);
	}
}

SM_INLINE smMat4 smMat4::operator*(const smMat4& a) const {
	int i, j;
	const mfloat32* m1Ptr, * m2Ptr;
	mfloat32* dstPtr;
	smMat4 dst;

	m1Ptr = reinterpret_cast<const mfloat32*>(this);
	m2Ptr = reinterpret_cast<const mfloat32*>(&a);
	dstPtr = reinterpret_cast<mfloat32*>(&dst);

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

SM_INLINE smMat4& smMat4::operator*=(const mfloat32 a) {
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

SM_INLINE smMat4 operator*(const mfloat32 a, const smMat4& mat) {
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

SM_INLINE mbool smMat4::Compare(const smMat4& a) const {
	muint32 i;
	const mfloat32* ptr1, * ptr2;

	ptr1 = reinterpret_cast<const mfloat32*>(mat);
	ptr2 = reinterpret_cast<const mfloat32*>(a.mat);
	for (i = 0; i < 4 * 4; i++) {
		if (ptr1[i] != ptr2[i]) {
			return false;
		}
	}
	return true;
}

SM_INLINE mbool smMat4::Compare(const smMat4& a, const mfloat32 epsilon) const {
	muint32 i;
	const mfloat32* ptr1, * ptr2;

	ptr1 = reinterpret_cast<const mfloat32*>(mat);
	ptr2 = reinterpret_cast<const mfloat32*>(a.mat);
	for (i = 0; i < 4 * 4; i++) {
		if (Math::Fabs(ptr1[i] - ptr2[i]) > epsilon) {
			return false;
		}
	}
	return true;
}

SM_INLINE mbool smMat4::operator==(const smMat4& a) const {
	return Compare(a);
}

SM_INLINE mbool smMat4::operator!=(const smMat4& a) const {
	return !Compare(a);
}

SM_INLINE void smMat4::Zero(void) {
	memset(mat, 0, sizeof(smMat4));
}

SM_INLINE void smMat4::Identity(void) {
	*this = smMat4_identity;
}

SM_INLINE mbool smMat4::IsIdentity(const mfloat32 epsilon) const {
	return Compare(smMat4_identity, epsilon);
}

SM_INLINE mbool smMat4::IsSymmetric(const mfloat32 epsilon) const {
	for (int i = 1; i < 4; i++) {
		for (int j = 0; j < i; j++) {
			if (Math::Fabs(mat[i][j] - mat[j][i]) > epsilon) {
				return false;
			}
		}
	}
	return true;
}

SM_INLINE mbool smMat4::IsDiagonal(const mfloat32 epsilon) const {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (i != j && Math::Fabs(mat[i][j]) > epsilon) {
				return false;
			}
		}
	}
	return true;
}

SM_INLINE mbool smMat4::IsRotated(void) const {
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

SM_INLINE mfloat32 smMat4::Trace(void) const {
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

//END Definition: smMat4

END_NAMESPACE