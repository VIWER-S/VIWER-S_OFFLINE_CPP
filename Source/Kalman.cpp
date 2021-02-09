#include "Kalman.h"


Kalman::Kalman(){}

Kalman::Kalman(double dt, double pos) :
    m_dt(dt),
    m_estimate({pos, 0.0})
{
    m_A = { {1.0, m_dt}, {0.0, 1.0} };
    resetRemaining();
}

double Kalman::calculateDt(int hopsize, int samplingrate) {
    return ((double)hopsize) / samplingrate;
}

void Kalman::resetRemaining() {
    m_numBlocksRemaining = (int)(NUMSECONDSTILLDEATH / m_dt);
}

void Kalman::decreaseRemaining() {
    m_numBlocksRemaining--;
}

int Kalman::getNumBlocksRemaining() {
    return m_numBlocksRemaining;
}

double Kalman::iterate(double nNewMeasurement) {

    // Observed State
    makeObservation(nNewMeasurement);

    // State Covariance Update
    updateStateCovariance();

    // Kalman Gain
    calculateKalmanGain();

    // Process Error Update
    updateProcessError();

    // Estimation
    return estimate();

}

void Kalman::makeObservation(double nNewMeasurement) {

    m_observation = nNewMeasurement + m_observationError * m_distribution(m_generator);

}

void Kalman::updateStateCovariance() {

    m_stateCovariance = addTwoByTwo(multiplyTwoByTwoTransposed(multiplyTwoByTwo(m_A, m_processError), m_A), m_Q);

}

void Kalman::calculateKalmanGain() {

    double tmp = (m_stateCovariance.at(0).at(0) + m_observationError * m_observationError);
    m_kalmanGain.at(0) = m_stateCovariance.at(0).at(0) / tmp;
    m_kalmanGain.at(1) = m_stateCovariance.at(1).at(0) / tmp;

}

void Kalman::updateProcessError() {

    m_processError.at(0).at(0) = m_stateCovariance.at(0).at(0) - m_kalmanGain.at(0) * m_stateCovariance.at(0).at(0);
    m_processError.at(0).at(1) = m_stateCovariance.at(0).at(1) - m_kalmanGain.at(0) * m_stateCovariance.at(0).at(1);
    m_processError.at(1).at(0) = m_stateCovariance.at(1).at(0) - m_kalmanGain.at(1) * m_stateCovariance.at(0).at(0);
    m_processError.at(1).at(1) = m_stateCovariance.at(1).at(1) - m_kalmanGain.at(1) * m_stateCovariance.at(0).at(1);

}

double Kalman::estimate() {

    std::vector<double> tmp = { m_A.at(0).at(0) * m_estimate.at(0) + m_A.at(0).at(1) * m_estimate.at(1), m_A.at(1).at(0) * m_estimate.at(0) + m_A.at(1).at(1) * m_estimate.at(1) };

    double tmpDifference = m_observation - (m_A.at(0).at(0) * m_estimate.at(0) + m_A.at(0).at(1) * m_estimate.at(1));

    m_estimate.at(0) = tmp.at(0) + m_kalmanGain.at(0) * tmpDifference;
    m_estimate.at(1) = tmp.at(1) + m_kalmanGain.at(1) * tmpDifference;

    return m_estimate.at(0);

}

double Kalman::getEstimate() {
    return m_estimate.at(0);
}

std::vector<std::vector<double>> Kalman::newZeroMatrix(int nRows, int nCols) {
    std::vector<std::vector<double>> zero; 
    zero.resize(nRows);
    for (int iRow = 0; iRow < nCols; iRow++) {
        zero.at(iRow).resize(nCols, 0.0);
    }
    return zero;
}

std::vector<std::vector<double>> Kalman::addTwoByTwo(std::vector<std::vector<double>> a, std::vector<std::vector<double>> b) {

    // 2x2 Matrix Addition 

    std::vector<std::vector<double>> result = { {0.0f, 0.0f}, {0.0f, 0.0f} };

    result.at(0).at(0) = a.at(0).at(0) + b.at(0).at(0);
    result.at(0).at(1) = a.at(0).at(1) + b.at(0).at(1);
    result.at(1).at(0) = a.at(1).at(0) + b.at(1).at(0);
    result.at(1).at(1) = a.at(1).at(1) + b.at(1).at(1);

    return result;
}

std::vector<std::vector<double>> Kalman::multiplyTwoByTwo(std::vector<std::vector<double>> a, std::vector<std::vector<double>> b) {

    // 2x2 Matrix Multiplication 

    std::vector<std::vector<double>> result = { {0.0f, 0.0f}, {0.0f, 0.0f} };

    result.at(0).at(0) = a.at(0).at(0) * b.at(0).at(0) + a.at(0).at(1) * b.at(1).at(0);
    result.at(0).at(1) = a.at(0).at(0) * b.at(0).at(1) + a.at(0).at(1) * b.at(1).at(1);
    result.at(1).at(0) = a.at(1).at(0) * b.at(0).at(0) + a.at(1).at(1) * b.at(1).at(0);
    result.at(1).at(1) = a.at(1).at(0) * b.at(0).at(1) + a.at(1).at(1) * b.at(1).at(1);

    return result;
}

std::vector<std::vector<double>> Kalman::multiplyTwoByTwoTransposed(std::vector<std::vector<double>> a, std::vector<std::vector<double>> b) {

    // 2x2 Matrix Multiplication 

    std::vector<std::vector<double>> result = { {0.0f, 0.0f}, {0.0f, 0.0f} };

    result.at(0).at(0) = a.at(0).at(0) * b.at(0).at(0) + a.at(0).at(1) * b.at(0).at(1);
    result.at(0).at(1) = a.at(0).at(0) * b.at(1).at(0) + a.at(0).at(1) * b.at(1).at(1);
    result.at(1).at(0) = a.at(1).at(0) * b.at(0).at(0) + a.at(1).at(1) * b.at(0).at(1);
    result.at(1).at(1) = a.at(1).at(0) * b.at(1).at(0) + a.at(1).at(1) * b.at(1).at(1);

    return result;
}

