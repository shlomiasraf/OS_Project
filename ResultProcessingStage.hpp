#include "ActiveObject.hpp"
class ResultProcessingStage : public ActiveObject {

//kruskal output.
//prim output.

public:
    void enqueue(std::function<void()> request) override;
    void execute() override;
    // Methods for formatting and sending results
};

