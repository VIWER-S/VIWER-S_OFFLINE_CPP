
#include "Beamforming.h"

Beamforming::Beamforming() {};

Beamforming::Beamforming(int samplerate, int blocksize, int num_theta_loc) {

    m_sensors = SENSORS;
    m_samplerate = samplerate;
    m_blocksize = blocksize;
    m_fftlenHalf = (int)BUFFER_SIZE / 2 + 1;
    m_num_theta_loc = num_theta_loc;

    //this.hrtf = new HRTF(num_theta_loc);

}


std::vector<std::vector<std::vector<float>>> Beamforming::filter(double spec[SENSORS][BUFFER_SIZE / 2 + 1][2], std::vector<Source> sources) {

    // left/right, real/imag, bins
    std::vector<std::vector<std::vector<float>>> specOut;
    specOut.resize(2);
    specOut.at(0).resize(2);
    specOut.at(0).at(0).resize(BUFFER_SIZE / 2 + 1);
    specOut.at(0).at(1).resize(BUFFER_SIZE / 2 + 1);
    specOut.at(1).resize(2);
    specOut.at(1).at(0).resize(BUFFER_SIZE / 2 + 1); 
    specOut.at(1).at(1).resize(BUFFER_SIZE / 2 + 1);
    

    // Bins, L/R, Real/Imag
    for (int iDir = 0; iDir < sources.size(); iDir++) {

        // Sensors, Bins, Real/Imag
        std::vector<std::vector<std::vector<double>>> m_theta = m_Steeringvector.generateDelayTensor_DSB(sources.at(iDir).getAngle());

        if (sources.at(iDir).getAngle() < 0) {
            sources.at(iDir).setAngle(sources.at(iDir).getAngle()+m_num_theta_loc - 1);
        }
        std::vector<std::vector<std::vector<double>>> hrtfCoefficients = m_Hrtf.getHRTF(sources.at(iDir).getAngle());

        // Bins, Real/Imag
        double monoSpec[BUFFER_SIZE/2+1][2];
        for (int iBin = 0; iBin < BUFFER_SIZE/2+1; iBin++) {
            monoSpec[iBin][0] = 0.0;
            monoSpec[iBin][1] = 0.0;
        }

        // Beamforming
        for (int iSensor = 0; iSensor < SENSORS; iSensor++) {
            for (int iBin = 0; iBin < BUFFER_SIZE/2+1; iBin++) {
                monoSpec[iBin][0] += spec[iSensor][iBin][0] * m_theta.at(iSensor).at(iBin).at(0) - spec[iSensor][iBin][1] * m_theta.at(iSensor).at(iBin).at(1);
                monoSpec[iBin][1] += spec[iSensor][iBin][0] * m_theta.at(iSensor).at(iBin).at(1) + spec[iSensor][iBin][1] * m_theta.at(iSensor).at(iBin).at(0);
            }
        }

        // HRTFs
        for (int iBin = 0; iBin < BUFFER_SIZE/2+1; iBin++) {
            // Real, Left
            specOut.at(0).at(0).at(iBin) += monoSpec[iBin][0] * hrtfCoefficients.at(0).at(iBin).at(0) - monoSpec[iBin][1] * hrtfCoefficients.at(0).at(iBin).at(1);
            // Imag, Left
            specOut.at(0).at(1).at(iBin) += monoSpec[iBin][0] * hrtfCoefficients.at(0).at(iBin).at(1) + monoSpec[iBin][1] * hrtfCoefficients.at(0).at(iBin).at(0);
            // Real, Right
            specOut.at(1).at(0).at(iBin) += monoSpec[iBin][0] * hrtfCoefficients.at(1).at(iBin).at(0) - monoSpec[iBin][1] * hrtfCoefficients.at(1).at(iBin).at(1);
            // Imag, Right
            specOut.at(1).at(1).at(iBin) += monoSpec[iBin][0] * hrtfCoefficients.at(1).at(iBin).at(1) + monoSpec[iBin][1] * hrtfCoefficients.at(1).at(iBin).at(0);
        }
    }

    /*
    // IFFT
    Complex[] tmp = new Complex[this.blocksize];

    // Left
    for (int iBin = 0; iBin < this.fftlenHalf; iBin++) {
        tmp[iBin] = new Complex(stereoSpec[iBin][0][0], stereoSpec[iBin][0][1]);
    }
    for (int iBin = 1; iBin < this.fftlenHalf - 1; iBin++) {
        tmp[blocksize - iBin] = new Complex(stereoSpec[iBin][0][0], -stereoSpec[iBin][0][1]);
    }
    tmp = FFT.ifft(tmp);
    for (int iSample = 0; iSample < this.blocksize; iSample++) {
        stereoOut[iSample][0] = tmp[iSample].re;
    }

    // Right
    for (int iBin = 0; iBin < this.fftlenHalf; iBin++) {
        tmp[iBin] = new Complex(stereoSpec[iBin][1][0], stereoSpec[iBin][1][1]);
    }
    for (int iBin = 1; iBin < this.fftlenHalf - 1; iBin++) {
        tmp[blocksize - iBin] = new Complex(stereoSpec[iBin][1][0], -stereoSpec[iBin][1][1]);
    }
    tmp = FFT.ifft(tmp);
    for (int iSample = 0; iSample < this.blocksize; iSample++) {
        stereoOut[iSample][1] = tmp[iSample].re;
    }
    */
    return specOut;
}


