#pragma once

#include <vector>

#include "SteeringVector.h"
#include "HRTF.h"
#include "Source.h"

class Beamforming
{

public:

    Beamforming::Beamforming();
    Beamforming::Beamforming(int samplerate, int blocksize, int num_theta_loc);
    std::vector<std::vector<std::vector<float>>> Beamforming::filter(double spec[SENSORS][BUFFER_SIZE / 2 + 1][2], std::vector<Source> sources);

private:

    int m_sensors;
    int m_samplerate;
    int m_blocksize;
    int m_fftlenHalf;
    std::vector<std::vector<double>> m_coordinates;
    SteeringVector m_Steeringvector;
    HRTF m_Hrtf = HRTF(MAX_THETA / STEP_THETA);
    int m_num_theta_loc;
  
    double m_theta[SENSORS][BUFFER_SIZE/2+1][2];

    // L/R, Samples, real/imag
    //double m_specOut[2][BUFFER_SIZE][2];


};

