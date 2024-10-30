#ifndef ACTIVEOBJECT_HPP
#define ACTIVEOBJECT_HPP

#include <queue>
#include <stack>
#include <climits>
#include <mutex>
#include <stdio.h>
#include <condition_variable>
#include <thread>
#include <functional>
#include <iostream>
#include <atomic>
#include <unistd.h>
#include <algorithm>
#include <sys/socket.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include "primMST.hpp"
#include "kruskalMST.hpp"
#include "Graph.hpp"

enum class Command {
    Newgraph,
    Prim,
    Kruskal,
    Addedge,
    Removeedge,
    Exit,
    Invalid
};

// Abstract base class for active objects
class ActiveObject {
public:
    ActiveObject();
    ~ActiveObject();
    void start();
    void stop();
    void enqueue(std::function<void()> task);

private:
    std::atomic<bool> stopRequested_;  // Correctly declared here
    void process();
    std::thread worker_;
    std::mutex mutex_;
    std::condition_variable condition_;
    std::queue<std::function<void()>> tasks_;

};
#endif // ACTIVEOBJECT_HPP
