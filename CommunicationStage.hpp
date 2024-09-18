#include "ActiveObject.hpp"

class CommunicationStage : public ActiveObject {

private:

    // Private member variables for task queue and synchronization
    std::queue<std::function<void()>> taskQueue;
    std::mutex queueMutex;
    std::condition_variable cv;
public:
    Command getCommandFromString(const std::string& commandStr);
    std::string handleReceiveData(int client_fd);
    void enqueue(std::function<void()> request) override;
    void execute() override;
};

