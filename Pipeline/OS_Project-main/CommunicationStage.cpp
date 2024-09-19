#include "CommunicationStage.hpp"



void CommunicationStage::enqueue(std::function<void()> request) 
{
    // Lock the queue and push the new task
    std::unique_lock<std::mutex> lock(queueMutex);
    taskQueue.push(request);
    cv.notify_one();  // Notify a thread that there is work
}

void CommunicationStage::execute() 
{
    while (true) 
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            // Wait for tasks in the queue
            cv.wait(lock, [this] { return !taskQueue.empty(); });  // Capture 'this' to access taskQueue
            task = taskQueue.front();
            taskQueue.pop();
        }
        task();  // Execute the task
    }
}

std::string CommunicationStage::handleReceiveData(int client_fd) {
    char buf[256];
    memset(buf, 0, sizeof(buf));
    int nbytes = recv(client_fd, buf, sizeof(buf) - 1, 0);
    if (nbytes <= 0) {
        if (nbytes == 0) {
            std::cout << "socket " << client_fd << " hung up\n";
        } else {
            std::cerr << "recv error\n";
        }
        close(client_fd);
        return "exit\n";
    }
    buf[nbytes] = '\0';
    return std::string(buf);
}

Command CommunicationStage::getCommandFromString(const std::string& commandStr) 
{
    std::string lowerCommand = commandStr;
    if (!lowerCommand.empty() && lowerCommand.back() == '\n') 
    {
        lowerCommand.pop_back();
    }
    std::transform(lowerCommand.begin(), lowerCommand.end(), lowerCommand.begin(), ::tolower);

    if (lowerCommand == "newgraph") {
        return Command::Newgraph;
    } else if (lowerCommand == "prim") {
        return Command::Prim;
    } else if (lowerCommand == "kruskal") {
        return Command::Kruskal;
    } else if (lowerCommand == "addedge") {
        return Command::Addedge;
    } else if (lowerCommand == "removeedge") {
        return Command::Removeedge;
    } else if (lowerCommand == "exit") {
        return Command::Exit;
    }
    return Command::Invalid;
}
