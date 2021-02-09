#pragma once

#include <string>
#include <vector>

class MicArray
{

public:

	MicArray::MicArray();
	MicArray::MicArray(std::string name);
	std::string MicArray::getName();
	std::vector<std::vector<double>> MicArray::getCoordinates();
	void MicArray::establishCoordinatesMatrixVoice();
	void MicArray::establishCoordinatesBoomerang();
	int MicArray::getNumSensors();

private:

	std::string m_name;
	std::vector<std::vector<double>> m_coordinates;
	int m_sensors;

	

};

