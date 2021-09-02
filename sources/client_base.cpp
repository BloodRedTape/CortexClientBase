#include "client/client_base.hpp"
#include "common/repository.hpp"
#include "common/protocol.hpp"
#include "common/log.hpp"
#include "yaml-cpp/yaml.h"
#include <sstream>

ClientBase::ClientBase(const char *config_filepath):
    Config(config_filepath)
{}

void ClientBase::DispatchIncomingMessages(){
    sf::Packet packet;

    Socket::Status status = ServerConnection.receive(packet);

    if(status == Socket::Done){
        Header header;
        packet >> header;

        if(header.MagicWord != s_MagicWord)
            return Log("Garbage Message");

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
            case MsgType::AllRepositoriesStateNotify:{
                AllRepositoriesStateNotify info;

                packet >> info;

                OnAllRepositoriesStateNotify(std::move(info));
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

void ClientBase::OnAllRepositoriesStateNotify(AllRepositoriesStateNotify info){
    Log("Server has sent {} repositories", info.Repositories.size());
    for(auto &&repo: info.Repositories){
        if(!Registry.IsOpen(repo.RepositoryName)){
            fs::path path = Config.GetPath(repo.RepositoryName);
        
            Registry.OpenRepository(std::move(path), std::move(repo.RepositoryName));
        }
            
        OnRepositoryStateNotify(repo);
    }
}