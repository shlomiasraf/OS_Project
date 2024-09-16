#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <sstream>
#include <pthread.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/wait.h>
#include <functional>
#include <queue>
#include <condition_variable>
#include <thread>
#include "MSTFactory.hpp"
#include "primMST.hpp"
#include "kruskalMST.hpp"
#include "Graph.hpp"

#define PORT "9034"

// Mutex for thread-safe access to the shared graph
pthread_mutex_t graph_mutex = PTHREAD_MUTEX_INITIALIZER;

// Shared graph object for all clients
Graph graph(0, 0);

class CommandProcessor {
public:
    CommandProcessor() : stop(false) {
        worker = std::thread([this]() { this->processCommands(); });
    }

    ~CommandProcessor() {
        {
            std::unique_lock<std::mutex> lock(mutex);
            stop = true;
        }
        cond_var.notify_all();
        worker.join();
    }

    void submitCommand(const std::function<void()>& command) {
        std::unique_lock<std::mutex> lock(mutex);
        commandQueue.push(command);
        cond_var.notify_one();
    }

private:
    void processCommands() {
        while (true) {
            std::function<void()> command;
            {
                std::unique_lock<std::mutex> lock(mutex);
                cond_var.wait(lock, [this]() { return !commandQueue.empty() || stop; });
                if (stop && commandQueue.empty()) return;
                command = commandQueue.front();
                commandQueue.pop();
            }
            command();
        }
    }

    std::queue<std::function<void()>> commandQueue;
    std::mutex mutex;
    std::condition_variable cond_var;
    std::thread worker;
    bool stop;
};

// Active Object for handling client commands asynchronously
CommandProcessor commandProcessor;

Command getCommandFromString(const std::string& commandStr) {
    std::string lowerCommand = commandStr;
    std::transform(lowerCommand.begin(), lowerCommand.end(), lowerCommand.begin(), ::tolower);

    if (lowerCommand == "newgraph\n") {
        return Command::Newgraph;
    } else if (lowerCommand == "prim\n") {
        return Command::Prim;
    } else if (lowerCommand == "kruskal\n") {
        return Command::Kruskal;
    } else if (lowerCommand == "addedge\n") {
        return Command::Addedge;
    } else if (lowerCommand == "removeedge\n") {
        return Command::Removeedge;
    } else if (lowerCommand == "exit\n") {
        return Command::Exit;
    }
    return Command::Invalid;
}

void Newgraph(int clientfd) {
    int vertex, edges;
    std::string message;
    
    // Request for the number of vertices and edges
    message = "Please enter the number of vertices and edges: \n";
    send(clientfd, message.c_str(), message.size(), 0);
    message.clear();
    
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
    message.clear();
    // Loop to receive each edge
    for (int i = 0; i < edges; ++i) {
        nbytes = recv(clientfd, buf, sizeof(buf) - 1, 0);
        if (nbytes > 0) {
            buf[nbytes] = '\0';
            std::istringstream iss(buf);
            int u, v, weight;
            iss >> u >> v >> weight;

            // Validate the input before adding the edge
            if (u > vertex || v > vertex) {
                message = "Invalid edge, please enter again.\n";
                send(clientfd, message.c_str(), message.size(), 0);
                --i; // Retry this iteration
            } else {
                graph.addEdge(u - 1, v - 1, weight);
            }
        } else {
            std::cerr << "Error receiving edge input from client.\n";
            return;
        }
    }

    // Notify the client that the graph has been created
    message = "The graph has been created!\n";
    send(clientfd, message.c_str(), message.size(), 0);
}


