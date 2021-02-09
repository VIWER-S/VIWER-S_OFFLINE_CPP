/*
  ==============================================================================

    FindPeaks.cpp
    Created: 3 Nov 2020 9:08:17am
    Author:  Ulrik

  ==============================================================================
*/

#include "FindPeaks.h"

std::vector<int> FindPeaks::findPeaks(std::vector<double> inArray) {
    return findPeaks(inArray, 1);
}

std::vector<int> FindPeaks::findPeaks(std::vector<double> inArray, int peakImportance) {

    std::vector<int> peaks;

    if (peakImportance > inArray.size()) {
        peakImportance = inArray.size() - 1;
    }

    // Only a maximum of N/2 peaks possible (+1 to account for truncation)
    const int nProv = (inArray.size() + 1) / 2;
    std::vector<int> accu;

    // Edge case: Array has either 1 or 0 entries. -> return 0
    if (inArray.size() < 2) {
        peaks.push_back(0);
        return peaks;
    }

    // Edge case: Array has 2 entries: 1>2 / 2>1 / 1==2
    if (inArray.size() == 2) {
        if (inArray.at(0) > inArray.at(1)) {
            accu.push_back(0);
        }
        else if (inArray.at(0) < inArray.at(1)) {
            accu.push_back(1);
        }
        else {
            accu.push_back(0);
            accu.push_back(1);
        }
    }

    // Padding/repeating the original array at the start and end by peak importance # samples
    std::vector<double> tmpArray;
    tmpArray.resize(2 * peakImportance + inArray.size(), 0.0f);
    for (int idx = 0; idx < peakImportance; idx++) {
        tmpArray.at(idx) = inArray.at(inArray.size() - peakImportance + idx);
        tmpArray.at(inArray.size() + peakImportance + idx) = inArray.at(idx);
    }
    for (int idx = 0; idx < inArray.size(); idx++) {
        tmpArray.at(idx + peakImportance) = inArray.at(idx);
    }

    // Search the array from its original beginning to its original end
    for (int idx = peakImportance; idx < tmpArray.size() - peakImportance; idx++) {

        std::vector<int> bPeak;
        bPeak.resize(peakImportance, 0.0f);
        for (int iPad = 1; iPad < peakImportance + 1; iPad++) {

            if (tmpArray.at(idx) > tmpArray.at(idx - iPad) && tmpArray.at(idx) > tmpArray.at(idx + iPad)) {
                bPeak.at(iPad - 1) = 0;
            }
            else {
                bPeak.at(iPad - 1) = 1;
            }
        }

        int nTest = 0;
        for (int iPad = 0; iPad < bPeak.size(); iPad++) {
            nTest += bPeak.at(iPad);
        }
        if (nTest == 0) {
            accu.push_back(idx - peakImportance);
        }
    }
    return accu;
}

