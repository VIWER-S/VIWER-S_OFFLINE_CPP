/*
  ==============================================================================

    HRTF.cpp
    Created: 19 Nov 2020 5:17:05pm
    Author:  Ulrik

  ==============================================================================
*/

#include "HRTF.h"

HRTF::HRTF() {};

HRTF::HRTF(int num_theta_loc) :
    m_numThetaLoc(num_theta_loc) // number of equidistant directions during localisation
{
    readFile();

    m_theta_step = 360.0 / m_numDirections;
    m_thetaTransform = m_theta_step * m_numDirections / m_numThetaLoc;
    m_thetaFactor = m_numDirections / m_numThetaLoc;

}

HRTF::~HRTF() {}

void HRTF::readFile() {

    std::ifstream fileHRTF("I:/Development/ViwerS/data/hrtf.txt", std::ios_base::in);
    double tmp_real, tmp_imag, tmp, tmp_dir;

    //check to see that the file was opened correctly:
    if (!fileHRTF.is_open()) {
        DBG("There was a problem opening the input file!");

    }

    // First value in file is number of directions
    fileHRTF >> tmp;
    m_numDirections = (int)tmp;
    m_directions.resize(m_numDirections);
    fileHRTF >> tmp;
    m_nFFT = (int)tmp;

    // Read directions from file
    for (int iDir = 0; iDir < m_numDirections; iDir++) {
        fileHRTF >> tmp_dir;
        m_directions.at(iDir) = tmp_dir;
    }

    m_HRTF.resize(2);
    m_HRTF.at(0).resize(m_numDirections);
    m_HRTF.at(1).resize(m_numDirections);

    // Read left HRTF values from file
    for (int iDir = 0; iDir < m_numDirections; iDir++) {
        m_HRTF.at(0).at(iDir).resize(m_nFFT);
        for (int iBin = 0; iBin < m_nFFT; iBin++) {
            fileHRTF >> tmp_real;
            fileHRTF >> tmp_imag;

            m_HRTF.at(0).at(iDir).at(iBin) = { tmp_real, tmp_imag };
        }
    }

    // Read right HRTF values from file
    for (int iDir = 0; iDir < m_numDirections; iDir++) {
        m_HRTF.at(1).at(iDir).resize(m_nFFT);
        for (int iBin = 0; iBin < m_nFFT; iBin++) {
            fileHRTF >> tmp_real;
            fileHRTF >> tmp_imag;

            m_HRTF.at(1).at(iDir).at(iBin) = { tmp_real, tmp_imag };
        }
    }

    fileHRTF.close();

}

std::vector<std::vector<std::vector<double>>> HRTF::getHRTF(double direction) {

    // L/R, Bin, Real/Imag
    std::vector<std::vector<std::vector<double>>> tmp;// [BUFFER_SIZE / 2 + 1] [2] [2] ;
    tmp.resize(2);
    tmp.at(0).resize(m_nFFT);
    tmp.at(1).resize(m_nFFT);

    int* tmpIdx = getDirectionIndex(direction);
    int directionIdx[2];
    directionIdx[0] = *tmpIdx;
    directionIdx[1] = *(tmpIdx + 1);


    double factorOfFirst = getDirectionFactor(direction, directionIdx);

    for (int iBin = 0; iBin < BUFFER_SIZE/2+1; iBin++) {
        tmp.at(0).at(iBin).resize(2);
        tmp.at(1).at(iBin).resize(2);
        // Real, Left
        tmp.at(0).at(iBin).at(0) = factorOfFirst * m_HRTF.at(0).at(directionIdx[0]).at(iBin).at(0) + (1.0 - factorOfFirst) * m_HRTF.at(0).at(directionIdx[1]).at(iBin).at(0);
        // Imag, Left
        tmp.at(0).at(iBin).at(1) = factorOfFirst * m_HRTF.at(0).at(directionIdx[0]).at(iBin).at(1) + (1.0 - factorOfFirst) * m_HRTF.at(0).at(directionIdx[1]).at(iBin).at(1);
        // Real, Right
        tmp.at(1).at(iBin).at(0) = factorOfFirst * m_HRTF.at(1).at(directionIdx[0]).at(iBin).at(0) + (1.0 - factorOfFirst) * m_HRTF.at(1).at(directionIdx[1]).at(iBin).at(0);
        // Imag, Right
        tmp.at(1).at(iBin).at(1) = factorOfFirst * m_HRTF.at(1).at(directionIdx[0]).at(iBin).at(1) + (1.0 - factorOfFirst) * m_HRTF.at(1).at(directionIdx[1]).at(iBin).at(1);

    }

    return tmp;
}

int* HRTF::getDirectionIndex(double angle) {

    int minIdx = (int)(angle * m_thetaFactor);

    int tmp[2];
    if (minIdx == m_numTheta - 1) {
        // Cyclic value domain
        tmp[0] = m_numTheta - 1;
        tmp[1] = 0;
    }
    else {
        tmp[0] = minIdx;
        tmp[1] = minIdx + 1;
    }

    return tmp;
}

double HRTF::getDirectionFactor(double angle, int idx[2]) {

    return 1.0 - (angle * m_thetaTransform - (double) m_directions[idx[0]]) / m_theta_step;

}