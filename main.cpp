#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <string>
#include <iostream>
#include "ssl_client_stream.h"

// Very basic main: we send GET / and print the response.
int main (int argc, char **argv)
{
    ssl_client c;
    int ret = 0;
    std::string buf;
    ssl_client_stream scs("www.baidu.com", "https");
    scs << "GET https://www.baidu.com/"
           " HTTP/1.1\r\n\r\n";
    scs.flush();
    scs >> buf;
    /*ret = c.connect("www.baidu.com", "https");
    ret = c.write("GET https://www.baidu.com/"
            " HTTP/1.1\r\n\r\n");
    ret = c.read(buf);
    c.disconnect();*/
    std::cout << buf << std::endl;

    return 0;
}
