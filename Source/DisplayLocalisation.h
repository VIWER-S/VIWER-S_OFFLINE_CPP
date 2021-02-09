/*
  ==============================================================================

    DisplayLocalisation.h
    Created: 14 Nov 2020 12:21:34pm
    Author:  Ulrik

  ==============================================================================
*/

#pragma once

#include "Constants.h"

#include <JuceHeader.h>

//==============================================================================
/*
*/
class DisplayLocalisation  : public juce::Component
{
public:
    DisplayLocalisation::DisplayLocalisation();
    DisplayLocalisation::~DisplayLocalisation() override;

    void DisplayLocalisation::paint(juce::Graphics&) override;
    void DisplayLocalisation::resized() override;

    float DisplayLocalisation::getWidth();
    float DisplayLocalisation::getHeight();

    void DisplayLocalisation::setLoc(double* loc, double* loc_kal, double* loc_fad);
    void DisplayLocalisation::setImp(double* imp, double* imp_kal, double* imp_fad);
    void DisplayLocalisation::setHTBuffer(float* ptr_ht);

private:

    float m_width = 300;
    float m_height = 300;
    float m_circleLineWidth = 2.0;

    // Pointers to convey found directions from Viwer to DisplayLocalisation
    double* m_ptrLoc;
    double* m_ptrImp;
    // Pointers to convey tracked sources from Viwer to DisplayLocalisation
    double* m_ptrLoc_kal;
    double* m_ptrImp_kal;
    // Pointers to convey fading sources from Viwer to DisplayLocalisation
    double* m_ptrLoc_fad;
    double* m_ptrImp_fad;
    // Pointer to convey head tracker direction from Viwer to DisplayLocalisation
    float* m_ptr_ht;

    int m_numPeaks;
    std::vector<std::vector<double>> m_peaks;

    int m_numPeaks_kal;
    std::vector<std::vector<double>> m_peaks_kal;

    int m_numPeaks_fad;
    std::vector<std::vector<double>> m_peaks_fad;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DisplayLocalisation)
};
