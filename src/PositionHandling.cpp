#include "PositionHandling.h"
#include "Exceptions.h"
#include "Log.h"
#include "Sequence.h"

#include <array>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <stdexcept>
#include <vector>

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
	calcTransMat();
}

auto PositionHandling::resetOriginAtEnd(Kinova::Objective targetObjective) -> bool {
	if (SequenceCounter < points[targetObjective - 1].size()) {
		false;
	}
	if (ZeroObjectives.count(targetObjective - 1)) {
		for (int i = 0; i < 6; i++) {
			location[targetObjective - 1][i] = 0;
		}
	}
	return true;
}

//Writes Requested targetCoordinates[6] of Objective by pos-number and its Subpositions by Sequence-number. Takes currentPosition[6] if Objective is 0.
//Returns 0 if sequence has ended.
auto PositionHandling::getCoordinates(Kinova::Objective targetObjective) -> Kinova::Coordinates {
	std::array<float, 6> targetCoordinates{};
	if (targetObjective == Kinova::NoObjective) {
		throw std::invalid_argument{"Cannot call GetCoordinates() for NoObjective"};
	}

	if (sequences.count(targetObjective) == 0) {
		throw Kinova::composeException<std::invalid_argument>("Objective does not have a sequence: ", targetObjective, ' ',
				Kinova::getObjectiveName(targetObjective));
	}

	//Check if current Position
	bool isZero = true;
	for (int i = 0; i < 6; i++) {
		if (points[targetObjective - 1][SequenceCounter][i] != 0) {
			ALL_LOG(logDEBUG3) << "PositionHandling::getCoordinates: " << "Points[" << targetObjective - 1 << "][" << SequenceCounter << "][" << i
					<< "] != 0";
			isZero = false;
		}
	}
	if (isZero) {
		for (int i = 0; i < 6; i++) {
			targetCoordinates[i] = location[targetObjective - 1][i];
		}
	} else {
		for (int i = 0; i < 6; i++) {
			targetCoordinates[i] = points[targetObjective - 1][SequenceCounter][i];
		}
		coordTransform(targetCoordinates.data(), TransMat[targetObjective - 1]);
	}
	ALL_LOG(logDEBUG4) << "PositionHandling::getCoordinates: " << "TargetCoordinates: (" << targetCoordinates[0] << ", " << targetCoordinates[1]
			<< ", " << targetCoordinates[2] << ", " << targetCoordinates[3] << ", " << targetCoordinates[4] << ", " << targetCoordinates[5] << ")";
	return Kinova::Coordinates{targetCoordinates};
}

/*Check if targetObjective is known ZeroObjective. Inserts currentCoordinates and recalcs TransMat at beginning of Sequence.*/
void PositionHandling::setZeroObjective(Kinova::Objective targetObjective, float *currentCoordinates) {
	if (ZeroObjectives.count(targetObjective - 1) && SequenceCounter == 0) {
		for (int i = 0; i < 6; i++) {
			location[targetObjective - 1][i] = currentCoordinates[i];
		}
		calcTransMat();
	}
}

auto PositionHandling::getOrigin(Kinova::Coordinates &targetCoordinates, Kinova::Objective targetObjective) -> bool {
	bool isZero = true;
	for (int i = 0; i < 6; i++) {
		if (location[targetObjective - 1][i] != 0) {
			isZero = false;
		}
	}
	if (isZero) {
		return false;
	} else {
		for (int i = 0; i < 6; i++) {
			targetCoordinates[i] = location[targetObjective - 1][i];
		}
		return true;
	}
}

void PositionHandling::incrementSequence() {
	++SequenceCounter;
}
void PositionHandling::decrementSequence() {
	--SequenceCounter;
}
void PositionHandling::resetSequence() {
	SequenceCounter = 0;
}

