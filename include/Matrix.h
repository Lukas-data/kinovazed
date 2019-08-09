#ifndef KINOVA_MATRIX_H_
#define KINOVA_MATRIX_H_

#include "Coordinates.h"

#include <array>
#include <cmath>
#include <vector>

using f2d_vec_t = std::vector<std::vector<float>>;

constexpr float halfPi = 1.5708;

/*Multiplies two matrices and returns resulting matrix.*/
inline auto matMultiply(const f2d_vec_t &mat1, const f2d_vec_t &mat2) -> f2d_vec_t {
	if (mat1.empty()) {
		return {};
	}
	int d1 = mat1.size();
	int d2 = mat1[0].size();
	int d3 = mat2[0].size();

	f2d_vec_t ans(d1, std::vector<float>(d3, 0));
	for (int i = 0; i < d1; i++) {
		for (int j = 0; j < d3; j++) {
			for (int k = 0; k < d2; k++) {
				ans[i][j] += mat1[i][k] * mat2[k][j];
			}
		}
	}
	return ans;
}

/*takes Array of 3 Angles alpha, beta, gamma and returns RotationMatrix of Euler XYZ*/
inline auto rotMatrix(float angle[3]) -> f2d_vec_t {
	f2d_vec_t mat(3, std::vector<float>(3, 0));
	double c[3], s[3];
	for (int i = 0; i < 3; i++) {
		c[i] = std::cos(angle[i]);
		s[i] = std::sin(angle[i]);
	}
	//Hardcoded RotationMatrix of Euler XYZ
	mat[0][0] = c[1] * c[2];
	mat[0][1] = -c[1] * s[2];
	mat[0][2] = s[1];
	mat[1][0] = c[0] * s[2] + s[0] * c[2] * s[1];
	mat[1][1] = c[0] * c[2] - s[0] * s[2] * s[1];
	mat[1][2] = -s[0] * c[1];
	mat[2][0] = s[0] * s[2] - c[0] * c[2] * s[1];
	mat[2][1] = s[0] * c[2] + c[0] * s[2] * s[1];
	mat[2][2] = c[0] * c[1];
	return mat;
}

/*returns Euler Angles of Euler XYZ rotational matrix.*/
inline auto getEulerAngles(const f2d_vec_t rotMat) -> std::vector<float> {
	std::vector<float> res(3);
	if (rotMat[0][2] < 1) {
		if (rotMat[0][2] > -1) {
			res[0] = std::atan2(-rotMat[1][2], rotMat[2][2]);
			res[1] = std::asin(rotMat[0][2]);
			res[2] = std::atan2(-rotMat[0][1], rotMat[0][0]);
		} else { // <= -1
			res[0] = -std::atan2(rotMat[1][0], rotMat[1][1]);
			res[1] = -halfPi;
			res[2] = 0;
		}
	} else { //>= 1
		res[0] = std::atan2(rotMat[1][0], rotMat[1][1]);
		res[1] = halfPi;
		res[2] = 0;
	}
	return res;
}

/*Transforms coordinates Objective coordinate system to Basis coordinate system*/
inline auto coordTransform(float *coordinates, const f2d_vec_t &transMat) -> Kinova::Coordinates {
	//define point Vector ([x;y;z;1]) and angle Vector ([alpha, beta, gamma])
	f2d_vec_t cord(4, std::vector<float>(1, 0));
	float ang[3];
	for (int i = 0; i < 3; i++) {
		cord[i][0] = coordinates[i];
		ang[i] = coordinates[i + 3];
	}
	cord[3][0] = 1;
	//Multiply Point Vector with TransformationMatrix
	cord = matMultiply(transMat, cord);

	//Get rotational Matrix from Objective in Base Coordinate System. (extracted from inverse transformation Matrix
	f2d_vec_t R1(3, std::vector<float>(3, 0));
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			R1[i][j] = transMat[i][j];
		}
	}
	//Get rotational Matrix from Point angles in Objective coordinate system.
	f2d_vec_t R2 = rotMatrix(ang);
	//Calculate rotational Matrix from Point in Objective Coordinate System
	f2d_vec_t R3 = matMultiply(R1, R2);
	std::vector<float> angles = getEulerAngles(R3);
	//write coordinates
	for (int i = 0; i < 3; i++) {
		coordinates[i] = cord[i][0];
		coordinates[i + 3] = angles[i];
	}
	return {coordinates[0], coordinates[1], coordinates[2], coordinates[3], coordinates[4], coordinates[5]};
}

inline auto coordTransform(Kinova::Coordinates const & coordinates, f2d_vec_t const & transformationMatrix) -> Kinova::Coordinates {
	std::array<float, 6> data = coordinates;
	return coordTransform(data.data(), transformationMatrix);
}


#endif /* KINOVA_MATRIX_H_ */
