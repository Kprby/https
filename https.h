#pragma once

#include <string>
#include <map>
#include <iostream>

struct status_line
{
    std::string http_version;
    int status_code = 0;
    std::string reason_phrase;
};

struct less_no_case
{
    bool operator() (const std::string &lhs, const std::string &rhs);
};

class http_headers : public std::map<std::string, std::string, less_no_case> {};

std::istream& operator>> (std::istream &is, status_line &line);
std::istream& operator>> (std::istream &is, http_headers &headers);

int https_download(const char *host, const char *rest, const char *file_path);



