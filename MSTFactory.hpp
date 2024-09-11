// MSTFactory.h
#ifndef MSTFACTORY_HPP
#define MSTFACTORY_HPP

#include "Graph.hpp"
#include <functional>

enum class AlgorithmType {
    PRIM,
    KRUSKAL
};

class MSTFactory {
    public:
        static std::function<void(Graph&)> getMSTAlgorithm(AlgorithmType type);
};

#endif
