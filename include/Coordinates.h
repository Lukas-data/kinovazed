#ifndef KINOVA_COORDINATES_H_
#define KINOVA_COORDINATES_H_

#include "Exceptions.h"
#include "nlohmann/json.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace Kinova {


struct Coordinates {
	Coordinates() = default;
	Coordinates(float x, float y, float z, float pitch, float yaw, float roll)
	    : x{x}
	    , y{y}
	    , z{z}
	    , pitch{pitch}
	    , yaw{yaw}
	    , roll{roll} {
		checkCooridateValues(x, y, z);
	}

	explicit Coordinates(std::vector<float> const &data)
	    : Coordinates{data[0], data[1], data[2], data[3], data[4], data[5]} {
	}

	explicit Coordinates(std::array<float, 6> const &data)
	    : Coordinates{data[0], data[1], data[2], data[3], data[4], data[5]} {
	}

	auto operator[](std::size_t index) -> float & {
		switch (index) {
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		case 3:
			return pitch;
		case 4:
			return yaw;
		case 5:
			return roll;
		}
		std::ostringstream errorMessage{};
		errorMessage << "PosCoordinate index is out of range. Must be <6: " << index;
		throw std::invalid_argument{errorMessage.str()};
	}

	operator std::array<float, 6>() const {
		return {x, y, z, pitch, yaw, roll};
	}

	auto isZero() const -> bool {
		auto isZero = [](auto const &location) { return std::fabs(location) < epsilon; };
		std::array<float, 6> values = *this;
		return std::all_of(begin(values), end(values), isZero);
	}

	friend void from_json(nlohmann::json const &j, Coordinates &c);
	friend void to_json(nlohmann::json &j, Coordinates const &c);

	constexpr static float epsilon = 0.00001f;

	float x = 0.0f; // Kinova Left/Right(-)  0.8..-0.8
	float y = 0.0f; // Kinova Front(-)/Back  -0.85..0.85
	float z = 0.0f; // Kinova Up  1.15..
	float pitch = 0.0f;
	float yaw = 0.0f;
	float roll = 0.0f;

  private:
	static void checkCooridateValues(float x, float y, float z) {
		if (x < -0.8f || x > 0.8) {
			throw composeException<std::invalid_argument>("Value of x is out of valid range [-0.8, 0.8]: ", x);
		}
		if (y < -0.85 || y > 0.85) {
			throw composeException<std::invalid_argument>("Value of y is out of valid range [-0.85, 0.85]: ", y);
		}
		if (z >= 1.15) {
			throw Kinova::composeException<std::invalid_argument>("Value of z is out of valid range [?, 1.15]: ", z);
		}
	}
};

static auto isEqualFloat(float lhs, float rhs) {
	return std::fabs(lhs - rhs) < Coordinates::epsilon;
}

inline auto operator==(Coordinates const &lhs, Coordinates const &rhs) -> bool {
	return isEqualFloat(lhs.x, rhs.x) && isEqualFloat(lhs.y, rhs.y) && isEqualFloat(lhs.z, rhs.z) &&
	    isEqualFloat(lhs.pitch, rhs.pitch) && isEqualFloat(lhs.yaw, rhs.yaw) && isEqualFloat(lhs.roll, rhs.roll);
}

inline auto operator!=(Coordinates const &lhs, Coordinates const &rhs) -> bool {
	return !(lhs == rhs);
}

inline auto operator<<(std::ostream &out, Coordinates const &coordinates) -> std::ostream & {
	out << "Coordinates{x=" << coordinates.x << ", y=" << coordinates.y << ", z=" << coordinates.z
	    << ", pitch=" << coordinates.pitch << ", yaw=" << coordinates.yaw << ", roll=" << coordinates.roll << '}';
	return out;
}

} // namespace Kinova

#endif /* KINOVA_COORDINATES_H_ */
