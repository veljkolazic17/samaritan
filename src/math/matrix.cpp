#include <math/matrix.hpp>
#include <math/math.hpp>

BEGIN_NAMESPACE

smMat4 smMat4::Transpose(void) const {
	smMat4	transpose;
	smint32		i, j;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			transpose[i][j] = mat[j][i];
		}
	}
	return transpose;
}

smMat4& smMat4::TransposeSelf(void) {
	smfloat32	temp;
	smint32		i, j;

	for (i = 0; i < 4; i++) {
		for (j = i + 1; j < 4; j++) {
			temp = mat[i][j];
			mat[i][j] = mat[j][i];
			mat[j][i] = temp;
		}
	}
	return *this;
}

smfloat32 smMat4::Determinant(void) const {

	// 2x2 sub-determinants
	smfloat32 det2_01_01 = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
	smfloat32 det2_01_02 = mat[0][0] * mat[1][2] - mat[0][2] * mat[1][0];
	smfloat32 det2_01_03 = mat[0][0] * mat[1][3] - mat[0][3] * mat[1][0];
	smfloat32 det2_01_12 = mat[0][1] * mat[1][2] - mat[0][2] * mat[1][1];
	smfloat32 det2_01_13 = mat[0][1] * mat[1][3] - mat[0][3] * mat[1][1];
	smfloat32 det2_01_23 = mat[0][2] * mat[1][3] - mat[0][3] * mat[1][2];

	// 3x3 sub-determinants
	smfloat32 det3_201_012 = mat[2][0] * det2_01_12 - mat[2][1] * det2_01_02 + mat[2][2] * det2_01_01;
	smfloat32 det3_201_013 = mat[2][0] * det2_01_13 - mat[2][1] * det2_01_03 + mat[2][3] * det2_01_01;
	smfloat32 det3_201_023 = mat[2][0] * det2_01_23 - mat[2][2] * det2_01_03 + mat[2][3] * det2_01_02;
	smfloat32 det3_201_123 = mat[2][1] * det2_01_23 - mat[2][2] * det2_01_13 + mat[2][3] * det2_01_12;

	return (-det3_201_123 * mat[3][0] + det3_201_023 * mat[3][1] - det3_201_013 * mat[3][2] + det3_201_012 * mat[3][3]);
}

