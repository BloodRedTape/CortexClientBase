#include "client/client_base.hpp"
#include "common/repository.hpp"
#include "common/protocol.hpp"
#include "common/log.hpp"
#include <sstream>

void ClientBase::CheckForChanges(){
    sf::Packet packet;

    Socket::Status status = ServerConnection.receive(packet);

    if(status == Socket::Done){
        Header header;
        packet >> header;

        if(header.MagicWord != s_MagicWord)
            return Log("Garbage Packet");

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
            case MsgType::RepositoriesInfo:{
                RepositoriesInfo info;

                packet >> info;

                OnRepositoriesInfo(std::move(info));
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
    assert(false);
    
    Packet packet;

    Header header;
    header.MagicWord = s_MagicWord;
    header.Type = MsgType::FileContentRequest;

    packet << header;

    FileContentRequest request;
    request.FileName = entry_name.string(); // XXX garbage copy??

    packet << request;

    ServerConnection.send(packet);
}

void ClientBase::OnFileContentResponce(FileContentResponce responce){
    Log("Got: FileContentResponce");
    Println("File: {}", responce.FileName);
    Println("FileSize: {}", responce.FileContent.size());
}

void ClientBase::OnRepositoryStateNotify(RepositoryStateNotify notify){
    Log("Got: RepositoryStateNotify");
    Println("Name: ", notify.RepositoryName);
    Println("{}",((std::stringstream&)(std::stringstream() << notify.RepositoryState)).str());
}

void ClientBase::OnRepositoriesInfo(RepositoriesInfo info){
    Log("Server has {} repositories", info.RepositoryNames.size());
    for(auto &name: info.RepositoryNames)
        Println("{}", name);
}