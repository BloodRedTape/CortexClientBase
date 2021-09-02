#ifndef CORTEX_CLIENT_BASE_HPP
#define CORTEX_CLIENT_BASE_HPP

#include "common/connection.hpp"
#include "common/fs.hpp"
#include "common/protocol.hpp"

struct ClientBase{
    Connection ServerConnection;
    RepositoriesRegistry Registry;
    RepositoriesPathConfig Config;

    ClientBase(const char *config_filepath);

    bool IsConnected()const{
        return ServerConnection.getRemoteAddress() != IpAddress::Any;
    }

    void DispatchIncomingMessages();

    bool Connect(IpAddress address, Uint16 port);

    void Disconnect();

    virtual void OnFileContentResponce(FileContentResponce responce);

    virtual void OnRepositoryStateNotify(RepositoryStateNotify notify);

    virtual void OnAllRepositoriesStateNotify(AllRepositoriesStateNotify info);

};

#endif//CORTEX_CLIENT_BASE_HPP