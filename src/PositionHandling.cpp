#include "PositionHandling.h"
#include "Exceptions.h"
#include "Log.h"
#include "Sequence.h"

#include <algorithm>

#include <array>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <stdexcept>
#include <vector>
#include <iostream>

#define FILEPATH "CybathlonObjectives.dat"

PositionHandling::PositionHandling(std::istream &in) :
		PositionHandling{} {
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

//Writes Requested targetCoordinates[6] of Objective by pos-number and its Subpositions by Sequence-number. Takes currentPosition[6] if Objective is 0.
//Returns 0 if sequence has ended.
auto PositionHandling::getCoordinates(Kinova::Objective targetObjective) const -> Kinova::Coordinates {

	if (targetObjective == Kinova::NoObjective) {
		throw std::invalid_argument{"Cannot call GetCoordinates() for NoObjective"};
	}

	if (sequences.count(targetObjective) == 0) {
		throw Kinova::composeException<std::invalid_argument>("Objective does not have a sequence: ", targetObjective, ' ',
				Kinova::getObjectiveName(targetObjective));
	}
	auto const sequence = sequences.at(targetObjective);
	//Check if current Position
	Kinova::Coordinates targetCoordinates = sequence.getCurrentCoordinates();

	if (targetCoordinates.isZero()) {
		targetCoordinates = sequence.getOrigin();
	} else {
		ALL_LOG(logDEBUG3) << "PositionHandling::getCoordinates: " << "Points[" << targetObjective - 1 << "][" << sequence.currentSequencePoint() << "] is not zero " << targetCoordinates;
		targetCoordinates = sequences.at(targetObjective).getTransformedCoordinates();
	}
	ALL_LOG(logDEBUG4) << "PositionHandling::getCoordinates: " << "TargetCoordinates: " << targetCoordinates;
	return targetCoordinates;
}

//auto PositionHandling::getSequence(Kinova::Objective targetObjective) const -> Kinova::Sequence {
//	return sequences.at(targetObjective);
//}

/*Check if targetObjective is known ZeroObjective. Inserts currentCoordinates and recalcs TransMat at beginning of Sequence.*/
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
		throw Kinova::composeException<std::out_of_range>("PositionHandling::getOrigin(): Cannot access origin of invalid objective ", targetObjective);
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
	std::cout << "PositionHandling::resetSequence() " << Kinova::ObjectiveNames[targetObjective] << std::endl;
	sequences.at(targetObjective).reset();
}

/*Saves coordinates to current Sequence Point or as Location in object. Returns true if successful.*/
bool PositionHandling::savePoint(Kinova::Coordinates coordinates, Kinova::Objective targetObjective) {
	//check targetObjective
	if (targetObjective <= 0 || targetObjective > Kinova::NumberOfObjectives) {
		ALL_LOG(logERROR) << "PositionHandling::SavePoint: targetObjective is out of bound: " << targetObjective;
		return false;
	}
	auto const result = sequences[targetObjective].savePoint(coordinates);
	if (!result) {
		ALL_LOG(logWARNING) << "PositionHandling::SavePoint: SequenceCounter is out of bound: " << sequences[targetObjective].currentSequencePoint() << " at SequenceSize off "
				<< sequences[targetObjective].numberOfPoints();
	}
	return result;
}

/*Saves coordinates as origin of objective.*/
void PositionHandling::saveOrigin(Kinova::Coordinates coordinates, Kinova::Objective targetObjective) {
	sequences.at(targetObjective).setOrigin(coordinates);
}

void PositionHandling::deletePoint(Kinova::Objective targetObjective) {
	//check targetObjective and SequenceCounter
	if (targetObjective != Kinova::NoObjective && Kinova::isValidObjective(targetObjective)) {
		sequences.at(targetObjective).deletePoint();
	}
}

