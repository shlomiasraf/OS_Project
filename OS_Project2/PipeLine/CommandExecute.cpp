#include "CommandExecute.hpp"
#include <iostream>
#include <sstream>
#include <cstring>
#include <unistd.h>

Command CommandExecute::processCommand(int client_fd, Command command) {
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
                break;
      		  }
    return command;
}

void CommandExecute::AddEdge(int client_fd) {

    std::string message = "Enter edge to add (i, j, weight): \n";
    std::cout << std::flush;
    send(client_fd, message.c_str(), message.size(), 0);
    char buf[256];
    int nbytes = recv(client_fd, buf, sizeof(buf) - 1, 0);
    buf[nbytes] = '\0';
    int u, v, weight;
    std::istringstream edgeIss(buf);
    edgeIss >> u >> v >> weight;
    if(u <= graph.V && v <= graph.V)
    {
        graph.addEdge(u - 1, v - 1, weight);
        message = "Edge was created successfully!\n";
    }
    else
    {
        message = "Invalid edge\n";
    }

    send(client_fd, message.c_str(), message.size(), 0);
}

void CommandExecute::RemoveEdge(int client_fd) {
    int i, j;
    std::string message = "Enter edge to remove (i j): \n";
    std::cout << std::flush;
    send(client_fd, message.c_str(), message.size(), 0);
    char buf[256];
    int nbytes = recv(client_fd, buf, sizeof(buf) - 1, 0);
    buf[nbytes] = '\0';
    std::istringstream edgeIss(buf);
    edgeIss >> i >> j;
    if(i <= graph.V && j <= graph.V)
    {
        graph.removeEdge(i - 1, j - 1);
        message = "Edge was removed successfully!\n";
    }
    else
    {
        message = "Invalid edge\n";
    }

    send(client_fd, message.c_str(), message.size(), 0);
}

void CommandExecute::Newgraph(int client_fd) {
    int vertex, edges;
    std::lock_guard<std::mutex> lock(resultMutex);

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
    message = "Enter edges to add (i, j, weight): \n";
    buf[nbytes] = '\0';
    input = buf;
    std::istringstream iss(input);
    if (!(iss >> vertex >> edges)) {
        std::cerr << "Error parsing vertex/edges input.\n";
        return;
    }
    graph = Graph(vertex, edges);   
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

       graph.addEdge(u - 1, v - 1, weight);
    }

    message = "Graph created successfully.\n";
    send(client_fd, message.c_str(), message.size(), 0);
}



void CommandExecute::getMSTAlgorithm(Command type, int client_fd) 
{
    std::string message;
     if (graph.V < 2) {
        message = "Cannot Compute MST on Graphs with fewer than 2 vertices.\n";
        send(client_fd, message.c_str(), message.size(), 0);
        return;
    }
        pipe.run(graph, client_fd,type);
}



