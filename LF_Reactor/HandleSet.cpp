#include "HandleSet.hpp"

HandleSet::HandleSet() {
    FD_ZERO(&readfds); // Initialize the file descriptor set
}

void HandleSet::addHandle(int handle) {
    handles.insert(handle);
    FD_SET(handle, &readfds); // Add to the fd_set
}

void HandleSet::removeHandle(int handle) {
    handles.erase(handle);
    FD_CLR(handle, &readfds); // Remove from the fd_set
}

int HandleSet::waitForEvent() {
    fd_set tempSet = readfds; // Copy the original set to modify
    int maxFd = getMaxFd();

    // Wait for an event
    int activity = select(maxFd + 1, &tempSet, nullptr, nullptr, nullptr);
    if (activity < 0) {
        std::cerr << "select error" << std::endl;
        return -1; // Error handling
    }

    // Check which handle has activity
    for (int handle : handles) {
        if (FD_ISSET(handle, &tempSet)) {
            return handle; // Return the ready handle
        }
    }
    return -1; // No handles were ready
}

int HandleSet::getMaxFd() {
    return handles.empty() ? -1 : *std::max_element(handles.begin(), handles.end());
}

