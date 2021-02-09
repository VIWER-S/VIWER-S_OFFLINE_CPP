
#define _USE_MATH_DEFINES

#include <cmath>

#include "Source.h"

Source::Source(double angle, double strength, double dt, double nSpread) :
    m_angle(angle),
    m_strength(strength),
    m_spread(nSpread),
    isNew(true)
{
    m_Kalman = Kalman(dt, m_angle);
    m_fadeCounter = (int) (FADEOUTLENGTH / dt);
    m_fadeFrames = m_fadeCounter;
}


std::vector<double> Source::iterate(std::vector<double> nCandidate) {

    isNew = false;
    m_blocksWithoutUpdate = 0;
    m_angle = (1.0 - m_spatialSmoothingConstant) * m_Kalman.iterate(nCandidate.at(0)) + m_spatialSmoothingConstant * m_angle;
    m_strength = (1.0 - m_spatialSmoothingConstant) * nCandidate.at(1) + m_spatialSmoothingConstant * m_strength;
    std::vector<double> result = { m_angle, m_strength };
    return result;

}

std::vector<double> Source::iterateWeighted(std::vector<std::vector<double>> candidates) {

    // Account for cyclic range 
    int nCandidates = candidates.size();

    std::vector<std::vector<double>> tempCandidates;
    tempCandidates.resize(3 * nCandidates);
    for (int iCandidate = 0; iCandidate < nCandidates; iCandidate++) {
        tempCandidates.at(iCandidate) = { candidates.at(iCandidate).at(0) - m_spread, candidates.at(iCandidate).at(1)};
        tempCandidates.at(iCandidate + nCandidates) = { candidates.at(iCandidate).at(0), candidates.at(iCandidate).at(1) };
        tempCandidates.at(iCandidate + 2 * nCandidates) = { candidates.at(iCandidate).at(0) + m_spread, candidates.at(iCandidate).at(1) };
    }

    std::vector<double> weights;
    weights.resize((int) 3 * nCandidates, 0.0);
    double nSumWeights = 0.0;
    double tmp = 0.0;
    double tmp_strength = 0.0;
    for (int iWeight = 0; iWeight < 3 * nCandidates; iWeight++) {
        tmp = pdf(tempCandidates.at(iWeight).at(0), m_angle, m_acceptanceProbability);
        weights.at(iWeight) = tmp;
        nSumWeights += tmp;
        tmp_strength += tempCandidates.at(iWeight).at(1);
    }

    double nEstiPos = 0.0;
    double nEstiStrength = 0.0;
    for (int iCandidate = 0; iCandidate < nCandidates; iCandidate++) {
        nEstiPos += candidates.at(iCandidate).at(0) * weights.at(iCandidate);
        nEstiStrength += candidates.at(iCandidate).at(1) * weights.at(iCandidate);
    }
    for (int iCandidate = 0; iCandidate < nCandidates; iCandidate++) {
        nEstiPos += candidates.at(iCandidate).at(0) * weights.at((int) iCandidate + nCandidates);
        nEstiStrength += candidates.at(iCandidate).at(1) * weights.at(iCandidate + nCandidates);
    }
    for (int iCandidate = 0; iCandidate < nCandidates; iCandidate++) {
        nEstiPos += candidates.at(iCandidate).at(0) * weights.at((int) iCandidate + 2 * nCandidates);
        nEstiStrength += candidates.at(iCandidate).at(1) * weights.at(iCandidate + 2 * nCandidates);
    }
    nEstiPos /= nSumWeights;
    nEstiStrength /= nSumWeights;

    m_angle = nEstiPos;
    m_strength = nEstiStrength;

    std::vector<double> result = { m_angle, m_strength };
    return result;
}

std::vector<double> Source::noUpdate() {
    m_blocksWithoutUpdate++;
    std::vector<double> estimate = { m_Kalman.getEstimate(), m_strength };
    return estimate;
}

std::vector<double> Source::fadeOut() {
    m_fadeCounter--;
    //m_strength *= m_alpha_strength;
    m_strength = getFadeFactor();
    std::vector<double> estimate = { m_Kalman.getEstimate(), m_strength };
    return estimate;
}

std::vector<double> Source::getSourceVector() {
    std::vector<double> result = { m_angle, m_strength };
    return result;
}

int Source::getFadeOutCounter() {
    return m_fadeCounter;
}

double Source::getStrength() {
    return m_strength;
}

bool Source::getIsNew() {
    return isNew;
}

int Source::getBlocksWithoutUpdate() {
    return m_blocksWithoutUpdate;
}

double Source::getAngle() {
    return m_Kalman.getEstimate();
}

void Source::setAngle(double angle) {
    m_angle = angle;
}

double Source::getPdf(double x) {
    return pdf(x, getAngle(), m_acceptanceProbability);
}

// return pdf(x) = standard Gaussian pdf
double Source::pdf(double x) {
    return exp(-x * x / 2) / sqrt(2 * M_PI);
}

// return pdf(x, mu, sigma) = Gaussian pdf with mean mu and stddev sigma
double Source::pdf(double x, double mu, double sigma) {
    return pdf((x - mu) / sigma) / sigma;
}

double Source::getFadeFactor() {
    return (1 + cos((m_fadeFrames - m_fadeCounter) / m_fadeFrames * M_PI)) * 0.5;
}

