#include "LFCompute.hpp"
#include "MSTFactory.hpp"
LFCompute* LFCompute::instance = nullptr;
std::mutex LFCompute::creation;


LFCompute::LFCompute() : pool(10){};

LFCompute &LFCompute::getInstance()
{
    
  std::unique_lock<std::mutex> lock(creation);
    if (instance == nullptr) {
        instance = new LFCompute(); // Assign new instance if null
    }
    return *instance; // Return the instance}
    
}

void LFCompute::Compute(Graph &graph, int clientfd, Command type)
{
   
    MSTResult result({}, 0, 0, 0);  
    // Enqueue a single task that performs all the operations sequentially
    pool.enqueue([this, &graph, clientfd, type,result = std::move(result)]() mutable {        std::string message;
        
        message = "Calculating MST...";
        threadSafePrint(message);
        auto mstAlgo = MSTFactory::create(type); // Assuming this returns a unique_ptr<MSTAlgo>
        // Calculate MST using the provided MSTAlgo instance
        result.mst = mstAlgo->FindMST(graph);
    
        // Task: Calculate total weight
        result.totalWeight = MSTInfo::calculateTotalWeight(result.mst, result.mst.size());
        message = "Total weight calculation complete.";
        threadSafePrint(message);

        // Task: Calculate average distance
        result.averageDistance = MSTInfo::calculateAverageDistance(result.totalWeight, result.mst.size());
        message = "Average distance calculation complete.";
        threadSafePrint(message);

        // Task: Calculate longest distance
        result.longestDistance = MSTInfo::findLongestDistance(result.mst, result.mst.size());
        message = "Longest distance calculation complete.";
        threadSafePrint(message);

        // Task: Calculate shortest distance
        result.shortestDistance = MSTInfo::calculateShortestDistance(result.mst, result.mst.size());
        message = "Shortest distance calculation complete.";
        threadSafePrint(message);

        // Task: Send MST details to the client
        result.sendMSTDetails(clientfd);
        message = "MST details sent to client.";
        threadSafePrint(message);
        taskCompleted.store(true);

        // Notify client
        {
            std::unique_lock<std::mutex> lock(clientMutex);
            clientCv.wait(lock, [&] { return taskCompleted.load() == true; }); // Wait until the task is completed
        }
    });
}


void LFCompute::threadSafePrint(const std::string &message)
{
    std::lock_guard<std::mutex> lock(coutMutex);
    std::cout << message << std::endl;
}

LFCompute::~LFCompute() {
    cleanup();  // Ensure cleanup when LFCompute is destroyed
}

void LFCompute::cleanup() {
    if (isCleaningUp) return; // Prevent recursive cleanup
    isCleaningUp = true;
    pool.shutdown();  // Ensure the thread pool stops accepting new tasks and completes ongoing ones
    delete instance;
    instance = nullptr;
}
