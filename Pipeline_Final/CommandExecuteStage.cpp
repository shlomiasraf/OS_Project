#include "CommandExecuteStage.hpp"
#include <iostream>
#include <sstream>
#include <cstring>

CommandExecuteStage::CommandExecuteStage() {start();}

CommandExecuteStage::~CommandExecuteStage() {
    stop(); // Ensure graceful shutdown
}


Command CommandExecuteStage::processCommand(int client_fd, Command command) {
     enqueue([client_fd, command, this]() {
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
            });
    return command;
}

void CommandExecuteStage::AddEdge(int client_fd) {
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

void CommandExecuteStage::RemoveEdge(int client_fd) {
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

void CommandExecuteStage::Newgraph(int client_fd) {
    int vertex, edges;
    std::string message = "Please enter the number of vertices and edges: \n";
    send(client_fd, message.c_str(), message.size(), 0);

    char buf[8192];
    std::string input;

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

    message = "Please enter the edges (u v weight): \n";
    send(client_fd, message.c_str(), message.size(), 0);

    // Read edges
    for (int i = 0; i < edges; ++i) {
        input.clear();
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
            --i;  // Retry this edge
            continue;
        }

        GlobalGraph::getInstance().getGraph().addEdge(u - 1, v - 1, weight);
    }

    message = "Graph created successfully.\n";
    send(client_fd, message.c_str(), message.size(), 0);
}

void CommandExecuteStage::getMSTAlgorithm(Command type, int client_fd) {
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


