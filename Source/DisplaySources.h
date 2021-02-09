/*
  ==============================================================================

    DisplaySources.h
    Created: 23 Nov 2020 7:21:46pm
    Author:  Ulrik

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "Constants.h"


class DisplaySources : public juce::Component 
{

    public:

        DisplaySources::DisplaySources();
        DisplaySources::~DisplaySources();

        int DisplaySources::getWidth();
        int DisplaySources::getHeight();

        void DisplaySources::paint(juce::Graphics& g);
        void DisplaySources::resized();

        void DisplaySources::setLoc(double* loc, double* loc_kal, double* loc_fad);
        void DisplaySources::setImp(double* imp, double* imp_kal, double* imp_fad);

    private:

        float m_width = 120;
        float m_height = 60;

        juce::Label m_label_sources, m_label_fading, m_box_sources, m_box_fading;

        // Pointers to convey found directions from Viwer to DisplayLocalisation
        double* m_ptrLoc;
        double* m_ptrImp;
        // Pointers to convey tracked sources from Viwer to DisplayLocalisation
        double* m_ptrLoc_kal;
        double* m_ptrImp_kal;
        // Pointers to convey fading sources from Viwer to DisplayLocalisation
        double* m_ptrLoc_fad;
        double* m_ptrImp_fad;

        int m_numPeaks;
        std::vector<std::vector<double>> m_peaks;

        int m_numPeaks_kal;
        std::vector<std::vector<double>> m_peaks_kal;

        int m_numPeaks_fad;
        std::vector<std::vector<double>> m_peaks_fad;
};