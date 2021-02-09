#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{

    m_formatManager.registerBasicFormats();

    m_ui_load.setButtonText("Load File");
    m_ui_load.addListener(this);
    addAndMakeVisible(m_ui_load);

    setSize (600, 400);
}

MainComponent::~MainComponent()
{


}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    m_ui_load.setBounds(10, 10, 60, 20);

    if (m_sample != 0) {
        float wid = ((float) getWidth() - 20) / m_numBlocks;
        float hei = ((float) getHeight() - 80) / m_numChannels;

        for (int iBlock = 0; iBlock < m_numBlocks; iBlock++) {
            for (int iChannel = 0; iChannel < m_numChannels; iChannel++) {

                unsigned char col = (m_levels.at(iChannel).at(iBlock) - minLevel) * 255 / (maxLevel - minLevel);
                g.setColour(juce::Colour(col, col, col, 1.0f));

                juce::Rectangle<float> tmp(10 + iBlock * wid, 40 + iChannel * hei, wid, hei);
                g.fillRect(tmp);

            }
        }
    }

}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.

}

void MainComponent::callbackOpen()
{

    m_buff.clear();

    //choose a file
    juce::FileChooser chooser("Choose a Wav or AIFF File", juce::File::getSpecialLocation(juce::File::userDesktopDirectory), "*.wav; *.mp3");

    //if the user chooses a file
    if (chooser.browseForFileToOpen())
    {
        juce::File myFile;

        //what did the user choose?
        myFile = chooser.getResult();

        //read the file
        juce::AudioFormatReader* reader = m_formatManager.createReaderFor(myFile);

        if (reader != nullptr)
        {
            //get the file ready to play
            std::unique_ptr<juce::AudioFormatReaderSource> tempSource(new juce::AudioFormatReaderSource(reader, true));

            m_transport.setSource(tempSource.get());
            m_playSource.reset(tempSource.release());
            m_samplerate = reader->sampleRate;
            m_numSamples = reader->lengthInSamples;
            m_numChannels = reader->numChannels;
            m_sample = 0;
            m_numBlocks = floor(m_numSamples / BUFFER_SIZE);
            m_block = 0;

            m_buff.setSize((int)reader->numChannels, (int)reader->lengthInSamples); 
            reader->read(&m_buff, 0, m_numSamples, 0, true, true);

            m_levels.resize(m_numChannels);
            for (int iChannel = 0; iChannel < m_numChannels; iChannel++) {
                m_levels.at(iChannel).resize(m_numBlocks, 0.0f);
            }

            loop();
        }
    }
   
}

void MainComponent::loop() {
    while (m_sample < m_numSamples - BUFFER_SIZE) {
    


        //DBG(m_buffer.buffer->getNumSamples());

        for (int iChannel = 0; iChannel < m_numChannels; iChannel++) {
            const float* tmp = m_buff.getReadPointer(iChannel, m_sample);
            m_levels.at(iChannel).at(m_block) = rmsLevel(tmp, BUFFER_SIZE);

            if (m_levels.at(iChannel).at(m_block) < minLevel) {
                minLevel = m_levels.at(iChannel).at(m_block);
            }
            else if (m_levels.at(iChannel).at(m_block) > maxLevel) {
                maxLevel = m_levels.at(iChannel).at(m_block);
            }
        }

        m_sample += BUFFER_SIZE;
        m_block++;
        
    }

    repaint();
    DBG("finished.");
}



void MainComponent::process() {
    //File file("d:/my.wav");
    //AudioFormatManager formatManager;
    //formatManager.registerBasicFormats();
    //ScopedPointer<AudioFormatReader> reader = formatManager.createReaderFor(file);
    //if (reader != 0)
    {
        //AudioSampleBuffer buffer(reader->numChannels, reader->lengthInSamples);
        //buffer.read(reader, 0, reader->lengthInSamples, 0, true, true);
        //float* firstChannelSamples = buffer.getSampleData(0, 0);
    }
}

double MainComponent::rmsLevel(const float* inBuffer, int numSamples) {

    double sum = 0;

    for (int iSample = 0; iSample < numSamples; iSample++) {
        sum += inBuffer[iSample] * inBuffer[iSample];
    }
    sum /= numSamples;
    sum = sqrt(sum);
    sum = juce::Decibels::gainToDecibels(sum);

    return sum;
}
