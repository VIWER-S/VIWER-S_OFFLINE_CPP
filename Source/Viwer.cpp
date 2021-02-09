/*
  ==============================================================================

    Viwer.cpp
    Created: 6 Oct 2020 5:29:11pm
    Author:  Ulrik

  ==============================================================================
*/

#define _USE_MATH_DEFINES

#include "Viwer.h"

Viwer::Viwer() :
    m_samplerate{ SAMPLERATE },
    m_numChannels{ SENSORS },
    m_blockSize{ BUFFER_SIZE },
    m_blockSizeHalf{BUFFER_SIZE/2+1}
{
    m_window = sqrtHann(BUFFER_SIZE);
    m_signalChannelDouble.resize(BUFFER_SIZE, 0.0);
    m_real.resize(m_blockSizeHalf, 0.0);
    m_imag.resize(m_blockSizeHalf, 0.0);

    m_numTheta = m_Localisation.getNumTheta();
    m_dt = Kalman::calculateDt(m_blockSize, m_samplerate);
    m_SourceManager = SourceManager(m_dt, m_numTheta);

    m_audioOut.resize(2);
    m_audioOut.at(0).resize(BUFFER_SIZE);
    m_audioOut.at(1).resize(BUFFER_SIZE);

    m_HeadTracking.init_audio_processing_float();
}

Viwer::~Viwer()
{
    m_HeadTracking.unsetPointer();
}

void Viwer::setDirectionBuffer(double* directions, double* dir_kal, double* dir_fad) {
    m_ptrDir = directions;
    m_ptrDir_kal = dir_kal;
    m_ptrDir_fad = dir_fad;
}

void Viwer::setImportanceBuffer(double* importances, double* imp_kal, double* imp_fad) {
    m_ptrImp = importances;
    m_ptrImp_kal = imp_kal;
    m_ptrImp_fad = imp_fad;
}

void Viwer::setHTBuffer(float* ptr_ht, bool* ptr_cal) {
    m_HeadTracking.setPointer(ptr_ht, ptr_cal);
}

void Viwer::setSamplerate(int samplerate)
{
    m_samplerate = samplerate;
}

void Viwer::setBlocksize(int blocksize)
{
    m_blockSize = blocksize;
}

void Viwer::setNumChannels(int channels) {
    m_numChannels = channels;
}

std::vector<double> Viwer::sqrtHann(int size) {

    std::vector<double> window;
    window.resize(size);

    if (size > 1) {
        for (int iSample = 0; iSample < size; iSample++) {
            window.at(iSample) = sqrt(0.5 * (1 - cos(2 * M_PI * iSample / (size - 1))));
        }
    }

    return window;
}

