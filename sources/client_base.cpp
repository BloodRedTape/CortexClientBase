#include "client/client_base.hpp"


void ClientBase::CheckForChanges(){

}

bool ClientBase::Connect(IpAddress address, Uint16 port){
    ServerConnection.setBlocking(false);
    return ServerConnection.connect(address, port) == Socket::Done;
}