void PositionHandling::loadData(std::istream &in) {
	ZeroObjectives.clear();
	std::vector<Kinova::Coordinates> loadedOrigins{};
	std::vector<std::vector<Kinova::Coordinates>> loadedPoints{Kinova::NumberOfObjectives};
	std::string line{};
	//Reads Objectives from Files
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
				copy(begin(coordinateData), end(coordinateData), std::ostream_iterator<float>{incompleteCoordinate, ", "});
				ALL_LOG(logERROR) << "PositionHandling::ReadFromFile: Incomplete coordinates " << incompleteCoordinate.str();
				break;
			}
			Kinova::Coordinates const loadedCoordinates{coordinateData};
			loadedOrigins.push_back(loadedCoordinates);
			if (loadedCoordinates.isZero()) {
				auto const objective = static_cast<Kinova::Objective>(i + 1);
				ZeroObjectives.insert(objective);
				ALL_LOG(logDEBUG2) << "PositionHandling::ReadFromFile: " << "Objective " << (i + 1) << " [" << Kinova::ObjectiveNames[objective] << "] is Zero";
			}
		}
	}

	//Empty Line
	std::getline(in, line);

	//Reads Points from Files
	int location = 0;
	int sequence = 0;
	//bool PrePoint = true;
//	f2d_vec_t::iterator it = points[location].begin();

	while (std::getline(in, line) && location < Kinova::NumberOfObjectives) {
		ALL_LOG(logDEBUG4) << "PositionHandling::ReadFromFile: " << "Objective: " << location << ", Point: " << sequence;
		std::istringstream iss(line);
		int n;
		std::vector<float> coordinateData;
		coordinateData.reserve(6);
		while (iss >> n) {
			coordinateData.push_back(static_cast<float>(n) / 1000.0f);
		}
		if (coordinateData.size() == 0) {
			//start new Objective.
			location++;
			sequence = 0;
			ALL_LOG(logDEBUG4) << "PositionHandling::ReadFromFile: " << "Empty line. Next Objective.";
		} else {
			//save Point
			loadedPoints[location].push_back(Kinova::Coordinates{coordinateData});
			sequence++;
		}
	}

	sequences[Kinova::NoObjective] = Kinova::Sequence{};
	for (auto index = 0; index < loadedOrigins.size(); index++) {
		sequences[static_cast<Kinova::Objective>(index + 1)] = Kinova::Sequence{loadedOrigins.at(index), loadedPoints.at(index)};
	}
}

/*Reads Location and Points Vectors from SaveFile*/
void PositionHandling::readFromFile() {
	std::ifstream infile(FILEPATH);
	loadData(infile);
	ALL_LOG(logINFO) << "Points successfully loaded from File.";
}

/*Writes Location and Points Vectors to SaveFile*/
void PositionHandling::writeToFile() {
//	std::ofstream saveFile(FILEPATH);
//	if (saveFile.is_open()) {
//		//Write Locations
//		for (int i = 0; i < Kinova::NumberOfObjectives; i++) {
//			if (ZeroObjectives.count(i)) {
//				//Objecitve is a ZeroObjective.
//				for (int j = 0; j < 6; j++) {
//					saveFile << 0 << " ";
//				}
//			} else {
//				//Normal Objective
//				for (int j = 0; j < 6; j++) {
//					saveFile << (int) (location[i][j] * 1000) << " ";
//				}
//			}
//			saveFile << "\n";
//		}
//		saveFile << "\n";
//		//Write Sequences
//		for (int i = 0; i < Kinova::NumberOfObjectives; i++) {
//			for (int j = 0; j < points[i].size(); j++) {
//				for (int k = 0; k < 6; k++) {
//					saveFile << (int) (points[i][j][k] * 1000) << " ";
//				}
//				saveFile << "\n";
//			}
//			saveFile << "\n";
//		}
//	} else {
//		ALL_LOG(logERROR) << "PosiionHandling::writeToFile: Unable to open file.";
//	}
//	saveFile.close();
//	ALL_LOG(logINFO) << "Point successfully saved to File.";
}

auto PositionHandling::getSequence(Kinova::Objective targetObjective) const -> int {
	return static_cast<int>(sequences.at(targetObjective).currentSequencePoint());
}

