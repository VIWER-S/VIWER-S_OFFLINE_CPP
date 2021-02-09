#include "Loudness.h"



double Loudness::rms(std::vector<double> inArray) {

    double len = inArray.size();
    double sum = 0.0;

    for (int iSample = 0; iSample < len; iSample++) {
        sum += inArray[iSample] * inArray[iSample];
    }
    sum /= len;

    double rms = sqrt(sum);

    return rms;
}

void Loudness::rmsTest() {
    std::vector<double> testArray = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
    double expectedResult = 6.2048;

    if (abs(rms(testArray) - expectedResult) < 0.001) {
        DBG("Test successful.");
    }
    else {
        DBG("Test not successful.");
    }

}

