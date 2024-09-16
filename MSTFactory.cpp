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
#define PORT "9034"
#include "proactor.hpp"
#include "MSTFactory.hpp"
#include "primMST.hpp"
#include "kruskalMST.hpp"
#include "Graph.hpp"
#include <functional>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
Graph graph(0,0);
void MSTFactory::getMSTAlgorithm(Command type ,int client_fd) 
{
    PrimMST primInstance;
    KruskalMST kruskalInstance;
    std::string message;
    switch (type) {
        case Command::Prim:
            message = primInstance.primFunction(graph);
            break;
        case Command::Kruskal:
            message = kruskalInstance.kruskalFunction(graph);  // Assuming similar function for Kruskal
            break;
        default:
            message = "Invalid MST command.";
            break;
    }
    send(client_fd, message.c_str(), message.size(), 0);
}

Command getCommandFromString(const std::string& commandStr) 
{
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
    else {
        return Command::Invalid;
    }
}

void Newgraph(int clientfd)
{
    int vertex,edges;
    std::string message;
    message="Please enter the number of vertices and edges: \n";
    send(clientfd, message.c_str(), message.size(), 0);
    message.clear();
    std::cin >> vertex >> edges;
    message="Please enter the edges: \n";
    send(clientfd, message.c_str(), message.size(), 0);
    message.clear();
    graph = Graph(vertex, edges);    
    for (int i = 0; i < edges; i++) 
    {
        int u, v, weight;
        std::cin >> u >> v >> weight;
        while(u > vertex || v > vertex)
        {
            std::cout << "Please enter valid edges: " << std::endl;
            std::cin >> u >> v >> weight;
        }
        graph.addEdge(u-1,v-1,weight);
    }
    message="The graph has created!\n";
    send(clientfd, message.c_str(), message.size(), 0);
    message.clear();
    return;
}

void Addedge(int clientfd)
{
    int u, v, weight;
    std::string message="Please enter edge you wish to add\n";
    send(clientfd, message.c_str(), message.size(), 0);
    message.clear();
    std::cin >> u >> v >> weight;
    graph.addEdge(u-1,v-1,weight);
}

void RemoveEdge(int clientfd)
{
    int i, j;
    std::string message="Enter edge to remove (i j): ";
    send(clientfd, message.c_str(), message.size(), 0);
    message.clear();
    std::cin >> i >> j;
    graph.removeEdge(i-1,j-1);
}

std::string handle_recieve_data(int client_fd){
// If not the listener, w int sender_fd = poll_fds[index].fd;e're just a regular client
    char buf[256];
    int nbytes = recv(client_fd, buf, sizeof buf, 0);
    if (nbytes <= 0) {
     // Got error or connection closed by client
        if(nbytes==0){
          printf("socket %d hung up\n", client_fd);
         }else
                std::cout<<"recv\n";
                    close(client_fd);
                    return "exit";      
    }   
        buf[nbytes]='\0';
        std::string input(buf);
        return input;
}

void * Command_Shift(void * client_socket)
{
        pthread_mutex_lock(&mutex);
        int client_fd=  *(int*)client_socket;
        dup2(client_fd, STDIN_FILENO);  
        std::string input;
        Command command = Command::Invalid;
        while (command != Command::Exit) 
        {
        
            input=handle_recieve_data(client_fd);
            command = getCommandFromString(input);

            switch (command) 
            {
                case Command::Newgraph:
                    Newgraph(client_fd);
                    break;
                case Command::Prim:
                    MSTFactory::getMSTAlgorithm(command, client_fd);
                    break;
                case Command::Kruskal:
                    MSTFactory::getMSTAlgorithm(command, client_fd);
                    break;
                case Command::Addedge:
                    Addedge(client_fd);
                    break;

                case Command::Removeedge:
                    RemoveEdge(client_fd);
                    break;

                case Command::Invalid:
                    std::cout << "Invalid command!" << std::endl;
                    break;

                case Command::Exit:
                    break;
            
        }
    }      
        pthread_mutex_unlock(&mutex);
        return NULL;
}

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int setup_server() {

    int server_fd;
    struct sockaddr_in  address;                            
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
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
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
// Function to add a client.
int setup_client_connection(int server_fd) {
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen = sizeof(remoteaddr);
    char remoteIP[INET6_ADDRSTRLEN];
    int new_fd;

    // Accept the new connection
    new_fd = accept(server_fd, (struct sockaddr*)&remoteaddr, &addrlen);
    if (new_fd == -1) {
        perror("accept");
        return -1;
    }

    // Convert the IP address to a string and print it
    if (remoteaddr.ss_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in*)&remoteaddr;
        inet_ntop(AF_INET, &s->sin_addr, remoteIP, sizeof(remoteIP));
    } else { // AF_INET6
        struct sockaddr_in6 *s = (struct sockaddr_in6*)&remoteaddr;
        inet_ntop(AF_INET6, &s->sin6_addr, remoteIP, sizeof(remoteIP));
    }
    printf("New connection from %s on socket %d\n", remoteIP, new_fd);

    return new_fd;
}

int main() {
    // Set up server
    int server_fd = setup_server();
    if (server_fd < 0) {
        std::cerr << "Failed to set up server\n";
        return 1;
    }
    while (true) {
        int client_fd= setup_client_connection(server_fd);  
            if(client_fd==-1){
                close(client_fd);
                return 1;
            }
            pthread_t client=startProactor(client_fd,Command_Shift);
            pthread_detach(client);
            //stopProactor(client);
    }

    close(server_fd);
    return 0;
}