#include "ActiveObject.hpp"
class MSTComputationStage : public ActiveObject {
//
//MST shlomi implement.private methods
public:
    void enqueue(std::function<void()> request) override;
    void execute() override;
    // Methods for performing MST computations
};

