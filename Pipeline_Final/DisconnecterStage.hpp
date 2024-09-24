#ifndef DISCONNECTERSTAGE_HPP
#define DISCONNECTERSTAGE_HPP

#include "ActiveObject.hpp"

class DisconnecterStage : public ActiveObject {
public:
    DisconnecterStage();
    ~DisconnecterStage();
    void disconnectClient(int client_fd);
protected:

};

#endif // DISCONNECTERSTAGE_HPP

