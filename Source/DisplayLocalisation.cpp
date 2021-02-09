 /*
  ==============================================================================

    DisplayLocalisation.cpp
    Created: 14 Nov 2020 12:21:34pm
    Author:  Ulrik

  ==============================================================================
*/

#define _USE_MATH_DEFINES

#include <JuceHeader.h>
#include <cmath>

#include "DisplayLocalisation.h"
#include "SourceManager.h"

//==============================================================================
DisplayLocalisation::DisplayLocalisation()
{

}

DisplayLocalisation::~DisplayLocalisation()
{
}

void DisplayLocalisation::setLoc(double* loc, double* loc_kal, double* loc_fad) {
    m_ptrLoc = loc;
    m_ptrLoc_kal = loc_kal;
    m_ptrLoc_fad = loc_fad;
}

void DisplayLocalisation::setImp(double* imp, double* imp_kal, double* imp_fad) {
    m_ptrImp = imp;
    m_ptrImp_kal = imp_kal;
    m_ptrImp_fad = imp_fad;
}

void DisplayLocalisation::setHTBuffer(float* ptr_ht) {
    m_ptr_ht = ptr_ht;
}

void DisplayLocalisation::paint(juce::Graphics& g)
{

    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour(juce::Colours::grey);

    juce::Rectangle<float> circle1{ m_circleLineWidth / 2, m_circleLineWidth / 2, getWidth() - m_circleLineWidth, getHeight() - m_circleLineWidth };
    g.drawEllipse(circle1, 2.0);

    juce::Rectangle<float> circle2{ m_circleLineWidth / 2 + 5, m_circleLineWidth / 2 + 5, getWidth() - m_circleLineWidth - 10, getHeight() - m_circleLineWidth - 10 };
    g.drawEllipse(circle2, 2.0);

    m_peaks.resize(MAX_SOURCES);
    m_numPeaks = 0;
    double pk, imp;
    for (int iPeak = 0; iPeak < MAX_SOURCES; iPeak++) {
        pk = *(m_ptrLoc + iPeak);
        imp = *(m_ptrImp + iPeak);
        if (pk != -255.0) {
            m_peaks.at(m_numPeaks).resize(2);
            m_peaks.at(m_numPeaks).at(0) = pk;
            m_peaks.at(m_numPeaks).at(1) = imp;
            m_numPeaks++;
        }
    }
    m_peaks.resize(m_numPeaks);

    m_peaks_kal.resize(MAX_SOURCES);
    m_numPeaks_kal = 0;
    for (int iPeak = 0; iPeak < MAX_SOURCES; iPeak++) {
        pk = *(m_ptrLoc_kal + iPeak);
        imp = *(m_ptrImp_kal + iPeak);
        if (pk != -255.0) {
            m_peaks_kal.at(m_numPeaks_kal).resize(2);
            m_peaks_kal.at(m_numPeaks_kal).at(0) = pk;
            m_peaks_kal.at(m_numPeaks_kal).at(1) = imp;
            m_numPeaks_kal++;
        }
    }
    m_peaks_kal.resize(m_numPeaks_kal);

    m_peaks_fad.resize(MAX_SOURCES);
    m_numPeaks_fad = 0;
    for (int iPeak = 0; iPeak < MAX_SOURCES; iPeak++) {
        pk = *(m_ptrLoc_fad + iPeak);
        imp = *(m_ptrImp_fad + iPeak);
        if (pk != -255.0) {
            m_peaks_fad.at(m_numPeaks_fad).resize(2);
            m_peaks_fad.at(m_numPeaks_fad).at(0) = pk;
            m_peaks_fad.at(m_numPeaks_fad).at(1) = imp;
            m_numPeaks_fad++;
        }
    }
    m_peaks_fad.resize(m_numPeaks_fad);

    // Draw Boomerang

    juce::Path array;

    array.startNewSubPath(m_width / 2 - 20, m_height / 2 + 20);
    array.lineTo(m_width / 2 + 20, m_height / 2 + 20);
    array.lineTo(m_width / 2 + 20, m_height / 2 - 10);
    array.lineTo(m_width / 4 * 3, m_height / 2 - 60);
    array.lineTo(m_width / 4 * 3, m_height / 2 - 70);
    array.lineTo(m_width / 4 * 3 - 10, m_height / 2 - 70);
    array.lineTo(m_width / 2 + 20 - 10, m_height / 2 - 30);
    array.lineTo(m_width / 2 - 20 + 10, m_height / 2 - 30);
    array.lineTo(m_width / 4 + 10, m_height / 2 - 70);
    array.lineTo(m_width / 4, m_height / 2 - 70);
    array.lineTo(m_width / 4, m_height / 2 - 60);
    array.lineTo(m_width / 2 - 20, m_height / 2 - 10);
    array.lineTo(m_width / 2 - 20, m_height / 2 + 20);

    array.closeSubPath();
    array.applyTransform(juce::AffineTransform::translation(0, 20));

    auto fillType = juce::FillType();
    fillType.setColour(juce::Colours::white);
    fillType.setOpacity(0.5);
    g.setFillType(fillType);

    g.fillPath(array);
   
 
    // Fading sources
    g.setColour(juce::Colours::blue);
    for (int iDir = 0; iDir < m_numPeaks_fad; iDir++) {

        float angle = m_peaks_fad.at(iDir).at(0) * STEP_THETA / MAX_THETA * M_PI + M_PI;
        float linLen = pow(std::min(1.0, m_peaks_fad.at(iDir).at(1) / 5), 1.0 / 4.0) * m_width / 2;

        juce::Line<float> line((float)(m_width / 2), (float)(m_height / 2), (float)(m_width / 2 + linLen * cos(angle)), (float(m_height / 2 + linLen * sin(angle))));
        g.drawArrow(line, 2.0, 5.0, 10.0);

    }

    // Tracked sources
    g.setColour(juce::Colours::red);
    for (int iDir = 0; iDir < m_numPeaks_kal; iDir++) {

        float angle = m_peaks_kal.at(iDir).at(0) * STEP_THETA / MAX_THETA * M_PI + M_PI;
        float linLen = pow(std::min(1.0, m_peaks_kal.at(iDir).at(1) / 5), 1.0 / 4.0) * m_width / 2;

        juce::Line<float> line((float)(m_width / 2), (float)(m_height / 2), (float)(m_width / 2 + linLen * cos(angle)), (float(m_height / 2 + linLen * sin(angle))));
        g.drawArrow(line, 2.0, 5.0, 10.0);

    }

    // Found directions
    g.setColour(juce::Colours::white);
    for (int iDir = 0; iDir < m_numPeaks; iDir++) {

        float angle = m_peaks.at(iDir).at(0) * STEP_THETA / MAX_THETA * M_PI + M_PI;
        float linLen = pow(std::min(1.0, m_peaks.at(iDir).at(1) / 5), 1.0 / 4.0) * m_width / 2;
        
        juce::Line<float> line((float)(m_width / 2), (float)(m_height / 2), (float)(m_width / 2 + linLen * cos(angle)), (float(m_height / 2 + linLen * sin(angle))));
        g.drawArrow(line, 2.0, 5.0, 10.0);
        
    }

    // Head tracker look direction
    g.setColour(juce::Colours::green);
    float linLen = (getWidth() - m_circleLineWidth) / 2.0 - 5.0;
    float angle = *m_ptr_ht / 180 * M_PI;
    juce::Line<float> line((float)(m_width / 2), (float)(m_height / 2), (float)(m_width / 2 + linLen * cos(angle)), (float(m_height / 2 + linLen * sin(angle))));
    g.drawArrow(line, 2.0, 5.0, 10.0);

}

void DisplayLocalisation::resized() {}

float DisplayLocalisation::getWidth() {
    return m_width;
}

float DisplayLocalisation::getHeight() {
    return m_height;
}