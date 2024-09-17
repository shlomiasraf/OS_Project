#include "CommandExecuteStage.hpp"



void CommandExecuteStage::enqueue(std::function<void()> request)
{
}

void CommandExecuteStage::execute()
{
}

void CommandExecuteStage::Addedge(int clientfd) {
    int u, v, weight;
    std::string message = "Please enter edge you wish to add\n";
    send(clientfd, message.c_str(), message.size(), 0);
    message.clear();
    recv(clientfd, (char*)&u, sizeof(u), 0);
    recv(clientfd, (char*)&v, sizeof(v), 0);
    recv(clientfd, (char*)&weight, sizeof(weight), 0);
    graph.addEdge(u - 1, v - 1, weight);
}

void CommandExecuteStage::RemoveEdge(int clientfd) {
    int i, j;
    std::string message = "Enter edge to remove (i j): \n";
    send(clientfd, message.c_str(), message.size(), 0);
    recv(clientfd, (char*)&i, sizeof(i), 0);
    recv(clientfd, (char*)&j, sizeof(j), 0);
    graph.removeEdge(i - 1, j - 1);
}

void CommandExecuteStage::Newgraph(int clientfd) {
    int vertex, edges;
    std::string message;
    
    // Request for the number of vertices and edges
    message = "Please enter the number of vertices and edges: \n";
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

    // Create the graph
    graph = Graph(vertex, edges);
    
    // Request for the edges
    message = "Please enter the edges (format: u v weight): \n";
    send(clientfd, message.c_str(), message.size(), 0);
    
    // Loop to receive each edge
    for (int i = 0; i < edges; ++i) {
        nbytes = recv(clientfd, buf, sizeof(buf) - 1, 0);
        if (nbytes > 0) {
            buf[nbytes] = '\0';
            
            std::istringstream iss(buf);
            int u, v, weight;
            if (!(iss >> u >> v >> weight)) {
                std::cerr << "Error parsing edge input\n";
                message = "Invalid edge input format.\n";
                send(clientfd, message.c_str(), message.size(), 0);
                --i; // Retry this iteration
                continue;
            }
            memset(buf,0,sizeof(buf));
            // Validate the input before adding the edge
            if (u > vertex || v > vertex) {
                message = "Invalid edge, please enter again.\n";
                send(clientfd, message.c_str(), message.size(), 0);
                --i; // Retry this iteration
            } else {
                graph.addEdge(u - 1, v - 1, weight);
            }
        } else if (nbytes == 0) {
            std::cerr << "Client disconnected\n";
            return;
        } else {
            std::cerr << "Error receiving edge input from client.\n";
            return;
        }
    }

    // Notify the client that the graph has been created
    message = "The graph has been created!\n";
    send(clientfd, message.c_str(), message.size(), 0);
}
void CommandExecuteStage::processCommand(int client_fd, Command command) {
    switch (command) {
        case Command::Newgraph:
            submitCommand([client_fd]() { Newgraph(client_fd); });
            break;
        case Command::Addedge:
            submitCommand([client_fd]() { Addedge(client_fd); });
            break;
        case Command::Removeedge:
            submitCommand([client_fd]() { RemoveEdge(client_fd); });
            break;
        case Command::Prim:
        case Command::Kruskal:
            submitCommand([client_fd, command]() { MSTFactory::getMSTAlgorithm(command, client_fd); });
            break;
        case Command::Invalid:
            send(client_fd, "Invalid command!\n", 18, 0);
            break;
        case Command::Exit:
            // Handle exit case separately if needed
            break;
    }
}
