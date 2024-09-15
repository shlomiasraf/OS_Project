#ifndef MSTFACTORY_HPP
#define MSTFACTORY_HPP

#include <functional>
#include "Graph.hpp"

enum class Command
{
    Newgraph,
    Prim,
    Kruskal,
    Newedge,
    Removeedge,
    Exit,
    Invalid
};
class MSTFactory 
{
public:
    static std::function<void(Graph&)> 
    getMSTAlgorithm(Command type);
};

#endif