/*Saves coordinates to current Sequence Point or as Location in object. Returns true if successful.*/
bool PositionHandling::savePoint(float coordinates[6], Kinova::Objective targetObjective) {
	//check targetObjective
	if (targetObjective > 0 && targetObjective <= Kinova::NumberOfObjectives) {
		coordTransform(coordinates, InvTransMat[targetObjective - 1]);
//    std::vector<float> vec_coord(coordinates, coordinates + 6);
		std::vector<float> vec_coord(coordinates, coordinates + 6);
		//check SequenceCounter
		if (SequenceCounter == points[targetObjective - 1].size()) {
			points[targetObjective - 1].push_back(vec_coord);
			return true;
		} else if (SequenceCounter == -1) {
			points[targetObjective - 1].insert(points[targetObjective - 1].begin(), vec_coord);
			SequenceCounter = 0;
			return true;
		} else if (SequenceCounter >= 0 && SequenceCounter < points[targetObjective - 1].size()) {
			for (int i = 0; i < 6; i++) {
				points[targetObjective - 1][SequenceCounter][i] = coordinates[i];
			}
			return true;
		} else {
			ALL_LOG(logWARNING) << "PositionHandling::SavePoint: SequenceCounter is out of bound: " << SequenceCounter << " at SequenceSize off "
					<< points[targetObjective - 1].size();
			return false;
		}
	} else {
		ALL_LOG(logERROR) << "PositionHandling::SavePoint: targetObjective is out of bound: " << targetObjective;
		return false;
	}
}

/*Saves coordinates as origin of objectiv.*/
void PositionHandling::saveOrigin(float coordinates[6], Kinova::Objective targetObjective) {
	for (int i = 0; i < 6; i++) {
		location[targetObjective - 1][i] = coordinates[i];
	}
	calcTransMat();
}

void PositionHandling::deletePoint(Kinova::Objective targetObjective) {
	//check targetObjective and SequenceCounter
	if (targetObjective > 0 && targetObjective <= Kinova::NumberOfObjectives && SequenceCounter >= 0
			&& SequenceCounter < points[targetObjective - 1].size()) {
		//Delete Element
		points[targetObjective - 1].erase(points[targetObjective - 1].begin() + SequenceCounter);
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
			std::vector<int> vec_int;
			vec_int.reserve(6);
			while (iss >> n) {
				vec_int.push_back(n);
			}
			if (vec_int.size() == 0) {
				ALL_LOG(logERROR) << "PositionHandling::ReadFromFile: " << "File-Mismatch: No Objectives found";
				break;
			}
			bool isZero = true;
			for (int j = 0; j < 6; j++) {
				if (vec_int[j] != 0) {
					isZero = false;
				}
				location[i][j] = (float) vec_int[j] / 1000;
			}
			Kinova::Coordinates loadedCoordinates{location[i]};
			loadedOrigins.push_back(loadedCoordinates);
			if (isZero) {
				ZeroObjectives.insert(i);
				ALL_LOG(logDEBUG2) << "PositionHandling::ReadFromFile: " << "Objective " << i << " is Zero";
			}
		}
	}

	//Empty Line
	std::getline(in, line);

	//reset Point-Vector
	for (int i = 0; i < Kinova::NumberOfObjectives; i++) {
		f2d_vec_t(0, std::vector<float>(6)).swap(points[i]);
	}

	//Reads Points from Files
	int location = 0;
	int sequence = 0;
	//bool PrePoint = true;
	f2d_vec_t::iterator it = points[location].begin();

	while (std::getline(in, line) && location < Kinova::NumberOfObjectives) {
		ALL_LOG(logDEBUG4) << "PositionHandling::ReadFromFile: " << "Objective: " << location << ", Point: " << sequence;
		std::istringstream iss(line);
		int n;
		std::vector<int> vec_int;
		vec_int.reserve(6);
		while (iss >> n) {
			vec_int.push_back(n);
		}
		if (vec_int.size() == 0) {
			//start new Objective.
			location++;
			sequence = 0;
			it = points[location].begin();
			ALL_LOG(logDEBUG4) << "PositionHandling::ReadFromFile: " << "Empty line. Next Objective.";
		} else {
			//save Point
			std::vector<float> vec_float(6);
			for (int i = 0; i < 6; i++) {
				vec_float[i] = (float) vec_int[i] / 1000;
			}
			points[location].push_back(vec_float);
			loadedPoints[location].push_back(Kinova::Coordinates{vec_float});
			sequence++;
		}
	}

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
	std::ofstream saveFile(FILEPATH);
	if (saveFile.is_open()) {
		//Write Locations
		for (int i = 0; i < Kinova::NumberOfObjectives; i++) {
			if (ZeroObjectives.count(i)) {
				//Objecitve is a ZeroObjective.
				for (int j = 0; j < 6; j++) {
					saveFile << 0 << " ";
				}
			} else {
				//Normal Objective
				for (int j = 0; j < 6; j++) {
					saveFile << (int) (location[i][j] * 1000) << " ";
				}
			}
			saveFile << "\n";
		}
		saveFile << "\n";
		//Write Sequences
		for (int i = 0; i < Kinova::NumberOfObjectives; i++) {
			for (int j = 0; j < points[i].size(); j++) {
				for (int k = 0; k < 6; k++) {
					saveFile << (int) (points[i][j][k] * 1000) << " ";
				}
				saveFile << "\n";
			}
			saveFile << "\n";
		}
	} else {
		ALL_LOG(logERROR) << "PosiionHandling::writeToFile: Unable to open file.";
	}
	saveFile.close();
	ALL_LOG(logINFO) << "Point successfully saved to File.";
}

