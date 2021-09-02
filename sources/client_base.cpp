#include "client/client_base.hpp"
#include "common/repository.hpp"
#include "common/protocol.hpp"
#include "common/log.hpp"
#include "yaml-cpp/yaml.h"
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

bool ClientBase::GetOrCreateStoragePathForRepo(const std::string &name, fs::path &path){
    YAML::Node config = YAML::LoadFile("client_config.yaml");

    constexpr const char *DefaultRepositoriesStorage = "DefaultRepositoriesStorage";
    constexpr const char *Repositories = "Repositories";
    constexpr const char *Path = "Path";

    if(config.IsNull())
        return Error("Can't read a client_config.yaml");
    if(!(config[DefaultRepositoriesStorage] && config[DefaultRepositoriesStorage].IsScalar()))
        return Error("Config should have a '{}' value", DefaultRepositoriesStorage);
    if(!fs::exists(config[DefaultRepositoriesStorage].as<std::string>()))
        return Error("{} does not exist or ill-formated", DefaultRepositoriesStorage);
    
    const YAML::Node &repositories = config[Repositories];

    if(repositories
    && repositories[name]
    && repositories[name][Path]){
        path = config[Repositories][name][Path].as<std::string>();
        Log("Repository '{}' has been found in config with path {}", name, path.string());
    }else{
        Log("Repository storage path is not found or ill-formated, using default one");

        std::string default_path = config[DefaultRepositoriesStorage].as<std::string>();
        path = default_path + "/" + name;

        fs::create_directories(path);
    }
    
    return true;
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
        fs::path path;
        if(GetOrCreateStoragePathForRepo(repo.RepositoryName, path)){
            Registry.OpenRepository(std::move(path), std::move(repo.RepositoryName));
            
            OnRepositoryStateNotify(repo);
        }else{
            Error("Can't open '{}' repo", repo.RepositoryName);
        }
    }
}