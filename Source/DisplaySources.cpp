/*
  ==============================================================================

    DisplaySources.cpp
    Created: 23 Nov 2020 7:21:46pm
    Author:  Ulrik

  ==============================================================================
*/

#include "DisplaySources.h"


DisplaySources::DisplaySources() {

    m_label_sources.setText("Sources: ", juce::NotificationType::dontSendNotification);
    m_label_sources.setColour(juce::Label::ColourIds::textColourId, juce::Colours::red);
    addAndMakeVisible(m_label_sources);
    m_label_fading.setText("Fading: ", juce::NotificationType::dontSendNotification);
    m_label_fading.setColour(juce::Label::ColourIds::textColourId, juce::Colours::blue);
    addAndMakeVisible(m_label_fading);
    m_box_sources.setText("0", juce::NotificationType::dontSendNotification);
    m_box_sources.setColour(juce::Label::ColourIds::textColourId, juce::Colours::red);
    addAndMakeVisible(m_box_sources);
    m_box_fading.setText("0", juce::NotificationType::dontSendNotification);
    m_box_fading.setColour(juce::Label::ColourIds::textColourId, juce::Colours::blue);
    addAndMakeVisible(m_box_fading);

}

DisplaySources::~DisplaySources() {};

void DisplaySources::setLoc(double* loc, double* loc_kal, double* loc_fad) {
    m_ptrLoc = loc;
    m_ptrLoc_kal = loc_kal;
    m_ptrLoc_fad = loc_fad;
}

void DisplaySources::setImp(double* imp, double* imp_kal, double* imp_fad) {
    m_ptrImp = imp;
    m_ptrImp_kal = imp_kal;
    m_ptrImp_fad = imp_fad;
}

int DisplaySources::getHeight() {
    return m_height;
}


int DisplaySources::getWidth() {
    return m_width;
}

void DisplaySources::paint(juce::Graphics& g) {

    double pk, imp;

    m_numPeaks_kal = 0;
    for (int iPeak = 0; iPeak < MAX_SOURCES; iPeak++) {
        pk = *(m_ptrLoc_kal + iPeak);
        imp = *(m_ptrImp_kal + iPeak);
        if (pk != -255.0) {
            m_numPeaks_kal++;
        }
    }

    m_numPeaks_fad = 0;
    for (int iPeak = 0; iPeak < MAX_SOURCES; iPeak++) {
        pk = *(m_ptrLoc_fad + iPeak);
        imp = *(m_ptrImp_fad + iPeak);
        if (pk != -255.0) {
            m_numPeaks_fad++;
        }
    }


    m_box_sources.setText(std::to_string(m_numPeaks_kal), juce::NotificationType::dontSendNotification);
    m_box_fading.setText(std::to_string(m_numPeaks_fad), juce::NotificationType::dontSendNotification);

    m_label_sources.setBounds(0, 0, 70, 25);
    m_box_sources.setBounds(70, 0, 20, 25);
    m_label_fading.setBounds(0, 30, 70, 25);
    m_box_fading.setBounds(70, 30, 20, 25);

}

void DisplaySources::resized() {};