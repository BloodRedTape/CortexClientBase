#ifndef CORTEX_CLIENT_BASE_HPP
#define CORTEX_CLIENT_BASE_HPP

#include "common/connection.hpp"
#include "common/fs.hpp"
#include "common/protocol.hpp"

struct ClientBase{
    Connection ServerConnection;
    RepositoriesRegistry Registry;

    bool IsConnected()const{
        return ServerConnection.getRemoteAddress() != IpAddress::Any;
    }

    void CheckForChanges();

    bool Connect(IpAddress address, Uint16 port);

    void Disconnect();

    void RequestFileContent(const fs::path &entry_name);

    bool GetOrCreateStoragePathForRepo(const std::string &name, fs::path &path);

    virtual void OnFileContentResponce(FileContentResponce responce);

    virtual void OnRepositoryStateNotify(RepositoryStateNotify notify);

    virtual void OnAllRepositoriesStateNotify(AllRepositoriesStateNotify info);

};

#endif//CORTEX_CLIENT_BASE_HPP