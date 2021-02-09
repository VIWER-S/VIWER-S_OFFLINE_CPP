
#define _USE_MATH_DEFINES

#include <cmath>
#include <vector>

#include "SteeringVector.h"
#include "Constants.h"

//extern MicArray m_array;

SteeringVector::SteeringVector() {	
	m_blockSize = BUFFER_SIZE;
	m_blockSizeHalf = BUFFER_SIZE / 2 + 1;
	m_arrayName = NAME;
	m_samplerateDividedByBlockSize = ((double) SAMPLERATE) / m_blockSize;

	// Generate frequency vector over onesided spectrum
	m_frequencies.resize(m_blockSizeHalf, 0);
	for (int iFreq = 0; iFreq < m_blockSizeHalf; iFreq++) {
		m_frequencies.at(iFreq) = iFreq * m_samplerateDividedByBlockSize;
	}

	// Generate frequency vector over all frequencies
	m_frequenciesFull.resize(m_blockSize, 0);
	for (int iFreq = 0; iFreq < m_blockSize; iFreq++) {
		m_frequenciesFull.at(iFreq) = iFreq * m_samplerateDividedByBlockSize;
	}

	m_array = MicArray(m_arrayName);
	m_sensors = m_array.getNumSensors();
	m_coordinates = m_array.getCoordinates();
}

int SteeringVector::getNumSensors() {
	return m_array.getNumSensors();
}

std::vector<std::vector<double>> SteeringVector::getCoordinates() {
	return m_coordinates;
}

std::vector<double> SteeringVector::calculateDelays(double theta) {

	std::vector<double> delays;
	delays.resize(m_sensors, 0.0f);

	for (int iSensor = 0; iSensor < m_sensors; iSensor++) {
		delays.at(iSensor) = (m_coordinates[iSensor][0] * sin(theta) + m_coordinates[iSensor][1] * cos(theta)) / SPEED_OF_SOUND;
	}

	return delays;
}

std::vector<std::vector<std::vector<std::vector<double>>>> SteeringVector::generateDelayTensor_DSB(std::vector<double> thetaVector) {

	// Sensors, Bins, Real/Imag
	std::vector<std::vector<std::vector<std::vector<double>>>> thetaTensor;
	thetaTensor.resize(thetaVector.size());
	for (int iTheta = 0; iTheta < thetaVector.size(); iTheta++) {
		thetaTensor.at(iTheta).resize(m_sensors);

		// Calculation of Delays
		std::vector<double> tauTheta = calculateDelays(thetaVector.at(iTheta));

		for (int iSensor = 0; iSensor < m_sensors; iSensor++) {
			thetaTensor.at(iTheta).at(iSensor).resize(m_blockSizeHalf);
			for (int iBin = 0; iBin < m_blockSizeHalf; iBin++) {
				thetaTensor.at(iTheta).at(iSensor).at(iBin).resize(2);
				thetaTensor.at(iTheta).at(iSensor).at(iBin).at(0) = cos(-2.0f * M_PI * m_frequencies.at(iBin) * tauTheta.at(iSensor));
				thetaTensor.at(iTheta).at(iSensor).at(iBin).at(1) = sin(-2.0f * M_PI * m_frequencies.at(iBin) * tauTheta.at(iSensor));
			}
		}
	}

	return thetaTensor;
}

std::vector<std::vector<std::vector<double>>> SteeringVector::generateDelayTensor_DSB(double theta) {

	// Sensors, Bins, Real/Imag
	std::vector<std::vector<std::vector<double>>> tmp_theta;
	tmp_theta.resize(SENSORS);// [BUFFER_SIZE / 2 + 1] [2] ;

	// Calculation of Delays
	std::vector<double> v_tau_theta = calculateDelays(theta);

	// Creation of Steering Vector
	for (int iSensor = 0; iSensor < SENSORS; iSensor++) {
		tmp_theta.at(iSensor).resize(BUFFER_SIZE/2+1);
		for (int iFreq = 0; iFreq < BUFFER_SIZE/2+1; iFreq++) {
			tmp_theta.at(iSensor).at(iFreq).resize(2);
			tmp_theta.at(iSensor).at(iFreq).at(0) = cos(-2.0 * M_PI * m_frequencies.at(iFreq) * v_tau_theta.at(iSensor));
			tmp_theta.at(iSensor).at(iFreq).at(1) = sin(-2.0 * M_PI * m_frequencies.at(iFreq) * v_tau_theta.at(iSensor));
		}
	}

	return tmp_theta;
}

std::vector<double> SteeringVector::getFrequencies() {
	return m_frequencies;
}

std::vector<double> SteeringVector::getFrequenciesFull() {
	return m_frequenciesFull;
}

double SteeringVector::calculateNorm(double inArray[]) {
	return sqrt(inArray[0] * inArray[0] + inArray[1] * inArray[1] + inArray[2] * inArray[2]);
}
