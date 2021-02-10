/*
  ==============================================================================

    WavWriter.cpp
    Created: 4 Jan 2021 5:06:55pm
    Author:  Ulrik

  ==============================================================================
*/

#include "WavWriter.h"

WavWriter::WavWriter() {}

WavWriter::~WavWriter() {}

bool WavWriter::init(int samplerate, int bits, std::string name) {

    if (m_isClear) {

        m_isClear = false;

        //DBG("Initialising");

        m_samplerate = samplerate;
        m_bits = bits;
        m_channels = 2;

        m_numSamples = 0;

        m_byterate = m_samplerate * m_bits * m_channels / 8; // (Sample Rate * BitsPerSample * Channels) / 8
        m_blockAlign = m_channels * m_bits / 8; // data block size (size of two integer samples, one for each channel, in bytes)

        m_file.open(name, std::ios::out | std::ios::binary);

        if (!m_file) {
            DBG("Cannot open file!");
        }
        else {
            m_file.write("RIFF", 4);
            m_file.write("----", 4);
            m_file.write("WAVE", 4);
            m_file.write("fmt ", 4);
            m_file.write((char*)(&m_subChunkSize), 4);
            m_file.write((char*)(&m_audioFormat), 2);
            m_file.write((char*)(&m_channels), 2); // number of channels
            m_file.write((char*)(&m_samplerate), 4); // sample rate
            m_file.write((char*)(&m_byterate), 4);
            m_file.write((char*)(&m_blockAlign), 2);
            m_file.write((char*)(&m_bits), 2); // number of bits per sample (use a multiple of 8)
            m_file.write((char*)"data", 4);
            m_file.write("----", 4);
        }
        m_isClear = true;
        return true;
    } else { 
        return false; 
    }
}

void WavWriter::write(float in[2][HOP_SIZE]) {

    if (m_isClear) {

        m_isClear = false;

        //DBG("Recording");

        for (int iSample = 0; iSample < HOP_SIZE; iSample++)
        {
            for (int iChannel = 0; iChannel < m_channels; iChannel++) {
                short sample = floor(in[iChannel][iSample] * 32767 / 50);
                m_file.write((char*)(&sample), 2);
            }

            m_numSamples++;
        }

        m_isClear = true;
    }
}


void WavWriter::write(std::vector<std::vector<float>> in) {

    if (m_isClear) {

        m_isClear = false;

        //DBG("Recording");

        for (int iSample = 0; iSample < HOP_SIZE; iSample++)
        {
            for (int iChannel = 0; iChannel < m_channels; iChannel++) {
                short sample = floor(in.at(iChannel).at(iSample) * 32767 / 50);
                m_file.write((char*)(&sample), 2);
                
                m_numSamples++;
            }

            
        }

        m_isClear = true;
    }
}

bool WavWriter::close() {

    if (m_isClear) {

        m_isClear = false;

        DBG("Closing");

        long file_length = m_file.tellp();
        m_file.seekp(0 + 4);
        int chunkSize = file_length - 8;
        m_file.write((char*)(&chunkSize), 4);
        m_file.seekp(0 + 40);
        int subChunk2Size = m_numSamples * 4;
        m_file.write((char*)(&subChunk2Size), 4);
        m_file.close();

        std::string mess = "";
        mess.append(std::to_string(m_numSamples));
        mess.append(" frames written to file: ");
        mess.append(m_filename);
        DBG(mess);

        m_isClear = true;
        return true;
    }
    else {
        return false;
    }
}


bool WavWriter::isOpen() {
    return m_file.is_open();
}

bool WavWriter::getClear() {
    return m_isClear;
}

uint32_t WavWriter::swap32(uint32_t in)
{
    return  ((in & 0xFF000000) >> 24) | ((in & 0x00FF0000) >> 8) |
        ((in & 0x0000FF00) << 8) | ((in & 0x000000FF) << 24);
}

uint16_t WavWriter::swap16(uint16_t in)
{
    return (in >> 8) | (in << 8);
}
