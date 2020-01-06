#include "PositionHandling.h"

#include "Constants.h"
#include "Exceptions.h"
#include "Paths.h"
#include "Sequence.h"
#include "nlohmann/json.hpp"
#include "spdlog/fmt/ostr.h"
#include "spdlog/spdlog.h"

#include <algorithm>
#include <array>
#include <fstream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

PositionHandling::PositionHandling(std::istream &in)
    : PositionHandling{} {
	loadData(in);
}

PositionHandling::~PositionHandling() {
	writeToFile();
}

void PositionHandling::init() {
	readFromFile();
}

auto PositionHandling::resetOriginAtEnd(Kinova::Objective targetObjective) -> bool {
	if (!sequences[targetObjective].endReached()) {
		return false;
	}
	if (ZeroObjectives.count(targetObjective)) {
		sequences[targetObjective].resetOrigin();
	}
	return true;
}

// Writes Requested targetCoordinates[6] of Objective by pos-number and its Subpositions by Sequence-number. Takes
// currentPosition[6] if Objective is 0. Returns 0 if sequence has ended.
auto PositionHandling::getCoordinates(Kinova::Objective targetObjective) const -> Kinova::Coordinates {

	if (targetObjective == Kinova::NoObjective) {
		throw std::invalid_argument{"Cannot call GetCoordinates() for NoObjective"};
	}

	if (sequences.count(targetObjective) == 0) {
		throw Kinova::composeException<std::invalid_argument>(
		    "Objective does not have a sequence: ", targetObjective, ' ', Kinova::getObjectiveName(targetObjective));
	}
	auto const sequence = sequences.at(targetObjective);
	// Check if current Position
	Kinova::Coordinates targetCoordinates = sequence.getCurrentCoordinates();

	if (targetCoordinates.isZero()) {
		targetCoordinates = sequence.getOrigin();
	} else {
		spdlog::debug("PositionHandling::getCoordinates: Points[{0}][{1}] is not zero {2}",
		              targetObjective - 1,
		              sequence.currentSequencePoint(),
		              targetCoordinates);
		targetCoordinates = sequences.at(targetObjective).getTransformedCoordinates();
	}
	spdlog::debug("PositionHandling::getCoordinates: TargetCoordinates: {0}", targetCoordinates);
	return targetCoordinates;
}

// auto PositionHandling::getSequence(Kinova::Objective targetObjective) const -> Kinova::Sequence {
//	return sequences.at(targetObjective);
//}

/*Check if targetObjective is known ZeroObjective. Inserts currentCoordinates and recalcs TransMat at beginning of
 * Sequence.*/
void PositionHandling::setZeroObjective(Kinova::Coordinates currentCoordinates, Kinova::Objective targetObjective) {
	if (ZeroObjectives.count(targetObjective) && sequences.at(targetObjective).currentSequencePoint() == 0) {
		sequences.at(targetObjective).setOrigin(currentCoordinates);
	}
}

auto PositionHandling::hasOrigin(Kinova::Objective targetObjective) const -> bool {
	if (targetObjective == Kinova::NoObjective || !isValidObjective(targetObjective)) {
		return false;
	}
	return !sequences.at(targetObjective).getOrigin().isZero();
}

auto PositionHandling::getOrigin(Kinova::Objective targetObjective) const -> Kinova::Coordinates {
	if (targetObjective == Kinova::NoObjective || !isValidObjective(targetObjective)) {
		throw Kinova::composeException<std::out_of_range>(
		    "PositionHandling::getOrigin(): Cannot access origin of invalid objective ", targetObjective);
	}
	return sequences.at(targetObjective).getOrigin();
}

void PositionHandling::incrementSequence(Kinova::Objective targetObjective) {
	sequences.at(targetObjective).nextPoint();
}
void PositionHandling::decrementSequence(Kinova::Objective targetObjective) {
	sequences.at(targetObjective).previousPoint();
}
void PositionHandling::resetSequence(Kinova::Objective targetObjective) {
	sequences.at(targetObjective).reset();
}

/*Saves coordinates to current Sequence Point or as Location in object. Returns true if successful.*/
bool PositionHandling::savePoint(Kinova::Coordinates coordinates, Kinova::Objective targetObjective) {
	// check targetObjective
	if (targetObjective <= 0 || targetObjective > Kinova::NumberOfObjectives) {
		spdlog::error("PositionHandling::SavePoint: targetObjective is out of bound: {0}", targetObjective);
		return false;
	}
	auto const result = sequences[targetObjective].savePoint(coordinates);
	if (!result) {
		spdlog::warn("PositionHandling::SavePoint: SequenceCounter is out of bound: {0} at SequenceSize off {1}",
		             sequences[targetObjective].currentSequencePoint(),
		             sequences[targetObjective].numberOfPoints());
	}
	return result;
}

/*Saves coordinates as origin of objective.*/
void PositionHandling::saveOrigin(Kinova::Coordinates coordinates, Kinova::Objective targetObjective) {
	sequences.at(targetObjective).setOrigin(coordinates);
}

void PositionHandling::deletePoint(Kinova::Objective targetObjective) {
	// check targetObjective and SequenceCounter
	if (targetObjective != Kinova::NoObjective && Kinova::isValidObjective(targetObjective)) {
		sequences.at(targetObjective).deletePoint();
	}
}

