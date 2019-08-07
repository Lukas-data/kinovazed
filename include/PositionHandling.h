#ifndef _POSHANDLING_H_
#define _POSHANDLING_H_

#include "Coordinates.h"
#include "Matrix.h"

#include <vector>

namespace KinovaPts {

static constexpr int NumberOfObjectives = 8; //excl. NoObjective
enum Objective {
	NoObjective, Home, Bell, Handle, OpenDoor, PullDoor, PlaceCup, Antenna, AntennaPull
};

}

struct PositionHandling {
	PositionHandling() :
			location(KinovaPts::NumberOfObjectives, std::vector<float>(6)), points(KinovaPts::NumberOfObjectives, f2d_vec_t(0, std::vector<float>(6))), TransMat(KinovaPts::NumberOfObjectives, f2d_vec_t(4, std::vector<float>(4))), InvTransMat(
					KinovaPts::NumberOfObjectives, f2d_vec_t(4, std::vector<float>(4))) {
	}
	~PositionHandling();

	void init();
	auto getCoordinates(float *coordinates, KinovaPts::Objective targetObjective, float *currentCoordinates) -> bool;
	auto getOrigin(float *coordinates, KinovaPts::Objective targetObjective, float *currentCoordinates) -> bool;
	auto getOrigin(Kinova::Coordinates & targetCoordinates, KinovaPts::Objective targetObjective) -> bool;
	void incrementSequence();
	void decrementSequence();
	void resetSequence();
	void setZeroObjective(KinovaPts::Objective targetObjective, float *currentCoordinates);
	auto savePoint(float coordinates[6], KinovaPts::Objective targetObjective) -> bool;
	void saveOrigin(float coordinates[6], KinovaPts::Objective targetObjective);
	void deletePoint(KinovaPts::Objective targetObjective);
	auto getSequence() -> int;
	void writeToFile();

private:
	using Location = std::vector<Kinova::Coordinates>;
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

	void readFromFile();
	void loadData(std::istream & in);
	void calcTransMat();
};

#endif
