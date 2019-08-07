#include "MatrixSuite.h"
#include "cute.h"
#include "Matrix.h"

#include <algorithm>

#include <iterator>
#include <array>
#include <cstddef>
#include <vector>

constexpr float epsilon = 0.00001;

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

struct VectorComparisonWrapper {
	explicit VectorComparisonWrapper(std::vector<float> data) : data{data}{}
	template <std::size_t N>
	explicit VectorComparisonWrapper(std::array<float, N> data) : data{begin(data), end(data)}{}
	std::vector<float> data;

	bool operator==(VectorComparisonWrapper const & other) const {
		return equal(begin(data), end(data), begin(other.data), end(other.data), [](auto lhs, auto rhs) {
			return fabs(lhs - rhs) < epsilon;
		});
	}
};

std::ostream & operator<<(std::ostream & out, VectorComparisonWrapper const & vector) {
	out << "Vector {\n";
		copy(begin(vector.data), end(vector.data), std::ostream_iterator<float>{out, ", "});
	out << "}\n";
	return out;
}

void testMatrixMultiplyEmptyMatrices() {
	f2d_vec_t const lhs{};
	f2d_vec_t const rhs{};
	f2d_vec_t const expected {};
	ASSERT_EQUAL(expected, matMultiply(lhs, rhs));
}

void testMatrixMultiply() {
	f2d_vec_t const lhs{
			{1.0, 2.0, 3.0, 4.0},
			{2.0, 3.0, 4.0, 5.0},
			{3.0, 4.0, 5.0, 6.0},
			{4.0, 5.0, 6.0, 7.0}
		};
	f2d_vec_t const rhs{
			{0.0, 1.0,  2.0,  4.0},
			{1.0, 2.0,  4.0,  8.0},
			{2.0, 4.0,  8.0, 16.0},
			{4.0, 8.0, 16.0, 32.0}
		};
	f2d_vec_t const expected {
			{24.0,  49.0,  98.0, 196.0},
			{31.0,  64.0, 128.0, 256.0},
			{38.0,  79.0, 158.0, 316.0},
			{45.0,  94.0, 188.0, 376.0}
	};
	ASSERT_EQUAL(expected, matMultiply(lhs, rhs));
}

void testMatrixMultiplyDifferentSizes() {
	f2d_vec_t const lhs{
		{1.0, 2.0, 3.0, 4.0},
		{5.0, 6.0, 7.0, 8.0}
	};
	f2d_vec_t const rhs{
		{1.0, 2.0},
		{3.0, 4.0},
		{5.0, 6.0},
		{7.0, 8.0}
	};
	f2d_vec_t const expected {
		{ 50.0,  60.0},
		{114.0, 140.0}
	};
	ASSERT_EQUAL(expected, matMultiply(lhs, rhs));
}

void assertMatrixEqual(f2d_vec_t const & expected, f2d_vec_t const & actual) {
	ASSERT_EQUAL(MatrixComparisonWrapper{expected}, MatrixComparisonWrapper{actual});
}

void testRotationMatrixPosition1OfSequenceOpenDoor() {
	std::array<float, 3> angles{{-0.027, -0.008, 0.713}};
	f2d_vec_t expected {
		{ 0.756379,  -0.654085, -0.00799991, },
		{ 0.654031,   0.755986,  0.0269959,  },
		{-0.0116098, -0.0256513, 0.999604,   }
	};
	assertMatrixEqual(expected, rotMatrix(angles.data()));
}

void testRotationMatrixPosition2OfSequenceOpenDoor() {
	std::array<float, 3> angles{{-0.06, 0.192, 0.598}};
	f2d_vec_t expected {
		{ 0.811277, -0.552645,  0.190823  },
		{ 0.552521,  0.831418,  0.0588621 },
		{-0.191183,  0.0576799, 0.979858  }
	};
	assertMatrixEqual(expected, rotMatrix(angles.data()));
}

void testRotationMatrixPosition3OfSequenceOpenDoor() {
	std::array<float, 3> angles{{-0.059, 0.069, -1.543}};
	f2d_vec_t expected {
		{ 0.0277266,   0.997235, 0.0689453 },
		{-0.997987,   0.0236806, 0.0588255 },
		{ 0.0570302, -0.0704375, 0.995885  }
	};
	assertMatrixEqual(expected, rotMatrix(angles.data()));
}

void testRotationMatrixPosition4OfSequenceOpenDoor() {
	std::array<float, 3> angles{{-0.053, 0.055, -1.555}};
	f2d_vec_t expected {
		{ 0.0157718,  0.998363,  0.0549723 },
		{-0.998517,   0.0128617, 0.0528951 },
		{ 0.0521015, -0.055725,  0.997086  }
	};
	assertMatrixEqual(expected, rotMatrix(angles.data()));
}

