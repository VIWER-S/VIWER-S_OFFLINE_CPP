#pragma once

#include <vector>
#include <random>

#define NUMSECONDSTILLDEATH 1


class Kalman
{

	public:

		Kalman::Kalman();
		Kalman::Kalman(double dt, double pos);
		static double Kalman::calculateDt(int hopsize, int samplingrate);
		void Kalman::resetRemaining();
		void Kalman::decreaseRemaining();
		int Kalman::getNumBlocksRemaining();
		double Kalman::iterate(double nNewMeasurement);
		void Kalman::makeObservation(double nNewMeasurement);
		void Kalman::updateStateCovariance();
		void Kalman::calculateKalmanGain();
		void Kalman::updateProcessError();
		double Kalman::estimate();
		double Kalman::getEstimate();

	private:

		std::vector<std::vector<double>> Kalman::newZeroMatrix(int nRows, int nCols);
		std::vector<std::vector<double>> Kalman::addTwoByTwo(std::vector<std::vector<double>> a, std::vector<std::vector<double>> b);
		std::vector<std::vector<double>> Kalman::multiplyTwoByTwo(std::vector<std::vector<double>> a, std::vector<std::vector<double>> b);
		std::vector<std::vector<double>> Kalman::multiplyTwoByTwoTransposed(std::vector<std::vector<double>> a, std::vector<std::vector<double>> b);

		int m_numBlocksRemaining;
		double m_dt;

		// MatrixVoice: 1e-3f
		double m_observationError = 1e-3f;
		double m_observation;
		std::vector<double> m_kalmanGain = { 0.0f, 0.0f };
		std::vector<double> m_estimate = { 0.0f, 0.0f };
		std::vector<std::vector<double>> m_A;
		std::vector<std::vector<double>> m_Q = { {0.1, 0.0}, {0.0, 0.1} }; // { {0.1, 0.0}, {0.0, 0.1} }
		// MatrixVoice: {{1.0f, 0.0f}, {0.0f, 1.0f}};
		std::vector<std::vector<double>> m_processError = { {1.0, 1.0}, {0.0, 1.0} }; // { {1.0, 1.0}, { 0.0, 1.0 } }
		std::vector<std::vector<double>> m_stateCovariance = { {0.0, 0.0}, {0.0, 0.0} }; // { {0.0, 0.0}, {0.0, 0.0} }

		std::default_random_engine m_generator;
		std::normal_distribution<double> m_distribution{ 1.0, 1.0 }; // { 1.0, 1.0 }

};

