#pragma once

#include "Constants.h"
#include "JuceHeader.h"
#include "Viwer.h"
#include "WavWriter.h"


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::Component,
                       public juce::Button::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:

    void MainComponent::buttonClicked(juce::Button* button) {
        if (button == &m_ui_load) {
            callbackOpen();
        }
    }

    void MainComponent::callbackOpen();
    void MainComponent::loop();
    void MainComponent::process();
    double MainComponent::rmsLevel(const float* inBuffer, int numSamples);

    juce::TextButton m_ui_load;

    int m_samplerate = 0;
    int m_numSamples = 0;
    int m_numChannels = 0;
    int m_sample = 0;
    int m_numBlocks = 0;
    int m_block = 0;

    float minLevel = 100; 
    float maxLevel = 0;

    std::vector<std::vector<float>> m_levels;

    juce::AudioFormatManager m_formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> m_playSource;
    juce::AudioTransportSource m_transport;
    juce::AudioSampleBuffer m_buff;

    Viwer m_Viwer;
    std::vector<int> m_channelOrder;
    std::vector<std::vector<float>> m_processed;
    std::vector<std::vector<float>> m_processed_complete;
    float m_audioOut_L[BUFFER_SIZE];
    float m_audioOut_R[BUFFER_SIZE];
    double m_inBuffer[SENSORS][BUFFER_SIZE];
    float m_record[2][BUFFER_SIZE];

    // Pointer arrays to convey tracked sources from Viwer to DisplayLocalisation
    double m_directions_kal[MAX_SOURCES];
    double* m_ptrLoc_kal = m_directions_kal;
    double m_Importance_kal[MAX_SOURCES];
    double* m_ptrImp_kal = m_Importance_kal;
    // Pointer arrays to convey fading sources from Viwer to DisplayLocalisation
    double m_directions_fad[MAX_SOURCES];
    double* m_ptrLoc_fad = m_directions_fad;
    double m_Importance_fad[MAX_SOURCES];
    double* m_ptrImp_fad = m_Importance_fad;

    std::vector<std::vector<float>> m_kalman_dir;
    std::vector<std::vector<float>> m_kalman_imp;
    std::vector<std::vector<float>> m_fading_dir;
    std::vector<std::vector<float>> m_fading_imp;

    WavWriter m_WavWriter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
