#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{

    m_formatManager.registerBasicFormats();

    m_ui_load.setButtonText("Load File");
    m_ui_load.addListener(this);
    addAndMakeVisible(m_ui_load);

    setSize (600, 400);

    for (int iChannel = 0; iChannel < SENSORS; iChannel++) {
        for (int iSample = 0; iSample < BUFFER_SIZE; iSample++) {
            m_inBuffer[iChannel][iSample] = 0.0;
        }
    }

    m_channelOrder.resize(SENSORS, 0);
    m_channelOrder = { 1, 0, 3, 2, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14 };

    for (int iSource = 0; iSource < MAX_SOURCES; iSource++) {
        m_directions_kal[iSource] = -255.0;
        m_Importance_kal[iSource] = -255.0;
        m_directions_fad[iSource] = -255.0;
        m_Importance_fad[iSource] = -255.0;
    }

    m_Viwer.setDirectionBuffer(nullptr, m_ptrLoc_kal, m_ptrLoc_fad);
    m_Viwer.setImportanceBuffer(nullptr, m_ptrImp_kal, m_ptrImp_fad);
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

        // Draw levels

        float wid = ((float) getWidth() - 20) / m_numBlocks;
        float hei = ((float) 100) / m_numChannels;

        for (int iBlock = 0; iBlock < m_numBlocks; iBlock++) {
            for (int iChannel = 0; iChannel < m_numChannels; iChannel++) {

                unsigned char col = (m_levels.at(iChannel).at(iBlock) - minLevel) * 255 / (maxLevel - minLevel);
                g.setColour(juce::Colour(col, col, col, 1.0f));

                juce::Rectangle<float> tmp(10 + iBlock * wid, 40 + iChannel * hei, wid, hei);
                g.fillRect(tmp);

            }
        }

        // Draw sources

        hei = ((float)100) / (MAX_THETA / STEP_THETA);
        
        for (int iBlock = 0; iBlock < m_numBlocks; iBlock++) {
            for (int iSource = 0; iSource < MAX_SOURCES; iSource++) {
                if (m_kalman_dir.at(iBlock).at(iSource) != -255) {
                    g.setColour(juce::Colour(m_kalman_imp.at(iBlock).at(iSource) * 255, 0, 0));
                    g.drawRect(10 + iBlock * wid, 150 + m_kalman_dir.at(iBlock).at(iSource) * hei, wid, 1.0f);
                }
                if (m_fading_dir.at(iBlock).at(iSource) != -255) {
                    g.setColour(juce::Colour(100, 100, m_fading_imp.at(iBlock).at(iSource) * 255));
                    g.drawRect(10 + iBlock * wid, 150 + m_fading_dir.at(iBlock).at(iSource) * hei, wid, 1.0f);
                }
            }
        }

        // Draw wave left

        wid = ((float)getWidth() - 20) / m_numSamples;

        g.setColour(juce::Colours::white);

        juce::Path array;
        array.startNewSubPath(10, 285);
        g.fillPath(array);
        for (int iSample = 0; iSample < m_numSamples; iSample++) {
            array.lineTo(10 + iSample * wid, 285 + 25 * m_processed_complete.at(0).at(iSample));
        }
        array.closeSubPath();
        g.fillPath(array);

        // Draw wave right

        array.startNewSubPath(10, 345);
        g.fillPath(array);
        for (int iSample = 0; iSample < m_numSamples; iSample++) {
            array.lineTo(10 + iSample * wid, 345 + 25 * m_processed_complete.at(1).at(iSample));
        }
        array.closeSubPath();
        g.fillPath(array);
    }

    // Borders
    g.setColour(juce::Colours::white);
    // Levels
    g.drawRect(9, 39, getWidth() - 18, 102, 1.0);
    // Source directions
    g.drawRect(9, 149, getWidth() - 18, 102, 1.0);
    // Wave output
    g.drawRect(9, 259, getWidth() - 18, 52, 1.0);
    g.drawRect(9, 319, getWidth() - 18, 52, 1.0);
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
            m_kalman_dir.resize(m_numBlocks);
            m_kalman_imp.resize(m_numBlocks);
            m_fading_dir.resize(m_numBlocks);
            m_fading_imp.resize(m_numBlocks);
            for (int iBlock = 0; iBlock < m_numBlocks; iBlock++) {
                m_kalman_dir.at(iBlock).resize(MAX_SOURCES, -255.0f);
                m_kalman_imp.at(iBlock).resize(MAX_SOURCES, -255.0f);
                m_fading_dir.at(iBlock).resize(MAX_SOURCES, -255.0f);
                m_fading_imp.at(iBlock).resize(MAX_SOURCES, -255.0f);
            }

            m_processed_complete.resize(2);
            m_processed_complete.at(0).resize(m_numSamples, 0.0f);
            m_processed_complete.at(1).resize(m_numSamples, 0.0f);

            m_WavWriter.init(m_samplerate, 16, myFile.getFileNameWithoutExtension().toStdString().append("_processed.wav"));

            loop();
        }
    }
   
}

void MainComponent::loop() {

    while (m_sample < m_numSamples - BUFFER_SIZE) {

        for (int iChannel = 0; iChannel < m_numChannels; iChannel++) {
            const float* tmp = m_buff.getReadPointer(iChannel, m_sample);
            m_levels.at(iChannel).at(m_block) = rmsLevel(tmp, BUFFER_SIZE);

            if (m_levels.at(iChannel).at(m_block) < minLevel && iChannel != 5) {
                minLevel = m_levels.at(iChannel).at(m_block);
            }
            else if (m_levels.at(iChannel).at(m_block) > maxLevel && iChannel != 5) {
                maxLevel = m_levels.at(iChannel).at(m_block);
            }
        }


        int sensor = 0;
        for (int iChan : m_channelOrder)
        {
            const float* inBuffer = m_buff.getReadPointer(iChan, m_sample);
            for (int iSample = 0; iSample < BUFFER_SIZE; iSample++)
            {
                m_inBuffer[sensor][iSample] = (double)inBuffer[iSample];
            }
            sensor++;
        }

        m_processed = m_Viwer.process(m_inBuffer);

        for (int iSource = 0; iSource < MAX_SOURCES; iSource++) {
            m_kalman_dir.at(m_block).at(iSource) = *(m_ptrLoc_kal + iSource);
            m_kalman_imp.at(m_block).at(iSource) = *(m_ptrImp_kal + iSource);
            m_fading_dir.at(m_block).at(iSource) = *(m_ptrLoc_fad + iSource);
            m_fading_imp.at(m_block).at(iSource) = *(m_ptrImp_fad + iSource);
        }

        for (int iChannel = 0; iChannel < 2; iChannel++) {

            for (int iSample = 0; iSample < BUFFER_SIZE; iSample++) {
              
                if (iChannel == 0) {
                    m_processed_complete.at(0).at(m_sample + iSample) = m_processed.at(iChannel).at(iSample) / 50;
                    
                }
                else {
                    m_processed_complete.at(1).at(m_sample + iSample) = m_processed.at(iChannel).at(iSample) / 50;
                }
            }
        }

        m_WavWriter.write(m_processed);
        
        m_sample += BUFFER_SIZE;
        m_block++;
        
    }

    m_WavWriter.close();

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
