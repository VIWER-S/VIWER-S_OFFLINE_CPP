/*
  ==============================================================================

    WavWriter.h
    Created: 4 Jan 2021 5:06:55pm
    Author:  Ulrik

  ==============================================================================
*/

#pragma once

#include <fstream>
#include <iostream>
#include <vector>
#include "Constants.h"
#include "JuceHeader.h"

class WavWriter {

public:

    WavWriter::WavWriter();
    WavWriter::~WavWriter();

    bool WavWriter::init(int samplerate, int bits);
    void WavWriter::write(float in[2][HOP_SIZE]);
    void WavWriter::write(std::vector<std::vector<float>> in);
    bool WavWriter::close();
    bool WavWriter::getClear();
    bool WavWriter::isOpen();

    uint32_t WavWriter::swap32(uint32_t in);
    uint16_t WavWriter::swap16(uint16_t in);

private:

    std::ofstream m_file;
    std::string m_filename = "out.wav";
    long m_numSamples = 0;
    int m_samplerate = 0;
    int m_bits = 0;
    int m_channels = 0;
    int m_byterate = 0;
    int m_blockAlign = 0;
    int m_bitsPerSample = 0;
    int m_audioFormat = 1;
    int m_subChunkSize = 16;

    bool m_isClear = true;
};