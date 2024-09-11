// Threads.cpp
#include <iostream>
#include <thread>
#include <vector>

void processRequest(int thread_id) {
    std::cout << "Thread " << thread_id << " is processing request...\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));  // Simulate work
    std::cout << "Thread " << thread_id << " finished processing\n";
}

int main() {
    const int NUM_THREADS = 5;
    std::vector<std::thread> threads;

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back(processRequest, i + 1);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << "All threads completed\n";
    return 0;
}
