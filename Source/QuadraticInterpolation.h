/*
  ==============================================================================

    QuadraticInterpolation.h
    Created: 2 Nov 2020 7:02:28pm
    Author:  Ulrik

  ==============================================================================
*/

#pragma once

#define _USE_MATH_DEFINES

#include <cmath>
#include <vector>

class QuadraticInterpolation {

public:

    static std::vector<double> QuadraticInterpolation::quadraticInterpolation(double magPrev, double mag, double magNext);
    static std::vector<std::vector<double>> QuadraticInterpolation::findRealPeaks(std::vector<double> inArray, std::vector<int> vPeaks);

private:



};