void MSTFactory::getMSTAlgorithm(Command type, int client_fd) 
{
    PrimMST primInstance;
    KruskalMST kruskalInstance;
    std::string message;

    switch (type) {
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
void Addedge(int clientfd) {
    int u, v, weight;
    std::string message = "Please enter edge you wish to add\n";
    send(clientfd, message.c_str(), message.size(), 0);
    message.clear();
    std::cin >> u >> v >> weight;
    graph.addEdge(u - 1, v - 1, weight);
}

void RemoveEdge(int clientfd) {
    int i, j;
    std::string message = "Enter edge to remove (i j): \n";
    send(clientfd, message.c_str(), message.size(), 0);
    message.clear();
    std::cin >> i >> j;
    graph.removeEdge(i - 1, j - 1);
}

std::string handle_recieve_data(int client_fd) {
    char buf[256];
    int nbytes = recv(client_fd, buf, sizeof(buf), 0);
    if (nbytes <= 0) {
        if (nbytes == 0) {
            printf("socket %d hung up\n", client_fd);
        } else {
            std::cout << "recv\n";
                    close(client_fd);
        }
        close(client_fd);
        return "exit\ns";
    }
    buf[nbytes] = '\0';
  
    return std::string(buf);
}

void* Command_Shift(void* client_socket) {
    int client_fd = *(int*)client_socket;
    dup2(client_fd, STDIN_FILENO);
    std::string input;
    Command command = Command::Invalid;
    while (command != Command::Exit) {
        input = handle_recieve_data(client_fd);
        command = getCommandFromString(input);
        switch (command) {
        
            case Command::Newgraph:
                pthread_mutex_lock(&graph_mutex);
                commandProcessor.submitCommand([client_fd]() {
                    Newgraph(client_fd);
                        pthread_mutex_unlock(&graph_mutex);
                });
                break;
                
            case Command::Prim:
            case Command::Kruskal:
                pthread_mutex_lock(&graph_mutex);
                commandProcessor.submitCommand([client_fd, command]() {
                    MSTFactory::getMSTAlgorithm(command, client_fd);
                        pthread_mutex_unlock(&graph_mutex);
                });
                break;
                
            case Command::Addedge:
                pthread_mutex_lock(&graph_mutex);
                commandProcessor.submitCommand([client_fd]() {
                    Addedge(client_fd);
                        pthread_mutex_unlock(&graph_mutex);
                });
                
                break;
            case Command::Removeedge:
                pthread_mutex_lock(&graph_mutex);
                commandProcessor.submitCommand([client_fd]() {
                    RemoveEdge(client_fd);
                        pthread_mutex_unlock(&graph_mutex);
                });
                
                break;
            case Command::Invalid:
                pthread_mutex_lock(&graph_mutex);
                send(client_fd, "Invalid command!\n", 18, 0);
		    pthread_mutex_unlock(&graph_mutex);              
                break;
            case Command::Exit:
                close(client_fd);
                return NULL;
        }
    }

    return NULL;
}

void* get_in_addr(struct sockaddr* sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int setup_server() {
    int server_fd;
    struct sockaddr_in address;
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(9034);

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        return -1;
    }
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", 9034);
    return server_fd;
}

int setup_client_connection(int server_fd) {
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen = sizeof(remoteaddr);
    char remoteIP[INET6_ADDRSTRLEN];
    int new_fd;

    new_fd = accept(server_fd, (struct sockaddr*)&remoteaddr, &addrlen);
    if (new_fd == -1) {
        perror("accept");
        return -1;
    }

    if (remoteaddr.ss_family == AF_INET) {
        struct sockaddr_in* s = (struct sockaddr_in*)&remoteaddr;
        inet_ntop(AF_INET, &s->sin_addr, remoteIP, sizeof(remoteIP));
    } else {
        struct sockaddr_in6* s = (struct sockaddr_in6*)&remoteaddr;
        inet_ntop(AF_INET6, &s->sin6_addr, remoteIP, sizeof(remoteIP));
    }
    printf("New connection from %s on socket %d\n", remoteIP, new_fd);

    return new_fd;
}

int main() {
    int server_fd = setup_server();
    if (server_fd < 0) {
        std::cerr << "Failed to set up server\n";
        return 1;
    }

    while (true) {
        int client_fd = setup_client_connection(server_fd);
        if (client_fd == -1) {
            close(client_fd);
            return 1;
        }

        pthread_t client;
        pthread_create(&client, NULL, Command_Shift, (void*)&client_fd);
        pthread_detach(client);
    }

    close(server_fd);
    return 0;
}

