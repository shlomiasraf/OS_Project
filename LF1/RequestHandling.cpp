#include "RequestHandling.hpp"

    
   RequestHandling::RequestHandling(Graph& graph)
    : graph(graph){}

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

      graph = Graph(vertex, edges);


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

            graph.addEdge(u - 1, v - 1, weight);
        } else {
            message = "Invalid edge\n";
            send(clientfd, message.c_str(), message.size(), 0);
            --i;
        }
      
	}
	 message = "The graph has been created!\n";
    	send(clientfd, message.c_str(), message.size(), 0);
}
void RequestHandling::getMSTAlgorithm(Command type, int client_fd) 
{

    PrimMST primInstance;
    KruskalMST kruskalInstance;
    std::string message;

    {
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
           Newgraph(client_fd);
            break;
        case Command::Addedge:
            Addedge(client_fd);
            break;
        case Command::Removeedge:
           RemoveEdge(client_fd);
            break;
        case Command::Prim:
        case Command::Kruskal:
          getMSTAlgorithm(command, client_fd);
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
    // Lock the global mutex to ensure only one client can process commands at a time
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
