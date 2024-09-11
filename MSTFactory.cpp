#include "MSTFactory.hpp"
#include "primMST.hpp"
#include "kruskalMST.hpp"
#include "Graph.hpp"
#include <functional>

std::function<void(Graph&)> MSTFactory::getMSTAlgorithm(AlgorithmType type) 
{
    PrimMST primInstance;
    KruskalMST kruskalInstance;

    switch (type) 
    {
        case AlgorithmType::PRIM:
            return [&primInstance](Graph& graph) { primInstance.primFunction(graph); };
        case AlgorithmType::KRUSKAL:
            return [&kruskalInstance](Graph& graph) { kruskalInstance.kruskalfunction(graph); };
        default:
            return nullptr;
    }
}
