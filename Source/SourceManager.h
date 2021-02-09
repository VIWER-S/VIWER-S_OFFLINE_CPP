#pragma once

#define _USE_MATH_DEFINES

#include <vector>
#include <cmath>
#include <set>
#include <iostream>

#include "JuceHeader.h"
#include "Source.h"
#include "CandidateList.h"




class SourceManager
{
    public:

        SourceManager::SourceManager();
        SourceManager::SourceManager(double dt, double num_theta);
        std::vector<Source> SourceManager::trackSources(std::vector<std::vector<double>> peaks);
        std::vector<std::vector<double>> SourceManager::sortPeaks(std::vector<std::vector<double>> peaks);
        int SourceManager::getNumSources();
        std::vector<Source> SourceManager::getFadingSources();
        void SourceManager::addSource(std::vector<double> peak);
        //void SourceManager::fadeSources(std::vector<int> peak);
        //void SourceManager::deleteSource(int items);
        void SourceManager::fadeSources();
        void SourceManager::deleteSource(std::vector<int> items);
        //void SourceManager::KillSource(std::vector<int> items);
        std::vector<double>SourceManager::getProbabilities(double nCandidate);
        int SourceManager::argMax(std::vector<double> inVec);
        int SourceManager::argMax(std::vector<double> inVec, double threshold);
    
    private:

        int m_maxBlocksWithoutUpdate;
        int m_numSources;

        std::vector<Source> m_sources;
        std::vector<Source> m_fading;
        std::vector<std::vector<std::vector<double>>> m_candidateList;

        double m_dt;
        double m_numTheta;
        double m_thresholdProbability = 0.01;
        double m_thresholdPeak_dB = -40.0; //-20.0
        double m_thresholdPeak = pow(10, m_thresholdPeak_dB / 20);

        std::vector<std::vector<double>> m_kalmanPeaks;
        std::vector<std::vector<double>> m_kalmanPeaksWeighted;
        std::vector<std::vector<double>> m_candidates;

};

