#include "ssl_client_buf.h"

ssl_client_buf::ssl_client_buf()
    : _input_buffer(new char[ssl_max_record_size])
    , _output_buffer(new char[ssl_max_record_size])
{

}

ssl_client_buf::~ssl_client_buf()
{
    delete[] _input_buffer;
    delete[] _output_buffer;
}

int ssl_client_buf::connect(const char *node, const char *service)
{
    setg(nullptr, nullptr, nullptr);
    int ret = _ssl_client.connect(node, service);
    if (ret == 0)
        setp(_output_buffer, _output_buffer + ssl_max_record_size);
    else
        setp(nullptr, nullptr);
    return ret;
}

void ssl_client_buf::disconnect()
{
    _ssl_client.disconnect();
    setg(nullptr, nullptr, nullptr);
    setp(nullptr, nullptr);
}

std::streambuf::int_type ssl_client_buf::underflow()
{
    int ret = 0;
    if (_ssl_client.connected())
        ret = _ssl_client.read(_input_buffer, ssl_max_record_size);

    if (ret > 0)
    {
        setg(_input_buffer, _input_buffer, _input_buffer + ret);
        return traits_type::to_int_type(*_input_buffer);
    }
    else
    {
        setg(nullptr, nullptr, nullptr);
        return traits_type::eof();
    }
}

std::streambuf::int_type ssl_client_buf::overflow(std::streambuf::int_type ch)
{
    if (sync() == 0)
    {
        if (traits_type::eq_int_type(ch, traits_type::eof()) != true)
            sputc(ch);
        return traits_type::to_int_type(0);
    }
    else
        return traits_type::eof();
}

int ssl_client_buf::sync()
{
    int ret = 0;
    if (_ssl_client.connected())
        ret = _ssl_client.write(pbase(), pptr() - pbase());

    if (ret > 0)
    {
        setp(_output_buffer, _output_buffer + ssl_max_record_size);
        return 0;
    }
    else
    {
        setp(nullptr, nullptr);
        return -1;
    }
}
