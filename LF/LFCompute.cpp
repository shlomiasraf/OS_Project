#include "LFCompute.hpp"

LFCompute::LFCompute() { }

void LFCompute::Compute(Graph &graph, int clientfd, std::string type) {
    MSTResult result({}, 0, 0, 0);  // Initialize with an empty adjacency list

    // Enqueue a single task that performs all the operations sequentially
    pool.enqueue([&graph, &result, clientfd, type]() {
        // Task: Calculate MST using Kruskal or Prim
        if (type == "Kruskal") {
            std::cout << "Calculating MST Using Kruskal Algo..." << std::endl;
            KruskalMST kruskalInstance;
            result.mst = kruskalInstance.kruskalFunction(graph);
        } else {
            std::cout << "Calculating MST Using Prim Algo..." << std::endl;
            PrimMST primInstance;
            result.mst = primInstance.primFunction(graph);
        }
        std::cout << "MST calculation complete." << std::endl;

        // Task: Calculate total weight
        result.totalWeight = MSTInfo::calculateTotalWeight(result.mst, result.mst.size());
        std::cout << "Total weight calculation complete." << std::endl;

        // Task: Calculate average distance
        result.averageDistance = MSTInfo::calculateAverageDistance(result.totalWeight, result.mst.size());
        std::cout << "Average distance calculation complete." << std::endl;

        // Task: Calculate longest distance
        result.longestDistance = MSTInfo::findLongestDistance(result.mst, result.mst.size());
        std::cout << "Longest distance calculation complete." << std::endl;

        // Task: Calculate shortest distance
        result.shortestDistance = MSTInfo::calculateShortestDistance(result.mst, result.mst.size());
        std::cout << "Shortest distance calculation complete." << std::endl;

        // Task: Send MST details to the client
        result.sendMSTDetails(clientfd);
        std::cout << "MST details sent to client." << std::endl;
    });
}
