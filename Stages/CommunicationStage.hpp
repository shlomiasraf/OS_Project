#include "ActiveObject.hpp"
class CommunicationStage : public ActiveObject {
private:
    std::string handleReceiveData(int client_fd);
    Command getCommandFromString(const std::string& commandStr);
public:
    void enqueue(std::function<void()> request) override;
    void execute() override;
};

