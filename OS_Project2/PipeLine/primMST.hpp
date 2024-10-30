// PrimMST.hpp
#ifndef PRIMMST_HPP
#define PRIMMST_HPP
#include "MSTAlgo.hpp"
class PrimMST : public MSTAlgo 
{
    public:
    std::vector<std::vector<std::pair<int, int>>> FindMST(Graph &graph) override;

};
#endif
