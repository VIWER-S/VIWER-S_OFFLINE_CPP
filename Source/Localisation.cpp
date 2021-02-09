#define _USE_MATH_DEFINES

#include "Localisation.h"

Localisation::Localisation() :
    m_blockSize{ BUFFER_SIZE },
    m_blockSizeHalf{ BUFFER_SIZE / 2 + 1 },
    m_lowerBin{ 40 },
    m_upperBin{ m_blockSizeHalf },
    m_alpha{ 0.4 },
    m_oneMinusAlpha{ 1.0 - m_alpha },
    m_maxTheta{ MAX_THETA },
    m_stepTheta{ STEP_THETA },
    m_numTheta{ m_maxTheta / m_stepTheta }
{

    m_SteeringVector = SteeringVector();
    m_sensors = m_SteeringVector.getNumSensors();
    m_sensorsSquare = m_sensors * m_sensors;

    // Generate Complex Eye Matrix
    m_eye.resize(m_sensors);
    for (int iRow = 0; iRow < m_sensors; iRow++) {
        m_eye.at(iRow).resize(m_sensors);
        for (int iCol = 0; iCol < m_sensors; iCol++) {
            m_eye.at(iRow).at(iCol).resize(2, 0.0);
            if (iRow == iCol) {
                m_eye.at(iRow).at(iCol).at(0) = 1.0;
            }
            else {
                m_eye.at(iRow).at(iCol).at(0) = 0.0;
            }
        }
    }

    // Establish List of Angles
    m_thetaVector.resize(m_numTheta);
    int idxTheta = 0;
    int idx = 0;
    while (idxTheta < m_maxTheta) {
        m_thetaVector.at(idx) = M_PI * idxTheta / 180.0f;
        idx++;
        idxTheta += m_stepTheta;
    }

    // Build delay tensor
    m_thetaTensor = m_SteeringVector.generateDelayTensor_DSB(m_thetaVector);

    // Get frequencies 1 ... fs/2
    m_frequencies = m_SteeringVector.getFrequencies();
    for (int iFreq = 0; iFreq < m_frequencies.size(); iFreq++) {
        m_frequencies.at(iFreq) *= 2 * M_PI;
    }

    // Get frequencies 1 ... fs
    m_frequenciesFull = m_SteeringVector.getFrequenciesFull();
    for (int iFreq = 0; iFreq < m_frequenciesFull.size(); iFreq++) {
        m_frequenciesFull.at(iFreq) *= 2 * M_PI;
    }


    /*m_delays.resize(m_numTheta); // is this needed?
    for (int iTheta = 0; iTheta < m_numTheta; iTheta++) {
        m_delays.at(iTheta) = m_SteeringVector.calculateDelays(m_thetaVector.at(iTheta));
    }*/

}

Localisation::~Localisation() {
}

double Localisation::getRealTrace(double matrix[SENSORS][SENSORS][BUFFER_SIZE / 2 + 1][2], int iFreq) {

    double real = 0.0;

    for (int iRow = 0; iRow < m_sensors; iRow++) {
        real += matrix[iRow][iRow][iFreq][0];
    }
    return real;
}

int Localisation::getNumTheta() {
    return m_numTheta;
}

std::vector<std::vector<std::vector<double>>> Localisation::getSteeringVectorDouble(int iTheta) {
    return m_thetaTensor.at(iTheta);
}

std::vector<std::vector<std::vector<double>>> Localisation::generateEyeTimesComplex(int size, std::vector<double> complexNumber) {

    // Obtain copy of unity matrix
    std::vector<std::vector<std::vector<double>>> tmpEye = m_eye;

    /*for (int iRow = 0; iRow < size; iRow++) {
        for (int iCol = 0; iCol < size; iCol++) {
            tmpEye.at(iRow).at(iCol).at(0) = 0;
            tmpEye.at(iRow).at(iCol).at(1) = 0;
        }
    }*/

    for (int iRow = 0; iRow < size; iRow++) {
        tmpEye.at(iRow).at(iRow) = complexNumber;
    }

    return tmpEye;
}

