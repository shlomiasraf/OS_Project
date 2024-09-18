#include "ActiveObject.hpp"
#include "DisconnecterStage.hpp"  // Include the header for DisconnecterStage
#include <functional>

class CommandExecuteStage : public ActiveObject 
{
    // Private member variables for task queue and synchronization
    std::queue<std::function<void()>> taskQueue;
    std::mutex queueMutex;
    std::condition_variable cv;
    Graph graph{0,0};
    DisconnecterStage disconnector;

    std::mutex graphMutex; // Mutex to protect access to the graph

public:
    void enqueue(std::function<void()> request) override;
    void execute() override;
    
    // Command handlers
    void processCommand(int client_fd, Command command);
    void Newgraph(int clientfd);
    void Addedge(int clientfd);
    void RemoveEdge(int clientfd);
    void getMSTAlgorithm(Command type, int client_fd);

    // Constructor and destructor (optional)
    CommandExecuteStage() = default;
    ~CommandExecuteStage() = default;
};

