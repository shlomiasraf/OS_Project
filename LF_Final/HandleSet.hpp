#ifndef HANDLESET_HPP
#define HANDLESET_HPP

#include <set>
#include <sys/select.h> // For select function
#include <iostream>
#include <algorithm> 
class HandleSet {
public:
    HandleSet(); // Constructor to initialize the fd_set

    // Adds a new handle (client socket) to the set
    void addHandle(int handle);

    // Removes a handle from the set
    void removeHandle(int handle);

    // Waits for an event on any of the handles and returns the ready handle
    int waitForEvent();

private:
    std::set<int> handles; // Set of client file descriptors
    fd_set readfds; // Set for select

    // Helper function to find the maximum file descriptor
    int getMaxFd();
};

#endif // HANDLESET_HPP

