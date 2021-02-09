/*
  ==============================================================================

    DisplayWaveform.h
    Created: 24 Nov 2020 9:48:00pm
    Author:  Ulrik

  ==============================================================================
*/

#pragma once

/*
  ==============================================================================

    DisplaySources.h
    Created: 23 Nov 2020 7:21:46pm
    Author:  Ulrik

  ==============================================================================
*/

#pragma once

#include <vector>

#include "JuceHeader.h"
#include "Constants.h"


class DisplayWaveform : public juce::Component
{

public:

    DisplayWaveform::DisplayWaveform(std::string name);
    DisplayWaveform::~DisplayWaveform();

    int DisplayWaveform::getWidth();
    int DisplayWaveform::getHeight();

    void DisplayWaveform::paint(juce::Graphics& g);
    void DisplayWaveform::resized();

    void DisplayWaveform::setPointer(float* ptr);

private:

    float m_width = BUFFER_SIZE;
    float m_height = 100;

    // Pointer to the data vector
    float* m_ptr_buffer;

    std::string m_name;
};