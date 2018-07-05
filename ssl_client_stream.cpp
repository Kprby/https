#include "ssl_client_stream.h"

ssl_client_stream::ssl_client_stream(const char *node, const char *service)
    : std::iostream(&_ssl_client_buf)
    , _ssl_client_buf(node, service)
{

}
