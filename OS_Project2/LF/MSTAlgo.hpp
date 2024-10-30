#ifndef MSTALGO_H
#define MSTALGO_H

#include "Graph.hpp"
#include "MSTInfo.hpp"
#include <vector>
#include <utility>

class MSTAlgo {
public:
    MSTAlgo() = default;
    virtual ~MSTAlgo() = default;
    virtual std::vector<std::vector<std::pair<int, int>>> FindMST(Graph &graph) = 0;
};

#endif // MSTALGO_H
