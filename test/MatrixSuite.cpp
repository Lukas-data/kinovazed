#include "MatrixSuite.h"
#include "cute.h"
#include "Matrix.h"

#include <algorithm>

#include <iterator>

constexpr float epsilon = 0.000001;

struct MatrixComparisonWrapper {
	explicit MatrixComparisonWrapper(f2d_vec_t data) : data{data}{}
	f2d_vec_t data;

	bool operator==(MatrixComparisonWrapper const & other) const {
		if (data.size() != other.data.size()) {
			return false;
		}
		for (auto rowIndex = 0u; rowIndex < data.size(); rowIndex++) {
			if (data[rowIndex].size() != other.data[rowIndex].size()) {
				return false;
			}
			for (auto colIndex = 0u; colIndex < data[rowIndex].size(); colIndex++) {
				if (fabs(data[rowIndex][colIndex] - other.data[rowIndex][colIndex]) > epsilon) {
					return false;
				}
			}
		}
		return true;
	}
};

std::ostream & operator<<(std::ostream & out, MatrixComparisonWrapper const & matrix) {
	out << "Matrix {\n";
	for (auto const & row : matrix.data) {
		out << "\t{";
		copy(begin(row), end(row), std::ostream_iterator<float>{out, ", "});
		out << "}\n";
	}
	out << "}\n";
	return out;
}

void testMatrixMultiplyEmptyMatrices() {
	f2d_vec_t lhs {};
	f2d_vec_t rhs {};
	f2d_vec_t expected {};
	ASSERT_EQUAL(expected, matMultiply(lhs, rhs));
}

void testMatrixMultiply() {
	f2d_vec_t lhs {
			{1.0, 2.0, 3.0, 4.0},
			{2.0, 3.0, 4.0, 5.0},
			{3.0, 4.0, 5.0, 6.0},
			{4.0, 5.0, 6.0, 7.0}
		};
	f2d_vec_t rhs {
			{0.0, 1.0,  2.0,  4.0},
			{1.0, 2.0,  4.0,  8.0},
			{2.0, 4.0,  8.0, 16.0},
			{4.0, 8.0, 16.0, 32.0}
		};
	f2d_vec_t expected {
			{24.0,  49.0,  98.0, 196.0},
			{31.0,  64.0, 128.0, 256.0},
			{38.0,  79.0, 158.0, 316.0},
			{45.0,  94.0, 188.0, 376.0}
	};
	ASSERT_EQUAL(expected, matMultiply(lhs, rhs));
}

void testRotationMatrixPosition1OfSequenceOpenDoor() {
	float angles[]{-0.027, -0.008, 0.713};
	f2d_vec_t expected {
		{ 0.756379,  -0.654085, -0.00799991, },
		{ 0.654031,   0.755986,  0.0269959,  },
		{-0.0116098, -0.0256513, 0.999604,   }
	};
	ASSERT_EQUAL(MatrixComparisonWrapper{expected}, MatrixComparisonWrapper{rotMatrix(angles)});
}

void testRotationMatrixPosition2OfSequenceOpenDoor() {
	float angles[]{-0.06, 0.192, 0.598};
	f2d_vec_t expected {
		{ 0.811277, -0.552645,  0.190823  },
		{ 0.552521,  0.831418,  0.0588621 },
		{-0.191183,  0.0576799, 0.979858  }
	};
	ASSERT_EQUAL(MatrixComparisonWrapper{expected}, MatrixComparisonWrapper{rotMatrix(angles)});
}

void testRotationMatrixPosition3OfSequenceOpenDoor() {
	float angles[]{-0.059, 0.069, -1.543};
	f2d_vec_t expected {
		{ 0.0277266,   0.997235, 0.0689453 },
		{-0.997987,   0.0236806, 0.0588255 },
		{ 0.0570302, -0.0704375, 0.995885  }
	};
	ASSERT_EQUAL(MatrixComparisonWrapper{expected}, MatrixComparisonWrapper{rotMatrix(angles)});
}

void testRotationMatrixPosition4OfSequenceOpenDoor() {
	float angles[]{-0.053, 0.055, -1.555};
	f2d_vec_t expected {
		{ 0.0157718,  0.998363,  0.0549723 },
		{-0.998517,   0.0128617, 0.0528951 },
		{ 0.0521015, -0.055725,  0.997086  }
	};
	ASSERT_EQUAL(MatrixComparisonWrapper{expected}, MatrixComparisonWrapper{rotMatrix(angles)});
}

void testRotationMatrixPosition5OfSequenceOpenDoor() {
	float angles[]{-0.006, 0.614, -0.106};
	f2d_vec_t expected {
		{ 0.812762,  0.0864769, 0.576141   },
		{-0.109237,  0.994004,  0.00490407 },
		{-0.572263, -0.0669219, 0.817335   }
	};
	ASSERT_EQUAL(MatrixComparisonWrapper{expected}, MatrixComparisonWrapper{rotMatrix(angles)});
}


void testRotationMatrixPosition6OfSequenceOpenDoor() {
	float angles[]{0.023, 0.216, -0.125};
	f2d_vec_t expected {
		{ 0.969142,  0.121778,    0.214324  },
		{-0.119751,  0.99255,    -0.0224636 },
		{-0.215463, -0.00389522,  0.976504  }
	};
	ASSERT_EQUAL(MatrixComparisonWrapper{expected}, MatrixComparisonWrapper{rotMatrix(angles)});
}


cute::suite make_suite_MatrixSuite() {
	cute::suite s { };
	s.push_back(CUTE(testMatrixMultiply));
	s.push_back(CUTE(testRotationMatrixPosition1OfSequenceOpenDoor));
	s.push_back(CUTE(testRotationMatrixPosition2OfSequenceOpenDoor));
	s.push_back(CUTE(testRotationMatrixPosition3OfSequenceOpenDoor));
	s.push_back(CUTE(testRotationMatrixPosition4OfSequenceOpenDoor));
	s.push_back(CUTE(testRotationMatrixPosition5OfSequenceOpenDoor));
	s.push_back(CUTE(testRotationMatrixPosition6OfSequenceOpenDoor));
	s.push_back(CUTE(testMatrixMultiplyEmptyMatrices));
	return s;
}
