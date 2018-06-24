#include "https.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <string>
#include "ssl_client.h"

// Simple structure to keep track of the handle, and
// of what needs to be freed later.
typedef struct
{
    int socket;
    SSL *sslHandle;
    SSL_CTX *sslContext;
} connection;

// For this example, we'll be testing on openssl.org
//#define SERVER  "183.57.48.62" // api.weixin.qq.com 服务器地址
#define SERVER  "www.baidu.com"
#define PORT 443

// Establish a regular tcp connection
int tcpConnect ()
{
    int error, handle;
    struct hostent *host;
    struct sockaddr_in server;

    host = gethostbyname (SERVER);
    handle = socket (AF_INET, SOCK_STREAM, 0);
    if (handle == -1)
    {
        perror ("Socket");
        handle = 0;
    }
    else
    {
        server.sin_family = AF_INET;
        server.sin_port = htons (PORT);
        server.sin_addr = *((struct in_addr *) host->h_addr);
        bzero (&(server.sin_zero), 8);

        error = connect (handle, (struct sockaddr *) &server,
                         sizeof (struct sockaddr));
        if (error == -1)
        {
            perror ("Connect");
            handle = 0;
        }
    }

    return handle;
}

// Establish a connection using an SSL layer
connection *sslConnect (void)
{
    connection *c;

    c = (connection*)malloc (sizeof (connection));
    c->sslHandle = NULL;
    c->sslContext = NULL;

    c->socket = tcpConnect ();
    if (c->socket)
    {
        // Register the error strings for libcrypto & libssl
        SSL_load_error_strings ();

        // Register the available ciphers and digests
        //SSL_library_init ();
        //OpenSSL_add_all_algorithms();

        // New context saying we are a client
        c->sslContext = SSL_CTX_new(TLS_client_method());
        if (c->sslContext == NULL)
            ERR_print_errors_fp (stderr);

        // Create an SSL struct for the connection
        c->sslHandle = SSL_new (c->sslContext);
        if (c->sslHandle == NULL)
            ERR_print_errors_fp (stderr);

        // Connect the SSL struct to our connection
        if (!SSL_set_fd (c->sslHandle, c->socket))
            ERR_print_errors_fp (stderr);

        // Initiate SSL handshake
        if (SSL_connect (c->sslHandle) != 1)
            ERR_print_errors_fp (stderr);
    }
    else
    {
        perror ("Connect failed");
    }

    return c;
}

// Disconnect & free connection struct
void sslDisconnect (connection *c)
{
    if (c->socket)
        close (c->socket);
    if (c->sslHandle)
    {
        SSL_shutdown (c->sslHandle);
        SSL_free (c->sslHandle);
    }
    if (c->sslContext)
        SSL_CTX_free (c->sslContext);

    free (c);
}

// Read all available text from the connection
std::string sslRead (connection *c)
{
    const int readSize = 1024;
    std::string rc;
    int received = 0;
    char buffer[1024];

    if (c)
    {
        while (1)
        {
            received = SSL_read (c->sslHandle, buffer, readSize);
            buffer[received] = '\0';

            if (received > 0)
                rc += buffer;

            if (rc.size() >= 14722)
                break;
        }
    }

    return rc;
}

// Write text to the connection
void sslWrite (connection *c, const char *text)
{
    if (c)
        SSL_write (c->sslHandle, text, strlen (text));
}

// Very basic main: we send GET / and print the response.
int main (int argc, char **argv)
{
    /*connection *c;
    std::string response;

    c = sslConnect ();

    //sslWrite (c, "GET / HTTP/1.1\r\nHost: localhost\r\nconnection: close\r\n\r\n");
    // 这里记得写上自己的申请的appid 和 secret就可以了
    /*sslWrite (c, "GET /cgi-bin/token?grant_type=client_credential&appid=your_appid&"
    "secret=your_secret HTTP/1.1\r\nHost: localhost\r\nconnection: close\r\n\r\n");
    sslWrite (c, "GET https://www.baidu.com/"
    " HTTP/1.1\r\n\r\n");
    response = sslRead (c);

    printf ("%s\n", response.c_str());
    printf ("%u\n", response.size());

    sslDisconnect (c);*/

    ssl_client c;
    c.connect("www.baidu.com", "https");

    return 0;
}
