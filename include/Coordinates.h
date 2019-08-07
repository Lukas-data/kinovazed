#ifndef KINOVA_COORDINATES_H_
#define KINOVA_COORDINATES_H_


#include <array>
#include <cstddef>
#include <sstream>
#include <stdexcept>


namespace Kinova {

struct Coordinates {
	Coordinates() = default;
	Coordinates(float x, float y, float z, float pitch, float yaw, float roll) :
			x{x}, y{y}, z{z}, pitch{pitch}, yaw{yaw}, roll{roll} {
		checkCooridateValues(x, y, z);
	}

	auto operator[](std::size_t index) -> float & {
		switch (index) {
		case 0: return x;
		case 1: return y;
		case 2: return z;
		case 3: return pitch;
		case 4: return yaw;
		case 5: return roll;
		}
		std::ostringstream errorMessage{"PosCoordinate index is out of range. Must be <6: "};
		errorMessage << index;
		throw std::invalid_argument{errorMessage.str()};
	}

	operator std::array<float, 6>() const {
		return {x, y, z, pitch, yaw, roll};
	}

	float x = 0.0f; //Kinova Left/Right(-)  0.8..-0.8
	float y = 0.0f; //Kinova Front(-)/Back  -0.85..0.85
	float z = 0.0f; //Kinova Up  1.15..
	float pitch = 0.0f;
	float yaw = 0.0f;
	float roll = 0.0f;

private:
	static void checkCooridateValues(float x, float y, float z) {
		if (x < -0.8f || x > 0.8) {
			std::ostringstream errorMessage{"Value of x is out of valid range [-0.8, 0.8]: "};
			errorMessage << x;
			throw std::invalid_argument{errorMessage.str()};
		}
		if (y < -0.85 || y > 0.85) {
			std::ostringstream errorMessage{"Value of y is out of valid range [-0.85, 0.85]: "};
			errorMessage << y;
			throw std::invalid_argument{errorMessage.str()};
		}
		if (z >= 1.15) {
			std::ostringstream errorMessage{"Value of z is out of valid range [?, 1.15]: "};
			errorMessage << z;
			throw std::invalid_argument{errorMessage.str()};
		}
	}
};

}

#endif /* KINOVA_COORDINATES_H_ */
