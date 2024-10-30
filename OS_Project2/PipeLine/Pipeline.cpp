#include "Pipeline.hpp"
#include "MSTFactory.hpp"

Pipeline* Pipeline::instance = nullptr;
std::mutex Pipeline::creation;

Pipeline& Pipeline::getInstance() {
    std::unique_lock<std::mutex> lock(creation);
    if (instance == nullptr) {
        instance = new Pipeline(); // Assign new instance if null
    }
    return *instance; // Return the instance
}

Pipeline::Pipeline() {
    // Emplace back to create instances of ActiveObject
    for (int i = 0; i < 6; ++i) {
        executors.emplace_back(std::make_unique<ActiveObject>());
    }
    // Start each executor
    for (auto& executor : executors) {
        executor->start();
    }
}
void Pipeline::cleanup() {
    std::unique_lock<std::mutex> lock(creation);
    delete instance;
    instance = nullptr;
}

Pipeline::~Pipeline() {
    for (auto& executor : executors) {
        executor->stop(); // Assuming you have a stop method
    }
}

void Pipeline::threadSafePrint(const std::string& message) {
    std::lock_guard<std::mutex> lock(coutMutex);
    std::cout << message << std::endl;
}

void Pipeline::run(Graph& graph, int clientfd,Command type) {
    MSTResult result({}, 0, 0, 0);
    // Create a mutex for this specific client
    std::mutex clientMutex;
    std::condition_variable clientCv;
    std::atomic<int> completedTasks{0};

    // Notify and increment
    auto notifyAndIncrement = [&]() {
        std::lock_guard<std::mutex> lock(clientMutex);
        completedTasks++;
        clientCv.notify_all();
    };

    // Vector to hold task completion status
    std::vector<bool> taskCompletionStatus(6, false); // Assuming 6 tasks

    auto passTask = [&](std::function<void()> taskFunction, int dependencyIndex = -1) {
        if (dependencyIndex != -1) {
            std::unique_lock<std::mutex> lock(clientMutex);
            clientCv.wait(lock, [&] { return completedTasks.load() > dependencyIndex; });
        }

        // Directly call the task function instead of using std::async
        taskFunction();
        notifyAndIncrement();
    };

    // Task 1: Calculate MST
    passTask([&, type]() {
        std::string message="MST Calculation..";
        threadSafePrint(message);
       auto mstAlgo = MSTFactory::create(type); // Assuming this returns a unique_ptr<MSTAlgo>
        // Calculate MST using the provided MSTAlgo instance
        result.mst = mstAlgo->FindMST(graph);
    
    });

    // Task 2: Calculate Total Weight
    passTask([&]() {
        result.totalWeight = MSTInfo::calculateTotalWeight(result.mst, result.mst.size());
        std::string message = "Finish task 2";
        threadSafePrint(message);
    }, 0);

    // Task 3: Calculate Average Distance
    passTask([&]() {
        result.averageDistance = MSTInfo::calculateAverageDistance(result.totalWeight, result.mst.size());
        std::string message = "Finish task 3";
        threadSafePrint(message);
    }, 1);

    // Task 4: Calculate Longest Distance
    passTask([&]() {
        result.longestDistance = MSTInfo::findLongestDistance(result.mst, result.mst.size());
        std::string message = "Finish task 4";
        threadSafePrint(message);
    }, 2);

    // Task 5: Calculate Shortest Distance
    passTask([&]() {
        result.shortestDistance = MSTInfo::calculateShortestDistance(result.mst, result.mst.size());
        std::string message = "Finish task 5";
        threadSafePrint(message);
    }, 3);

    // Task 6: Send Info
    passTask([&, clientfd]() {
        result.sendMSTDetails(clientfd);
        std::string message = "Finish task 6";
        threadSafePrint(message);
    }, 4);\
    
       // Wait for all tasks to complete
    {
        std::unique_lock<std::mutex> lock(clientMutex);
        clientCv.wait(lock, [&] { return completedTasks.load() == 6; }); // Wait until all tasks are completed
    }
}
