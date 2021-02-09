#pragma once

#include <string>
#include <windows.h>
#include <stdio.h>
#include <stdint.h>

#include "Constants.h"
#include "HeadTrackerCpp/Comm.h"
#include "JuceHeader.h"



float head_tracker(byte* input, int ninput);

static class HeadTracking
{
	

public:

	
	HeadTracking::HeadTracking();
	HeadTracking::~HeadTracking();
	float HeadTracking::map180(float angle);
	void HeadTracking::init_audio_processing_float(void);
	
	void HeadTracking::audio_processing_float(void);
	void HeadTracking::delete_audio_processing_float(void);


	void HeadTracking::setPointer(float* ptr_yaw, bool* ptr_cal);
	void HeadTracking::unsetPointer();


private:

	Comm* comm = nullptr;

};