void testRotationMatrixPosition5OfSequenceOpenDoor() {
	std::array<float, 3> angles{-0.006, 0.614, -0.106};
	f2d_vec_t expected {
		{ 0.812762,  0.0864769, 0.576141   },
		{-0.109237,  0.994004,  0.00490407 },
		{-0.572263, -0.0669219, 0.817335   }
	};
	assertMatrixEqual(expected, rotMatrix(angles.data()));
}


void testRotationMatrixPosition6OfSequenceOpenDoor() {
	std::array<float, 3> angles{{0.023, 0.216, -0.125}};
	f2d_vec_t expected {
		{ 0.969142,  0.121778,    0.214324  },
		{-0.119751,  0.99255,    -0.0224636 },
		{-0.215463, -0.00389522,  0.976504  }
	};
	assertMatrixEqual(expected, rotMatrix(angles.data()));
}

void assertVectorEqual(std::vector<float> const & expected, std::vector<float> const & actual) {
	ASSERT_EQUAL(VectorComparisonWrapper{expected}, VectorComparisonWrapper{actual});
}

void testGetEulerAnglesPosition1OfSequenceOpenDoor() {
	f2d_vec_t const rotationMatrix {
		{0.345612, -0.370787,  0.862015  },
		{0.660426, -0.556479, -0.504151  },
		{0.666625,  0.743538,  0.0525519 }
	};
	std::vector<float> const expected{1.46693, 1.03923, 0.820525};
	assertVectorEqual(expected, getEulerAngles(rotationMatrix));
}

void testGetEulerAnglesPosition2OfSequenceOpenDoor() {
	f2d_vec_t const rotationMatrix {
		{0.220324, -0.250443,  0.942728  },
		{0.797602, -0.510097, -0.321918  },
		{0.561505,  0.822848,  0.0873672 }

	};
	std::vector<float> const expected{1.30578, 1.23072, 0.849289};
	assertVectorEqual(expected, getEulerAngles(rotationMatrix));
}

void testGetEulerAnglesPosition3OfSequenceOpenDoor() {
	f2d_vec_t const rotationMatrix {
		{ 0.094172,   0.433854,  0.896048  },
		{-0.00195328, 0.900127, -0.435623  },
		{-0.995554,   0.0392733, 0.0856142 }
	};
	std::vector<float> const expected{1.37674, 1.11079, -1.35705};
	assertVectorEqual(expected, getEulerAngles(rotationMatrix));
}

void testGetEulerAnglesPosition4OfSequenceOpenDoor() {
	f2d_vec_t const rotationMatrix {
		{ 0.0839724,  0.447508,  0.890329  },
		{-0.00987216, 0.893814, -0.448329  },
		{-0.996419,   0.0288578, 0.0794736 }
	};
	std::vector<float> const expected{1.39535, 1.09807, -1.38531};
	assertVectorEqual(expected, getEulerAngles(rotationMatrix));
}

void testGetEulerAnglesPosition5OfSequenceOpenDoor() {
	f2d_vec_t const rotationMatrix {
		{-0.0888857, -0.0458622, 0.994985  },
		{  0.989982,  0.10596,   0.0933228 },
		{-0.109708,   0.993312,  0.0359845 }
	};
	std::vector<float> const expected{-1.20277, 1.47061, 2.66525};
	assertVectorEqual(expected, getEulerAngles(rotationMatrix));
}

void testGetEulerAnglesPosition6OfSequenceOpenDoor() {
	f2d_vec_t const rotationMatrix {
		{ 0.298574, 0.0263837, 0.954022   },
		{ 0.948221, 0.105242, -0.299669   },
		{-0.10831,  0.994097,  0.00640495 }
	};
	std::vector<float> const expected{1.54943, 1.26638, -0.0881369};
	assertVectorEqual(expected, getEulerAngles(rotationMatrix));
}

void testGetEulerAnglesForMaximumBetaRotation() {
	f2d_vec_t const rotationMatrix {
		{ 0.298574, 0.0263837, 1.0   },
		{ 0.948221, 0.105242, -0.299669   },
		{-0.10831,  0.994097,  0.00640495 }
	};
	std::vector<float> const expected{1.46026, 1.5708, 0};
	assertVectorEqual(expected, getEulerAngles(rotationMatrix));
}

void testGetEulerAnglesForMinumumBetaRotation() {
	f2d_vec_t const rotationMatrix {
		{ 0.298574, 0.0263837, -1.0   },
		{ 0.948221, 0.105242, -0.299669   },
		{-0.10831,  0.994097,  0.00640495 }
	};
	std::vector<float> const expected{-1.46026, -1.5708, 0};
	assertVectorEqual(expected, getEulerAngles(rotationMatrix));
}

template <std::size_t N>
void assertArrayEqual(std::array<float, N> const & expected, std::array<float, N> const & actual) {
	ASSERT_EQUAL(VectorComparisonWrapper{expected}, VectorComparisonWrapper{actual});
}

f2d_vec_t const openDoorTransformationMatrix {
	{0.498557, -0.0310022,   0.866302,  0.212972 },
	{0.866681, -0.00230676, -0.498857, -0.271539 },
	{0.017464,  0.999517,    0.025719,  0.491391 },
	{0.0,       0.0,         0.0,       1.0 }
};

