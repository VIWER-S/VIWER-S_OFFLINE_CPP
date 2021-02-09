#pragma once

#include <vector>

class CandidateList
{

    public:

        CandidateList::CandidateList();
        void CandidateList::add(std::vector<double> entry);
        std::vector<double> CandidateList::get(int idx);
        int CandidateList::size();

    private:

        std::vector<std::vector<double>> m_list;

};




