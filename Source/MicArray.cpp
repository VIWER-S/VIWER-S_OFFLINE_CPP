
#define _USE_MATH_DEFINES

#include <string>
#include <cmath>

#include "MicArray.h"
#include "Constants.h"

MicArray::MicArray(std::string name) :
	m_name {name}
{
	if (m_name.compare("MATRIXVOICE") == 0) {
		m_sensors = SENSORS_MATRIXVOICE;
		m_coordinates.resize(m_sensors);
		establishCoordinatesMatrixVoice();
	} else if (m_name.compare("BOOMERANG") == 0) {
		m_sensors = SENSORS_BOOMERANG;
		m_coordinates.resize(m_sensors);
		establishCoordinatesBoomerang();
	}
}

MicArray::MicArray() {
	MicArray(NAME);
}

std::string MicArray::getName() {
	return m_name;
}

std::vector<std::vector<double>> MicArray::getCoordinates() {
	return m_coordinates;
}

int MicArray::getNumSensors() {
	return m_sensors;
}

void MicArray::establishCoordinatesMatrixVoice() {

	//Generate Coordinates for MatrixVoice Array
	
	for (int iSensor = 0; iSensor < m_coordinates.size(); iSensor++) {
		m_coordinates.at(iSensor).resize(3, 0.0f);
	}

	double omega;

	for (int iSensor = 1; iSensor < m_sensors; iSensor++) {
		omega = -2.0f * M_PI * (iSensor - 1) / (m_sensors - 1) - M_PI;
		m_coordinates.at(iSensor).at(0) = M_PI * cos(omega) * 0.01f;
		m_coordinates.at(iSensor).at(1) = M_PI * sin(omega) * 0.01f;
		m_coordinates.at(iSensor).at(2) = 0.0f;
	}
}

void MicArray::establishCoordinatesBoomerang() {

	// Generate Coordinates for the Viwer Boomerang v.01

	m_coordinates.at(0) = {-0.11381f, -0.17055f, 0.0f};
	m_coordinates.at(1) = {-0.07911f, -0.13218f, 0.0f};
	m_coordinates.at(2) = {-0.052397f, -0.10086f, 0.0f};
	m_coordinates.at(3) = {-0.03225f, -0.074911f, 0.0f};
	m_coordinates.at(4) = {-0.017594f, -0.052949f, 0.0f};
	m_coordinates.at(5) = {-0.0076496f, -0.033807f, 0.0f};
	m_coordinates.at(6) = {-0.0018873f, -0.016465f, 0.0f};
	m_coordinates.at(7) = {0.0f, 0.0f, 0.0f};
	m_coordinates.at(8) = {-0.0018873f, 0.016465f, 0.0f};
	m_coordinates.at(9) = {-0.0076496f, 0.033807f, 0.0f};
	m_coordinates.at(10) = {-0.017594f, 0.052949f, 0.0f};
	m_coordinates.at(11) = {-0.03225f, 0.074911f, 0.0f};
	m_coordinates.at(12) = {-0.052397f, 0.10086f, 0.0f};
	m_coordinates.at(13) = {-0.07911f, 0.13218f, 0.0f};
	m_coordinates.at(14) = {-0.11381f, 0.17055f, 0.0f};

}