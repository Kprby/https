#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <string>
#include <iostream>
#include <sstream>
#include "ssl_client_stream.h"
#include "https.h"

// Very basic main: we send GET / and print the response.
int main (int argc, char **argv)
{
    https_download("www.baidu.com", nullptr, "test.txt");
    return 0;
}