std::vector<std::vector<std::vector<double>>> Localisation::multiplayMatrices(std::vector<std::vector<std::vector<std::vector<double>>>> m1, std::vector<std::vector<std::vector<std::vector<double>>>> m2, int iFreq) {

    std::vector<std::vector<std::vector<double>>> ergebnismatrix;

    if (m1.at(0).size() == m2.size()) {

        int zeilenm1 = m1.size();
        int spaltenm1 = m1[0].size();
        int spaltenm2 = m2[0].size();

        ergebnismatrix.resize(zeilenm1);

        for (int i = 0; i < zeilenm1; i++) {
            ergebnismatrix.at(i).resize(spaltenm2);
            for (int j = 0; j < spaltenm2; j++) {
                ergebnismatrix.at(i).at(j).resize(2, 0.0);
                for (int k = 0; k < spaltenm1; k++) {
                    ergebnismatrix.at(i).at(j).at(0) += (m1.at(i).at(k).at(iFreq).at(0) * m2.at(k).at(j).at(iFreq).at(0) - m1.at(i).at(k).at(iFreq).at(1) * m2.at(k).at(j).at(iFreq).at(1));
                    ergebnismatrix.at(i).at(j).at(1) += (m1.at(i).at(k).at(iFreq).at(0) * m2.at(k).at(j).at(iFreq).at(1) + m1.at(i).at(k).at(iFreq).at(1) * m2.at(k).at(j).at(iFreq).at(0));
                }
            }
        }
    }

    else {

        int zeilen = m1.size();
        int spalten = m1.at(0).size();

        ergebnismatrix.resize(zeilen);

        for (int i = 0; i < zeilen; i++) {
            ergebnismatrix.at(i).resize(spalten);
            for (int j = 0; j < spalten; j++) {
                ergebnismatrix.at(i).at(j).resize(2, 0.0);
            }
        }
    }
    return ergebnismatrix;
}

std::vector<std::vector<double>> Localisation::sourceDirections_SRP(double spec[SENSORS][BUFFER_SIZE / 2 + 1][2]) {

    if (m_firstBlock) {

        for (int iFreq = m_lowerBin; iFreq < m_upperBin; iFreq++) {
            for (int iRow = 0; iRow < m_sensors; iRow++) {
                for (int iCol = iRow; iCol < m_sensors; iCol++) {
                    // Real
                    m_psd[iRow][iCol][iFreq][0] = spec[iRow][iFreq][0] * spec[iCol][iFreq][0] + spec[iRow][iFreq][1] * spec[iCol][iFreq][1];
                    // Imaginary
                    m_psd[iRow][iCol][iFreq][1] = spec[iRow][iFreq][1] * spec[iCol][iFreq][0] - spec[iRow][iFreq][0] * spec[iCol][iFreq][1];
                }
            }
        }
    }
    else {

        for (int iFreq = m_lowerBin; iFreq < m_upperBin; iFreq++) {
            for (int iRow = 0; iRow < m_sensors; iRow++) {
                for (int iCol = iRow; iCol < m_sensors; iCol++) {

                    // Real
                    m_psd[iRow][iCol][iFreq][0] = m_oneMinusAlpha * (spec[iRow][iFreq][0] * spec[iCol][iFreq][0] + spec[iRow][iFreq][1] * spec[iCol][iFreq][1]) +
                        m_alpha * m_psd[iRow][iCol][iFreq][0];
                    // Imaginary
                    m_psd[iRow][iCol][iFreq][1] = m_oneMinusAlpha * (spec[iRow][iFreq][1] * spec[iCol][iFreq][0] - spec[iRow][iFreq][0] * spec[iCol][iFreq][1]) +
                        m_alpha * m_psd[iRow][iCol][iFreq][1];
                }
            }
        }
    }

    m_pAbsSum.clear();
    m_pAbsSum.resize(m_numTheta, 0.0f);

    for (int iTheta = 0; iTheta < m_numTheta; iTheta++) {

        for (int iSensor = 0; iSensor < m_sensors; iSensor++) {
            for (int iFreq = 0; iFreq < m_blockSizeHalf; iFreq++) {
                m_firstMulti[iSensor][iFreq][0] = 0.0;
                m_firstMulti[iSensor][iFreq][1] = 0.0;
            }
        }

        std::vector<std::vector<double>> tmpPHAT;
        tmpPHAT.resize(m_blockSizeHalf);

        std::vector<std::vector<std::vector<double>>> steeringVec = getSteeringVectorDouble(iTheta);

        for (int iFreq = 0; iFreq < m_lowerBin; iFreq++) {
            tmpPHAT.at(iFreq).resize(2, 0.0);
        }
        for (int iFreq = m_upperBin; iFreq < m_blockSizeHalf; iFreq++) {
            tmpPHAT.at(iFreq).resize(2, 0.0);
        }

        for (int iFreq = m_lowerBin; iFreq < m_upperBin; iFreq++) {
            tmpPHAT.at(iFreq).resize(2, 0.0);

            for (int iRow = 0; iRow < m_sensors; iRow++) {
                for (int iCol = iRow; iCol < m_sensors; iCol++) {
                    m_firstMulti[iRow][iFreq][0] += m_psd[iRow][iCol][iFreq][0] * steeringVec.at(iCol).at(iFreq).at(0) -
                        m_psd[iRow][iCol][iFreq][1] * steeringVec.at(iCol).at(iFreq).at(1);
                    m_firstMulti[iRow][iFreq][1] += m_psd[iRow][iCol][iFreq][1] * steeringVec.at(iCol).at(iFreq).at(0) +
                        m_psd[iRow][iCol][iFreq][0] * steeringVec.at(iCol).at(iFreq).at(1);
                }
            }

            for (int iCol = 0; iCol < m_sensors; iCol++) {
                tmpPHAT.at(iFreq).at(0) += m_firstMulti[iCol][iFreq][0] * steeringVec.at(iCol).at(iFreq).at(0) +
                    m_firstMulti[iCol][iFreq][1] * steeringVec.at(iCol).at(iFreq).at(1);
                tmpPHAT.at(iFreq).at(1) += m_firstMulti[iCol][iFreq][1] * steeringVec.at(iCol).at(iFreq).at(0) -
                    m_firstMulti[iCol][iFreq][0] * steeringVec.at(iCol).at(iFreq).at(1);
            }

            m_pAbsSum.at(iTheta) += sqrt(tmpPHAT.at(iFreq).at(0) * tmpPHAT.at(iFreq).at(0) + tmpPHAT.at(iFreq).at(1) * tmpPHAT.at(iFreq).at(1));
        }
    }

    // Peak Picking
    std::vector<int> peaks = FindPeaks::findPeaks(m_pAbsSum, m_peakImportance + 2);

    // Quadratic Interpolation of Peak Positions
    std::vector<std::vector<double>> realPeaks = QuadraticInterpolation::findRealPeaks(m_pAbsSum, peaks);

    if (m_firstBlock) {
        m_firstBlock = false;
    }

    return realPeaks;

}

