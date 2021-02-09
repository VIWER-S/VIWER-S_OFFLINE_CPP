/*
  ==============================================================================

    QuadraticInterpolation.cpp
    Created: 2 Nov 2020 7:02:28pm
    Author:  Ulrik

  ==============================================================================
*/

#include "QuadraticInterpolation.h"


std::vector<double> QuadraticInterpolation::quadraticInterpolation(double magPrev, double mag, double magNext) {

    std::vector<double> result;
    result.resize(2);
    result.at(0) = (magNext - magPrev) / (2.0 * (2.0 * mag - magNext - magPrev));
    result.at(1) = mag - 0.25 * (magPrev - magNext) * result[0];
    return result;

}

std::vector<std::vector<double>> QuadraticInterpolation::findRealPeaks(std::vector<double> inArray, std::vector<int> vPeaks) {

    std::vector<std::vector<double>> vRealPeaks;
    vRealPeaks.resize(vPeaks.size());

    for (int iPeak = 0; iPeak < vPeaks.size(); iPeak++) {

        vRealPeaks.at(iPeak).resize(2);

        std::vector<double> tmp;
        tmp.resize(2);

        // Avoid circular confusion
        if (vPeaks[iPeak] == 0) {
            tmp = quadraticInterpolation(inArray.at(inArray.size() - 1), inArray.at(0), inArray.at(1));
        }
        else if (vPeaks[iPeak] == inArray.size() - 1) {
            tmp = quadraticInterpolation(inArray.at(inArray.size() - 2), inArray.at(inArray.size() - 1), inArray.at(0));
        }
        else {
            tmp = quadraticInterpolation(inArray.at(vPeaks.at(iPeak) - 1), inArray.at(vPeaks.at(iPeak)), inArray.at(vPeaks.at(iPeak) + 1));
        }

        vRealPeaks.at(iPeak).at(0) = tmp.at(0) + vPeaks.at(iPeak);
        vRealPeaks.at(iPeak).at(1) = tmp.at(1);
    }

    return vRealPeaks;


}