void PositionHandling::loadData(std::istream &in) {
	ZeroObjectives.clear();

	// TODO replace with in and remove code afert objectives have been loaded
	std::ifstream file{Paths::DEFAULT_OBJ_FILE_JSON};

	nlohmann::json root{};
	file >> root;
	objectives = root.get<std::vector<Kinova::JSONObjective>>();

	std::vector<Kinova::Coordinates> loadedOrigins{};
	std::vector<std::vector<Kinova::Coordinates>> loadedPoints{Kinova::NumberOfObjectives};
	std::string line{};
	// Reads Objectives from Files
	for (int i = 0; i < Kinova::NumberOfObjectives; i++) {
		if (std::getline(in, line)) {
			std::istringstream iss(line);
			int n;
			std::vector<float> coordinateData{};
			coordinateData.reserve(6);
			while (iss >> n) {
				coordinateData.push_back(static_cast<float>(n) / 1000.0f);
			}
			if (coordinateData.size() < 6) {
				std::ostringstream incompleteCoordinate{};
				copy(begin(coordinateData),
				     end(coordinateData),
				     std::ostream_iterator<float>{incompleteCoordinate, ", "});
				spdlog::error("PositionHandling::ReadFromFile: Incomplete coordinates {0}", incompleteCoordinate.str());
				break;
			}
			Kinova::Coordinates const loadedCoordinates{coordinateData};
			loadedOrigins.push_back(loadedCoordinates);
			if (loadedCoordinates.isZero()) {
				auto const objective = static_cast<Kinova::Objective>(i + 1);
				ZeroObjectives.insert(objective);
				spdlog::debug("PositionHandling::ReadFromFile: Objective {0} [{1}] is Zero",
				              (i + 1),
				              Kinova::ObjectiveNames[objective]);
			}
		}
	}

	// Empty Line
	std::getline(in, line);

	// Reads Points from Files
	int location = 0;
	int sequence = 0;
	// bool PrePoint = true;
	//	f2d_vec_t::iterator it = points[location].begin();

	while (std::getline(in, line) && location < Kinova::NumberOfObjectives) {
		spdlog::debug("PositionHandling::ReadFromFile: Objective: {0}, Point: {1}", location, sequence);
		std::istringstream iss(line);
		int n;
		std::vector<float> coordinateData;
		coordinateData.reserve(6);
		while (iss >> n) {
			coordinateData.push_back(static_cast<float>(n) / 1000.0f);
		}
		if (coordinateData.size() == 0) {
			// start new Objective.
			location++;
			sequence = 0;
			spdlog::debug("PositionHandling::ReadFromFile: Empty line. Next Objective.");
		} else {
			// save Point
			loadedPoints[location].push_back(Kinova::Coordinates{coordinateData});
			sequence++;
		}
	}

	sequences[Kinova::NoObjective] = Kinova::Sequence{};
	for (auto index = 0u; index < loadedOrigins.size(); index++) {
		sequences[static_cast<Kinova::Objective>(index + 1)] =
		    Kinova::Sequence{loadedOrigins.at(index), loadedPoints.at(index)};
	}
}

/*Reads Location and Points Vectors from SaveFile*/
void PositionHandling::readFromFile() {
	std::ifstream infile(Paths::DEFAULT_OBJ_FILE_DAT);
	loadData(infile);
	spdlog::info("Points successfully loaded from File.");
}

/*Writes Location and Points Vectors to SaveFile*/
void PositionHandling::writeToFile() {
	std::ofstream saveFile(Paths::DEFAULT_OBJ_FILE_DAT);
	if (saveFile.is_open()) {
		// Write Locations
		for (int i = 0; i < Kinova::NumberOfObjectives; i++) {
			auto const objective = static_cast<Kinova::Objective>(i + 1);
			if (ZeroObjectives.count(objective)) {
				for (int j = 0; j < 6; j++) {
					saveFile << 0 << " ";
				}
			} else {
				std::array<float, 6> origin = sequences[objective].getOrigin();
				transform(begin(origin), end(origin), std::ostream_iterator<int>{saveFile, " "}, [](float component) {
					return static_cast<int>(component * 1000);
				});
			}
			saveFile << "\n";
		}
		saveFile << "\n";
		// Write Sequences
		for (int i = 0; i < Kinova::NumberOfObjectives; i++) {
			auto const objective = static_cast<Kinova::Objective>(i + 1);
			auto sequence = sequences[objective];
			sequence.reset();
			while (!sequence.endReached()) {
				std::array<float, 6> coordinate = sequence.getCurrentCoordinates();
				transform(begin(coordinate),
				          end(coordinate),
				          std::ostream_iterator<int>{saveFile, " "},
				          [](float component) { return static_cast<int>(component * 1000); });
				saveFile << "\n";
				sequence.nextPoint();
			}
			saveFile << "\n";
		}
	} else {
		spdlog::error("PosiionHandling::writeToFile: Unable to open file.");
	}
	saveFile.close();
	spdlog::info("Point successfully saved to File.");
}

auto PositionHandling::getSequence(Kinova::Objective targetObjective) const -> int {
	return static_cast<int>(sequences.at(targetObjective).currentSequencePoint());
}