std::vector<std::vector<float>> Viwer::process(double inBuffer[SENSORS][BUFFER_SIZE]){
   
    time_t tstart, tend;
    tstart = time(0);

    //float arr[2] = { 10 * log10(pow(inBuffer[0][0], 2)), 10 * log10(pow(inBuffer[1][0], 2)) };
 

    // Generation of the Spectrum
    for (int iSensor = 0; iSensor < m_numChannels; iSensor++) {
        for (int iSample = 0; iSample < m_blockSize; iSample++) {
            m_signalChannelDouble.at(iSample) = inBuffer[iSensor][iSample] * m_window.at(iSample);
        }

        double* data = m_signalChannelDouble.data();
        double* real = m_real.data();
        double* imag = m_imag.data();

        m_Fft.fft(data, real, imag);

        for (int iFreq = 0; iFreq < m_blockSizeHalf; iFreq++) {
            m_spec[iSensor][iFreq][0] = real[iFreq];
            m_spec[iSensor][iFreq][1] = imag[iFreq];
        }
    }


    // Localisation
    std::vector<std::vector<double>> directions = m_Localisation.sourceDirections_SRP(m_spec);
    //std::vector<std::vector<double>> directions = m_Localisation.sourceDirections_DU(m_spec);

    // Tracking
    std::vector<Source> kalmanPeaks = m_SourceManager.trackSources(directions);

    std::vector<Source> fadingSources = m_SourceManager.getFadingSources();

    // Write peaks to buffer
    for (int iPeak = 0; iPeak < directions.size(); iPeak++) {
        *(m_ptrDir + iPeak) = directions.at(iPeak).at(0);
        *(m_ptrImp + iPeak) = directions.at(iPeak).at(1);
    }
    for (int iPeak = directions.size(); iPeak < MAX_SOURCES; iPeak++) {
        *(m_ptrDir + iPeak) = -255.0;
        *(m_ptrImp + iPeak) = -255.0;
    }

    // Write kalman sources to buffer
    for (int iSource = 0; iSource < kalmanPeaks.size(); iSource++) {
        *(m_ptrDir_kal + iSource) = kalmanPeaks.at(iSource).getAngle();
        *(m_ptrImp_kal + iSource) = kalmanPeaks.at(iSource).getStrength();
    }
    for (int iSource = kalmanPeaks.size(); iSource < MAX_SOURCES; iSource++) {
        *(m_ptrDir_kal + iSource) = -255.0;
        *(m_ptrImp_kal + iSource) = -255.0;
    }
   
    
    // Write fading sources to buffer
    // ATTENTION::: NUMBER OF FADING SOURCES NEEDS TO BE CONTROLLED!!!
    for (int iFade = 0; iFade < fadingSources.size(); iFade++) {
        *(m_ptrDir_fad + iFade) = fadingSources.at(iFade).getAngle();
        *(m_ptrImp_fad + iFade) = fadingSources.at(iFade).getStrength();
    }
    for (int iFade = fadingSources.size(); iFade < MAX_SOURCES; iFade++) {
        *(m_ptrDir_fad + iFade) = -255.0;
        *(m_ptrImp_fad + iFade) = -255.0;
    }

    /*for (int iDir = 0; iDir < kalmanPeaks.size(); iDir++) {
        std::vector<std::vector<std::vector<double>>> hrtf = m_Hrtf.getHRTF(kalmanPeaks.at(iDir).getAngle());
    }*/

    std::vector<std::vector<std::vector<float>>> m_specOut = m_Beamforming.filter(m_spec, kalmanPeaks);

    // left
    float* ptr_real_l = m_specOut.at(0).at(0).data();
    float* ptr_imag_l = m_specOut.at(0).at(1).data();
    float* ptr_audio_l = m_audioOut.at(0).data();

    m_Fft.ifft(ptr_real_l, ptr_imag_l, ptr_audio_l);

    // right
    float* ptr_real_r = m_specOut.at(1).at(0).data();
    float* ptr_imag_r = m_specOut.at(1).at(1).data();
    float* ptr_audio_r = m_audioOut.at(1).data();

    m_Fft.ifft(ptr_real_r, ptr_imag_r, ptr_audio_r);

    //m_HeadTracking.audio_processing_float();
    
    /*std::string mess = "Directions: ";
    mess.append(std::to_string(m_trackDirection[0]));
    mess.append(", ");
    mess.append(std::to_string(m_trackDirection[1]));
    mess.append(", "); 
    mess.append(std::to_string(m_trackDirection[2]));*/
    //DBG(mess);


    /*std::ofstream file1("data_orig.txt");
    std::ofstream file2("data_real.txt");
    std::ofstream file3("data_imag.txt");

    for (int iSample = 0; iSample < BUFFER_SIZE; iSample++) {
        file1 << inBuffer[0][iSample] * m_window.at(iSample) << ",";
        file2 << specDouble[0][iSample][0] << ",";
        file3 << specDouble[0][iSample][1] << ",";
    }*/

  /*
    tend = time(0);
    std::string mess = "It took "; 
    mess.append(std::to_string(difftime(tend, tstart)));
    mess.append(" second(s).");
    DBG(mess);
    */
    return m_audioOut;// arr;

}
