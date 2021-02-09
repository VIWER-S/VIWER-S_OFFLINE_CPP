#pragma once

#include <vector>

#include "Constants.h"
#include "Kalman.h"

class Source
{

    public:

        Source::Source(double angle, double strength, double dt, double nSpread);
        std::vector<double> Source::iterate(std::vector<double> nCandidate);
	    std::vector<double> Source::iterateWeighted(std::vector<std::vector<double>> candidates);
        std::vector<double> Source::noUpdate();
        std::vector<double> Source::fadeOut();
        std::vector<double> Source::getSourceVector();
        bool Source::getIsNew();
        int Source::getBlocksWithoutUpdate();
        int Source::getFadeOutCounter();
        double Source::getAngle();
        void Source::setAngle(double angle);
        double Source::getStrength();
        double Source::getPdf(double x);
        static double Source::pdf(double x);
        static double Source::pdf(double x, double mu, double sigma);
        double Source::getFadeFactor();
       
    private:

        Kalman m_Kalman;
        double m_angle;
        double m_spread;
        double m_strength = 0.0;
        double m_acceptanceProbability = 1.0; //4.0
        int m_blocksWithoutUpdate = 0;
        double m_spatialSmoothingConstant = 0.85; //0.85
        double m_alpha_strength = 0.95;
        bool isNew;
        int m_fadeCounter = 0;
        double m_fadeFrames = 0.0;

};

