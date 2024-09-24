#include "ClientConnectionStage.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

ClientConnectionStage::ClientConnectionStage(int server_fd)
    : server_fd(server_fd){
    start();
}

ClientConnectionStage::~ClientConnectionStage() {
    stop();  // Ensure that the thread stops when the object is destroyed
}

int ClientConnectionStage::StartConnectingClients() {
     int client_fd=0;
    // Call to setup_client_connection and block until a client connects
    client_fd = setup_client_connection();  // Call to setup_client_connection
    if (client_fd < 0) {
        std::cerr << "Failed to accept client connection." << std::endl;
    } else {
        std::cout << "Client connected with fd: " << client_fd << std::endl;
    }         
    return client_fd;  // Return the client_fd (will be -1 if the connection fails)
}

int ClientConnectionStage::setup_client_connection() {

    struct sockaddr_storage remoteaddr;
    socklen_t addrlen = sizeof(remoteaddr);
    char remoteIP[INET6_ADDRSTRLEN];
    int new_fd;
    
    new_fd = accept(server_fd, (struct sockaddr*)&remoteaddr, &addrlen);
    if (new_fd == -1) {
        perror("accept");
        return -1;  // Return -1 to indicate failure
    }

    if (remoteaddr.ss_family == AF_INET) {
        struct sockaddr_in* s = (struct sockaddr_in*)&remoteaddr;
        inet_ntop(AF_INET, &s->sin_addr, remoteIP, sizeof(remoteIP));
    } else {
        struct sockaddr_in6* s = (struct sockaddr_in6*)&remoteaddr;
        inet_ntop(AF_INET6, &s->sin6_addr, remoteIP, sizeof(remoteIP));
    }
    std::cout << "New connection from " << remoteIP << " on socket " << new_fd << std::endl;
    return new_fd;
}



