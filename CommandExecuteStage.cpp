#include "CommandExecuteStage.hpp"




void CommandExecuteStage::enqueue(std::function<void()> request) 
{
    std::unique_lock<std::mutex> lock(queueMutex);
    taskQueue.push(request);
    cv.notify_one();
}

void CommandExecuteStage::execute() 
{
    while (true) 
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            cv.wait(lock, [this]{ return !taskQueue.empty(); });
            task = taskQueue.front();
            taskQueue.pop();
        }
        task(); // Execute the task
    }
}

void CommandExecuteStage::Addedge(int clientfd) 
{
    int u, v, weight;
    std::string message = "Please enter edge you wish to add\n";
    send(clientfd, message.c_str(), message.size(), 0);

    recv(clientfd, (char*)&u, sizeof(u), 0);
    recv(clientfd, (char*)&v, sizeof(v), 0);
    recv(clientfd, (char*)&weight, sizeof(weight), 0);

    {
        std::lock_guard<std::mutex> lock(graphMutex); // Protect the graph
        graph.addEdge(u - 1, v - 1, weight);
    }
}

void CommandExecuteStage::RemoveEdge(int clientfd) 
{
    int i, j;
    std::string message = "Enter edge to remove (i j): \n";
    send(clientfd, message.c_str(), message.size(), 0);

    recv(clientfd, (char*)&i, sizeof(i), 0);
    recv(clientfd, (char*)&j, sizeof(j), 0);

    {
        std::lock_guard<std::mutex> lock(graphMutex); // Protect the graph
        graph.removeEdge(i - 1, j - 1);
    }
}

void CommandExecuteStage::Newgraph(int clientfd) 
{
    int vertex, edges;
    std::string message = "Please enter the number of vertices and edges: \n";
    send(clientfd, message.c_str(), message.size(), 0);

    char buf[256];
    int nbytes = recv(clientfd, buf, sizeof(buf) - 1, 0);
    if (nbytes > 0) {
        buf[nbytes] = '\0';
        std::istringstream iss(buf);
        iss >> vertex >> edges;
    } else {
        std::cerr << "Error receiving vertex/edge input from client.\n";
        return;
    }

    {
        std::lock_guard<std::mutex> lock(graphMutex); // Protect the graph
        graph = Graph(vertex, edges);
    }

    message = "Please enter the edges (format: u v weight): \n";
    send(clientfd, message.c_str(), message.size(), 0);

    for (int i = 0; i < edges; ++i) 
    {
        nbytes = recv(clientfd, buf, sizeof(buf) - 1, 0);
        if (nbytes > 0) {
            buf[nbytes] = '\0';
            std::istringstream iss(buf);
            int u, v, weight;
            if (!(iss >> u >> v >> weight)) {
                std::cerr << "Error parsing edge input\n";
                message = "Invalid edge input format.\n";
                send(clientfd, message.c_str(), message.size(), 0);
                --i;
                continue;
            }

            if (u > vertex || v > vertex) {
                message = "Invalid edge, please enter again.\n";
                send(clientfd, message.c_str(), message.size(), 0);
                --i;
            } else {
                std::lock_guard<std::mutex> lock(graphMutex); // Protect the graph
                graph.addEdge(u - 1, v - 1, weight);
            }
        } else {
            std::cerr << "Error receiving edge input from client.\n";
            return;
        }
    }

    message = "The graph has been created!\n";
    send(clientfd, message.c_str(), message.size(), 0);
}

void CommandExecuteStage::getMSTAlgorithm(Command type, int client_fd) 
{
    PrimMST primInstance;
    KruskalMST kruskalInstance;
    std::string message;

    std::lock_guard<std::mutex> lock(graphMutex); // Protect the graph
    switch (type) 
    {
        case Command::Prim:
            message = primInstance.primFunction(graph);
            break;
        case Command::Kruskal:
            message = kruskalInstance.kruskalFunction(graph);
            break;
        default:
            message = "Invalid MST command.\n";
            break;
    }
    send(client_fd, message.c_str(), message.size(), 0);
}

void CommandExecuteStage::processCommand(int client_fd, Command command) 
{
    switch (command) 
    {
        case Command::Newgraph:
            enqueue([this, client_fd]() { 
                Newgraph(client_fd); });
            break;
        case Command::Addedge:
            enqueue([this, client_fd]() { Addedge(client_fd); });
            break;
        case Command::Removeedge:
            enqueue([this, client_fd]() { RemoveEdge(client_fd); });
            break;
        case Command::Prim:
        case Command::Kruskal:
            enqueue([this, client_fd, command]() { getMSTAlgorithm(command, client_fd); });
            break;
        case Command::Invalid:
            send(client_fd, "Invalid command!\n", 18, 0);
            break;
        case Command::Exit:
            // Handle exit case separately if needed
            break;
    }
}
