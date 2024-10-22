// PrimMST.hpp
#ifndef PRIMMST_HPP
#define PRIMMST_HPP

#include "Graph.hpp"
class PrimMST 
{
    public:
      std::vector<std::vector<std::pair<int, int>>>  primFunction(Graph& graph); 
};
#endif
