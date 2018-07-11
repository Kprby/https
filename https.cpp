#include "https.h"
#include "ssl_client_stream.h"
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <algorithm>
#include <sstream>

std::istream &operator>>(std::istream &is, status_line &line)
{
    is >> line.http_version >> line.status_code;
    while (isspace(is.peek()))
        is.ignore();
    std::getline(is, line.reason_phrase, '\r');
    is.ignore();
    return is;
}

std::istream &operator>>(std::istream &is, http_headers &headers)
{
    headers.clear();

    std::string line;
    size_t fb, fe, sb, se;
    size_t pos;
    while (true)
    {
        std::getline(is, line);
        if (is && line.size() > 1)
        {
            std::string first, second;

            for (pos = 0; pos < line.size() && isspace(line[pos]); ++pos);
            fb = pos;
            for (++pos; pos < line.size() && line[pos] != ':' && !isspace(line[pos]); ++pos);
            fe = pos;
            for (++pos; pos < line.size() && (isspace(line[pos]) || line[pos] == ':'); ++pos);
            sb = pos;
            for (++pos; pos < line.size() && line[pos] != '\r'; ++pos);
            se = pos;

            if (fe < line.size())
            {
                first = line.substr(fb, fe - fb);
                if (sb < line.size() - 2)
                    second = line.substr(sb, se - sb);
                headers.emplace(std::move(first), std::move(second));
            }
        }
        else
            break;
    }
    return is;
}

bool less_no_case::operator() (const std::string &lhs, const std::string &rhs)
{
    std::string lhs_copy(lhs.size(), '\0');
    std::string rhs_copy(rhs.size(), '\0');
    std::transform(lhs.begin(), lhs.end(), lhs_copy.begin(), tolower);
    std::transform(rhs.begin(), rhs.end(), rhs_copy.begin(), tolower);
    return lhs_copy < rhs_copy;
}

int https_download(const char *host, const char *rest, const char *file_path)
{
    assert(host && file_path);

    ssl_client_stream scs;
    status_line line;
    http_headers headers;
    scs.connect(host, "https");
    if (!rest)
        rest = "/";
    scs << "GET " << rest << " HTTP/1.1\r\n"
        << "Host: " << host << "\r\n"
        << "Connection: keep-alive\r\n"
        << "\r\n";
    scs.flush();

    scs >> line;
    if (line.status_code != 200)
    {
        fprintf(stderr, "https request error:%d, reason phrase:%s\n",
                line.status_code, line.reason_phrase.c_str());
        return -1;
    }

    scs >> headers;
    http_headers::iterator iter = headers.find("content-length");
    if (iter == headers.end())
    {
        fprintf(stderr, "response headers error\n");
        return -1;
    }
    size_t content_length = 0;
    std::istringstream sin(iter->second);
    sin >> content_length;


    char *html = new char[content_length];
    scs.read(html, content_length);
    std::cout.write(html, content_length);
    delete[] html;

    return 0;
}
