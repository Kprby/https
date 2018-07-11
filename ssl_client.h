#pragma once

#include <openssl/ssl.h>
#include <stdint.h>
#include <string>

constexpr int ssl_max_record_size = 16 * 1024;

class ssl_client
{
public:
    ssl_client();
    ~ssl_client();

    //成功返回0
    int connect(const char *node, const char *service);
    void disconnect();
    bool connected()
    {
        return static_cast<bool>(_ssl);
    }

    /*SSLv3 / TLSv1的最大记录大小为16kB*/
    int read(std::string &buf, int num = ssl_max_record_size);
    int read(void *buf, int num = ssl_max_record_size);

    /*The following return values can occur:

    > 0
    The write operation was successful,
    the return value is the number of bytes actually written to the TLS/SSL connection.

    <= 0
    The write operation was not successful,
    because either the connection was closed,
    an error occurred or action must be taken by the calling process.
    Call SSL_get_error() with the return value ret to find out the reason.

    Old documentation indicated a difference between 0 and -1,
    and that -1 was retryable.
    You should instead call SSL_get_error() to find out if it's retryable.*/
    int write(const std::string &buf);
    int write(void *buf, int num);

private:
    void clear();

    int _tcp_sfd = -1;
    SSL *_ssl = nullptr;
    SSL_CTX *_ssl_ctx = nullptr;
};
