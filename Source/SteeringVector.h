#pragma once

#define _USE_MATH_DEFINES

#include <vector>
#include <cmath>

#include "MicArray.h"

class SteeringVector
{

public:

	SteeringVector::SteeringVector();
	std::vector<std::vector<double>> SteeringVector::getCoordinates();
	std::vector<double> SteeringVector::calculateDelays(double theta);
	std::vector<std::vector<std::vector<std::vector<double>>>> SteeringVector::generateDelayTensor_DSB(std::vector<double> theta);
	std::vector<std::vector<std::vector<double>>> SteeringVector::generateDelayTensor_DSB(double theta);
	std::vector<double> SteeringVector::getFrequencies();
	std::vector<double> SteeringVector::getFrequenciesFull();
	double SteeringVector::calculateNorm(double array[]);
	int SteeringVector::getNumSensors();

private:

	int m_sensors;
	int m_blockSize;
	int m_blockSizeHalf;
	std::vector<double> m_frequencies;
	std::vector<double> m_frequenciesFull;
	double m_samplerateDividedByBlockSize;
	std::vector<std::vector<double>> m_coordinates;
	std::string m_arrayName;
	
	std::vector<std::vector<std::vector<std::vector<double>>>> m_theta;

	MicArray m_array;
};

