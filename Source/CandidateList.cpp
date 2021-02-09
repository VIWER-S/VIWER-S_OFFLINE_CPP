#include "CandidateList.h"


CandidateList::CandidateList() {}

 void CandidateList::add(std::vector<double> entry) {
    m_list.push_back(entry);
}

 std::vector<double> CandidateList::get(int idx) {
    return m_list.at(idx);
}

 int CandidateList::size() {
    return m_list.size();
}
