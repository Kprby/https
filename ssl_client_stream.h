#pragma once

#include "ssl_client_buf.h"
#include <istream>

class ssl_client_stream : public std::iostream
{
public:
    ssl_client_stream();

    //成功返回0
    int connect(const char *node, const char *service)
    {
        return _ssl_client_buf.connect(node, service);
    }
    void disconnect()
    {
        _ssl_client_buf.disconnect();
    }

private:
    ssl_client_buf _ssl_client_buf;
};
