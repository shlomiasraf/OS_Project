#ifndef CLIENTCONNECTIONSTAGE_HPP
#define CLIENTCONNECTIONSTAGE_HPP

#include "ActiveObject.hpp"
#include "CommandExecuteStage.hpp"
#include "CommunicationStage.hpp"
#include "DisconnecterStage.hpp"

class ClientConnectionStage : public ActiveObject {
public:

    ClientConnectionStage(int server_fd);
    ~ClientConnectionStage();
    int StartConnectingClients();

private:
    int server_fd;  // File descriptor for the server
    int setup_client_connection();
};

#endif // CLIENTCONNECTIONSTAGE_HPP

