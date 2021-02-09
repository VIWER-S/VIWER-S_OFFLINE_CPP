#pragma once

#include "Constants.h"
#include "JuceHeader.h"


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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
