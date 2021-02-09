#include "SourceManager.h"


SourceManager::SourceManager() {}

SourceManager::SourceManager(double dt, double num_theta) :
    m_dt(dt),
    m_numTheta(num_theta)
{
    m_numSources = 0;
    m_maxBlocksWithoutUpdate = (int)(MAX_SECONDS_WITHOUT_UPDATE / m_dt);
    m_kalmanPeaks.resize(MAX_SOURCES);
    m_kalmanPeaksWeighted.resize(MAX_SOURCES);
    for (int iSource = 0; iSource < MAX_SOURCES; iSource++) {
        m_kalmanPeaks.at(iSource).resize(2, -255.0);
        m_kalmanPeaksWeighted.at(iSource).resize(2, -255.0);
    }
    
}

std::vector<Source> SourceManager::trackSources(std::vector<std::vector<double>> peaks) {

    peaks = sortPeaks(peaks);

    std::vector<int> sourcesToDelete;

    int nPeaks = std::min(MAX_SOURCES, (int)peaks.size());

    for (int iSource = 0; iSource < MAX_SOURCES; iSource++) {
        m_kalmanPeaks.at(iSource).resize(2, -255.0);
        m_kalmanPeaksWeighted.at(iSource).resize(2, -255.0);
    }
    m_candidateList.clear();
    m_candidateList.resize(m_numSources);

    // Iterate over all found peaks
    for (int iPeak = 0; iPeak < nPeaks; iPeak++) {

        if (m_numSources == 0) {
            // No sources yet -> every peak is a potential new source

            
            std::vector<std::vector<double>> tmpResult;
            tmpResult.resize(std::min((int)peaks.size(), MAX_SOURCES));
            
            int idxtmp = 0;
            for (int iPk = 0; iPk < peaks.size(); iPk++) {
                if (peaks.at(iPk).at(1) > m_thresholdPeak && m_numSources < MAX_SOURCES) {
                    addSource(peaks.at(iPk));
                    tmpResult.at(idxtmp) = peaks.at(iPk);
                    std::vector<double> tmp = m_sources.at(m_sources.size() - 1).iterate(peaks.at(iPk));
                    m_kalmanPeaks.at(idxtmp) = tmp;
                    idxtmp++;
                }
            }

            std::vector<std::vector<double>> firstResult;
            firstResult.resize(idxtmp);
            for (int iPk = 0; iPk < idxtmp; iPk++) {
                firstResult.at(iPk) = tmpResult.at(iPk);
            }
            // Fist time a source is seen -> no notice, let's see if it holds
            firstResult.resize(0);

            // No sources but possibly some fading out
            fadeSources();

           
            firstResult.resize( m_fading.size());
            for (int iSource = 0; iSource < m_fading.size(); iSource++) {
                firstResult.at(iSource) = m_fading.at(iSource).getSourceVector();
            }

            /*std::string mess = "Sources: ";
            mess.append(std::to_string(m_sources.size()));
            mess.append(", Fading: ");
            mess.append(std::to_string(m_fading.size()));
            DBG(mess);*/

            return m_sources;

        }
        else {

            if (peaks.at(iPeak).at(1) > m_thresholdPeak) {

                // Sources exist
                std::vector<double> vProbabilities = getProbabilities(peaks.at(iPeak).at(0));
                int idx = argMax(vProbabilities, m_thresholdProbability);

                if (idx == -255) {
                    // No matching source found:
                    // Peak creates a new source
                    if (m_numSources < MAX_SOURCES) {
                        addSource(peaks.at(iPeak));
                        m_candidateList.at(m_candidateList.size() - 1).push_back(peaks.at(iPeak));
                    }
                }
                else {
                    // Matching source found:
                    // Candidate List stores all available new candidates for every source in the list
                    m_candidateList.at(idx).push_back(peaks.at(iPeak));
                }
            }
        }
    }

    // Kalman Filtering
    for (int iSource = 0; iSource < m_numSources; iSource++) {
        // Load all candidates for a source into dedicated vector
        if (m_candidateList.size() > 0) {
            m_candidates.resize(m_candidateList.at(iSource).size());
            m_candidates = m_candidateList.at(iSource);
        }
        else {
            m_candidates.at(0) = peaks.at(iSource);
        }

        if (m_candidateList.at(iSource).size() == 0) {
            // Remove spurious peaks. If a peak that was new in the last frame has no equivalent in the current - delete
            if (m_sources.at(iSource).getIsNew()) {
                sourcesToDelete.push_back(iSource);
            }
            else {
                // If it was not new, let's see if it comes back
                m_kalmanPeaks.at(iSource) = m_sources.at(iSource).noUpdate();
            }
        }
        else if (m_candidateList.at(iSource).size() == 1) {
            // Found one peak that fits
            std::vector<double> tmp = m_sources.at(iSource).iterate(m_candidates.at(0));
            m_kalmanPeaks.at(iSource) = tmp;
        }
        else {
            // Found multiple peaks in the vicinity, use a weighted average
            std::vector<double> tmp = m_sources.at(iSource).iterateWeighted(m_candidates);
            m_kalmanPeaks.at(iSource) = tmp;
        }

        // Sources with more blocks without update than MAX_BLOCKS_WITHOUT_UPDATE are deleted
        if (m_sources.at(iSource).getBlocksWithoutUpdate() >= m_maxBlocksWithoutUpdate) {
            sourcesToDelete.push_back(iSource);
        }
    }

    deleteSource(sourcesToDelete);

    fadeSources();

    std::vector<std::vector<double>> result;
    result.resize(m_numSources + m_fading.size());
    for (int iSource = 0; iSource < m_numSources; iSource++) {
        result.at(iSource) = m_kalmanPeaks.at(iSource);
    }
    for (int iSource = 0; iSource < m_fading.size(); iSource++) {
        result.at(iSource) = m_fading.at(iSource).getSourceVector();
    }

    /*std::string mess = "Sources: ";
    mess.append(std::to_string(m_sources.size()));
    mess.append(", Fading: ");
    mess.append(std::to_string(m_fading.size()));
    DBG(mess);*/

    return m_sources;
}