int PositionHandling::getSequence() {
	return SequenceCounter;
}

/*Calculate Transformation-Matrices for every Objectives coordinate system*/
void PositionHandling::calcTransMat() {
	double p[3], c[3], s[3];
	//Calculate Sinus and Cosinus of all angles
	for (int i = 0; i < Kinova::NumberOfObjectives; i++) {
		for (int j = 0; j < 3; j++) {
			p[j] = location[i][j];
			c[j] = cos(location[i][j + 3]);
			s[j] = sin(location[i][j + 3]);
		}

		//Hardcoded Euler XYZ Transformation Matrix
		TransMat[i][0][0] = c[1] * c[2];
		TransMat[i][0][1] = -c[1] * s[2];
		TransMat[i][0][2] = s[1];
		TransMat[i][0][3] = p[0];
		TransMat[i][1][0] = c[0] * s[2] + s[0] * c[2] * s[1];
		TransMat[i][1][1] = c[0] * c[2] - s[0] * s[2] * s[1];
		TransMat[i][1][2] = -s[0] * c[1];
		TransMat[i][1][3] = p[1];
		TransMat[i][2][0] = s[0] * s[2] - c[0] * c[2] * s[1];
		TransMat[i][2][1] = s[0] * c[2] + c[0] * s[2] * s[1];
		TransMat[i][2][2] = c[0] * c[1];
		TransMat[i][2][3] = p[2];
		TransMat[i][3][0] = 0;
		TransMat[i][3][1] = 0;
		TransMat[i][3][2] = 0;
		TransMat[i][3][3] = 1;

		//Hardcoded inverse of Euler XYZ Transformation Matrix
		//from [0][0] till [2][2] transformation matrix is rotational matrix 'R', as 'R' is orthagonal, R^-1=R^T
		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < 3; k++) {
				InvTransMat[i][j][k] = TransMat[i][k][j];
			}
		}
		InvTransMat[i][0][3] = -p[0] * c[1] * c[2] - p[1] * (c[0] * s[2] + s[0] * c[2] * s[1]) + p[2] * (c[0] * c[2] * s[1] - s[0] * s[2]);
		InvTransMat[i][1][3] = p[0] * c[1] * s[2] + p[1] * (s[0] * s[2] * s[1] - c[0] * c[2]) - p[2] * (s[0] * c[2] + c[0] * s[2] * s[1]);
		InvTransMat[i][2][3] = -p[0] * s[1] + p[1] * s[0] * c[1] - p[2] * c[0] * c[1];
		InvTransMat[i][3][0] = 0;
		InvTransMat[i][3][1] = 0;
		InvTransMat[i][3][2] = 0;
		InvTransMat[i][3][3] = 1;
	}
	ALL_LOG(logDEBUG) << "Transformation matrices calculated";
}
