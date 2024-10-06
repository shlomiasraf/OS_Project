#ifndef REACTOR_HPP
#define REACTOR_HPP
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <sys/select.h>
#include <atomic>

typedef void * (*reactorFunc)(int fd);

typedef struct {
    struct pollfd *pfds;
    reactorFunc *funcs;    
    int curr_index;
    int fd_size;
  }Reactor;

    void * startReactor();
    int addFdToReactor(void * reactor, int fd, reactorFunc func) ; 
    int stopReactor(void * reactor);
    int removeFdFromReactor(void * reactor, int fd);
void runReactor(void* reactor, std::atomic<bool>& shutdown_flag);
 

#endif // REACTOR_HPP
