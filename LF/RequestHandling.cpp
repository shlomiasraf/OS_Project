#include "RequestHandling.hpp"

// Shared graph object
RequestHandling::RequestHandling(int numThreads) : graph(0, 0), threadPool(numThreads) {}

RequestHandling::~RequestHandling() {
    // Close all client connections
    {
        std::lock_guard<std::mutex> lock(clientMutex);
        for (int fd : client_fds) {
            close(fd);
        }
    }
}

Command RequestHandling::getCommandFromString(const std::string &commandStr)
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

void RequestHandling::Addedge(int clientfd) 
{
    int u, v, weight;
    std::string message = "Please enter edge you wish to add\n";
    send(clientfd, message.c_str(), message.size(), 0);

    recv(clientfd, (char*)&u, sizeof(u), 0);
    recv(clientfd, (char*)&v, sizeof(v), 0);
    recv(clientfd, (char*)&weight, sizeof(weight), 0);

    {
        std::lock_guard<std::mutex> lock(graphMutex);
        graph.addEdge(u - 1, v - 1, weight);
    }
}

void RequestHandling::RemoveEdge(int clientfd) 
{
    int i, j;
    std::string message = "Enter edge to remove (i j): \n";
    send(clientfd, message.c_str(), message.size(), 0);

    recv(clientfd, (char*)&i, sizeof(i), 0);
    recv(clientfd, (char*)&j, sizeof(j), 0);

    {
        std::lock_guard<std::mutex> lock(graphMutex);
        graph.removeEdge(i - 1, j - 1);
    }
}

void RequestHandling::Newgraph(int clientfd) 
{
    int vertex, edges;
    std::string message = "Please enter the number of vertices and edges: \n";
    send(clientfd, message.c_str(), message.size(), 0);

    // Parse vertices and edges
    char buf[8192];
    int nbytes = recv(clientfd, buf, sizeof(buf) - 1, 0);
    buf[nbytes] = '\0';

    std::istringstream iss(buf);
    if (!(iss >> vertex >> edges)) {
        message = "Invalid input!\n";
        send(clientfd, message.c_str(), message.size(), 0);
        return;
    }

    {
        std::lock_guard<std::mutex> lock(graphMutex);
        graph = Graph(vertex, edges);
    }

    message = "Graph created, enter edges: u v weight\n";
    send(clientfd, message.c_str(), message.size(), 0);

    // Now read edges in a loop
    for (int i = 0; i < edges; ++i) 
    {
        nbytes = recv(clientfd, buf, sizeof(buf) - 1, 0);
        buf[nbytes] = '\0';
        int u, v, weight;
        std::istringstream edgeIss(buf);
        edgeIss >> u >> v >> weight;

        if (u <= vertex && v <= vertex) {
            std::lock_guard<std::mutex> lock(graphMutex);
            graph.addEdge(u - 1, v - 1, weight);
        } else {
            message = "Invalid edge\n";
            send(clientfd, message.c_str(), message.size(), 0);
            --i;
        }
    }
}

void RequestHandling::getMSTAlgorithm(Command type, int client_fd) 
{
    PrimMST primInstance;
    KruskalMST kruskalInstance;
    std::string message;

    {
        std::lock_guard<std::mutex> lock(graphMutex);
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
    }
    send(client_fd, message.c_str(), message.size(), 0);
}

void RequestHandling::processCommand(int client_fd, Command command) 
{
    switch (command) 
    {
        case Command::Newgraph:
            threadPool.enqueueTask([this, client_fd]() { Newgraph(client_fd); });
            break;
        case Command::Addedge:
            threadPool.enqueueTask([this, client_fd]() { Addedge(client_fd); });
            break;
        case Command::Removeedge:
            threadPool.enqueueTask([this, client_fd]() { RemoveEdge(client_fd); });
            break;
        case Command::Prim:
        case Command::Kruskal:
            threadPool.enqueueTask([this, client_fd, command]() { getMSTAlgorithm(command, client_fd); });
            break;
        case Command::Exit:
            close(client_fd); // Close the client connection
            break;
        default:
            std::string message = "Invalid command!\n";
            send(client_fd, message.c_str(), message.size(), 0);
            break;
    }
}

void RequestHandling::processClient(int clientfd) 
{
    char buf[1024];
    std::string commandStr;
    
    while (true) {
        // Clear the buffer
        memset(buf, 0, sizeof(buf));
        
        // Receive the command from the client
        int nbytes = recv(clientfd, buf, sizeof(buf) - 1, 0);
        if (nbytes <= 0) {
            if (nbytes == 0) {
                // Connection closed by client
                std::cout << "Client disconnected.\n";
            } else {
                std::cerr << "Error receiving data from client.\n";
            }
            close(clientfd);
            return;
        }
        
        // Convert buffer to string and trim whitespace/newline
        commandStr.assign(buf);
        commandStr.erase(commandStr.find_last_not_of(" \n\r\t") + 1);
        
        // Parse the command from string
        Command command = getCommandFromString(commandStr);
        
        // Process the command
        processCommand(clientfd, command);
    }
}

void RequestHandling::monitorHandles()
{
    // Create a socket for listening
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Error creating socket\n";
        return;
    }

    // Set up the address structure
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080); // Use the appropriate port

    // Bind the socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed\n";
        close(server_fd);
        return;
    }

    // Listen for incoming connections
    if (listen(server_fd, 5) < 0) {
        std::cerr << "Listen failed\n";
        close(server_fd);
        return;
    }

    std::cout << "Server is listening on port 8080...\n";

    while (true) {
        // Accept a new connection
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd < 0) {
            std::cerr << "Accept failed\n";
            continue;
        }

        std::cout << "New client connected\n";

        // Add the client file descriptor to the set
        {
            std::lock_guard<std::mutex> lock(clientMutex);
            client_fds.insert(client_fd);
        }

        // Create a new thread to handle the client
        threadPool.enqueueTask([this, client_fd]() { processClient(client_fd); });
    }

    // Clean up
    close(server_fd);
}
