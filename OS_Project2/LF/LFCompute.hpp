#ifndef LFCOMPUTE_HPP  // Change this to your header file name
#define LFCOMPUTE_HPP
#include "ThreadPool.hpp"
#include "MSTInfo.hpp"
#include "kruskalMST.hpp"
#include "primMST.hpp"
#include <atomic>
#include <future>
class LFCompute{
 struct MSTResult {
    
    std::vector<std::vector<std::pair<int, int>>> mst;  // The adjacency list representation of the MST
    double totalWeight;        // The total weight of the MST
    double shortestDistance;   // Shortest distance between any two nodes in the MST
    double averageDistance;    // Average distance between all nodes in the MST
    double longestDistance;
    // Add more fields as necessary for other metrics

    // Constructor for easy initialization
    MSTResult(const std::vector<std::vector<std::pair<int, int>>>& mstData, double totalW, double shortestDist, double avgDist)
        : mst(mstData), totalWeight(totalW), shortestDistance(shortestDist), averageDistance(avgDist) {}

    // You can also add functions to calculate certain metrics if needed
    void sendMSTDetails(int clientfd) const {
    std::string message;
    
    // Construct the message with MST details
    message += "MST Details:\n";
    message += "Total Weight: " + std::to_string(totalWeight) + "\n";
    message += "Longest Distance: " + std::to_string(longestDistance) + "\n";
    message += "Shortest Distance: " + std::to_string(shortestDistance) + "\n";
    message += "Average Distance: " + std::to_string(averageDistance) + "\n";

    // Send the message over the socket
    ssize_t bytes_sent = send(clientfd, message.c_str(), message.size(), 0);
    if (bytes_sent == -1) {
        std::cerr << "Error sending MST details to client " << clientfd << std::endl;
         }
     }
    };
    static LFCompute * instance;
    std::atomic<bool> taskCompleted{false};
    std::mutex clientMutex;
    std::condition_variable clientCv;
    std::condition_variable taskCondition;
    ThreadPool pool;
    std::mutex coutMutex;
    static std::mutex creation;
    bool isCleaningUp = false;
public:
    LFCompute();
    static LFCompute& getInstance();
    LFCompute(const LFCompute&) = delete;
    LFCompute& operator=(const LFCompute&) = delete;
    void Compute(Graph& graph, int clientfd,Command type);
    void threadSafePrint(const std::string &message);
    void cleanup(); 
    ~LFCompute();
};
#endif // LFCOMPUTE_HPP