// TODO: Sort by strength!!
std::vector<std::vector<double>> SourceManager::sortPeaks(std::vector<std::vector<double>> peaks) {
    std::sort(peaks.begin(), peaks.end());
    return peaks;
}

int SourceManager::getNumSources() {
    return m_numSources;
}

std::vector<Source> SourceManager::getFadingSources() {
    return m_fading;
}

void SourceManager::addSource(std::vector<double> peak) {
    Source tmp(peak.at(0), peak.at(1), m_dt, m_numTheta);
    m_sources.push_back(tmp);
    m_numSources++;
    m_candidateList.push_back(std::vector<std::vector<double>>());
}

void SourceManager::fadeSources() {
    // Manage fading sources
    for (int iFading = m_fading.size() - 1; iFading >= 0; iFading--) {
        if (m_fading.at(iFading).getFadeOutCounter() > 0) {
            m_fading.at(iFading).fadeOut();
        }
        else {
            m_fading.erase(m_fading.begin() + iFading);
        }
    }
}

void SourceManager::deleteSource(std::vector<int> items) {

    std::sort(items.begin(), items.end());

    if (items.size() > 0) {
        for (int iSource = items.size() - 1; iSource >= 0; iSource--) {
            m_fading.push_back(m_sources.at(items.at(iSource)));
            m_sources.erase(m_sources.begin() + items.at(iSource)); 
            m_numSources--;
        }
    }

}

//void SourceManager::deleteSource(int item) {
//    m_sources.erase(m_sources.begin() + item);
//    m_numSources--;
//}

/*void SourceManager::KillSource(std::vector<int> items) {
    if (items.size() > 0) {
        for (int iSource = items.size() - 1; iSource >= 0; iSource--) {
            m_sources.erase(m_sources.begin() + items.at(iSource));
            m_numSources--;
        }
    }
}*/

std::vector<double>SourceManager::getProbabilities(double nCandidate) {
    std::vector<double> probabilities;
    probabilities.resize(m_numSources);

    for (int iSource = 0; iSource < m_numSources; iSource++) {
        probabilities.at(iSource) = m_sources.at(iSource).getPdf(nCandidate);
    }
    return probabilities;
}

int SourceManager::argMax(std::vector<double> inVec) {
    int arg = 0;
    double max = inVec.at(0);
    for (int idx = 0; idx < inVec.size(); idx++) {
        if (inVec.at(idx) > max) {
            max = inVec.at(idx);
            arg = idx;
        }
    }
    return arg;
}

int SourceManager::argMax(std::vector<double> inVec, double threshold) {
    int arg = -255;
    double max = 0;
    for (int idx = 0; idx < inVec.size(); idx++) {
        if (inVec.at(idx) > max && inVec.at(idx) > threshold) {
            max = inVec.at(idx);
            arg = idx;
        }
    }
    return arg;
}
