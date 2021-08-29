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
                FileContentResponce responce;
                packet >> responce;

                OnFileContentResponce(std::move(responce));
            }break;
            case MsgType::RepositoryStateNotify:{
                RepositoryStateNotify notify;
                packet >> notify;

                OnRepositoryStateNotify(std::move(notify));
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

void ClientBase::RequestFileContent(const fs::path &entry_name){
    Packet packet;

    Header header;
    header.MagicWord = s_MagicWord;
    header.Type = MsgType::FileContentRequest;

    packet << header;

    FileContentRequest request;
    request.Name = entry_name.string(); // XXX garbage copy??

    packet << request;

    ServerConnection.send(packet);
}

void ClientBase::OnFileContentResponce(FileContentResponce responce){
    std::cout << "FileContentResponce\n";
}

void ClientBase::OnRepositoryStateNotify(RepositoryStateNotify notify){
    std::cout << "RepositoryStateNotify\n";
    std::cout << "Name: " << notify.Name << std::endl;
    std::cout << notify.State;
}