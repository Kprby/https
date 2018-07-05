#pragma once

#include "ssl_client_buf.h"
#include <istream>

class ssl_client_stream : public std::iostream
{
public:
    ssl_client_stream(const char *node, const char *service);

private:
    ssl_client_buf _ssl_client_buf;
};
