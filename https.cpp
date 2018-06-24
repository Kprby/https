#include "https.h"

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

// Establish a regular tcp connection
int tcp_connect(const char *hostname)
{
    int error, fd;
    struct hostent *host;
    struct sockaddr_in server;

    host = gethostbyname(hostname);
    if (!host)
    {
        printf("gethostbyname(%s) fail:%s\n", hostname, hstrerror(h_errno));
        return -1;
    }
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        perror ("socket() fail");
        return -1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(443);
    server.sin_addr = *((struct in_addr *) host->h_addr);
    bzero (&(server.sin_zero), 8);

    error = connect(fd, (struct sockaddr *) &server, sizeof (struct sockaddr));
    if (error == -1)
    {
        perror ("connect() fail");
        return -1;
    }

    return fd;
}

int https_download(const char *url, const char *path)
{

}
