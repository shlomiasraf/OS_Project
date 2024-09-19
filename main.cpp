#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstring> 
#include "RequestHandling.hpp"

// Function to set up the server
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
// Function to accept a new client connection
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
std::string handleReceiveData(int client_fd) 
{
    char buf[256];
    memset(buf, 0, sizeof(buf));
    int nbytes = recv(client_fd, buf, sizeof(buf) - 1, 0);
    if (nbytes <= 0) {
        if (nbytes == 0) {
            std::cout << "socket " << client_fd << " hung up\n";
        } else {
            std::cerr << "recv error\n";
        }
        close(client_fd);
        return "exit\n";
    }
    buf[nbytes] = '\0';
    return std::string(buf);
}

int main() 
{
    // Initialize RequestHandling with 4 threads
    RequestHandling requestHandler(4);
    ThreadPool threadPool(4); // Create a ThreadPool with 4 threads

    int server_fd = setup_server();
    if (server_fd == -1) 
    {
        std::cerr << "Server setup failed\n";
        return 1;  // Server setup failed
    }

    while (true) 
    {
        int client_fd = setup_client_connection(server_fd);
        if (client_fd == -1) 
        {
            std::cerr << "Failed to accept client connection\n";
            continue;  // If accepting connection failed, continue to the next iteration
        }

        // Enqueue a task to handle the client using RequestHandling
        threadPool.enqueueTask([&requestHandler, client_fd]() 
        {
            std::string receivedData = handleReceiveData(client_fd);
            Command command = requestHandler.getCommandFromString(receivedData);
            requestHandler.processCommand(client_fd, command);
            close(client_fd);  // Close the client connection after processing
        });
    }

    close(server_fd);  // Close the server when finished
    return 0;
}