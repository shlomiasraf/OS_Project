#include "reactor.hpp"
#include <sys/select.h>

void *startReactor() {
    // Allocate memory for the Reactor structure
    Reactor *reactor = (Reactor *)malloc(sizeof(Reactor));
    if (!reactor) {
        perror("Failed to allocate memory for Reactor");
        return NULL;
    }

    // Initialize reactor members
    reactor->fd_size = 5;
    reactor->curr_index = 0;
    reactor->pfds = nullptr;
    reactor->funcs = nullptr;

    // Allocate memory for poll file descriptors
    reactor->pfds = (pollfd *)malloc(sizeof(pollfd) * reactor->fd_size);
    if (!reactor->pfds) {
        perror("Failed to allocate memory for pfds");
        free(reactor);
        return NULL;
    }

    // Allocate memory for function pointers
    reactor->funcs = (reactorFunc *)malloc(sizeof(reactorFunc) * reactor->fd_size);
    if (!reactor->funcs) {
        perror("Failed to allocate memory for funcs");
        free(reactor->pfds);
        free(reactor);
        return NULL;
    }

    // Initialize function pointers to null
    memset(reactor->funcs, 0, sizeof(reactorFunc) * reactor->fd_size);
    return reactor;
}

int addFdToReactor(void *reactor, int fd, reactorFunc func)
{   
  Reactor * reactorptr= (Reactor *)reactor;

    if(!reactorptr || !func) return -1;

   if (reactorptr->curr_index == reactorptr->fd_size) {
        reactorptr->fd_size *= 2; // Double it
         reactorptr->pfds = (pollfd *) realloc(reactorptr->pfds, sizeof(reactorptr->pfds) * (reactorptr->fd_size));
        reactorptr->funcs = (reactorFunc *)realloc(reactorptr->funcs, sizeof(reactorFunc) * (reactorptr->fd_size));
        if (!reactorptr->pfds || !reactorptr->funcs) {
            return -1;
        
     }
   }

   reactorptr->pfds[reactorptr->curr_index].fd = fd;
   reactorptr->pfds[reactorptr->curr_index].events = POLLIN;
   reactorptr->funcs[reactorptr->curr_index] = func;
   (reactorptr->curr_index  )++;
    return 0;
    
}

int removeFdFromReactor(void *reactor, int fd)
{
    Reactor * reactorptr= (Reactor *)reactor;
    for(int i=0; i<reactorptr->fd_size; i++){
     if(reactorptr->pfds[i].fd==fd)
     close(reactorptr->pfds[i].fd);
    // Copy the one from the end over this one
    reactorptr->pfds[i] = reactorptr->pfds[reactorptr->fd_size-1];
    (reactorptr->fd_size)--;
    return 0;
    }
    std::cout<< "Fd " << fd << " not found in reactor\n";
    return -1; // Return -1 if fd not found (error handling)
}

int stopReactor(void *reactor)
{
    Reactor * reactorptr = (Reactor *)reactor;
    free(reactorptr->pfds);
    free(reactorptr->funcs);
    free(reactorptr); return 0;
}
void runReactor(void* reactor_ptr, std::atomic<bool>& shutdown_flag) {
    Reactor* reactor = (Reactor*)reactor_ptr;
    while (!shutdown_flag.load()) 
    {
        // Use a small timeout (e.g., 100 milliseconds) to check for shutdown
        struct timespec timeout;
        timeout.tv_sec = 0;
        timeout.tv_nsec = 100 * 1000; // 100 milliseconds

        int poll_count = ppoll(reactor->pfds, reactor->curr_index, &timeout, nullptr); // Wait with timeout

        if (poll_count == -1) 
        {
            perror("poll");
            break;
        }

        for (int i = 0; i < reactor->curr_index; i++) {
            if (reactor->pfds[i].revents & POLLIN) { // Check if there is data to read
                reactor->funcs[i](reactor->pfds[i].fd); // Call the corresponding handler function
            }
        }
    }
}
