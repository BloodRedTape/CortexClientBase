#include "client/client_base.hpp"
#include "common/repository.hpp"
#include <iostream>

void ClientBase::CheckForChanges(){
    sf::Packet packet;

    Socket::Status status = ServerConnection.receive(packet);
    std::cout << status << std::endl;
    if(status == Socket::Done){
        puts("Got new state");
        RepositoryState state;
        packet >> state;

        std::cout << state;
    }
}

bool ClientBase::Connect(IpAddress address, Uint16 port){
    ServerConnection.setBlocking(true);

    bool res = ServerConnection.connect(address, port) == Socket::Done;

    ServerConnection.setBlocking(false);

    return res;
}

void ClientBase::Disconnect(){
    ServerConnection.disconnect();
}