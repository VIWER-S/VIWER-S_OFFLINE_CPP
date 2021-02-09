/*
  ==============================================================================

    HRTF.h
    Created: 19 Nov 2020 5:17:05pm
    Author:  Ulrik

  ==============================================================================
*/

#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include "Constants.h"
#include "JuceHeader.h"
//#include "../data/HRTF_data.h"

class HRTF {

public:

    HRTF::HRTF();
    HRTF::HRTF(int num_theta_loc);
    HRTF::~HRTF();
    void HRTF::readFile();
    std::vector<std::vector<std::vector<double>>> HRTF::getHRTF(double direction);
    int* HRTF::getDirectionIndex(double angle);
    double HRTF::getDirectionFactor(double angle, int idx[]);
    int HRTF::getNumDirections();
    int HRTF::getFFTSize();

private:


    int m_numTheta = 0;
    int m_numThetaLoc = 0;
    int m_channels = 2;
    std::vector<double> m_directions;
    // R/L, Direction, Bin, real/imag
    std::vector<std::vector<std::vector<std::vector<double>>>> m_HRTF;
    int m_numDirections;
    double m_theta_step;
    double m_thetaTransform;
    double m_thetaFactor;
    int m_nFFT;

    //HRTF_data m_HRTF_data;

};