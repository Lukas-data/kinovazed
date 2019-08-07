#ifndef _POSHANDLING_H_
#define _POSHANDLING_H_

#include "Matrix.h"

#include <array>
#include <cstddef>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace KinovaPts {

static constexpr int NumberOfObjectives = 8; //excl. NoObjective
enum Objective {
	NoObjective, Home, Bell, Handle, OpenDoor, PullDoor, PlaceCup, Antenna, AntennaPull
};

struct PosCoordinate {
	PosCoordinate() = default;
	PosCoordinate(float x, float y, float z, float pitch, float yaw, float roll) :
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
		if (z < 1.15) {
			std::ostringstream errorMessage{"Value of z is out of valid range [1.15, ?]: "};
			errorMessage << z;
			throw std::invalid_argument{errorMessage.str()};
		}
	}
};
}

struct PositionHandling {
	PositionHandling() :
			location(KinovaPts::NumberOfObjectives, std::vector<float>(6)), points(KinovaPts::NumberOfObjectives, f2d_vec_t(0, std::vector<float>(6))), TransMat(KinovaPts::NumberOfObjectives, f2d_vec_t(4, std::vector<float>(4))), InvTransMat(
					KinovaPts::NumberOfObjectives, f2d_vec_t(4, std::vector<float>(4))) {
	}
	~PositionHandling();

	void init();
	static int printPos();
	bool getCoordinates(float *coordinates, KinovaPts::Objective targetObjective, float *currentCoordinates);
	bool getOrigin(float *coordinates, KinovaPts::Objective targetObjective, float *currentCoordinates);
	auto getOrigin(KinovaPts::PosCoordinate & targetCoordinates, KinovaPts::Objective targetObjective) -> bool;
	void incrementSequence();
	void decrementSequence();
	void resetSequence();
	void setZeroObjective(KinovaPts::Objective targetObjective, float *currentCoordinates);
	bool savePoint(float coordinates[6], KinovaPts::Objective targetObjective);
	void saveOrigin(float coordinates[6], KinovaPts::Objective targetObjective);
	void deletePoint(KinovaPts::Objective targetObjective);
	int getSequence();
	void readFromFile();
	void writeToFile();

private:
	using Location = std::vector<KinovaPts::PosCoordinate>;
	using LocationSequence = std::vector<Location>;
	//static KinovaPts::posCoordinates Locations[KinovaPts::NumberOfObjectives][NUMBER_OF_SUBPOINTS];
	using f2d_vec_t = std::vector<std::vector<float>>;
	using f3d_vec_t = std::vector<f2d_vec_t>;
	f2d_vec_t location;
	f3d_vec_t points;
	int SequenceCounter{0};

	f3d_vec_t TransMat;
	f3d_vec_t InvTransMat;
	std::vector<int> ZeroObjectives;

	void calcTransMat();
};

#endif
