#ifndef CONCRETEHANDLE_HPP
#define CONCRETEHANDLE_HPP

#include "Handle.hpp"

class ConcreteHandle : public Handle {
public:
    ConcreteHandle(int clientId) : clientId(clientId) {}

    void process() override {
        // Implement the logic to handle the client's request
        std::cout << "Processing request for client " << clientId << std::endl;
    }

    std::string getType() const override {
        return "ConcreteHandle";
    }

private:
    int clientId; // Identifier for the client
};

#endif // CONCRETEHANDLE_HPP

