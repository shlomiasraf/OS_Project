// KruskalMST.hpp
#ifndef KRUSKALMST_HPP
#define KRUSKALMST_HPP
#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <stack>
#include <queue>
#include "Graph.hpp"

class KruskalMST 
{
    public:
        
std::vector<std::vector<std::pair<int, int>>> kruskalFunction(Graph& adj);
};
#endif
