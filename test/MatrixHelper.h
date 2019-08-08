#ifndef TEST_MATRIXHELPER_H_
#define TEST_MATRIXHELPER_H_

#include "Matrix.h"
#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
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

inline std::ostream & operator<<(std::ostream & out, MatrixComparisonWrapper const & matrix) {
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

inline void assertMatrixEqual(f2d_vec_t const & expected, f2d_vec_t const & actual) {
	ASSERT_EQUAL(MatrixComparisonWrapper{expected}, MatrixComparisonWrapper{actual});
}

inline std::ostream & operator<<(std::ostream & out, VectorComparisonWrapper const & vector) {
	out << "Vector {\n";
		copy(begin(vector.data), end(vector.data), std::ostream_iterator<float>{out, ", "});
	out << "}\n";
	return out;
}


inline void assertVectorEqual(std::vector<float> const & expected, std::vector<float> const & actual) {
	ASSERT_EQUAL(VectorComparisonWrapper{expected}, VectorComparisonWrapper{actual});
}

template <std::size_t N>
void assertArrayEqual(std::array<float, N> const & expected, std::array<float, N> const & actual) {
	ASSERT_EQUAL(VectorComparisonWrapper{expected}, VectorComparisonWrapper{actual});
}

#endif /* TEST_MATRIXHELPER_H_ */
