#ifndef _POSHANDLING_H_
#define _POSHANDLING_H_

#include "Coordinates.h"
#include "Matrix.h"
#include "Objective.h"
#include "Sequence.h"

#include <map>
#include <set>
#include <vector>


struct PositionHandling {
	explicit PositionHandling(std::istream & in);
	PositionHandling() :
			location(Kinova::NumberOfObjectives, std::vector<float>(6)), points(Kinova::NumberOfObjectives, f2d_vec_t(0, std::vector<float>(6))), TransMat(Kinova::NumberOfObjectives, f2d_vec_t(4, std::vector<float>(4))), InvTransMat(
					Kinova::NumberOfObjectives, f2d_vec_t(4, std::vector<float>(4))) {
	}
	~PositionHandling();

	void init();
	auto getCoordinates(float *coordinates, Kinova::Objective targetObjective) -> bool;
	auto getOrigin(Kinova::Coordinates & targetCoordinates, Kinova::Objective targetObjective) -> bool;
	void incrementSequence();
	void decrementSequence();
	void resetSequence();
	void setZeroObjective(Kinova::Objective targetObjective, float *currentCoordinates);
	auto savePoint(float coordinates[6], Kinova::Objective targetObjective) -> bool;
	void saveOrigin(float coordinates[6], Kinova::Objective targetObjective);
	void deletePoint(Kinova::Objective targetObjective);
	auto getSequence() -> int;
	void writeToFile();

	auto getLocations() const {
		return location;
	}

	auto getPoints() const {
		return points;
	}

	auto getSequences() const {
		return sequences;
	}

private:
	using f2d_vec_t = std::vector<std::vector<float>>;
	using f3d_vec_t = std::vector<f2d_vec_t>;
	std::map<Kinova::Objective, Kinova::Sequence> sequences{};

	f2d_vec_t location;
	f3d_vec_t points;
	int SequenceCounter{0};

	f3d_vec_t TransMat;
	f3d_vec_t InvTransMat;
	std::set<int> ZeroObjectives{};

	void readFromFile();
	void loadData(std::istream & in);
	void calcTransMat();
};

#endif
