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
        graph.removeEdge(i - 1, j - 1);
    }
}


void CommandExecuteStage::Newgraph(int clientfd) 
{
     std::unique_lock<std::mutex> lock(queueMutex);
    int vertex, edges;
    std::string message = "Please enter the number of vertices and edges: \n";
    send(clientfd, message.c_str(), message.size(), 0);

    // Loop until we get complete input
    char buf[8192];
    int nbytes = 0;
    std::string input;

    // Read until we get complete input for vertex/edges
    while (true) {
        nbytes = recv(clientfd, buf, sizeof(buf) - 1, 0);
        if (nbytes > 0) {
            buf[nbytes] = '\0';
            input += buf;
            if (input.find('\n') != std::string::npos) {  // Assuming input ends with newline
                break;
            }
        } else {
            std::cerr << "Error receiving vertex/edge input from client.\n";
            return;
        }
    }

    // Parse the input
    std::istringstream iss(input);
    if (!(iss >> vertex >> edges)) {
        std::cerr << "Error parsing vertex/edges input.\n";
        return;
    }

    {
        std::lock_guard<std::mutex> lock(graphMutex); // Protect the graph
        graph = Graph(vertex, edges);
    }

    message = "Please enter the edges (format: u v weight): \n";
    send(clientfd, message.c_str(), message.size(), 0);

    // Now read edges in a loop
    for (int i = 0; i < edges; ++i) 
    {
        input.clear();  // Reset input string for each edge
        while (true) {
            nbytes = recv(clientfd, buf, sizeof(buf) - 1, 0);
            if (nbytes > 0) {
                buf[nbytes] = '\0';
                input += buf;
                if (input.find('\n') != std::string::npos) {  // Assuming each edge ends with newline
                    break;
                }
            } else {
                std::cerr << "Error receiving edge input from client.\n";
                return;
            }
        }

        // Parse the edge input
        std::istringstream iss(input);
        int u, v, weight;
        if (!(iss >> u >> v >> weight)) {
            std::cerr << "Error parsing edge input\n";
            message = "Invalid edge input format.\n";
            send(clientfd, message.c_str(), message.size(), 0);
            --i;  // Retry this edge
            continue;
        }

        if (u > vertex || v > vertex) {
            message = "Invalid edge, please enter again.\n";
            send(clientfd, message.c_str(), message.size(), 0);
            --i;  // Retry this edge
        } else {
            graph.addEdge(u - 1, v - 1, weight);
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

            std::string message="";
    switch (command) 
    {
    
        case Command::Newgraph:
            enqueue([this, client_fd]() {  Newgraph(client_fd); });
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
            message="Invalid command!\n";
  	    send(client_fd, message.c_str(), message.size(), 0);
            break;
        case Command::Exit:
            // Send client to the disconnector
            break;
    }
}
