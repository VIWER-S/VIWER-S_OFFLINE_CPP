#pragma once

#define _USE_MATH_DEFINES

#include <vector>
#include <cmath>

#include "JuceHeader.h"

class Loudness
{

public:

	static double Loudness::rms(std::vector<double> inArray);
	static void Loudness::rmsTest();

private:


};

