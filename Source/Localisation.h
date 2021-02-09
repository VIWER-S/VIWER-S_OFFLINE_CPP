#pragma once

#include <vector>

#include <cmath>
#include <vector>
#include <memory>
#include <JuceHeader.h>

#include "Constants.h"
#include "SteeringVector.h"
#include "Loudness.h"
#include "FindPeaks.h"
#include "QuadraticInterpolation.h"



class Localisation
{

public:

    Localisation::Localisation();
    Localisation::~Localisation();
    std::vector<std::vector<std::vector<double>>> Localisation::generateEyeTimesComplex(int size, std::vector<double> complexNumber);
    double Localisation::getRealTrace(double matrix[SENSORS][SENSORS][BUFFER_SIZE / 2 + 1][2], int iFreq);
    std::vector<std::vector<std::vector<double>>> Localisation::getSteeringVectorDouble(int iTheta);
    int Localisation::getNumTheta();
    std::vector<std::vector<std::vector<double>>> Localisation::multiplayMatrices(std::vector<std::vector<std::vector<std::vector<double>>>> m1, std::vector<std::vector<std::vector<std::vector<double>>>> m2, int iFreq);
    std::vector<std::vector<double>> Localisation::sourceDirections_SRP(double spec[SENSORS][BUFFER_SIZE / 2 + 1][2]);
    std::vector<std::vector<double>> Localisation::sourceDirections_DU(double spec[SENSORS][BUFFER_SIZE / 2 + 1][2]);

private:

    int m_sensors;
    int m_sensorsSquare;
    int m_blockSize;
    int m_blockSizeHalf;
    double m_alpha;
    double m_oneMinusAlpha;
    int m_lowerBin;
    int m_upperBin;
    int m_maxTheta;
    int m_stepTheta;
    int m_numTheta;
    bool m_firstBlock = true;
    bool m_useImaginary = false;
    int m_peakImportance = 3;
    std::vector<double> m_thetaVector;
    std::vector<std::vector<std::vector<double>>> m_eye;
    std::vector<std::vector<std::vector<std::vector<double>>>> m_thetaTensor;
    std::vector<double> m_frequencies;
    std::vector<double> m_frequenciesFull;
    std::vector<double> m_pAbsSum;
    std::vector<std::vector<double>> m_delays;
    double m_psd[SENSORS][SENSORS][BUFFER_SIZE / 2 + 1][2];
    double m_firstMulti[SENSORS][BUFFER_SIZE / 2 + 1][2];

    SteeringVector m_SteeringVector;

};

