// KruskalMST.hpp
#ifndef KRUSKALMST_HPP
#define KRUSKALMST_HPP
#include "MSTAlgo.hpp"
class KruskalMST : public MSTAlgo
{

public:
std::vector<std::vector<std::pair<int, int>>> FindMST(Graph &graph) override;
};
#endif
