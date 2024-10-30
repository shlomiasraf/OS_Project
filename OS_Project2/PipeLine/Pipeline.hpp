#include "ActiveObject.hpp"
#include <atomic>
#include <future>
#include "MSTInfo.hpp"
class Pipeline{
    

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
    
    std::vector<std::unique_ptr<ActiveObject>> executors; // Use smart pointers
    
    std::mutex mtx1, mtx2, mtx3, mtx4, mtx5,deletetion,coutMutex;
    std::condition_variable cv1, cv2, cv3, cv4, cv5;
    std::atomic<int> completedTasks{0};
    Pipeline(); //  private Constructor
    static Pipeline * instance;
    void threadSafePrint(const std::string& message);
    static std::mutex creation;
    
    public:
    static Pipeline& getInstance();
    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;
    ~Pipeline();
    void cleanup();
    void run(Graph& graph,int clientfd,Command type);    
};