std::vector<std::vector<double>> Localisation::sourceDirections_DU(double spec[SENSORS][BUFFER_SIZE / 2 + 1][2]) {

    // Generation of the PSD

    if (m_firstBlock) {

        for (int iFreq = m_lowerBin; iFreq < m_upperBin; iFreq++) {
            for (int iRow = 0; iRow < m_sensors; iRow++) {
                for (int iCol = iRow; iCol < m_sensors; iCol++) {
                    // Real
                    m_psd[iRow][iCol][iFreq][0] = spec[iRow][iFreq][0] * spec[iCol][iFreq][0] + spec[iRow][iFreq][1] * spec[iCol][iFreq][1];
                    // Imaginary
                    m_psd[iRow][iCol][iFreq][1] = spec[iRow][iFreq][1] * spec[iCol][iFreq][0] - spec[iRow][iFreq][0] * spec[iCol][iFreq][1];
                }
            }
        }
    }
    else {

        for (int iFreq = m_lowerBin; iFreq < m_upperBin; iFreq++) {
            for (int iRow = 0; iRow < m_sensors; iRow++) {
                for (int iCol = iRow; iCol < m_sensors; iCol++) {

                    // Real
                    m_psd[iRow][iCol][iFreq][0] = m_oneMinusAlpha * (spec[iRow][iFreq][0] * spec[iCol][iFreq][0] + spec[iRow][iFreq][1] * spec[iCol][iFreq][1]) +
                        m_alpha * m_psd[iRow][iCol][iFreq][0];
                    // Imaginary
                    m_psd[iRow][iCol][iFreq][1] = m_oneMinusAlpha * (spec[iRow][iFreq][1] * spec[iCol][iFreq][0] - spec[iRow][iFreq][0] * spec[iCol][iFreq][1]) +
                        m_alpha * m_psd[iRow][iCol][iFreq][1];
                }
            }
        }
    }

    // Testing of various azimuth angles

    m_pAbsSum.assign(m_numTheta, 0.0);

    double nP;

    for (int iTheta = 0; iTheta < m_numTheta; iTheta++) {

        nP = 0.0;

        std::vector<std::vector<std::vector<double>>> steeringVec = getSteeringVectorDouble(iTheta);

        // Power formula 27
        for (int iFreq = m_lowerBin; iFreq < m_upperBin; iFreq++) {

            std::vector<double> trace = { getRealTrace(m_psd, iFreq), 0.0 };

            std::vector<std::vector<std::vector<double>>> eyeTimesTraceMinusPSD = generateEyeTimesComplex(m_sensors, trace);

            for (int iRow = 0; iRow < m_sensors; iRow++) {
                for (int iCol = 0; iCol < m_sensors; iCol++) {



                    //if (iCol == iRow) {
                    //    eyeTimesTraceMinusPSD.at(iRow).at(iCol).at(0) -= m_psd[iRow][iCol][iFreq][0];
                    //    eyeTimesTraceMinusPSD.at(iRow).at(iCol).at(1) -= m_psd[iRow][iCol][iFreq][1];
                    //}
                    //else {
                    eyeTimesTraceMinusPSD.at(iRow).at(iCol).at(0) -= m_psd[iRow][iCol][iFreq][0];
                    eyeTimesTraceMinusPSD.at(iRow).at(iCol).at(1) -= m_psd[iRow][iCol][iFreq][1];
                    //}
                      
                }
             

            }


            std::vector<std::vector<double>> tmp;
            tmp.resize(m_sensors);


            for (int iRow = 0; iRow < m_sensors; iRow++) {
                tmp.at(iRow).resize(2, 0.0);
                for (int iCol = 0; iCol < m_sensors; iCol++) {

                    tmp.at(iRow).at(0) += (eyeTimesTraceMinusPSD.at(iRow).at(iCol).at(0) * steeringVec.at(iCol).at(iFreq).at(0) -
                        eyeTimesTraceMinusPSD.at(iRow).at(iCol).at(1) * steeringVec.at(iCol).at(iFreq).at(1));
                    if (m_useImaginary) {
                        tmp.at(iRow).at(1) += (eyeTimesTraceMinusPSD.at(iRow).at(iCol).at(1) * steeringVec.at(iCol).at(iFreq).at(0) +
                            eyeTimesTraceMinusPSD.at(iRow).at(iCol).at(0) * steeringVec.at(iCol).at(iFreq).at(1));
                    }
                }
            }

            double tmp2[2] = { 0.0, 0.0 };

            for (int iRow = 0; iRow < m_sensors; iRow++) {
                for (int iCol = 0; iCol < m_sensors; iCol++) {

                    //tmp2[0] += steeringVec.at(iCol).at(iFreq).at(0) * tmp.at(iRow).at(0) +
                    //    steeringVec.at(iCol).at(iFreq).at(1) * tmp.at(iRow).at(1);
                    if (m_useImaginary) {
                        tmp2[1] += steeringVec.at(iCol).at(iFreq).at(0) * tmp.at(iRow).at(1) -
                            steeringVec.at(iCol).at(iFreq).at(1) * tmp.at(iRow).at(0);
                    }
                    else {
                        tmp2[0] += steeringVec.at(iCol).at(iFreq).at(0) * tmp.at(iRow).at(0);
                    }
                }
            }

            // Power summation over all (interesting) bins

            if (m_useImaginary) {
                nP += sqrt(tmp2[0] * tmp2[0] + tmp2[1] * tmp2[1]);
            }
            else {
                nP += tmp2[0];
            }
            /*std::string mess = "Freq[";
            mess.append(std::to_string(iFreq));
            mess.append("] ");
            mess.append(std::to_string(nP));
            DBG(mess);*/
        }

        m_pAbsSum.at(iTheta) = -1.0 / nP;

    }

    // Loudness Normalization
    double rms = Loudness::rms(m_pAbsSum);
    for (int iTheta = 0; iTheta < m_numTheta; iTheta++) {
        m_pAbsSum.at(iTheta) /= rms;
    }

    // Peak Picking
    std::vector<int> peaks = FindPeaks::findPeaks(m_pAbsSum, m_peakImportance);

    // Quadratic Interpolation of Peak Positions
    std::vector<std::vector<double>> realPeaks = QuadraticInterpolation::findRealPeaks(m_pAbsSum, peaks);

    if (m_firstBlock) {
        m_firstBlock = false;
    }

    return realPeaks;




}



