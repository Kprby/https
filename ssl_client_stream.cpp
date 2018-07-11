#include "ssl_client_stream.h"

ssl_client_stream::ssl_client_stream()
    : std::iostream(&_ssl_client_buf)
{

}
