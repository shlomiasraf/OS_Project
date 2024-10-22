#include "Pipeline.hpp"
Pipeline::Pipeline() : executors(6) { }

void Pipeline::run(Graph& graph, int clientfd, std::string type) {

  // Assign references to each executor
    ActiveObject& calculateMSTExecutor = executors[0];
    ActiveObject& calculateTotalWeightExecutor = executors[1];
    ActiveObject& calculateLongestDistanceExecutor = executors[2];
    ActiveObject& calculateAverageDistanceExecutor = executors[3];
    ActiveObject& calculateShortestDistanceExecutor = executors[4];
    ActiveObject& sendInfoExecutor = executors[5];

    // Start all ActiveObject threads using a loop
    for (auto& executor : executors) {
        executor.start();
    }
    MSTResult result({}, 0, 0, 0);  // Initialize with an empty adjacency list   
    // Task 1: Calculate MST using Kruskal or Prim
    calculateMSTExecutor.enqueue([&graph,clientfd,&result, type, &calculateTotalWeightExecutor,&calculateAverageDistanceExecutor,
     &calculateLongestDistanceExecutor, &calculateShortestDistanceExecutor, &sendInfoExecutor]() {
        if (type == "Kruskal") {
            std::cout << "Calculating MST Using Kruskal Algo..." << std::endl;
            KruskalMST kruskalInstance;
            result.mst = kruskalInstance.kruskalFunction(graph);
        } 
        else {
            std::cout << "Calculating MST Using Prim Algo..." << std::endl;
            PrimMST primInstance;
            result.mst = primInstance.primFunction(graph);
        }
        std::cout << "Finish task 1" << std::endl;

        // Enqueue Task 2 after Task 1 finishes
        calculateTotalWeightExecutor.enqueue([&result, &calculateAverageDistanceExecutor, &calculateLongestDistanceExecutor, &calculateShortestDistanceExecutor, &sendInfoExecutor, clientfd]() {
            result.totalWeight = MSTInfo::calculateTotalWeight(result.mst, result.mst.size());
            std::cout << "Finish task 2" << std::endl;

            // Enqueue Task 3 after Task 2 finishes
            calculateAverageDistanceExecutor.enqueue([&result, &calculateLongestDistanceExecutor, &calculateShortestDistanceExecutor, &sendInfoExecutor, clientfd]() {
                result.averageDistance = MSTInfo::calculateAverageDistance(result.totalWeight, result.mst.size());
                std::cout << "Finish task 3" << std::endl;

                // Enqueue Task 4 after Task 3 finishes
                calculateLongestDistanceExecutor.enqueue([&result, &calculateShortestDistanceExecutor, &sendInfoExecutor, clientfd]() {
                    result.longestDistance = MSTInfo::findLongestDistance(result.mst, result.mst.size());
                    std::cout << "Finish task 4" << std::endl;

                    // Enqueue Task 5 after Task 4 finishes
                    calculateShortestDistanceExecutor.enqueue([&result, &sendInfoExecutor, clientfd]() { 
                        result.shortestDistance = MSTInfo::calculateShortestDistance(result.mst, result.mst.size());
                        std::cout << "Finish task 5" << std::endl;

                        // Enqueue Task 6 after Task 5 finishes
                        sendInfoExecutor.enqueue([result, clientfd]() { 
                            result.sendMSTDetails(clientfd);
                            std::cout << "Finish task 6" << std::endl;
                        });
                    });
                });
            });
        });
    });
    // Stop all ActiveObject threads after processing using a loop
    for (auto& executor : executors) {
        executor.stop();
    }
    std::cout << "Pipe Finish\n";
}
