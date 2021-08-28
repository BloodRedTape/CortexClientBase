#include "client/client_base.hpp"
#include "common/repository.hpp"
#include "common/protocol.hpp"
#include <iostream>

void ClientBase::CheckForChanges(){
    sf::Packet packet;

    Socket::Status status = ServerConnection.receive(packet);

    if(status == Socket::Done){
        Header header;
        packet >> header;

        if(header.MagicWord != s_MagicWord){
            std::cout << "Garbage packet" << std::endl;
            return;
        }

        switch(header.Type){
            case MsgType::Nop:{}break;
            case MsgType::FileContentResponce:{
                std::cout << "FileContentResponce\n";

                FileContentResponce responce;
                packet >> responce;
            }break;
            case MsgType::RepositoryStateNotify:{
                std::cout << "RepositoryStateNotify\n";

                RepositoryStateNotify notify;
                packet >> notify;

                std::cout << notify.State;
            }break;
            default:break;
        }
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