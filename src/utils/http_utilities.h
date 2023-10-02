#pragma once

#include <HTTPClient.h>

namespace HTTP
{
    // perform a GET request and return the content's length. 
    int get_length(HTTPClient &httpClient, const char *url);
    int get_length(HTTPClient &httpClient, const char *path, const char *ext);
}