bool smMat4::InverseSelf(void) {
	// 84+4+16 = 104 multiplications
	//			   1 division
	smfloat64 det, invDet;

	// 2x2 sub-determinants required to calculate 4x4 determinant
	smfloat32 det2_01_01 = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
	smfloat32 det2_01_02 = mat[0][0] * mat[1][2] - mat[0][2] * mat[1][0];
	smfloat32 det2_01_03 = mat[0][0] * mat[1][3] - mat[0][3] * mat[1][0];
	smfloat32 det2_01_12 = mat[0][1] * mat[1][2] - mat[0][2] * mat[1][1];
	smfloat32 det2_01_13 = mat[0][1] * mat[1][3] - mat[0][3] * mat[1][1];
	smfloat32 det2_01_23 = mat[0][2] * mat[1][3] - mat[0][3] * mat[1][2];

	// 3x3 sub-determinants required to calculate 4x4 determinant
	smfloat32 det3_201_012 = mat[2][0] * det2_01_12 - mat[2][1] * det2_01_02 + mat[2][2] * det2_01_01;
	smfloat32 det3_201_013 = mat[2][0] * det2_01_13 - mat[2][1] * det2_01_03 + mat[2][3] * det2_01_01;
	smfloat32 det3_201_023 = mat[2][0] * det2_01_23 - mat[2][2] * det2_01_03 + mat[2][3] * det2_01_02;
	smfloat32 det3_201_123 = mat[2][1] * det2_01_23 - mat[2][2] * det2_01_13 + mat[2][3] * det2_01_12;

	det = (-det3_201_123 * mat[3][0] + det3_201_023 * mat[3][1] - det3_201_013 * mat[3][2] + det3_201_012 * mat[3][3]);

	if (Math::Fabs(det) < MATRIX_INVERSE_EPSILON) {
		return false;
	}

	invDet = 1.0f / det;

	// remaining 2x2 sub-determinants
	smfloat32 det2_03_01 = mat[0][0] * mat[3][1] - mat[0][1] * mat[3][0];
	smfloat32 det2_03_02 = mat[0][0] * mat[3][2] - mat[0][2] * mat[3][0];
	smfloat32 det2_03_03 = mat[0][0] * mat[3][3] - mat[0][3] * mat[3][0];
	smfloat32 det2_03_12 = mat[0][1] * mat[3][2] - mat[0][2] * mat[3][1];
	smfloat32 det2_03_13 = mat[0][1] * mat[3][3] - mat[0][3] * mat[3][1];
	smfloat32 det2_03_23 = mat[0][2] * mat[3][3] - mat[0][3] * mat[3][2];

	smfloat32 det2_13_01 = mat[1][0] * mat[3][1] - mat[1][1] * mat[3][0];
	smfloat32 det2_13_02 = mat[1][0] * mat[3][2] - mat[1][2] * mat[3][0];
	smfloat32 det2_13_03 = mat[1][0] * mat[3][3] - mat[1][3] * mat[3][0];
	smfloat32 det2_13_12 = mat[1][1] * mat[3][2] - mat[1][2] * mat[3][1];
	smfloat32 det2_13_13 = mat[1][1] * mat[3][3] - mat[1][3] * mat[3][1];
	smfloat32 det2_13_23 = mat[1][2] * mat[3][3] - mat[1][3] * mat[3][2];

	// remaining 3x3 sub-determinants
	smfloat32 det3_203_012 = mat[2][0] * det2_03_12 - mat[2][1] * det2_03_02 + mat[2][2] * det2_03_01;
	smfloat32 det3_203_013 = mat[2][0] * det2_03_13 - mat[2][1] * det2_03_03 + mat[2][3] * det2_03_01;
	smfloat32 det3_203_023 = mat[2][0] * det2_03_23 - mat[2][2] * det2_03_03 + mat[2][3] * det2_03_02;
	smfloat32 det3_203_123 = mat[2][1] * det2_03_23 - mat[2][2] * det2_03_13 + mat[2][3] * det2_03_12;

	smfloat32 det3_213_012 = mat[2][0] * det2_13_12 - mat[2][1] * det2_13_02 + mat[2][2] * det2_13_01;
	smfloat32 det3_213_013 = mat[2][0] * det2_13_13 - mat[2][1] * det2_13_03 + mat[2][3] * det2_13_01;
	smfloat32 det3_213_023 = mat[2][0] * det2_13_23 - mat[2][2] * det2_13_03 + mat[2][3] * det2_13_02;
	smfloat32 det3_213_123 = mat[2][1] * det2_13_23 - mat[2][2] * det2_13_13 + mat[2][3] * det2_13_12;

	smfloat32 det3_301_012 = mat[3][0] * det2_01_12 - mat[3][1] * det2_01_02 + mat[3][2] * det2_01_01;
	smfloat32 det3_301_013 = mat[3][0] * det2_01_13 - mat[3][1] * det2_01_03 + mat[3][3] * det2_01_01;
	smfloat32 det3_301_023 = mat[3][0] * det2_01_23 - mat[3][2] * det2_01_03 + mat[3][3] * det2_01_02;
	smfloat32 det3_301_123 = mat[3][1] * det2_01_23 - mat[3][2] * det2_01_13 + mat[3][3] * det2_01_12;

	mat[0][0] = -det3_213_123 * invDet;
	mat[1][0] = +det3_213_023 * invDet;
	mat[2][0] = -det3_213_013 * invDet;
	mat[3][0] = +det3_213_012 * invDet;

	mat[0][1] = +det3_203_123 * invDet;
	mat[1][1] = -det3_203_023 * invDet;
	mat[2][1] = +det3_203_013 * invDet;
	mat[3][1] = -det3_203_012 * invDet;

	mat[0][2] = +det3_301_123 * invDet;
	mat[1][2] = -det3_301_023 * invDet;
	mat[2][2] = +det3_301_013 * invDet;
	mat[3][2] = -det3_301_012 * invDet;

	mat[0][3] = -det3_201_123 * invDet;
	mat[1][3] = +det3_201_023 * invDet;
	mat[2][3] = -det3_201_013 * invDet;
	mat[3][3] = +det3_201_012 * invDet;

	return true;
}