void testCoordTransformPosition1OfSequenceOpenDoor() {
	std::array<float, 6> expected{0.211228, -0.274157, 0.499335, 1.46724, 1.03749, 0.820169};
	std::array<float, 6> coordinates{-0.003, 0.008, 0, -0.027, -0.008, 0.713};
	coordTransform(coordinates.data(), openDoorTransformationMatrix);
	assertArrayEqual(expected, coordinates);
}

void testCoordTransformPosition2OfSequenceOpenDoor() {
	std::array<float, 6> expected{0.0253932, -0.201899, 0.50074, 1.30707, 1.22903, 0.847925};
	std::array<float, 6> coordinates{-0.033, 0.015, -0.197, -0.06, 0.192, 0.598};
	coordTransform(coordinates.data(), openDoorTransformationMatrix);
	assertArrayEqual(expected, coordinates);
}

void testCoordTransformPosition3OfSequenceOpenDoor() {
	std::array<float, 6> expected{ 0.0169657, -0.231357, 0.485404, 1.37741, 1.10907, -1.35781 };
	std::array<float, 6> coordinates{ -0.063, 0, -0.19, -0.059, 0.069, -1.543};
	coordTransform(coordinates.data(), openDoorTransformationMatrix);
	assertArrayEqual(expected, coordinates);
}

void testCoordTransformPosition4OfSequenceOpenDoor() {
	std::array<float, 6> expected{ 0.0126352, -0.238613, 0.568293, 1.39595, 1.09635, -1.385981 };
	std::array<float, 6> coordinates{ -0.07, 0.083, -0.188, -0.053, 0.055, -1.555 };
	coordTransform(coordinates.data(), openDoorTransformationMatrix);
	assertArrayEqual(expected, coordinates);
}

void testCoordTransformPosition5OfSequenceOpenDoor() {
	std::array<float, 6> expected{ 0.0209449, 0.0357301, 0.516158, -1.19642, 1.47223, 2.65887 };
	std::array<float, 6> coordinates{ 0.171, 0.03, -0.319, -0.006, 0.614, -0.106 };
	coordTransform(coordinates.data(), openDoorTransformationMatrix);
	assertArrayEqual(expected, coordinates);
}

void testCoordTransformPosition6OfSequenceOpenDoor() {
	std::array<float, 6> expected{ 0.266373, -0.110374, 0.302327, 1.54954, 1.26463, -0.0882606 };
	std::array<float, 6> coordinates{ 0.163, -0.191, -0.039, 0.023, 0.216, -0.125 };
	coordTransform(coordinates.data(), openDoorTransformationMatrix);
	assertArrayEqual(expected, coordinates);
}

cute::suite make_suite_MatrixSuite() {
	cute::suite s { };
	s.push_back(CUTE(testMatrixMultiplyEmptyMatrices));
	s.push_back(CUTE(testMatrixMultiply));
	s.push_back(CUTE(testMatrixMultiplyDifferentSizes));
	s.push_back(CUTE(testRotationMatrixPosition1OfSequenceOpenDoor));
	s.push_back(CUTE(testRotationMatrixPosition2OfSequenceOpenDoor));
	s.push_back(CUTE(testRotationMatrixPosition3OfSequenceOpenDoor));
	s.push_back(CUTE(testRotationMatrixPosition4OfSequenceOpenDoor));
	s.push_back(CUTE(testRotationMatrixPosition5OfSequenceOpenDoor));
	s.push_back(CUTE(testRotationMatrixPosition6OfSequenceOpenDoor));
	s.push_back(CUTE(testGetEulerAnglesPosition1OfSequenceOpenDoor));
	s.push_back(CUTE(testGetEulerAnglesPosition2OfSequenceOpenDoor));
	s.push_back(CUTE(testGetEulerAnglesPosition3OfSequenceOpenDoor));
	s.push_back(CUTE(testGetEulerAnglesPosition4OfSequenceOpenDoor));
	s.push_back(CUTE(testGetEulerAnglesPosition5OfSequenceOpenDoor));
	s.push_back(CUTE(testGetEulerAnglesPosition6OfSequenceOpenDoor));
	s.push_back(CUTE(testGetEulerAnglesForMaximumBetaRotation));
	s.push_back(CUTE(testGetEulerAnglesForMinumumBetaRotation));
	s.push_back(CUTE(testCoordTransformPosition1OfSequenceOpenDoor));
	s.push_back(CUTE(testCoordTransformPosition2OfSequenceOpenDoor));
	s.push_back(CUTE(testCoordTransformPosition3OfSequenceOpenDoor));
	s.push_back(CUTE(testCoordTransformPosition4OfSequenceOpenDoor));
	s.push_back(CUTE(testCoordTransformPosition5OfSequenceOpenDoor));
	s.push_back(CUTE(testCoordTransformPosition6OfSequenceOpenDoor));
	return s;
}
