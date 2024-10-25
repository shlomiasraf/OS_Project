#include "CommandExecute.hpp"
#include <iostream>
#include <sstream>
#include <cstring>

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
                //std::cout<<"The client choose to disconnect.\n";
             	break;

            default:
            
                std::cerr << "Invalid command frtrom client " << client_fd << std::endl;
                break;
      		  }
    return command;
}

void CommandExecute::AddEdge(int client_fd) {

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

void CommandExecute::RemoveEdge(int client_fd) {
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

void CommandExecute::getMSTAlgorithm(Command type, int client_fd) {
    std::string message;
        if(graph.V<2){
         message = "Cannot Compute MST on Graphs that vertics number lower than 2.\n";
         send(client_fd, message.c_str(), message.size(), 0);
         return;
    }
    Pipeline& pipe = Pipeline::getInstance();
    if (type == Command::Prim) {
        pipe.run(graph,client_fd,"prim");
    } else if (type == Command::Kruskal) {
        pipe.run(graph,client_fd,"Kruskal");
    } else {
        message = "Invalid MST command.\n";
        send(client_fd, message.c_str(), message.size(), 0);
    }
}


