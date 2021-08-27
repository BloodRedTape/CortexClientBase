#ifndef CORTEX_CLIENT_BASE_HPP
#define CORTEX_CLIENT_BASE_HPP

#include "common/connection.hpp"

struct ClientBase{
    Connection ServerConnection;

    void CheckForChanges();

    bool Connect(IpAddress address, Uint16 port);
};

#endif//CORTEX_CLIENT_BASE_HPP