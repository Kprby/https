#pragma once

#include <streambuf>
#include "ssl_client.h"

class ssl_client_buf : public std::streambuf
{
public:
    ssl_client_buf();
    virtual ~ssl_client_buf() override;

    //成功返回0
    int connect(const char *node, const char *service);
    void disconnect();

protected:
    virtual int_type underflow() override;
    virtual int_type overflow(int_type ch) override;
    virtual int sync() override;

private:
    using traits_type = std::streambuf::traits_type;
    char* const _input_buffer;
    char* const _output_buffer;
    ssl_client _ssl_client;
};
