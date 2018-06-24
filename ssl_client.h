#ifndef SSL_CLIENT_H
#define SSL_CLIENT_H

#include <openssl/ssl.h>
#include <stdint.h>

class ssl_client
{
public:
    ssl_client();
    ~ssl_client();

    int connect(const char *node, const char *service);
    int disconnect();

private:
    void clear();

    int _tcp_sfd = -1;
    SSL *_ssl = nullptr;
    SSL_CTX *_ssl_ctx = nullptr;
};

#endif // SSL_CLIENT_H
