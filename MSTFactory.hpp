
#ifndef MSTFACTORY_HPP
#define MSTFACTORY_HPP

#include "Graph.hpp"
#include "primMST.hpp"
#include "kruskalMST.hpp"
#include <string>

enum class Command {
    Newgraph,
    Prim,
    Kruskal,
    Addedge,
    Removeedge,
    Exit,
    Invalid
};

class MSTFactory {
public:
    static void getMSTAlgorithm(Command type, int client_fd);
};

#endif

