#ifndef COMMUNICATIONSTAGE_HPP
#define COMMUNICATIONSTAGE_HPP

#include "ActiveObject.hpp"

class CommunicationStage : public ActiveObject {
public:
    CommunicationStage();
    ~CommunicationStage();

    // Public function to handle client communication
    Command processClient(int client_fd);
    bool shouldStop;  
    Command enqueueProcessClient(int client_fd);
private:

   std::string handleReceiveData(int client_fd);
    Command getCommandFromString(const std::string& commandStr);
};

#endif // COMMUNICATIONSTAGE_HPP