bool smMat4::InverseFastSelf(void) {
	// 84+4+16 = 104 multiplications
	//			   1 division
	smfloat64 det, invDet;

	// 2x2 sub-determinants required to calculate 4x4 determinant
	smfloat32 det2_01_01 = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
	smfloat32 det2_01_02 = mat[0][0] * mat[1][2] - mat[0][2] * mat[1][0];
	smfloat32 det2_01_03 = mat[0][0] * mat[1][3] - mat[0][3] * mat[1][0];
	smfloat32 det2_01_12 = mat[0][1] * mat[1][2] - mat[0][2] * mat[1][1];
	smfloat32 det2_01_13 = mat[0][1] * mat[1][3] - mat[0][3] * mat[1][1];
	smfloat32 det2_01_23 = mat[0][2] * mat[1][3] - mat[0][3] * mat[1][2];

	// 3x3 sub-determinants required to calculate 4x4 determinant
	smfloat32 det3_201_012 = mat[2][0] * det2_01_12 - mat[2][1] * det2_01_02 + mat[2][2] * det2_01_01;
	smfloat32 det3_201_013 = mat[2][0] * det2_01_13 - mat[2][1] * det2_01_03 + mat[2][3] * det2_01_01;
	smfloat32 det3_201_023 = mat[2][0] * det2_01_23 - mat[2][2] * det2_01_03 + mat[2][3] * det2_01_02;
	smfloat32 det3_201_123 = mat[2][1] * det2_01_23 - mat[2][2] * det2_01_13 + mat[2][3] * det2_01_12;

	det = (-det3_201_123 * mat[3][0] + det3_201_023 * mat[3][1] - det3_201_013 * mat[3][2] + det3_201_012 * mat[3][3]);

	if (Math::Fabs(det) < MATRIX_INVERSE_EPSILON) {
		return false;
	}

	invDet = 1.0f / det;

	// remaining 2x2 sub-determinants
	smfloat32 det2_03_01 = mat[0][0] * mat[3][1] - mat[0][1] * mat[3][0];
	smfloat32 det2_03_02 = mat[0][0] * mat[3][2] - mat[0][2] * mat[3][0];
	smfloat32 det2_03_03 = mat[0][0] * mat[3][3] - mat[0][3] * mat[3][0];
	smfloat32 det2_03_12 = mat[0][1] * mat[3][2] - mat[0][2] * mat[3][1];
	smfloat32 det2_03_13 = mat[0][1] * mat[3][3] - mat[0][3] * mat[3][1];
	smfloat32 det2_03_23 = mat[0][2] * mat[3][3] - mat[0][3] * mat[3][2];

	smfloat32 det2_13_01 = mat[1][0] * mat[3][1] - mat[1][1] * mat[3][0];
	smfloat32 det2_13_02 = mat[1][0] * mat[3][2] - mat[1][2] * mat[3][0];
	smfloat32 det2_13_03 = mat[1][0] * mat[3][3] - mat[1][3] * mat[3][0];
	smfloat32 det2_13_12 = mat[1][1] * mat[3][2] - mat[1][2] * mat[3][1];
	smfloat32 det2_13_13 = mat[1][1] * mat[3][3] - mat[1][3] * mat[3][1];
	smfloat32 det2_13_23 = mat[1][2] * mat[3][3] - mat[1][3] * mat[3][2];

	// remaining 3x3 sub-determinants
	smfloat32 det3_203_012 = mat[2][0] * det2_03_12 - mat[2][1] * det2_03_02 + mat[2][2] * det2_03_01;
	smfloat32 det3_203_013 = mat[2][0] * det2_03_13 - mat[2][1] * det2_03_03 + mat[2][3] * det2_03_01;
	smfloat32 det3_203_023 = mat[2][0] * det2_03_23 - mat[2][2] * det2_03_03 + mat[2][3] * det2_03_02;
	smfloat32 det3_203_123 = mat[2][1] * det2_03_23 - mat[2][2] * det2_03_13 + mat[2][3] * det2_03_12;

	smfloat32 det3_213_012 = mat[2][0] * det2_13_12 - mat[2][1] * det2_13_02 + mat[2][2] * det2_13_01;
	smfloat32 det3_213_013 = mat[2][0] * det2_13_13 - mat[2][1] * det2_13_03 + mat[2][3] * det2_13_01;
	smfloat32 det3_213_023 = mat[2][0] * det2_13_23 - mat[2][2] * det2_13_03 + mat[2][3] * det2_13_02;
	smfloat32 det3_213_123 = mat[2][1] * det2_13_23 - mat[2][2] * det2_13_13 + mat[2][3] * det2_13_12;

	smfloat32 det3_301_012 = mat[3][0] * det2_01_12 - mat[3][1] * det2_01_02 + mat[3][2] * det2_01_01;
	smfloat32 det3_301_013 = mat[3][0] * det2_01_13 - mat[3][1] * det2_01_03 + mat[3][3] * det2_01_01;
	smfloat32 det3_301_023 = mat[3][0] * det2_01_23 - mat[3][2] * det2_01_03 + mat[3][3] * det2_01_02;
	smfloat32 det3_301_123 = mat[3][1] * det2_01_23 - mat[3][2] * det2_01_13 + mat[3][3] * det2_01_12;

	mat[0][0] = -det3_213_123 * invDet;
	mat[1][0] = +det3_213_023 * invDet;
	mat[2][0] = -det3_213_013 * invDet;
	mat[3][0] = +det3_213_012 * invDet;

	mat[0][1] = +det3_203_123 * invDet;
	mat[1][1] = -det3_203_023 * invDet;
	mat[2][1] = +det3_203_013 * invDet;
	mat[3][1] = -det3_203_012 * invDet;

	mat[0][2] = +det3_301_123 * invDet;
	mat[1][2] = -det3_301_023 * invDet;
	mat[2][2] = +det3_301_013 * invDet;
	mat[3][2] = -det3_301_012 * invDet;

	mat[0][3] = -det3_201_123 * invDet;
	mat[1][3] = +det3_201_023 * invDet;
	mat[2][3] = -det3_201_013 * invDet;
	mat[3][3] = +det3_201_012 * invDet;

	return true;
}

//TODO : [MATH] implement this
smcstring smMat4::ToString(smint32 precision) const {
	return "";
}

smMat4 smMat4::Perspective(smfloat32 fovRadius, smfloat32 aspectRation, smfloat32 nearClip, smfloat32 farClip)
{
	smfloat32 halfTanFov = Math::Tan(fovRadius * 0.5f);
	smMat4 outMatrix = smMat4_zero;

	outMatrix[0][0] = 1.0f / (aspectRation * halfTanFov);
	outMatrix[1][1] = 1.0f / halfTanFov;
	outMatrix[2][2] = -(farClip + nearClip) / (farClip - nearClip);
	outMatrix[2][3] = -1;
	outMatrix[3][2] = - 2 * nearClip * farClip / (farClip - nearClip);

	return outMatrix;
}

END_NAMESPACE