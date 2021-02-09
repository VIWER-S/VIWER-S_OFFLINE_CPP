/*
  ==============================================================================

    Viwer.h
    Created: 6 Oct 2020 5:29:11pm
    Author:  Ulrik

  ==============================================================================
*/

#pragma once

#include <string>
#include <iostream>
#include <JuceHeader.h>
#include <cmath>
#include <vector>
#include <ctime>
#include <fstream>

#include "FFT.h"
#include "Localisation.h"
#include "Constants.h"
#include "SourceManager.h"
//#include "HRTF.h"
#include "HeadTracking.h"
#include "Beamforming.h"



class Viwer {

public:

    Viwer();
    ~Viwer();
    void Viwer::setSamplerate(int samplerate);
    void Viwer::setBlocksize(int blocksize);
    void Viwer::setNumChannels(int channels);
    std::vector<std::vector<float>> Viwer::process(double inBuffer[SENSORS][BUFFER_SIZE]);
    std::vector<double> Viwer::sqrtHann(int size);
    void Viwer::setDirectionBuffer(double* directions, double* dir_kal, double* dir_fad);
    void Viwer::setImportanceBuffer(double* importances, double* imp_kal, double* imp_fad);
    void Viwer::setHTBuffer(float* ptr_ht, bool* ptr_cal);

private:

    int m_samplerate;
    int m_blockSize;
    int m_numChannels;

    int m_blockSizeHalf;
    int m_hopSize;
    int m_sensors;
    int m_numTheta;
    double m_overlap;
    double m_dt;

    int margin = 20;

    std::vector<double> m_signalChannelDouble;
    std::vector<std::vector<float>> m_audioOut;
    double m_spec[SENSORS][BUFFER_SIZE/2+1][2];
    std::vector<double> m_window;
    std::vector<double> m_real;
    std::vector<double> m_imag;

    std::vector<std::vector<double>> m_directions;

    // Pointers to convey found directions from Viwer to DisplayLocalisation
    double* m_ptrDir;
    double* m_ptrImp;
    // Pointers to convey tracked sources from Viwer to DisplayLocalisation
    double* m_ptrDir_kal;
    double* m_ptrImp_kal;
    // Pointers to convey fading sources from Viwer to DisplayLocalisation
    double* m_ptrDir_fad;
    double* m_ptrImp_fad;
    // Pointer to convey head tracker direction from Viwer to DispalyLocalisation
    float* m_ptrHT;

    float m_trackDirection[3] = {0.0f, 0.0f, 0.0f};

    SourceManager m_SourceManager;
    Beamforming m_Beamforming;

    Localisation m_Localisation;
    spectrum m_Fft = spectrum(BUFFER_SIZE);
    HeadTracking m_HeadTracking;



    //HRTF m_Hrtf = HRTF(MAX_THETA / STEP_THETA);

    

};