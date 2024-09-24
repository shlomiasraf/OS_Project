#include "CommunicationStage.hpp"


CommunicationStage::CommunicationStage() 
    : 
    shouldStop(false) // Initialize shouldStop
{   
 start();
}

CommunicationStage::~CommunicationStage() {
    stop();
    if (workerThread.joinable()) {
        workerThread.join();
    }
}
Command CommunicationStage::processClient(int client_fd) {  
        std::string receivedData = handleReceiveData(client_fd);
        // Split the received data by newlines to handle multiple commands
        std::istringstream stream(receivedData);
        std::string commandStr;
        std::getline(stream, commandStr); 
        Command command = getCommandFromString(commandStr);
       return command;
}

Command CommunicationStage::enqueueProcessClient(int client_fd) {
    Command result;
    enqueue([this, client_fd, &result]() {
        // Call the original processClient method
        result = processClient(client_fd);
    });
    // Return the result (Note: this is only valid if you wait for the queue to process the task)
    return result;
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

Command CommunicationStage::getCommandFromString(const std::string& commandStr) {
    std::string lowerCommand = commandStr;
    if (!lowerCommand.empty() && lowerCommand.back() == '\n') {
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

