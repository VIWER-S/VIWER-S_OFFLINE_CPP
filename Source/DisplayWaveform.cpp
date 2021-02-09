/*
  ==============================================================================

    DisplayWaveform.cpp
    Created: 24 Nov 2020 9:48:00pm
    Author:  Ulrik

  ==============================================================================
*/

#include "DisplayWaveform.h"

DisplayWaveform::DisplayWaveform(std::string name) : m_name(name) {}
DisplayWaveform::~DisplayWaveform() {}

void DisplayWaveform::setPointer(float* ptr) {
    m_ptr_buffer = ptr;
}

int DisplayWaveform::getWidth() {
    return m_width;
}

int DisplayWaveform::getHeight() {
    return m_height;
}

void DisplayWaveform::paint(juce::Graphics& g) {

    // Box

    g.setColour(juce::Colours::darkgrey);

    juce::Path box;
    box.startNewSubPath(0, 0);
    box.lineTo(m_width, 0);
    box.lineTo(m_width, m_height);
    box.lineTo(0, m_height);
    box.closeSubPath();
    g.fillPath(box);

    // Outline

    g.setColour(juce::Colours::white);
    g.drawRect(0.0, 0.0, m_width, m_height, 1.0);

    // Waveform

    g.setColour(juce::Colours::white);

    juce::Path array;

    array.startNewSubPath(0, (m_ptr_buffer[0]+0.5)*m_height);
    for (int iSample = 1; iSample < BUFFER_SIZE; iSample++) {
        array.lineTo(iSample, (-0.5*(m_ptr_buffer[iSample]* m_ptr_buffer[iSample]* m_ptr_buffer[iSample])+1.5* m_ptr_buffer[iSample] + 0.5)*m_height);
    }
    array.closeSubPath();
    g.fillPath(array);

    // Text

    g.drawText(m_name, 5, 5, 100, 15, juce::Justification::topLeft, true);

}

void DisplayWaveform::resized() {}

