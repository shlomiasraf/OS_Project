#include "CommunicationStage.hpp"

CommunicationStage::CommunicationStage() {start();}

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
    // Use a shared_ptr to manage the promise
    auto commandPromise = std::make_shared<std::promise<Command>>();
    std::future<Command> commandFuture = commandPromise->get_future();
    // Enqueue a task that moves the promise into the lambda
    enqueue([this, client_fd, commandPromise]() mutable {
        Command result = processClient(client_fd);
        commandPromise->set_value(result); // Set the result
    });   
    Command command = commandFuture.get();   // This blocks until the command is processed
    processCommand(client_fd,command);
    return command; 
}

std::string CommunicationStage::handleReceiveData(int client_fd) {
    std::string commandStr;
    char buffer[256];
    ssize_t bytesRead;

    // Read data from the client socket until newline is encountered
    while (true) {
        bytesRead = read(client_fd, buffer, sizeof(buffer) - 1);
        if (bytesRead < 0) {
            std::cerr << "Error reading from socket." << std::endl;
            break;
        } else if (bytesRead == 0) {
            std::cerr << "Client disconnected." << std::endl;
            break;
        }
        buffer[bytesRead] = '\0'; // Null-terminate the string
        commandStr += buffer; // Append the buffer to the command string
        if (commandStr.find('\n') != std::string::npos) {
            break; // Break the loop if Enter is detected
        }
    }

    return commandStr;
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

void CommunicationStage::processCommand(int client_fd, Command command) {
    switch (command) {
        case Command::Newgraph:
            Newgraph(client_fd);
            break;
        case Command::Addedge:
            AddEdge(client_fd);
            break;
        case Command::Removeedge:
            RemoveEdge(client_fd);
            break;
        case Command::Prim:
        case Command::Kruskal:
            getMSTAlgorithm(command, client_fd);
            break;
        case Command::Exit:
            break;
        default:
            std::cerr << "Invalid command from client " << client_fd << std::endl;
            break;
    }
}


void CommunicationStage::AddEdge(int client_fd) {
    Graph& graph = GlobalGraph::getInstance().getGraph();
    int u, v, weight;
    std::string message = "Please enter edge you wish to add (u v weight): \n";
    send(client_fd, message.c_str(), message.size(), 0);

    if (recv(client_fd, (char*)&u, sizeof(u), 0) <= 0 ||
        recv(client_fd, (char*)&v, sizeof(v), 0) <= 0 ||
        recv(client_fd, (char*)&weight, sizeof(weight), 0) <= 0) {
        std::cerr << "Error receiving edge data.\n";
        return;
    }

    graph.addEdge(u, v, weight);
    message = "Edge added successfully.\n";
    send(client_fd, message.c_str(), message.size(), 0);
}

void CommunicationStage::RemoveEdge(int client_fd) {
    Graph& graph = GlobalGraph::getInstance().getGraph();
    int u, v;
    std::string message = "Enter edge to remove (u v): \n";
    send(client_fd, message.c_str(), message.size(), 0);

    if (recv(client_fd, (char*)&u, sizeof(u), 0) <= 0 ||
        recv(client_fd, (char*)&v, sizeof(v), 0) <= 0) {
        std::cerr << "Error receiving edge data.\n";
        return;
    }

    graph.removeEdge(u - 1, v - 1);
    message = "Edge removed successfully.\n";
    send(client_fd, message.c_str(), message.size(), 0);
}

void CommunicationStage::Newgraph(int client_fd) {
    int vertex, edges;
    std::string message = "Please enter the number of vertices and edges: \n";
    send(client_fd, message.c_str(), message.size(), 0);
    fflush(stdout);
    char buf[256];
    std::string input;
    memset(buf, 0, sizeof(buf));
    // Read vertices and edges
    int nbytes = recv(client_fd, buf, sizeof(buf) - 1, 0);
    if (nbytes <= 0) {
        std::cerr << "Error receiving data from client.\n";
        return;
    }
    buf[nbytes] = '\0';
    input = buf;
    std::istringstream iss(input);
    if (!(iss >> vertex >> edges)) {
        std::cerr << "Error parsing vertex/edges input.\n";
        return;
    }

    Graph graph(vertex, edges);
    GlobalGraph::getInstance().setGraph(graph);  // Set the global graph
    memset(buf, 0, sizeof(buf));
    message = "Please enter the edges (u v weight): \n";
    send(client_fd, message.c_str(), message.size(), 0);
    fflush(stdout);
    // Read edges
    for (int i = 0; i < edges; ++i) {
        input.clear();
        memset(buf, 0, sizeof(buf));
        nbytes = recv(client_fd, buf, sizeof(buf) - 1, 0);
        if (nbytes <= 0) {
            std::cerr << "Error receiving edge data.\n";
            return;
        }
        buf[nbytes] = '\0';
        input = buf;

        int u, v, weight;
        std::istringstream edgeStream(input);
        if (!(edgeStream >> u >> v >> weight)) {
            message = "Invalid edge input format. Please retry.\n";
            send(client_fd, message.c_str(), message.size(), 0);
            fflush(stdout);
            --i;  // Retry this edge
            continue;
        }

        GlobalGraph::getInstance().getGraph().addEdge(u - 1, v - 1, weight);
    }

    message = "Graph created successfully.\n";
    send(client_fd, message.c_str(), message.size(), 0);
}

void CommunicationStage::getMSTAlgorithm(Command type, int client_fd) {
    std::string message;
    if (type == Command::Prim) {
        PrimMST primInstance;
        message = primInstance.primFunction(GlobalGraph::getInstance().getGraph());
    } else if (type == Command::Kruskal) {
        KruskalMST kruskalInstance;
        message = kruskalInstance.kruskalFunction(GlobalGraph::getInstance().getGraph());
    } else {
        message = "Invalid MST command.\n";
    }

    send(client_fd, message.c_str(), message.size(), 0);
}


