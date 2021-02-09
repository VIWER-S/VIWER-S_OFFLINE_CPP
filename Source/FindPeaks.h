/*
  ==============================================================================

    FindPeaks.h
    Created: 3 Nov 2020 9:08:17am
    Author:  Ulrik

  ==============================================================================
*/

#pragma once

#define _USE_MATH_DEFINES

#include <vector>
#include <cmath>

class FindPeaks {

public:

    static std::vector<int> FindPeaks::findPeaks(std::vector<double> inArray);
    static std::vector<int> FindPeaks::findPeaks(std::vector<double> inArray, int peakImportance);

private:



};