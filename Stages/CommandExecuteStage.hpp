#include "ActiveObject.hpp"

class CommandExecuteStage : public ActiveObject {
void processCommand(int client_fd, Command command)
void Newgraph(int clientfd)
void Addedge(int clientfd);
void RemoveEdge(int clientfd);

public:
    void enqueue(std::function<void()> request) override;
    void execute() override;
    // Methods for parsing commands
};

