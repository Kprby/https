#include "ssl_client.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

ssl_client::ssl_client()
{

}

ssl_client::~ssl_client()
{
    clear();
}

int ssl_client::connect(const char *node, const char *service)
{
    int ecode = 0;
    int sfd = -1;
    int ret = 0;
    struct addrinfo hints;
    struct addrinfo *ai_head, *ai_ptr;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* tcp socket */
    ecode = getaddrinfo(node, service, &hints, &ai_head);
    if (ecode)
    {
        freeaddrinfo(ai_head);
        if (EAI_SYSTEM != ecode)
            fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(ecode));
        else
        {
            ecode = errno;
            perror("getaddrinfo()");
        }
        return ecode;
    }

    /* getaddrinfo() returns a list of address structures.
       Try each address until we successfully connect(2).
       If socket(2) (or connect(2)) fails, we (close the socket
       and) try the next address. */
    for (ai_ptr = ai_head; ai_ptr != NULL; ai_ptr = ai_ptr->ai_next)
    {
        sfd = socket(ai_ptr->ai_family, ai_ptr->ai_socktype, ai_ptr->ai_protocol);
        if (sfd != -1)
        {
            if (::connect(sfd, ai_ptr->ai_addr, ai_ptr->ai_addrlen) != -1)
                break;
            else
                close(sfd);
        }
    }
    freeaddrinfo(ai_head);

    if (sfd == -1)
    {
        ecode = errno;
        perror("socket()");
        return ecode;
    }
    if (!ai_ptr) /* No address succeeded */
    {
        ecode = errno;
        perror("connect()");
        return ecode;
    }
    _tcp_sfd = sfd;

    // New context saying we are a client
    _ssl_ctx = SSL_CTX_new(TLS_client_method());
    if (!_ssl_ctx)
    {
        ecode = -1;
        goto ssl_error;
    }

    // Create an SSL struct for the connection
    _ssl = SSL_new(_ssl_ctx);
    if (!_ssl)
    {
        ecode = -1;
        goto ssl_error;
    }

    // Connect the SSL struct to our connection
    if (SSL_set_fd(_ssl, _tcp_sfd) != 1)
    {
        ecode = -1;
        goto ssl_error;
    }

    SSL_set_mode(_ssl, SSL_get_mode(_ssl) | SSL_MODE_AUTO_RETRY);

    // Initiate SSL handshake
    ret = SSL_connect(_ssl);
    if (ret != 1)
    {
        ecode = SSL_get_error(_ssl, ret);
        goto ssl_error;
    }

    return ecode;

ssl_error:
    ERR_print_errors_fp(stderr);
    clear();
    return ecode;
}

void ssl_client::disconnect()
{
    SSL_shutdown(_ssl);
    clear();
}

int ssl_client::read(std::string &buf, int num)
{
    assert(num > 0);

    int ret = 0;
    buf.resize(num);
    ret = SSL_read(_ssl, &buf[0], num);
    if (ret <= 0)
        ERR_print_errors_fp(stderr);
    return ret;
}

int ssl_client::write(const std::string &buf)
{
    int ret = 0;
    ret = SSL_write(_ssl, buf.data(), buf.size());
    if (ret <= 0)
        ERR_print_errors_fp(stderr);
    return ret;
}

void ssl_client::clear()
{
    SSL_free(_ssl);
    _ssl = nullptr;

    SSL_CTX_free(_ssl_ctx);
    _ssl_ctx = nullptr;

    if (_tcp_sfd != -1)
    {
        close(_tcp_sfd);
        _tcp_sfd = -1;
    }
}

bool ssl_client::retryable(int ret_code)
{
    int ecode = 0;
    ecode = SSL_get_error(_ssl, ret_code);
    if (ecode == SSL_ERROR_WANT_READ || ecode == SSL_ERROR_WANT_WRITE)
        return true;
    else
        return false;
}
